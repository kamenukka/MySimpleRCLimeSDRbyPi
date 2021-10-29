import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.10
import QtQuick.Layouts 1.7
import QtQuick.Dialogs 1.0
import QtCharts 2.0

import QtQuick.Shapes 1.11

import QtQuick.Controls.Material 2.12



ApplicationWindow {

    id: window
    Material.theme: Material.Light
    Material.accent: Material.BlueGrey
    width: checkBoxAddSettings.checked?1200:800
    height: 600
    visible: true
    title: qsTr("RC LimeSDR")
    onClosing:
    {
        appCore.writeSettings(textInputIP.text, textInputPort.text,spinBoxFreq.value,spinBoxFreqFd.value,
                              radioButtonTrans.checked?qsTr("TX"):qsTr("RX"), sliderNormalizedGain.value,
                              spinBoxOverSampling.value,  radioButton1Chan.checked?0:1, radioButton1ChanRec.checked?0:1,
                              textInputFilePath.text, textInputFilePathOut.text, spinBoxTimeToPlay.value,
                              spinBoxTimesToRec.value, spinBoxOutputSize.value,
                              spinBoxtx_streamfifoSize.value, spinBoxtx_streamthroughputVsLatency.value,
                              spinBoxrx_streamfifoSize.value, spinBoxrx_streamthroughputVsLatency.value,
                              radioButtonFMT32.cheked?"float32":"int12", (window.Material.theme === Material.Light)?"Light":"Dark")
    }

    property var cntrRec: 0
    property var defaltFolderUrl: CurDirPath

   Connections {
        target: appCore // Указываем целевое соединение
        onSendToQmlSettings:{

            console.log(IP, port, freq, Fd,   mode, Gain, oversampling, NumChanTx,
                        NumChanRx,  nameIn,  nameOut,  SecondsToPlay,  NumOfCyclesRec,  LenArrayToRec,
                        txfifosize,  txThVslat,  rxfifosize,  rxThVslat, fmt, theme,(theme==qsTr("Dark")))
              textInputIP.text = IP
              textInputPort.text = port
              spinBoxFreq.value = freq
              spinBoxFreqFd.value = Fd
              radioButtonTrans.checked = (mode==qsTr("TX"))?true:false
              radioButtonRec.checked = (mode==qsTr("RX"))?true:false
              sliderNormalizedGain.value = Gain
              spinBoxOverSampling.value = oversampling
              radioButton1Chan.checked = (NumChanTx==0)?true:false
              radioButton2Chan.checked = (NumChanTx==1)?true:false
              radioButton1ChanRec.checked = (NumChanRx==0)?true:false
              radioButton2ChanRec.checked = (NumChanRx==1)?true:false
              textInputFilePath.text = nameIn
              textInputFilePathOut.text = nameOut
              spinBoxTimeToPlay.value = SecondsToPlay
              spinBoxTimesToRec.value = NumOfCyclesRec
              spinBoxOutputSize.value = LenArrayToRec
              spinBoxtx_streamfifoSize.value = txfifosize
              spinBoxtx_streamthroughputVsLatency.value = txThVslat
              spinBoxrx_streamfifoSize.value = rxfifosize
              spinBoxrx_streamthroughputVsLatency.value = rxThVslat
              radioButtonFMT32.checked = (fmt==qsTr("float32"))?true:false
              window.Material.theme = (theme==qsTr("Dark"))?Material.Dark:Material.Light
              window.Material.accent = (theme==qsTr("Dark"))?Material.Orange:Material.BlueGrey
              radioButtonDark.checked = (theme==qsTr("Dark"))?true:false
        }

        onSendToQml: {
            if (FlagInit)
            {
                labelStatus.text = "";
            }
            else
            {
                if (array2display[0][0]=="!")
                {
                 if (array2display[0][1]=="!")
                    {
                     var tmp = array2display[0].replace("!!","")

                     textAreaLog.append(Qt.formatTime(new Date(), "hh:mm:ss") +": " + tmp)
                    }
                }
                            }
        }
        onSendToDopQML: {

                 textAreaLogDop.append(Qt.formatTime(new Date(), "hh:mm:ss") +": " + stringFromServer)

        }

        onSendToQmlMsg:
        {
            if (flagConnect)
            {
                labelStatus.text = "Сonnected";
                textAreaLog.append(Qt.formatTime(new Date(), "hh:mm:ss") +": " + "Pi подключен по TCP")
                labelStatus.color = '#8BC34A'
                buttonCheckStatus.visible = true

            }
            else
            {
                labelStatus.text = "Disconnected";
                textAreaLog.append(Qt.formatTime(new Date(), "hh:mm:ss") +": " + "Pi отключен по TCP")
                labelStatus.color = '#F44336'
                buttonCheckStatus.visible = false


            }
        }
        onSpecValueChanged: {
            lineSeries1.clear();
            var i
            console.log(arr2spec.length)
            for (i=0;i<arr2spec.length;i++)
            {
            lineSeries1.append(i,arr2spec[i])
            }
//            axisX.min = lineSeries1.at(0).x
            axisX.max = arr2spec.length;
        }
    }

   Window
   {
      id: plotSpec
      flags:  Qt.Window //| Qt.WindowStaysOnBottomHint
      title: qsTr(qsTr("# SPEC"))
       width: 400
       height: 400
       visible: checkBoxshowSpec.checked?true:false
       onVisibilityChanged: {
           if (checkBoxshowSpec.checked)
               if (visible)
                   checkBoxshowSpec.checked = true
           else
                   checkBoxshowSpec.checked = false

       }

       ChartView {
           id: chartView
           width: parent.width
           height: parent.height
           anchors.fill: parent
           animationOptions: ChartView.NoAnimation
           antialiasing: true
           backgroundColor: "#1f1f1f"

           ValueAxis {
               id: axisY1
               min: -100
               max: 10
               gridVisible: false
               color: "#ffffff"
               labelsColor: "#ffffff"
               labelFormat: "%.0f"
           }

           ValueAxis {
               id: axisX
               min: 0
               max: 8192
               gridVisible: false
               color: "#ffffff"
               labelsColor: "#ffffff"
               labelFormat: "%.0f"
               tickCount: 5
           }

           LineSeries {
               id: lineSeries1
               name: "signal 1"
               color: "white"
               axisX: axisX
               axisY: axisY1
           }
       }

   }


    FileDialog {
        id: fileDialog
        title: "Выбор файла"
        folder: CurDirPath
        //fileMode: FileDialog.SaveFile
        selectExisting: true
        onAccepted: {
            ////console.log("You chose: " + fileDialog.fileUrls)
            var path = fileDialog.fileUrls.toString();
            // remove prefixed "file:///"
            path = path.replace(/^(file:\/{3})/,"");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            textInputFilePath.text = cleanPath
            visible = false
        }
        onRejected: {
            ////console.log("Canceled")
            visible = false
        }
        nameFilters: [ "Text files (*.txt)", "All files (*)" ]
        Component.onCompleted: visible = false
    }


    FileDialog {
        id: fileDialogPathOut
        title: "Выбор директории"
        folder: defaltFolderUrl
        //fileMode: FileDialog.SaveFile
        selectFolder: true
        selectExisting: false
        onAccepted: {
            ////console.log("You chose: " + fileDialog.fileUrls)
            var path = fileDialog.fileUrls.toString();
            path = path.replace(/^(file:\/{3})/,"");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            textInputFilePathOut.text  ="";
            textInputFilePathOut.text = defaltFolderUrl + "/signalOutput.txt"
            console.log(path,cleanPath)
            visible = false
        }
        onRejected: {
            ////console.log("Canceled")
            visible = false
        }
        onFolderChanged: {

             defaltFolderUrl = folder;

        }
        nameFilters: [ "Text files (*.txt)", "All files (*)" ]
        //Component.onCompleted: visible = false
    }


    Button {
        id: buttonConnect
        x: 23
        y: 46
        width: 117
        height: 45
        text: qsTr("Конект")
        font.pointSize: 9
        checkable: false
        autoRepeat: false
        highlighted: true
        visible: true
        onClicked: {

            appCore.writeSettings(textInputIP.text, textInputPort.text,spinBoxFreq.value,spinBoxFreqFd.value,
                                  radioButtonTrans.checked?qsTr("TX"):qsTr("RX"), sliderNormalizedGain.value,
                                  spinBoxOverSampling.value,  radioButton1Chan.checked?0:1, radioButton1ChanRec.checked?0:1,
                                  textInputFilePath.text, textInputFilePathOut.text, spinBoxTimeToPlay.value,
                                  spinBoxTimesToRec.value, spinBoxOutputSize.value,
                                  spinBoxtx_streamfifoSize.value, spinBoxtx_streamthroughputVsLatency.value,
                                  spinBoxrx_streamfifoSize.value, spinBoxrx_streamthroughputVsLatency.value,
                                  radioButtonFMT32.cheked?"float32":"int12",(window.Material.theme === Material.Light)?"Light":"Dark")


            appCore.tCPThread(textInputIP.text, textInputPort.text, )

            }
    }

    Button {
        id: buttonSend
        x: 126
        y: 540
        width: 190
        height: 44

        text: qsTr("Послать команду")
        font.pointSize: 12
        checkable: false
        autoRepeat: false
        highlighted: true
        visible: true
        onClicked: {
            cntrRec = 0;
            appCore.sendSomething2(textInputFilePath.text, checkBoxDefaultOutput.checked?qsTr(""):textInputFilePathOut.text, radioButtonTrans.checked?1:0,spinBoxFreq.value,
                                   spinBoxFreqFd.value,sliderNormalizedGain.value, spinBoxOverSampling.value, radioButton1Chan.checked?0:1,
                                   radioButton1ChanRec.checked?0:1,spinBoxTimeToPlay.value, spinBoxOutputSize.value,
                                   spinBoxtx_streamfifoSize.value, spinBoxtx_streamthroughputVsLatency.value,
                                   spinBoxrx_streamfifoSize.value, spinBoxrx_streamthroughputVsLatency.value,
                                   radioButtonFMT32.cheked?"float32":"int12")

        }
    }

    TextInput {
        id: textInput
        x: 912
        y: 0

        width: 80
        height: 20
        visible: false
        font.pixelSize: 12
        wrapMode: Text.WordWrap
        persistentSelection: false
        renderType: Text.NativeRendering
        clip: false
    }

    Button {
        id: buttonDisconnect
        x: 157
        y: 46
        width: 139
        height: 45

        text: qsTr("Дисконект")
        font.pointSize: 9
        checkable: false
        autoRepeat: false
        highlighted: true
        visible: true
        onClicked: {
            appCore.tCPstopTCP()


        }
    }

    Slider {
        id: sliderNormalizedGain
        x: 151
        y: 238
        width: 139
        height: 27
        value: 50
        stepSize: 1
        from: 0
        to: 73
        ToolTip.visible: hovered
        ToolTip.text: Math.floor(value*100)/100
    }

    SpinBox {
        id: spinBoxFreq
        x: 157
        y: 118
        width: 139
        height: 28
        editable: true
        font.pointSize: 9
        stepSize: 1
        value: 800
        to: 6000
        from: 10
    }

    Label {
        id: label
        x: 26
        y: 125
        text: qsTr("Частота, МГц")
        font.pointSize: 9
    }

    SpinBox {
        id: spinBoxFreqFd
        x: 157
        y: 157
        width: 139
        height: 25
        editable: true
        stepSize: 1
        value: 5000
        font.pointSize: 9
        to: 60000
        from: 0
    }

    Label {
        id: label1
        x: 23
        y: 157
        width: 117
        height: 14
        text: qsTr("Частота дискретизации, кГц")
        wrapMode: Text.WordWrap
        font.pointSize: 9
    }

    Label {
        id: label2
        x: 151
        y: 213
        text: qsTr("Усиление ПРД")
        font.pointSize: 9
    }

    Label {
        id: labelStatus
        x: 24
        y: 96
        font.pointSize: 7
        text: qsTr("тут будет текущий статус")

    }
    Column
    {
        x: 338
        y: 11
        width: 70
        height: 108
        RadioButton {
            id: radioButtonTrans
        x: 34
        y: 352
        width: 73
        height: 50
        text: qsTr("ПРД")
        checked: true
        font.pointSize: 9
        }

    RadioButton {
        id: radioButtonRec
        x: 34
        y: 398
        width: 73
        height: 50
        text: qsTr("ПРМ")
        font.pointSize: 9
    }
    }

    Column
    {
        x: -18
        y: 250
        RadioButton {
        id: radioButton1Chan
        x: 34
        y: 352
        text: qsTr("1")
        checked: true
        font.pointSize: 9
        }

    RadioButton {
        id: radioButton2Chan
        x: 34
        y: 398
        text: qsTr("2")
        font.pointSize: 9
    }
    }


    Label {
        id: label3
        x: 327
        y: 33
        width: 47
        height: 64
        text: qsTr("Режим работы:")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pointSize: 9
    }

    Label {
        id: label4
        x: 24
        y: 205
        width: 35
        height: 30
        text: qsTr("Номер канала ПРД")
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.pointSize: 9
    }

    Label {
        id: label5
        x: 23
        y: 13
        width: 11
        height: 25
        text: qsTr("IP")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }

    TextInput {
        id: textInputIP
        x: 45
        y: 13
        width: 102
        color: (window.Material.theme == Material.Dark)?"white":"black"
        height: 25
        text: qsTr("169.254.183.65")
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        persistentSelection: true
        cursorVisible: false

    }

    Label {
        id: label6
        x: 220
        y: 13
        width: 27
        height: 25
        text: qsTr("порт")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }

    TextInput {
        id: textInputPort
        x: 253
        y: 13
        width: 44
        height: 25
        color: (window.Material.theme == Material.Dark)?"white":"black"

        text: qsTr("4002")
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        id: rectangle
        x: 322
        y: 123
        width: 457
        height: 461
        border.width: 1
        color: "#00000000"



        Flickable {
            id: flickable
            anchors.fill: parent
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            boundsMovement: Flickable.StopAtBounds
            boundsBehavior: Flickable.DragAndOvershootBounds
            TextArea.flickable: TextArea {
                id: textAreaLog
                x: 0
                y: 0
                width: 406
                height: 461
                text:  "=======================================================\n"+Qt.formatTime(new Date(), "hh:mm:ss") +": " + "Старт работы программы"
                readOnly: false
                wrapMode: TextArea.Wrap
                font.pointSize: 8
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }

    Rectangle {
        id: rectangle1
        x: 23
        y: 366
        width: 217
        height: 25
        color: "#00000000"
        border.width: 1

        Button {
            id: buttonOpenFile
            x: 220
            y: -5
            width: 41
            height: 30
            text: qsTr("...")
            onClicked:
            {
                fileDialog.visible = true
            }
        }
    }

    TextInput {
        id: textInputFilePath
        x: 27
        y: 368
        width: 199
        height: 23
        color: (window.Material.theme == Material.Dark)?"white":"black"
        text: qsTr("D:\\signalInput.txt")
        wrapMode: Text.Wrap
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }


    Button {
        id: buttonCheckStatus
        x: 107
        y: 85
        width: 33
        height: 34
        //        text: qsTr("сохранить")
        icon.source: "src/help.png"
        visible: false
        background: transparent
        checkable: false
        autoRepeat: false
        onClicked: {
            appCore.sendMessages();
        }

    }

    Column {
        x: 50
        y: 250
        RadioButton {
            id: radioButton1ChanRec
            x: 34
            y: 352
            text: qsTr("1")
            checked: true
            font.pointSize: 9
        }

        RadioButton {
            id: radioButton2ChanRec
            x: 34
            y: 398
            text: qsTr("2")
            font.pointSize: 9
        }
    }

    Label {
        id: label7
        x: 86
        y: 205
        width: 35
        height: 30
        text: qsTr("Номер канала ПРМ")
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.pointSize: 9
    }

    Label {
        id: label8
        x: 151
        y: 271
        text: qsTr("Оверсемплинг ПРМ")
        font.pointSize: 9
    }

    SpinBox {
        id: spinBoxOverSampling
        x: 151
        y: 302
        width: 139
        height: 28
        value: 0
        editable: true
        font.pointSize: 9
        stepSize: 1
        to: 100
        from: 0
    }

    Label {
        id: label9
        x: 23
        y: 346
        text: qsTr("Квадратуры сигнала для передачи")
        font.pointSize: 9
    }

    Rectangle {
        id: rectangle2
        x: 23
        y: 441
        width: 217
        height: 25
        color: "#00000000"
        border.width: 1
        Button {
            id: buttonOpenFileOut
            x: 219
            y: -6
            width: 42
            height: 31
            text: qsTr("...")
            onClicked:
            {
                fileDialogPathOut.visible = true

            }
        }
    }

    TextInput {
        id: textInputFilePathOut
        x: 27
        y: 441
        width: 199
        height: 23
        text: qsTr("D:\\signalOutput.txt")
        color: (window.Material.theme == Material.Dark)?"white":"black"
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.NoWrap
    }

    Label {
        id: label12
        x: 23
        y: 407
        text: qsTr("Сохранение принятых квадратур")
        font.pointSize: 9
    }

    CheckBox {
        id: checkBoxDefaultOutput
        x: 214
        y: 407
        checked: true
        width: 98
        height: 25
        text: qsTr("default")
    }

    SpinBox {
        id: spinBoxOutputSize
        x: 118
        y: 479
        width: 175
        height: 28
        value: 65535
        editable: true
        font.pointSize: 9
        stepSize: 1
        to: 2097152
        from: 10
    }

    Label {
        id: label13
        x: 23
        y: 472
        width: 61
        height: 59
        text: qsTr("Длина считываемой выборки")
        wrapMode: Text.WordWrap
        font.pointSize: 9
    }

    CheckBox {
        id: checkBox
        x: 20
        y: 532
        width: 29
        height: 17
        text: qsTr("Проводить калибровки")
        font.pointSize: 6
        display: AbstractButton.TextBesideIcon
        checked: true
    }

    SpinBox {
        id: spinBoxtx_streamfifoSize
        x: 981
        y: 63
        width: 197
        height: 28
        value: 262144
        editable: true
        font.pointSize: 9
        stepSize: 1
        to: 2097152
        visible: checkBoxAddSettings.checked?true:false

        from: 1024
    }

    Label {
        id: label14
        x: 819
        y: 70
        width: 105
        height: 21
        text: qsTr("tx_stream.fifoSize")
        visible: checkBoxAddSettings.checked?true:false

        font.pointSize: 9
    }

    SpinBox {
        id: spinBoxtx_streamthroughputVsLatency
        x: 981
        y: 109
        width: 197
        height: 28
        value: 50
        editable: true
        font.pointSize: 9
        stepSize: 1
        visible: checkBoxAddSettings.checked?true:false

        to: 100
        from: 0
    }

    Label {
        id: label15
        x: 819
        y: 110
        width: 142
        height: 40
        text: qsTr("tx_stream. throughputVsLatency, %")
        visible: checkBoxAddSettings.checked?true:false

        wrapMode: Text.WordWrap
        font.pointSize: 9
    }

    SpinBox {
        id: spinBoxTimeToPlay
        x: 635
        y: 46
        width: 139
        height: 37
        value: 10
        editable: true
        font.pointSize: 9
        stepSize: 1
        to: 100
        from: 0
    }

    Label {
        id: label16
        x: 483
        y: 57
        width: 146
        height: 18
        text: qsTr("Воспроизвродить, с")
        font.pointSize: 9
    }

    CheckBox {
        id: checkBoxAddSettings
        x: 599
        y: 11
        width: 180
        height: 29
        text: qsTr("Дополнительные настройки")
        display: AbstractButton.TextBesideIcon
        checked: false
        font.pointSize: 6
    }

    SpinBox {
        id: spinBoxrx_streamfifoSize
        x: 981
        y: 158
        width: 197
        height: 28
        value: 1048576
        editable: true
        font.pointSize: 9
        visible: checkBoxAddSettings.checked?true:false

        stepSize: 1
        to: 2097152
        from: 1024
    }

    Label {
        id: label17
        x: 819
        y: 165
        width: 105
        height: 21
        visible: checkBoxAddSettings.checked?true:false

        text: qsTr("rx_stream.fifoSize")
        font.pointSize: 9
    }

    SpinBox {
        id: spinBoxrx_streamthroughputVsLatency
        x: 981
        y: 204
        width: 197
        height: 28
        value: 100
        editable: true
        visible: checkBoxAddSettings.checked?true:false

        font.pointSize: 9
        stepSize: 1
        to: 100
        from: 0
    }

    Label {
        id: label18
        x: 819
        y: 205
        width: 142
        height: 40
        visible: checkBoxAddSettings.checked?true:false
        text: qsTr("rx_stream. throughputVsLatency, %")
        wrapMode: Text.WordWrap
        font.pointSize: 9
    }

    Column {
        x: 1089
        y: 262
        visible: checkBoxAddSettings.checked?true:false

        RadioButton {
            id: radioButtonFMT32
            x: 34
            y: 352
            height: 33
            text: qsTr("LMS_FMT_F32")
            visible: checkBoxAddSettings.checked?true:false

            checked: true
            font.pointSize: 4
        }

        RadioButton {
            id: radioButtonFMTI12
            x: 34
            y: 398
            width: 72
            height: 30
            text: qsTr("LMS_FMT_I12")
            visible: checkBoxAddSettings.checked?true:false

            font.pointSize: 4
        }
    }

    CheckBox {
        id: checkBoxshowSpec
        x: 448
        y: 11
        width: 145
        height: 30
        opacity: 1
        text: qsTr("спектр")
        autoRepeat: true
        checked: false
        display: AbstractButton.TextBesideIcon
        font.pointSize: 6
        onClicked:
        {
            appCore.flagSpecSet(checked,checkBoxshowSpecAnalyser.checked, spinBoxTimesToRec.value)
        }
    }

    CheckBox {
        id: checkBoxshowSpecAnalyser
        x: 448
        y: 89
        width: 165
        height: 30
        visible: true
        text: qsTr("Считывать, раз")
        checked: false
        display: AbstractButton.TextBesideIcon
        font.pointSize: 8
        onClicked:
        {
            appCore.flagSpecSet(checkBoxshowSpec.checked,checked,spinBoxTimesToRec.value)
        }
    }

    SpinBox {
        id: spinBoxTimesToRec
        x: 635
        y: 85
        width: 139
        height: 34
        editable: true
        stepSize: 1
        value: 50
        font.pointSize: 9
        to: 1000
        from: 1
        onValueChanged:         {
            appCore.flagSpecSet(checkBoxshowSpec.checked,checkBoxshowSpecAnalyser.checked, value)
        }
    }

    Label {
        id: label11
        x: 483
        y: 13
        width: 137
        height: 25
        text: qsTr("Показывать спектр")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }

    Label {
        id: label19
        x: 483
        y: 89
        width: 44
        height: 25
        text: qsTr("Записывать в цикле, раз")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }

    Label {
        id: label20
        x: 635
        y: 13
        width: 155
        height: 25
        text: qsTr("Дополнительные настройки")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }

    Label {
        id: label21
        x: 250
        y: 407
        width: 47
        height: 25
        text: qsTr("дефолт")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }

    Label {
        id: label22
        x: 52
        y: 532
        width: 47
        height: 25
        text: qsTr("калибровки")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }

    Column {
        x: 1097
        y: 510
        width: 103
        height: 90
        visible: checkBoxAddSettings.checked?true:false

        RadioButton {
            id: radioButtonLight
            x: 34
            y: 352
            text: qsTr("Ligth")
            checked: true
            font.pointSize: 9
            onClicked:
            {
                window.Material.theme =  Material.Light
                window.Material.accent = Material.BlueGrey
            }
        }

        RadioButton {
            id: radioButtonDark
            x: 34
            y: 398
            text: qsTr("Dark")
            font.pointSize: 9
            onClicked:
            {
                window.Material.theme =  Material.Dark
                window.Material.accent = Material.Orange
            }
        }
    }

    Rectangle {
        id: rectangle_dop
        x: 794
        y: 262
        width: 330
        height: 322
        border.width: 1
        color: "#00000000"
        visible: checkBoxAddSettings.checked?true:false


        Flickable {
            id: flickableDop
            anchors.fill: parent
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            boundsMovement: Flickable.StopAtBounds
            boundsBehavior: Flickable.DragAndOvershootBounds
            TextArea.flickable: TextArea {
                id: textAreaLogDop
                x: 0
                y: 0
                width: 338
                height: 322
                text:  "=========================================\n"
                readOnly: false
                wrapMode: TextArea.Wrap
                font.pointSize: 8
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }

    Label {
        id: label10
        x: 1137
        y: 504
        text: qsTr("Тема")
        visible: checkBoxAddSettings.checked?true:false

    }

    Label {
        id: label23
        x: 322
        y: 103
        width: 27
        height: 25
        text: qsTr("Логер")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }

    Label {
        id: label24
        x: 794
        y: 240
        width: 27
        height: 25
        visible: checkBoxAddSettings.checked?true:false

        text: qsTr("Логер сообщений сервера")
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
    }
}





/*##^##
Designer {
    D{i:0;formeditorZoom:1.25}
}
##^##*/

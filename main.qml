import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.10
import QtQuick.Layouts 1.7
import QtQuick.Dialogs 1.0
import QtCharts 2.0

import QtQuick.Shapes 1.11



Window {
    id: window
    flags:  Qt.Window
//    modality: Qt.WindowModal
    width: checkBoxAddSettings.checked?1200:800
    height: 600
    visible: true
    color: "#ffffff"
    title: qsTr("RC LimeSDR")

    Connections {
        target: appCore // Указываем целевое соединение
        onSendToQml: {
            if (FlagInit)
            {
                labelStatus.text = "";
            }
            else
            {
                /*if (Res>0)
                    labelStatus.text = "Успешно отправлена команда";*/
                 textAreaLog.append(array2display[0])
            }
        }
        onSendToQmlMsg:
        {
            if (flagConnect)
            {
                labelStatus.text = "Сonnected";
                labelStatus.color = "green"
                buttonCheckStatus.visible = true

            }
            else
            {
                labelStatus.text = "Disconnected";
                labelStatus.color = "red"
                buttonCheckStatus.visible = false


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


    Button {
        id: buttonConnect
        x: 23
        y: 55
        width: 117
        height: 28
        text: qsTr("Конект")
        font.pointSize: 9
        checkable: false
        autoRepeat: false
        highlighted: true
        visible: true
        onClicked: {

               appCore.tCPThread(textInputIP.text, textInputPort.text)

            }
    }

    Button {
        id: buttonSend
        x: 157
        y: 540
        width: 159
        height: 44

        text: qsTr("Послать команду")
        font.pointSize: 12
        checkable: false
        autoRepeat: false
        highlighted: true
        visible: true
        onClicked: {
            appCore.sendSomething2(textInputFilePath.text)


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
        y: 53
        width: 139
        height: 30

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
        value: 0.7
        stepSize: 0.05
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
        x: 365
        y: 37
        RadioButton {
            id: radioButtonTrans
        x: 34
        y: 352
        text: qsTr("ПРД")
        checked: true
        font.pointSize: 9
        }

    RadioButton {
        id: radioButtonRec
        x: 34
        y: 398
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
        x: 349
        y: 43
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
        color: "#ffffff"
        border.width: 2


        Flickable {
            id: flickable
            anchors.fill: parent
            anchors.leftMargin: 0

            TextArea.flickable: TextArea {
                id: textAreaLog
                x: 0
                y: 0
                width: 406
                height: 461
                text: qsTr("")
                wrapMode: TextArea.Wrap
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
        color: "#ffffff"
        border.width: 2

        Button {
            id: buttonOpenFile
            x: 216
            y: 0
            width: 45
            height: 25
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
        color: "#ffffff"
        border.width: 2
        Button {
            id: buttonOpenFileOut
            x: 216
            y: 0
            width: 45
            height: 25
            text: qsTr("...")
        }
    }

    TextInput {
        id: textInputFilePathOut
        x: 27
        y: 441
        width: 199
        height: 23
        text: qsTr("D:\\signalOutput.txt")
        font.pixelSize: 12
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.Wrap
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
        width: 98
        height: 17
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
        width: 153
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
        id: spinBoxFreq3
        x: 619
        y: 79
        width: 160
        height: 28
        value: 50
        editable: true
        font.pointSize: 9
        stepSize: 1
        to: 100
        from: 0
    }

    Label {
        id: label16
        x: 494
        y: 86
        text: qsTr("Воспроизвродить, с")
        font.pointSize: 9
    }

    CheckBox {
        id: checkBoxAddSettings
        x: 626
        y: 41
        width: 153
        height: 17
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
}



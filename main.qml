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
    width: 1080
    height: 720
    visible: true
    color: "#ffffff"
    title: qsTr("RC LimeSDR")


    Button {
        id: buttonConnect
        x: 20
        y: 20
        text: qsTr("Конект")
        checkable: false
        autoRepeat: false
        highlighted: true
        visible: true
        onClicked: {
                appCore.TCPThread()


            }
    }

    Button {
        id: buttonSend
        x: 20
        y: buttonConnect.y+buttonConnect.height+20
        text: qsTr("Послать команду")
        checkable: false
        autoRepeat: false
        highlighted: true
        visible: true
        onClicked: {
            appCore.SendSomething2(textInput.text)


        }
    }

    TextInput {
        id: textInput
        x: 150
        y: buttonConnect.y+buttonConnect.height+20
        width: 80
        height: 20
        text: "test message"
        font.pixelSize: 12
        wrapMode: Text.WordWrap
        persistentSelection: false
        renderType: Text.NativeRendering
        clip: false
    }

    Button {
        id: buttonDisconnect
        x: 20
        y: buttonSend.y+buttonSend.height+20
        text: qsTr("отсоединиться")
        checkable: false
        autoRepeat: false
        highlighted: true
        visible: true
        onClicked: {
            appCore.TCPstopTCP()


        }
    }

}

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.10
import QtQuick.Layouts 1.7
import QtQuick.Dialogs 1.0
import QtCharts 2.0

//import QtQuick.Shapes 1.11

Item { // size controlled by width
    id: root
Window
{
    flags:  Qt.Window //| Qt.WindowStaysOnBottomHint
//            modality: Qt.WindowModal

   color: "white"
//           activeFocusControl
   title: qsTr(qsTr("# SPEC"))
    width: 400
    height: 400
//    visible: true
    ChartView {
        id: graph
        x: 17
        y: 19
        width: 566
        height: 427
        SplineSeries {
            name: "SplineSeries"
            XYPoint {
                x: 0
                y: 1
            }

            XYPoint {
                x: 3
                y: 4.3
            }

            XYPoint {
                x: 5
                y: 3.1
            }

            XYPoint {
                x: 8
                y: 5.8
            }
        }
    }

}
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

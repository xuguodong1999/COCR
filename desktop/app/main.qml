import QtQuick 2.15
import QtQuick.Window 2.15
import SketchItem 1.0
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 640
    height: 480
    visibility: "Maximized"
    title: qsTr("Paint2D")
    color: "purple"
    // 模式1：pincharea+mousearea，支持旋转缩放拖曳
    // 模式2：native画板，支持单点书写
    // （impossible）模式3：multitouch，支持十指书写
    Component.onCompleted: {
        console.log("Component.onCompleted")
        sketch_item.init(width, height)
        sketch_item.update()
        sketch_item.penWidth = 5
        sketch_item.penColor = "white"
    }
    // 一次滑动操作会生成id不同的点，无法利用这个api进行多点绘图
    //    MultiPointTouchArea {
    //        enabled: true
    //        anchors.fill: parent
    //        anchors.margins: 10
    //        minimumTouchPoints: 1
    //        maximumTouchPoints: 5
    //        touchPoints: [
    //            TouchPoint {
    //                id: touch1
    //                onXChanged: {
    //                    console.log("1.getPos2D=", x, ",", y)
    //                }
    //            },
    //            TouchPoint {
    //                id: touch2
    //                onXChanged: {
    //                    console.log("2.getPos2D=", x, ",", y)
    //                }
    //            }
    //        ]
    //    }
    Rectangle {
        anchors.fill: parent
        anchors.margins: 10
        SketchItem {
            id: sketch_item
            enabled: true
            anchors.fill: parent
        }

        PinchArea {
            id: pinch_area
            enabled: false
            anchors.fill: parent
            onEnabledChanged: {
                mouse_area.enabled = enabled
            }
            onPinchStarted: {
                sketch_item.invokePinchStarted(pinch.angle, pinch.scale)
            }
            onPinchUpdated: {
                sketch_item.invokePinchUpdated(pinch.angle, pinch.scale)
            }
            onPinchFinished: {
                sketch_item.invokePinchFinished(pinch.angle, pinch.scale)
            }
            MouseArea {
                id: mouse_area
                enabled: false
                anchors.fill: parent
                onPressed: {
                    sketch_item.invokePressed(mouse.x, mouse.y)
                }
                onPositionChanged: {
                    sketch_item.invokePositionChanged(mouse.x, mouse.y)
                }
                onReleased: {
                    sketch_item.invokeReleased(mouse.x, mouse.y)
                }
            }
        }
        Button {
            id: btn1
            x: 10
            y: 10
            width: 70
            height: 70
            text: "绘制"
            checkable: true
            checked: true
            onClicked: {
                pinch_area.enabled = false
                sketch_item.enabled = true
                checked = true
                btn2.checked = false
            }
            background: Rectangle {
                radius: width / 4
            }
        }
        Button {
            id: btn2
            x: 10
            y: 90
            width: 70
            height: 70
            checkable: true
            checked: false
            background: Rectangle {
                radius: width / 4
            }

            text: "编辑"
            onClicked: {
                pinch_area.enabled = true
                sketch_item.enabled = false
                checked = true
                btn1.checked = false
            }
        }
    }
}

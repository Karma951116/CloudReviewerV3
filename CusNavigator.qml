import QtQuick 2.0
import Toou2D 1.0

// 导航标签
TNavigationBar {
    id: navigator
    signal indexChanged(var index)

    implicitWidth: 32
    implicitHeight: 360

    itemWidth: 64
    itemHeight: 32
    spacing: 24
    label.font: local_font.name
    label.color: "#45E8E8E8"
    activeLabel.color: "#E8E8E8"

    TNavigationElement{
        id: navigator_0
        text: qsTr("短信登录")
    }

    TNavigationElement{
        id: navigator_1
        text: qsTr("密码登录")
    }

    children: [
        TDividerLine {
            anchors.bottom: parent.bottom;
            width: navigator.currentItem.width;
            height: 2;
            x: navigator.currentItem.x;
            color: "#5767F9"

            Behavior on x {
                NumberAnimation {
                    easing.type: Easing.OutBack
                    duration: 50;
                }
            }
        }
    ]
    onTriggered: {
        indexChanged(modelData.index);
    }
}

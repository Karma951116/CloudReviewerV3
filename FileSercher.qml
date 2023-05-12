import QtQuick 2.0
import Toou2D 1.0

TInputField {
    id: asset_search
    implicitWidth: 840
    implicitHeight: 62

    placeholderIcon.source: TAwesomeType.FA_search
    placeholderLabel.text: qsTr("搜索")
    placeholderLabel.color: "#B4B7BD"
    placeholderIcon.color: "#B4B7BD"
    placeholderPosition: TPosition.Left
    background.color: "transparent"
    background.radius: 5
    label.color: "#B4B7BD"
    cursorColor: label.color
    border.width: 0

    MouseArea {
        anchors.fill: parent
        z: -1
        hoverEnabled: true
        onEntered: {
            //asset_search.scale = 1.1
            //asset_search.opacity = 1.0
        }
        onExited: {
            //asset_search.scale = 1.0
            //asset_search.opacity = 0.5
        }
    }
    Behavior on scale {
        PropertyAnimation {
            duration: 250
        }
    }
    Behavior on opacity {
        PropertyAnimation {
            duration: 250
        }
    }
}

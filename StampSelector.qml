import QtQuick 2.0
import QtQuick.Controls 2.14

Popup {
    id: stamp_popup
    implicitWidth: 400
    implicitHeight: 400
//    anchors.bottom: stamp_btn.top
//    anchors.bottomMargin: 30
    visible: false
    z: 4

    closePolicy: Popup.CloseOnEscape |
                 Popup.CloseOnPressOutside |
                 Popup.NoAutoClose

    background: Rectangle {
        anchors.fill: parent
        color: "#161D31"
        border.width: 1
        border.color: "#283046"
        radius: 5
    }

    contentItem: GridView {
        id: stamp_display
        implicitWidth: parent.width
        implicitHeight: parent.height
        clip: true
        cellWidth: 36
        cellHeight: 36
        ListModel {
            id: stamp_display_model
        }

        model: stamp_display_model

        delegate: Label {
            text: unicode
            font.pixelSize: 24
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    comment_input.insert(
                                comment_input.cursorPosition,
                                unicode);
                    stamp_popup.close();
                }
            }
        }
    }

    Component.onCompleted: {
        getModel();
    }


    function getModel() {
        var emojis = JSON.parse(runtimeCtx.getEmojis());
        for(var index in emojis) {
            var item = {"unicode" : emojis[index]}
            stamp_display_model.append(item);
        }
    }

    function openAt(posX, posY) {
        stamp_popup.x = posX;
        stamp_popup.y = posY;
        stamp_popup.open();
    }
}


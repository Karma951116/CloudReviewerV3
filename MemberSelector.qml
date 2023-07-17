import QtQuick 2.0
import QtQuick.Controls 2.0
import Toou2D 1.0

Popup {
    id: member_popup
    implicitWidth: 210
    implicitHeight: 230
    signal memberSelected(var selectedUser)

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

    contentItem: Item {
        width: 178
        height: 198
        anchors.centerIn: parent

        Text {
            id: prompt
            width: parent.width
            height: 20
            anchors.top: parent.top
            anchors.left: parent.left
            text: qsTr("选择要提醒的人，或直接输入")
            font.pixelSize: 14
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: "#676D7D"
        }

        ListView {
            id: members_list
            width: parent.width
            height: parent.height - prompt.height - members_list.topMargin
            anchors.top: prompt.bottom
            anchors.topMargin: 12
            spacing: 12
            clip: true

            model: stakeholderModel

            delegate: Rectangle {
                id: member_card
                width: parent.width
                height: 42
                color: "transparent"
                radius: 5

                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: 6
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 20
                    TAvatar {
                        width: height
                        height: 30
                        radius: width / 2
                        source: httpFunctions.getInternalImageUrl(imageUrl)

                    }
                    Text {
                        height: 30
                        text: name
                        font.pixelSize: 14
                        font.family: local_font.name
                        font.weight: Font.DemiBold
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        member_card.color = "#283046";
                    }
                    onExited: {
                        member_card.color = "transparent";
                    }
                    onClicked: {
                        var selectedUser = {
                            "id": uuid,
                            "name": name
                        }
                        memberSelected(selectedUser);
                        member_popup.close();
                    }
                }
            }
        }
    }

    onOpened: {
        member_popup.y -= height;
        httpFunctions.getFileStakeHolder(runtimeCtx.getAuditContentUuid());
    }
    onClosed: {
        member_popup.y += height;
    }
    function openAt(posX, posY) {
        member_popup.x = posX;
        member_popup.y = posY;
        member_popup.open();
    }
}

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Shapes 1.14
import Toou2D 1.0

TRectangle {
    id: comment_node
    property int commentRow;
    property bool hovered :false
    implicitWidth: 22
    implicitHeight: 24
    anchors.verticalCenter: parent.verticalCenter
    color: "transparent"

    TRectangle {
        id: comment_range_bar
        width: parent.width
        height: 4
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        radius: 2
        color: "#5767F9"
        border.width: comment_node.hovered ? 1 : 0
        border.color: "#FFFFFF"
    }
    TAvatar {
        id: comment_avatar
        width: 22
        height: 22
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        radius: width / 2
        source: httpFunctions.getInternalImageUrl(sender["image"])
        border.width: comment_node.hovered ? 1 : 0
        border.color: "#FFFFFF"
    }
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            commentModel.setHoveredComment(commentModel.getRoleValue(commentRow, "commentUuid"));
            comment_node.z += 1;
            var x = comment_avatar.width / 2 - comment_node_popup.width / 2;
            var y = -(20 + comment_node_popup.height);
            openAt(x, y);
        }
        onExited: {
            commentModel.setHoveredComment("");
            comment_node.z -= 1;
            comment_node_popup.close();
        }
        onClicked: {

        }
    }

    Popup {
        id: comment_node_popup
        width: 230
        height: 76
        closePolicy: Popup.NoAutoClose
        background: Shape {
            id: comment_node_popup_bg
            width: 230
            height: 76
            anchors.centerIn: parent
            ShapePath {
                strokeWidth: 0
                strokeStyle: ShapePath.SolidLine
                strokeColor: "transparent"
                startX: 0
                startY: 0
                PathLine { x: 230; y: 0 }
                PathLine { x: 230; y: 70 }
                PathLine { x: 130; y: 70 }
                PathLine { x: 115; y: 76 }
                PathLine { x: 100; y: 70 }
                PathLine { x: 0; y: 70 }
                fillColor: Qt.rgba(0, 0, 0, 0.85)
            }
        }
        contentItem: Item {
            id: comment_node_popup_content
            width: 206
            height: 46
            anchors.top: comment_node_popup_bg.top
            anchors.topMargin: 12
            anchors.left: comment_node_popup_bg.left
            anchors.leftMargin: 12

            TAvatar {
                id: user_avatar
                width: 24
                height: 24
                anchors.top: parent.top
                anchors.left: parent.left
                source: utils.getHttpImageStr(config_helper.ImageIp,
                                              comment["commentSendUser"]["image"])
            }

            Text {
                id: user_name
                width: 114
                height: 18
                anchors.top: parent.top
                anchors.left: user_avatar.right
                anchors.leftMargin: 12
                text: {
                    var sender = commentModel.getRoleValue(commentRow, "sender");
                    return sender["name"];
                }
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
                font.family: local_font.name
                font.weight: Font.DemiBold
                color: "#B9B9C3"
            }

            Text {
                id: comment_time
                width: 44
                height: 18
                anchors.top: parent.top
                anchors.left: user_name.right
                anchors.leftMargin: 12
                text: ""

                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
                font.family: local_font.name
                font.weight: Font.DemiBold
                color: "#676D7D"
                Timer {
                    interval: 60000
                    repeat: true
                    running: true
                    triggeredOnStart: true
                    onTriggered: {
                        comment_time.text =
                                formatTransformer.timestamp2RelTime(commentModel.getRoleValue(commentRow, "createDate"));
                    }
                }
            }
            Text {
                id: comment_timestamp
                width: 50
                height: 18
                anchors.top: user_avatar.bottom
                anchors.topMargin: 6
                anchors.left: parent.left
                anchors.leftMargin: 12

                text: commentModel.getRoleValue(commentRow, "start")
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: "#5767F9"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        workstation_page.commentTimeStampClicked(
                                    comment["start"],
                                    comment["printscreen"]);
                    }
                }
            }
            Text {
                id: comment_content
                width: 140
                height: 18
                anchors.top: user_avatar.bottom
                anchors.topMargin: 6
                anchors.left: comment_timestamp.right
                anchors.leftMargin: 16
                anchors.verticalCenter: comment_timestamp.verticalCenter
                text: commentModel.getRoleValue(commentRow, "details")
                color: "#FFFFFF"
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.DemiBold
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }
        }
    }
    function openAt(pos_x, pos_y) {
        comment_node_popup.x = pos_x;
        comment_node_popup.y = pos_y;
        comment_node_popup.open();
    }

    Connections {
        target: commentModel
        onHoveredCommentChanged: {
            hovered = commentModel.getRoleValue(commentRow, "commentUuid") === commentModel.hoveredComment();
        }
    }
    Connections {
        target: comment_timeline
        onEditModeChanged: {
            if (comment_timeline.editMode) {
                comment_node.visible = false;
            }
            else {
                comment_node.visible = true;
            }
        }
        onClearNodes: {
            comment_node.destroy();
        }
    }
}


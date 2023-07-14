import QtQuick 2.0
import QtQuick.Controls 2.0
import Toou2D 1.0
import SortFilterProxyModel 0.2

TRectangle {
    id: comment_card
    property bool hasSubComment: sub_comments.count > 0 ?
                                     true : false
    property bool hovered: false

    width: 344
    height: {
        return content.height + sub_comments.contentHeight + 32 + 10
    }
    color: hovered ? "#5767F9" : "#30384D"
    radius: 5

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        z: 3
        onEntered: {
            //hovered = true;
            commentModel.setHoveredComment(commentUuid);
        }
        onExited: {
            //hovered = false;
            commentModel.setHoveredComment("");
        }
        onClicked: {
            mouse.accepted = false;
        }
    }

    TRectangle {
        id: content
        width: 312
        height: {
            return user_info_row.height +
                    middle_col.height +
                    comment_timestamp.height + 5
        }

        anchors.top: comment_card.top
        anchors.topMargin: 16
        anchors.left: comment_card.left
        anchors.leftMargin: 16
        z: 2
        color: "transparent"
        Row {
            id: user_info_row
            height: 18
            anchors.top: parent.top
            anchors.left: parent.left
            spacing: 12
            TAvatar {
                id: user_avatar
                width: 24
                height: 24
                source: utils.getHttpImageStr(config_helper.ImageIp,
                                              commentSendUser["image"])
            }

            Text {
                id: user_name
                height: 18
                text: sender["name"]
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
                font.family: local_font.name
                font.weight: Font.DemiBold
                color: comment_card.hovered ? "#FFFFFF" : "#B9B9C3"
            }

            TDividerLine {
                width: 1
                height: 18
                color: comment_card.hovered ? "#2DFFFFFF" : "#3B4253"
            }

            Text {
                id: comment_time
                height: 18
                text: formatTransformer.timestamp2RelTime(createDate)
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
                font.family: local_font.name
                font.weight: Font.DemiBold
                color: comment_card.hovered ? "#41FFFFFF" : "#676D7D"
            }
        }

        Column {
            id: middle_col
            width: parent.width
            height: {
                return comment_content.height + comment_printscreen.height
            }

            anchors.top: user_info_row.bottom
            anchors.left: parent.left
            TextArea {
                id: comment_content
                width: content.width
                height: contentHeight > 60 ? contentHeight : 60
                text: details
                readOnly: true
                color: comment_card.hovered ? "#FFFFFF" : "#B4B7BD"
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.DemiBold
                wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
            }
            Image {
                id: comment_printscreen
                width: 64
                height: visible ? 36 : 0
                source: ""
                Component.onCompleted: {
                    if (printscreen) {
                        comment_printscreen.source =
                                utils.getHttpImageStr(config_helper.ImageIp,
                                                      printscreen);
                    }
                    else {
                        comment_printscreen.visible = false;
                    }
                }
            }
        }
        Label {
            id: comment_timestamp
            anchors.top: middle_col.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            width: 38
            height: 20
            text: start
            font.pixelSize: 14
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            color: comment_card.hovered ? "#41FFFFFF" : "#5767F9"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    workstation_page.commentTimeStampClicked(start, printscreen);
                }
            }
        }
        TIconButton {
            id: comment_delete
            anchors.top: middle_col.bottom
            anchors.topMargin: 5
            anchors.right: parent.right
            width: 20
            height: 20
            icon.position: TPosition.Only
            icon.source: "qrc:/icon/comment_delete.svg"
            icon.color: comment_card.hovered ? "#FFFFFF" : "#676D7D"
            background.color: "transparent"
            border.width: 0
        }
        TIconButton {
            id: comment_reply

            anchors.top: middle_col.bottom
            anchors.topMargin: 5
            anchors.right: comment_delete.left
            anchors.rightMargin: 20
            width: 20
            height: 20
            icon.position: TPosition.Only
            icon.source: "qrc:/icon/comment_new.svg"
            icon.color: comment_card.hovered ? "#FFFFFF" : "#676D7D"
            background.color: "transparent"
            border.width: 0
            onClicked: {
                comment_editor.commentReplyClicked(sender, commentUuid);
            }
        }
    }
    CommentSubItem {
        id: sub_comments
        anchors.top: content.bottom
        anchors.topMargin: 10
        anchors.left: content.left
        anchors.leftMargin: 20
        hovered: comment_card.hovered
    }

    Connections {
        target: commentModel
        onHoveredCommentChanged: {
            hovered = commentUuid === commentModel.hoveredComment();
        }
    }
}

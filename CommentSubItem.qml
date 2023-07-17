import QtQuick 2.0
import QtQuick.Controls 2.0
import Toou2D 1.0
import SortFilterProxyModel 0.2

ListView {
    //id: sub_comments

    property bool hovered: false
    visible: sub_comment_model.count > 0 ?
                 true : false
    ListModel {
        id: sub_comment_model
    }
    model: sub_comment_proxy_model

    SortFilterProxyModel {
        id: sub_comment_proxy_model
        sourceModel: sub_comment_model
        sorters: [
            StringSorter {
                roleName: "childCreateDate"
                sortOrder: Qt.AscendingOrder
            }
        ]
    }
    delegate: TRectangle {
        id: sub_comment
        width: 292
        height: 126
        color: "transparent"
        TDividerLine {
            width: parent.width
            height: 1
            anchors.top: parent.top
            color: hovered ? "#FFFFFF" : "#3B4253"
        }
        Row {
            id: sub_user_info_row
            anchors.top: parent.top
            anchors.topMargin: 16
            anchors.left: parent.left
            spacing: 12
            TAvatar {
                id: sub_user_avatar
                width: 24
                height: 24
                source: httpFunctions.getInternalImageUrl(childSender["image"])
            }

            Text {
                id: sub_user_name
                height: 18
                text: childSender["name"]
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.DemiBold
                verticalAlignment: Text.AlignVCenter
                color: hovered ? "#FFFFFF" : "#B9B9C3"
            }

            TDividerLine {
                width: 1
                height: 15
                color: hovered ? "#2DFFFFFF" : "#3B4253"
            }

            Text {
                id: sub_comment_time
                height: 18
                text: formatTransformer.timestamp2RelTime(childCreateDate)
                font.pixelSize: 12
                font.family: local_font.name
                font.weight: Font.DemiBold
                verticalAlignment: Text.AlignVCenter
                color: hovered ? "#41FFFFFF" : "#676D7D"
            }
        }

        TextArea {
            id: sub_comment_content
            width: sub_comment.width
            height: 60
            anchors.top: sub_user_info_row.bottom
            anchors.left: parent.left
            readOnly: true
            text: childDetails
            font.pixelSize: 14
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: hovered ? "#FFFFFF" : "#B4B7BD"
            wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
        }

        TIconButton {
            id: sub_comment_delete
            anchors.top: sub_comment_content.bottom
            anchors.topMargin: 5
            anchors.right: parent.right
            width: 20
            height: 20
            icon.position: TPosition.Only
            icon.source: "qrc:/icon/comment_delete.svg"
            icon.color: hovered ? "#FFFFFF" : "#676D7D"
            background.color: "transparent"
            border.width: 0
        }

        TIconButton {
            anchors.top: sub_comment_content.bottom
            anchors.topMargin: 5
            anchors.right: sub_comment_delete.left
            anchors.rightMargin: 20
            id: sub_comment_reply
            width: 20
            height: 20
            icon.position: TPosition.Only
            icon.source: "qrc:/icon/comment_new.svg"
            icon.color: hovered ? "#FFFFFF" : "#676D7D"
            background.color: "transparent"
            border.width: 0
            onClicked: {
                comment_editor.commentReplyClicked(childSender, childParent);
            }
        }
    }
    Component.onCompleted: {
        for (var itemIdx in childrens) {
            sub_comment_model.append(childrens[itemIdx]);
        }
    }
}

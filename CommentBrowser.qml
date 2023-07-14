import QtQuick 2.0
import SortFilterProxyModel 0.2
import QtQuick.Controls 2.14
import Toou2D 1.0

Item {
    id: comment_display_panel
    implicitWidth: 360
    implicitHeight: 1018

    CommentBrowserToolbar {
        id: comment_toolbar
        width: 360
        height: 48
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ListView {
        id: comment_display
        property var currentComments;
        width: 360
        height: 970
        anchors.top: comment_toolbar.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        clip: true
        spacing: 16

        SortFilterProxyModel {
            id: main_comment_proxy_model
            sourceModel: commentModel
            filters: [
                RegExpFilter {
                    roleName: "commentDetails"
                    pattern: "^@" + clientUser.userName + " "
                    enabled: comment_toolbar.atMe
                }
            ]
            sorters: [
                StringSorter {
                    roleName: "createDate"
                    sortOrder: comment_toolbar.commentSortIndex == 0 ?
                                   Qt.AscendingOrder : Qt.DescendingOrder
                }
            ]
        }
        model: main_comment_proxy_model

        delegate: CommentItem {
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}



import QtQuick 2.0
import Toou2D 1.0
import SortFilterProxyModel 0.2

import "qrc:/Widget"

Rectangle {
    id: file_browser
    implicitWidth: 1660
    implicitHeight: 1018
    color: "#161D31"
    visible: runtimeCtx.projectName === "" ? false : true

    FileBrowseToolbar {
        id: file_toolbar
        width: 1604
        height: 38
        anchors.top: parent.top
        anchors.topMargin: 28
        anchors.horizontalCenter: parent.horizontalCenter
    }

    GridView {
        id: file_displayer
        property var checkedFiles: []
        width: 1628
        height: 936
        anchors.top: file_toolbar.bottom
        anchors.topMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
        cellWidth: 324
        cellHeight: 266

        SortFilterProxyModel {
            id: files_proxy_model
            sourceModel: fileModel
            sorters: [
                FilterSorter {
                    RegExpFilter {
                        roleName: "name"
                        pattern: file_sercher.text
                        caseSensitivity: Qt.CaseInsensitive
                    }
                    sortOrder: Qt.AscendingOrder
                    enabled: file_sercher.text.length == 0 ?
                                 false : true
                },
                StringSorter {
                    roleName: "name"
                    sortOrder: Qt.AscendingOrder
                    enabled: file_sercher.text.length == 0 ?
                                 false : true
                },
                StringSorter {
                    roleName: "createDate"
                    sortOrder: file_toolbar.sortMode === 0 ?
                                   Qt.AscendingOrder : Qt.DescendingOrder
                    priority: file_sercher.text.length === 0 ?
                                  1 : 0
                }
            ]
        }
        model: files_proxy_model

        delegate: FileBrowserItem {
            width: 300
            height: 242
        }
    }
    Connections {
        target: replyParser
        onReviewStatusReplyDone: function(success, msg) {
            if (success) {
                TToast.showSuccess("修改成功" ,
                                   TTimePreset.ShortTime2s,
                                   msg);
            }
            else {
                TToast.showError("修改失败" ,
                                   TTimePreset.ShortTime2s,
                                   msg);
            }
            fileModel.uncheckAll();
        }
    }
}

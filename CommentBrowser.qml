import QtQuick 2.0
import SortFilterProxyModel 0.2

ListView {
    id: workstation_comment_display
    property var currentComments;

    clip: true
    spacing: 16
    ListModel {
        id: main_comment_model
    }

    SortFilterProxyModel {
        id: main_comment_proxy_model
        sourceModel: main_comment_model
        filters: [
            RegExpFilter {
                roleName: "commentDetails"
                pattern: "^@" + global_prop.userData["username"] + " "
                enabled: workstation_at_me_btn.checked
            }
        ]
        sorters: [
            StringSorter {
                roleName: "createDate"
                sortOrder: workstation_comment_sort.currentIndex == 0 ?
                               Qt.AscendingOrder : Qt.DescendingOrder
            }
        ]
    }
    model: main_comment_proxy_model

//    delegate: Card_comment {
//        anchors.horizontalCenter: parent.horizontalCenter
//    }

    Component.onCompleted: {
        request_tools.getFileInfo(global_prop.auditFileFolderUuid)
    }
    Connections {
        target: request_tools
        onDataBack: {
            if (usageType === HttpRequestTool.FILE_INFO_QUERY ||
                    usageType === HttpRequestTool.REFRESH_COMMENT) {
                main_comment_model.clear();
                var return_data = JSON.parse(data);
                var commentFile = return_data["commentFile"];
                workstation_comment_display.currentComments = commentFile;
                for (var comment_idx in commentFile) {
                    main_comment_model.append(commentFile[comment_idx]);
                }
                // 准备推流
                if (usageType === HttpRequestTool.FILE_INFO_QUERY){
                    var fileFolder = return_data["auditFileFolder"];
                    var fileFolderUuid = fileFolder["auditFileFolderUuid"];
                    var netPath = fileFolder["netDiskPath"];
                    request_tools.getRtspFile(fileFolderUuid, netPath);
                }
            }
            if (usageType === HttpRequestTool.GET_RTSP_FILE) {
                return_data = JSON.parse(data);
                if (return_data["code"] === 200) {
                    global_prop.currentPid = return_data["result"]["pid"];
                }
                else {
                    TToast.showWarning(return_data["message"], TTimePreset.LongTime4s);
                }
            }
        }
    }
}

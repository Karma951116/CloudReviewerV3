import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Shapes 1.14
import Toou2D 1.0
//import ReplyParser 1.0
import FileListModel 1.0

Rectangle {
    id: grid_item
    property string suffix: fileSuffix
    property int type: fileType
    implicitWidth: 300
    implicitHeight: 242
    color: "#283046"
    radius: 5
    clip: true

    Image {
        id: bg
        width: 300
        height: 152
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        source: {
            if (grid_item.suffix === "other") {
                return "qrc:/icon/file_image_other";
            }
            else if (grid_item.suffix === "folder") {
                return "qrc:/icon/folder_icon";
            }

            else if (grid_item.suffix === "audio") {
                return "qrc:/icon/file_image_audio";
            }

            else {
                // 获取网络图像
                fileImageUrl;
                //httpFunctions.getNetworkImage(fileImageUrl, uuid);
            }
        }
    }
    // 文件类型标识
    TRectangle {
        width: file_duration.visible ? 58 : 22
        height: 20
        anchors.bottom: bg.bottom
        anchors.bottomMargin: 8
        anchors.left: bg.left
        anchors.leftMargin: 8
        color: "#40000000"
        radius: 5
        visible: {
            if (grid_item.type === 2) {
                return false;
            }
            if (grid_item.suffix === "other") {
                return false;
            }
            else {
                return true;
            }
        }
        Image {
            id: filetype_icon
            width: 10
            height: 10
            anchors.left: parent.left
            anchors.leftMargin: 6
            anchors.verticalCenter: parent.verticalCenter
            source: {
                if (grid_item.suffix === "video") {
                    return "qrc:/icon/file_type_video";
                }
                else if (grid_item.suffix === "audio") {
                    return "qrc:/icon/file_type_audio";
                }
                else if (grid_item.suffix === "picture") {
                    return "qrc:/icon/file_type_image";
                }
                else {
                    return "";
                }
            }
        }
        Text {
            id: file_duration
            anchors.left: filetype_icon.right
            anchors.leftMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            text: duration
            font.pixelSize: 12
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: "#D0D2D6"
            visible: grid_item.suffix === "picture" ? false : true
        }
    }
    CheckBox {
        id: file_checkbox
        anchors.top: bg.top
        anchors.topMargin: 12
        anchors.right: bg.right
        anchors.rightMargin: 12
        width: 24
        height: 24
        checked: fileModel.checkedListContains(index)
        visible: fileModel.checkedListContains(index)
        indicator: TRectangle {
            implicitWidth: 24
            implicitHeight: 24
            x: 0
            y: 0
            radius: 3
            color: "transparent"
            border.color: "#5767F9"
            border.width: 2
            TRectangle {
                anchors.fill: parent
                radius: 3
                color: "#5767F9"
                visible: file_checkbox.checked
                Shape {
                    id: indicator_shape
                    anchors.fill: parent
                    ShapePath {
                        strokeWidth: 4
                        strokeColor: "white"
                        fillColor: "transparent"

                        capStyle: ShapePath.RoundCap
                        property int joinStyleIndex: 0

                        property variant styles: [
                            ShapePath.BevelJoin,
                            ShapePath.MiterJoin,
                            ShapePath.RoundJoin
                        ]

                        joinStyle: styles[joinStyleIndex]
                        startX: 0
                        startY: indicator_shape.height / 2
                        PathLine {
                            x: indicator_shape.width / 2
                            y: indicator_shape.height
                        }
                        PathLine {
                            x: indicator_shape.width
                            y: 0
                        }
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (file_checkbox.checked) {
                    fileModel.removeCheckedList(index);
                }
                else {
                    fileModel.insertCheckedList(index);
                }
            }
            onDoubleClicked: {

            }
        }
    }

    Item {
        anchors.top: bg.bottom
        anchors.topMargin: 16
        anchors.left: parent.left
        anchors.leftMargin: 16
        Row {
            id: name_row
            anchors.top: parent.top
            anchors.left: parent.left
            Text {
                id: file_name
                text: name
                font.pixelSize: 16
                font.family: local_font.name
                font.weight: Font.Bold
                color: "#D0D2D6"
            }
            Item {
                width: 268 - file_name.implicitWidth - review_state_bg.width
                height: parent.height
            }
            TRectangle {
                id: review_state_bg
                width: review_state.implicitWidth + 10
                height: 20
                radius: 17
                color: "#0C5767F9"
                Text {
                    id: review_state
                    anchors.centerIn: parent
                    text: status === "0" ? "待审核" :
                          status === "1" ? "审核中" :
                          status === "2" ? "审核通过" :
                          status === "3" ? "审核不通过" : ""

                    font.pixelSize: 12
                    font.family: local_font.name
                    font.weight: Font.Bold
                    onTextChanged: {
                        if (review_state.text === "审核中") {
                            review_state.color = "#00CFE8";
                            review_state_bg.color = "#0C00CFE8";
                        }
                        else if (review_state.text === "审核通过") {
                            review_state.color = "#28C76F";
                            review_state_bg.color = "#0C28C76F";
                        }
                        else if (review_state.text === "审核不通过") {
                            review_state.color = "#EA5455";
                            review_state_bg.color = "#0CEA5455";
                        }
                        else if (review_state.text === "待审核") {
                            review_state.color = "#5767F9";
                            review_state_bg.color = "#0C5767F9";
                        }
                    }
                }
            }
        }
        Row {
            id: info_row
            anchors.top: name_row.bottom
            anchors.left: parent.left
            spacing: 12
            Text {
                id: file_size
                text:  grid_item.type === 2 ? "—— ——" : fileSize
                font.pixelSize: 12
                font.family: local_font.name
                font.weight: Font.DemiBold
                color: "#676D7D"
            }
            TDividerLine {
                width: 1
                height: parent.height
                color: "#3B4253"
            }
            Text {
                id: file_create_time
                text: createDate
                font.pixelSize: 12
                font.family: local_font.name
                font.weight: Font.DemiBold
                color: "#676D7D"
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        onEntered: {
            file_checkbox.visible = true;
        }
        onExited:  {
            if (!file_checkbox.checked) {
                file_checkbox.visible = false;
            }
        }
        onClicked: {
            if ((mouseX > file_checkbox.x &&
                mouseX < file_checkbox.x + file_checkbox.width) &&
                (mouseY > file_checkbox.y &&
                mouseY < file_checkbox.y + file_checkbox.height)) {
                    mouse.accepted = false;
            }
        }
        onDoubleClicked: {
            if (isTask) {
                fileModel.updateList(FileListModel.TASK,
                                     auditContentUuid);
            }
            else if (fileType === 2) {
                fileModel.updateList(FileListModel.FOLDER,
                                     auditContentUuid, auditFileFolderUuid);
            }
            else {
                runtimeCtx.setAuditContentUuid(auditContentUuid);
                runtimeCtx.setVersionUuid(versionUuid);
                runtimeCtx.setAuditFileFolderUuid(auditFileFolderUuid);
                runtimeCtx.setFileName(name);
                runtimeCtx
                root.goReview();
                httpFunctions.getFileInfo(auditFileFolderUuid);
            }
        }
    }

    Connections {
        target: replyParser
        onNetworkImageReplyDone: function(state, image, id){
            if (id !== uuid) {
                return;
            }
            else {
                bg.source = image;
            }
        }
    }
    Connections {
        target: fileModel
        onCheckedListChanged: {
            var isChecked = fileModel.checkedListContains(index);
            file_checkbox.checked = isChecked;
            file_checkbox.visible = isChecked;
        }
    }
}

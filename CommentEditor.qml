import QtQuick 2.0
import QtQuick.Controls 2.14
import Toou2D 1.0
import Qt.labs.platform 1.1
import RuntimeContext 1.0


Item {
    id: editor
    implicitWidth: 1280
    implicitHeight: 160

    property var bgColor: bg.color
    property var bgRadius: bg.radius
    signal paintToolsClicked();
    signal commentReplyClicked(var receiveUser, var commentParent)

    Rectangle {
        id: bg
        anchors.fill: parent
        color: "#283046"
        radius: 5
    }

    TextArea {
        id: comment_input
        property var at_set: []
        property string time;
        width: 1248
        height: 98
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        placeholderText: qsTr("在此填写批注")
        font.pixelSize: 14
        color: "white"
        wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
        selectByKeyboard: true
        selectByMouse: true
        selectionColor:"blue"
        background: Rectangle {
            implicitWidth: 1248
            implicitHeight: 78
            color: "transparent"
            border.color: "#3B4253"
            border.width: 2
            radius: 5
        }
        Keys.onPressed: {
            if (event.key === Qt.Key_At) {
                if (member_selector.opened) {
                    member_selector.close();
                }
                var global_coord = mapToItem(editor, 0, 0);
                member_selector.openAt(
                            global_coord.x + contentWidth,
                            global_coord.y + contentHeight -
                            20);
                event.accepted = true;
            }

            if (event.key !== Qt.Key_Backspace) {
                return;
            }

            if (text.charAt(cursorPosition - 1) === " ") {
                var last_at_index = text.lastIndexOf("@");
                // 得到不携带@和尾部空格的字符串
                var test_str = text.substring(last_at_index + 1, cursorPosition - 1);
                //var test_index = comment_input.at_set.indexOf(test_str);
                for(var user_idx in at_set) {
                    var user = at_set[user_idx];
                    if (test_str === user["name"]) {
                        comment_input.remove(last_at_index + 1, cursorPosition)
                        comment_input.at_set.pop(user);
                        break;
                    }
                }
            }
            else if (text.charAt(cursorPosition - 1) === ":") {
                last_at_index = text.lastIndexOf("回复@");
                // 得到不携带@和尾部空格的字符串
                test_str = text.substring(last_at_index + 3, cursorPosition - 1);
                if (test_str === send_comment_btn.curretReceiver["name"]) {
                    comment_input.remove(last_at_index + 1, cursorPosition);
                    send_comment_btn.curretReceiver = null;
                    send_comment_btn.curretCommentParent = null;
                }
            }
        }
        onFocusChanged: {
            if (focus) {
                video_player.pause();
                comment_input.time =
                        formatTransformer.sec2Time(video_player.currentTime.toFixed(0));
            }
        }
    }
    ListView {
        id: refer_repeater
        width: 1248
        height: 42
        anchors.bottom: comment_input.bottom
        anchors.left: comment_input.left
        model: refer_btn.refers
        delegate: CommentReference {
        }
        z:2
        visible: refer_model.count == 0 ? false : true
        clip: true
    }

    Item {
        width: 1248
        height: 30
        anchors.top: comment_input.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter

        TImageButton {
            id: at_btn
            width: 20
            height: 20
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/icon/at_btn"
            onClicked: {
                var global_coord = mapToItem(editor, 0, 0);
                if (!member_selector.opened) {
                    member_selector.openAt(global_coord.x - member_selector.width / 2,
                                           global_coord.y - 10);
                }
                else {
                    member_selector.close();
                }
            }
        }

        TImageButton {
            id: refer_btn
            property alias refers: refer_model
            width: 20
            height: 20
            anchors.left: at_btn.right
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/icon/reference_btn"
            onClicked: {
                video_player.pause();
                refer_pick_dialog.open();
            }
            ListModel {
                id: refer_model
            }
        }
        FileDialog {
            id: refer_pick_dialog
            title: "请选择附件"
            nameFilters: ["Pictures (*.png *.jpg *.jpeg)"]
            onAccepted: {
                var path = refer_pick_dialog.file.toString();
                //path = path.replace("file:///", "");
                var item = {"filePath": path};
                refer_btn.refers.append(item);
            }
        }
        TImageButton {
            id: stamp_btn
            width: 20
            height: 20
            anchors.left: refer_btn.right
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/icon/stamp_btn"
            onClicked: {
                var global_coord = mapToItem(editor, 0, 0);
                if (!stamp_selector.opened) {
                    stamp_selector.openAt(global_coord.x - stamp_selector.width / 2,
                                          global_coord.y - stamp_selector.height - 30);
                }
                else {
                    stamp_selector.close();
                }
            }
        }

        TButton {
            id: send_comment_btn
            property var curretReceiver: null
            property var curretCommentParent: null
            width: 80
            height: 30
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            label.text: qsTr("发送")
            label.font.pixelSize: 12
            label.font.weight: Font.Bold
            label.font.family: local_font.name
            label.color: "#FFFFFF"
            background.color: "#5767F9"
            border.width: 0
            onClicked: {
                var stakeholder_uuid_arr = [];
                for (var idx in comment_input.at_set) {
                    stakeholder_uuid_arr.push(comment_input.at_set[idx]["id"])
                }
                var printscreen;
                printscreen = paint_canvas.hasDraws() ?
                                  formatTransformer.image2Base64(paint_canvas.getImage()) : "";
                var attachment = [];
                for (var i = 0; i < refer_btn.refers.count; i++) {
                    var filePath = refer_btn.refers.get(i)["filePath"];
                    if(filePath === "") {
                        continue;
                    }
                    filePath = filePath.replace("file:///", "");
                    attachment.push(formatTransformer.image2Base64FromLocalFile(filePath));
                }
                var start = comment_timeline.start === null ?
                            comment_input.time :
                            comment_timeline.start;
                var end = comment_timeline.end === null ?
                            "" : comment_timeline.start;
                httpFunctions.postSendComment(runtimeCtx.auditFileFolderUuid, curretCommentParent,
                                              clientUser.uuid, curretReceiver,
                                              stakeholder_uuid_arr, comment_input.text,
                                              "2", start, end,
                                              printscreen, attachment);
            }
        }

        Row {
            id: comment_tools_row
            anchors.right: send_comment_btn.left
            anchors.rightMargin: 28
            anchors.verticalCenter: parent.verticalCenter
            spacing: 20
            TImageButton {
                id: clear_btn
                width: 28
                height: 28

                source: "qrc:/icon/canvas_clear"
                onClicked: {
                    paintToolsClicked();
                    paint_canvas.clear();
                }

            }
            TImageButton {
                id: undo_btn
                width: 28
                height: 28

                source: "qrc:/icon/canvas_undo"
                onClicked: {
                    paintToolsClicked();
                    paint_canvas.undo();
                }
            }
            TImageButton {
                id: redo_btn
                width: 28
                height: 28

                source: "qrc:/icon/canvas_redo"
                onClicked: {
                    paintToolsClicked();
                    paint_canvas.redo();
                }
            }
            TRectangle {
                id: green_btn
                width: 28
                height: 28
                radius: width / 2
                border.width: 0
                border.color: "white"
                color: "#6DD0A0"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        paintToolsClicked();
                        paint_canvas.setPaintColor("#6DD0A0");
                    }
                    onFocusChanged: {
                        if (green_btn.focus) {
                            green_btn.border.width = 5;
                        }
                        else {
                            green_btn.border.width = 0;
                        }
                    }
                }

            }
            TRectangle {
                id: blue_btn
                width: 28
                height: 28
                radius: width / 2
                border.width: 0
                border.color: "white"
                color: "#79C0EB"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        paintToolsClicked();
                        paint_canvas.setPaintColor("#79C0EB");
                    }
                    onFocusChanged: {
                        if (blue_btn.focus) {
                            blue_btn.border.width = 5;
                        }
                        else {
                            blue_btn.border.width = 0;
                        }
                    }
                }

            }
            TRectangle {
                id: yellow_btn
                width: 28
                height: 28
                radius: width / 2
                border.width:  0
                border.color: "white"
                color: "#F4C143"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        paintToolsClicked();
                        paint_canvas.setPaintColor("#F4C143");
                    }
                    onFocusChanged: {
                        if (yellow_btn.focus) {
                            yellow_btn.border.width = 5;
                        }
                        else {
                            yellow_btn.border.width = 0;
                        }
                    }
                }

            }
            TRectangle {
                id: red_btn
                width: 28
                height: 28
                radius: width / 2
                border.width: 0
                border.color: "white"
                color: "#D8524C"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        paintToolsClicked();
                        paint_canvas.setPaintColor("#D8524C");
                    }
                    onFocusChanged: {
                        if (red_btn.focus) {
                            red_btn.border.width = 5;
                        }
                        else {
                            red_btn.border.width = 0;
                        }
                    }
                }
            }
            TImageButton {
                id: paint_tool
                width: 28
                height: 28

                source: "qrc:/icon/canvas_pen"
                onClicked: {
                    paintToolsClicked();
                    paint_canvas.setPaintMode(0);
                }
                TRectangle {
                    width: parent.width
                    height: parent.height
                    radius: width / 2
                    color: "#5767F9"
                    anchors.centerIn: parent
                    z: -1
                    visible: parent.focus ? true : false;
                }
            }
            TImageButton {
                id: arrow_tool
                width: 28
                height: 28

                source: "qrc:/icon/canvas_arrow"
                onClicked: {
                    paintToolsClicked();
                    paint_canvas.setPaintMode(3);
                }
                TRectangle {
                    width: parent.width
                    height: parent.height
                    radius: width / 2
                    color: "#5767F9"
                    anchors.centerIn: parent
                    z: -1
                    visible: parent.focus ? true : false;
                }
            }
            TImageButton {
                id: rectangle_tool
                width: 28
                height: 28

                source: "qrc:/icon/canvas_rect"
                onClicked: {
                    paintToolsClicked();
                    paint_canvas.setPaintMode(1);
                }
                TRectangle {
                    width: parent.width
                    height: parent.height
                    radius: width / 2
                    color: "#5767F9"
                    anchors.centerIn: parent
                    z: -1
                    visible: parent.focus ? true : false;
                }
            }
        }
    }
    MemberSelector {
        id: member_selector
        width: 210
        height: 230
        z: 3
    }
    StampSelector {
        id: stamp_selector
        width: 400
        height: 400
        z: 3
    }

    Connections {
        target: member_selector
        onMemberSelected: {
            comment_input.at_set.push(selectedUser);
            var at_str = "@" + selectedUser["name"] + " ";
            comment_input.insert(
                        comment_input.cursorPosition,
                        at_str);
        }
    }
    Connections {
        target: comment_reply
        onCommentReplyClicked: {
            comment_input.clear();
            comment_input.insert(comment_input.cursorPosition,
                                 "回复@" + receiveUser["name"] + ": ");
        }
    }

    Connections {
        target: httpFunctions
        onCommentUploadReplyDone: {
            if (success === true) {
                TToast.showSuccess("发送成功", TTimePreset.LongTime4s);
                review.isPaintEnabled = false;
                paint_canvas.clear();
                comment_input.text = "";
                refer_btn.refers.clear();
                httpFunctions.getRefreshComment(runtimeCtx.auditFileFolderUuid);
            }
            else {
                TToast.showError("发送失败", TTimePreset.LongTime4s, msg);
            }
        }
    }
    Connections {
        target: comment_card
        onCommentReplyClicked: {
            send_comment_btn.curretReceiver = receiveUser;
            send_comment_btn.curretCommentParent = commentParent;
        }
    }
    Connections {
        target: runtimeCtx
        onFileTypeChanged: {
            if (runtimeCtx.getFileType() === RuntimeContext.VIDEO) {
                comment_tools_row.visible = true;
            }
            else if (runtimeCtx.getFileType() === RuntimeContext.AUDIO) {
                comment_tools_row.visible = false;
            }
        }
    }
}

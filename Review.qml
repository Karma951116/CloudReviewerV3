import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Window 2.0
import Qt.labs.platform 1.1
import Toou2D 1.0

import VideoPlayer 1.0
import PaintCanvas 1.0
import RuntimeContext 1.0
import "qrc:/Widget"

Rectangle {
    id: review
    color: "#283046"
    property bool isFullScreen: false
    property bool isPaintEnabled:false
    signal fullScreen();

    Item {
        id: left_header
        width: parent.width / 2
        height: 62
        anchors.top: parent.top
        anchors.left: parent.left
        TImageButton {
            id: menu_btn
            width: 24
            height: 24
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/icon/menu"
            onClicked: {
                if (runtimeCtx.getFileType() === RuntimeContext.VIDEO ||
                        runtimeCtx.getFileType() === RuntimeContext.AUDIO) {
                    video_player.stop();
                }
                video_panel.visible = image_panel.visible = false;
                root.goBrowse();
            }
        }

        Row {
            id: route
            width: 250
                //contentItem.width
            height: 14
            anchors.left: menu_btn.right
            anchors.leftMargin: 16
            anchors.verticalCenter: menu_btn.verticalCenter
            Text {
                text: qsTr(runtimeCtx.projectName)
                color: "#B4B7BD"
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.DemiBold
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                text: qsTr("/")
                color: "#B4B7BD"
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.DemiBold
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                text: qsTr(runtimeCtx.fileName)
                color: "#E8E8E8"
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.DemiBold
                verticalAlignment: Text.AlignVCenter
            }
        }

        VersionSelector {
            id: version_selector
            width: 150
            height: 34
            anchors.left: route.right
            anchors.leftMargin: 25
            anchors.verticalCenter: menu_btn.verticalCenter
        }
    }

    Item {
        id: right_header
        width: parent.width / 2
        height: 62
        anchors.top: parent.top
        anchors.right: parent.right

        UserCenter {
            id: review_user_info
            width: 150
            height: 38
            hasBell: false
            hasRole: true

            anchors.top: parent.top
            anchors.topMargin: 12
            anchors.right: parent.right
            anchors.rightMargin: 28
        }

        TIconButton {
            id: comment_export
            width: 80
            height: 38
            anchors.right: review_user_info.left
            anchors.rightMargin: 26
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "qrc:/icon/export.svg"
            icon.color: "#B4B7BD"
            label.text: qsTr("导出")
            label.color: "#B4B7BD"
            background.color: "#30384D"
            border.width: 0
            onClicked: {
                export_dialog.open();
            }
        }
        ComboBox {
            id: review_state
            width: 125
            height: 38
            anchors.right: comment_export.left
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
            hoverEnabled: true
            model: ListModel {
                ListElement {
                    stageId: "0"
                    stage: "待审核";
                    iconColor: "#5767F9"
                }
                ListElement {
                    stageId: "1"
                    stage: "审核中";
                    iconColor: "#00CFE8";
                }
                ListElement {
                    stageId: "2"
                    stage: "审核通过";
                    iconColor: "#28C76F";
                }
                ListElement {
                    stageId: "3"
                    stage: "审核不通过";
                    iconColor: "#EA5455";
                }
            }
            background: TRectangle {
                width: parent.width
                height: parent.height
                radius: 5
                color: review_state.hovered ? "#5767F9" : "#283046"
            }
            indicator: TImageButton {
                width: 16
                height: 16
                source: "qrc:/icon/arrow_down"
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter
            }
            contentItem: Text {
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("设置审核状态")
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.Bold
                color: review_state.hovered ? "white" : "#676D7D"
                verticalAlignment: Text.AlignVCenter
            }
            delegate: ItemDelegate {
                id: review_stage_delegate_item
                width: parent.width
                highlighted: parent.highlightedIndex === index
                contentItem: Item {
                    width: parent.width
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.verticalCenter: review_stage_delegate_item.background.verticalCenter
                    height: 36

                    TRectangle {
                        id: color_round
                        width: 8
                        height: 8
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        radius: width / 2
                        color: iconColor
                    }
                    Text {
                        anchors.left:color_round.right
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        text: stage
                        font.pixelSize: 14
                        font.family: local_font.name
                        font.weight: Font.Bold
                        color: "#B4B7BD"
                    }
                }
                background: TRectangle {
                    width: parent.width
                    height: 36
                    color: review_state.highlightedIndex === index ?
                               "#5767F9" : "transparent"
                    radius: 5
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var files = [runtimeCtx.getAuditFileFolderUuid()];
                        httpFunctions.postReviewStatus(runtimeCtx.getAuditContentUuid(),
                                                       runtimeCtx.getVersionUuid(),
                                                       files,
                                                       stageId);
                    }
                }
            }
            popup: Popup {
                width: parent.width
                y: parent.height + 8
                height: 185
                contentItem: ListView {
                    clip: true
                    spacing: 18
                    model: review_state.popup.visible ?
                               review_state.delegateModel : null
                }
                background: TRectangle {
                    color: "#283046"
                    radius: 5
                }
            }
        }
    }

    FileDialog {
        id: export_dialog
        fileMode: FileDialog.SaveFile
        onAccepted: {
            formatTransformer.exportCommentsJson(commentModel.getCommentsOringin(),
                                                 file);
        }
    }

    Rectangle {
        id: player_bg
        width: 1560
        height: 1018
        anchors.top: left_header.bottom
        anchors.left: parent.left
        color: "#161D31"
    }

    Item {
        id: video_panel
        width: 1280
        height: 798
        anchors.top: player_bg.top
        anchors.topMargin: 28
        anchors.left: player_bg.left
        anchors.leftMargin: 160
        z: 2
        visible: false

        VideoPlayer {
            id: video_player
            width: parent.width
            height: parent.height - 75
            anchors.top: parent.top
            anchors.left: parent.left

            MouseArea {
                anchors.fill: parent
                enabled: video_player.mode == VideoPlayer.VIDEO
                onClicked: {
                    if (video_player.getState() === VideoPlayer.PAUSE ||
                            video_player.getState() === VideoPlayer.STOP) {
                        video_player.play();
                    }
                    else {
                        video_player.pause();
                    }
                }
            }
        }

        Rectangle {
            id: replay_mask
            width: video_player.width
            height: video_player.height
            anchors.top: video_panel.top
            anchors.left: video_panel.left
            color: "#AA000000"
            visible: false
            TImageButton {
                id: restart_btn
                width: 24
                height: 24
                anchors.centerIn: parent
                source: "qrc:/icon/replay"
                onClicked: {
                    video_player.replay();
                    replay_mask.visible = false;
                }
            }
        }

        Rectangle {
            id: waiting_mask
            width: video_player.width
            height: video_player.height
            anchors.top: video_panel.top
            anchors.left: video_panel.left
            color: "#AA000000"
            visible: video_player.state == VideoPlayer.WAIT
            TImage {
                id: wait_mask_image
                width: 48
                height: 48
                opacity: 0.8
                anchors.centerIn: parent
                source: "qrc:/icon/loading.svg"
            }
        }

        Slider {
            id: audio_pogress_mask
            width: video_player.width
            height: video_player.height
            anchors.top: video_panel.top
            anchors.left: video_panel.left
            visible: video_player.mode == VideoPlayer.AUDIO
            padding: 0
            from: 0
            to: media_controller.timeDisplay.currentIndex == 0 ?
                    video_player.durationTime :
                    video_player.durationFrame
            value: media_controller.timeDisplay.currentIndex == 0 ?
                       video_player.currentTime :
                       video_player.currentFrame

            background: Rectangle {
                x: audio_pogress_mask.leftPadding
                y: audio_pogress_mask.topPadding +
                   audio_pogress_mask.availableHeight / 2 -
                   height / 2

                implicitWidth: video_player.width
                implicitHeight: video_player.height
                width: implicitWidth
                height: implicitHeight
                color: "#00000055"
                Rectangle {
                    width: audio_pogress_mask.visualPosition * parent.width
                    height: parent.height
                    color: "#a9a9a9FF"
                }
            }

            handle: Rectangle {
                x: audio_pogress_mask.leftPadding +
                   audio_pogress_mask.visualPosition *
                   (audio_pogress_mask.availableWidth - width)
                y: audio_pogress_mask.topPadding +
                   audio_pogress_mask.availableHeight / 2 -
                   height / 2

                implicitWidth: 3
                implicitHeight: video_player.height
                radius: 0
                color: "transparent"
                border.color: "transparent"
            }

            onPressedChanged: {
                if(!pressed) {
                    media_controller.timeDisplay.currentIndex == 0 ?
                                video_player.seekTime(value) :
                                video_player.seekFrame(value);
                }
            }
        }

        Slider {
            id: time_slider
            width: video_player.width
            height: 6
            anchors.top: video_player.bottom
            anchors.left: video_player.left
            padding: 0
            from: 0
            to: media_controller.timeDisplay.currentIndex == 0 ?
                    video_player.durationTime :
                    video_player.durationFrame
            value: media_controller.timeDisplay.currentIndex == 0 ?
                       video_player.currentTime :
                       video_player.currentFrame
            stepSize: 1
            background: Rectangle {
                x: time_slider.leftPadding
                y: time_slider.topPadding +
                   time_slider.availableHeight / 2 -
                   height / 2

                implicitWidth: 1280
                implicitHeight: 6
                width: implicitWidth
                height: implicitHeight
                color: Qt.rgba(87, 103, 249, 0.12)
                Rectangle {
                    width: time_slider.visualPosition * parent.width
                    height: parent.height
                    color: "#5767F9"
                }
            }

            handle: Rectangle {
                x: time_slider.leftPadding +
                   time_slider.visualPosition *
                   (time_slider.availableWidth - width)
                y: time_slider.topPadding +
                   time_slider.availableHeight / 2 -
                   height / 2

                implicitWidth: 3
                implicitHeight: 6
                radius: 0
                color: "transparent"
                border.color: "transparent"
            }

            onPressedChanged: {
                if(!pressed) {
                    media_controller.timeDisplay.currentIndex == 0 ?
                                video_player.seekTime(value) :
                                video_player.seekFrame(value);
                }
            }
        }

        CommentTimeline {
            id: comment_timeline
            width: video_player.width
            height: 24
            anchors.top: time_slider.bottom
            anchors.left: video_player.left
        }

        MediaController {
            id: media_controller
            width: parent.width
            height: 45
            anchors.top: comment_timeline.bottom
            anchors.left: video_player.left
            anchors.horizontalCenter: video_player.horizontalCenter
        }
    }

    Item {
        id: image_panel
        width: 1280
        height: 798
        anchors.top: player_bg.top
        anchors.topMargin: 28
        anchors.left: player_bg.left
        anchors.leftMargin: 160
        z: 2
        visible: runtimeCtx.getFileType() === runtimeCtx.IMAGE
        clip: true

        ImageDisplayer {
            id: image_displayer
            width: parent.width
            height: parent.height - 45
            anchors.top: parent.top
            anchors.left: parent.left
        }
        ImageContoller {
            id: image_controller
            width: parent.width
            height: 45
            anchors.top: image_displayer.bottom
            anchors.left: image_displayer.left
        }
    }

    CommentEditor {
        id: comment_editor
        width: 1280
        height: 160
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 25
        anchors.left: parent.left
        anchors.leftMargin: 160
        bgColor: "#283046"
        bgRadius: 5
    }

    CommentBrowser {
        id: comment_browser
        width: 360
        height: 1018
        anchors.top: right_header.bottom
        anchors.right: parent.right
    }

    PaintCanvas {
        id: paint_canvas
        width: video_player.width
        height: video_player.height
        anchors.top: runtimeCtx.getFileType() === RuntimeContext.IMAGE ?
                         image_panel.top : video_panel.top
        anchors.left: runtimeCtx.getFileType() === RuntimeContext.IMAGE ?
                          image_panel.left : video_panel.left
        z: video_panel.z + 1
        enabled: isPaintEnabled

        onEnabledChanged: {
            if (!enabled) {
                paint_canvas.clear();
            }
        }
    }

    Component.onCompleted: {
        video_player.setReplyParser(replyParser);
        video_player.setHttpFunctions(httpFunctions);
    }

    Connections {
        target: comment_editor
        onPaintToolsClicked: {
            if (video_player.getState() !== VideoPlayer.PAUSE) {
                video_player.pause();
            }
            isPaintEnabled = true;
            if (runtimeCtx.getFileType() === RuntimeContext.VIDEO) {
                paint_canvas.setImage(video_player.getCurrentImage());
            }
            else if (runtimeCtx.getFileType() === RuntimeContext.IMAGE) {
                paint_canvas.setImage(image_displayer.getCurrentImage());
            }
        }
    }

    Connections {
        target: video_player
        onVideoFinished: {
            replay_mask.visible = true;
        }
        onRequestMediaInfo: {
            httpFunctions.getMediaInfo(runtimeCtx.getAuditFileFolderUuid());
        }
    }

    Connections {
        target: review
        onFullScreen: {
            if (!isFullScreen) {
                root.visibility = Window.FullScreen;
                left_header.visible = false;
                right_header.visible = false;
                player_bg.visible = false;
                comment_browser.visible = false;

                if (runtimeCtx.getFileType() === RuntimeContext.VIDEO ||
                        runtimeCtx.getFileType() === RuntimeContext.AUDIO) {
                    video_panel.width = Screen.width;
                    video_panel.height = Screen.height;
                    video_panel.anchors.top = review.top;
                    video_panel.anchors.left = review.left;
                    video_panel.anchors.topMargin = 0;
                    video_panel.anchors.leftMargin = 0;
                }
                else if (runtimeCtx.getFileType() === RuntimeContext.IMAGE) {
                    image_panel.width = Screen.width;
                    image_panel.height = Screen.height;
                    image_panel.anchors.top = review.top;
                    image_panel.anchors.left = review.left;
                    image_panel.anchors.topMargin = 0;
                    image_panel.anchors.leftMargin = 0;
                }

                isFullScreen = true;
            }
            else {
                root.visibility = Window.Windowed;
                left_header.visible = true;
                right_header.visible = true;
                player_bg.visible = true;
                comment_browser.visible = true;

                if (runtimeCtx.getFileType() === RuntimeContext.VIDEO ||
                        runtimeCtx.getFileType() === RuntimeContext.AUDIO) {
                    video_panel.width = 1280;
                    video_panel.height = 798;
                    video_panel.anchors.top = player_bg.top;
                    video_panel.anchors.topMargin = 28;
                    video_panel.anchors.left = player_bg.left;
                    video_panel.anchors.leftMargin = 160;
                }
                else if (runtimeCtx.getFileType() === RuntimeContext.IMAGE) {
                    image_panel.width = 1280;
                    image_panel.height = 798;
                    image_panel.anchors.top = player_bg.top;
                    image_panel.anchors.topMargin = 28;
                    image_panel.anchors.left = player_bg.left;
                    image_panel.anchors.leftMargin = 160;
                }
                isFullScreen = false;
            }
        }
    }

    Connections {
        target: runtimeCtx
        onFileTypeChanged: {
            if (runtimeCtx.getFileType() === RuntimeContext.VIDEO) {
                video_panel.visible = true;
                video_player.setMode(VideoPlayer.VIDEO);
            }
            else if (runtimeCtx.getFileType() === RuntimeContext.AUDIO) {
                video_panel.visible = true;
                video_player.setMode(VideoPlayer.AUDIO);
            }
            else if (runtimeCtx.getFileType() === RuntimeContext.IMAGE) {
                image_panel.visible = true;
            }
        }
    }

}

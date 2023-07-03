import QtQuick 2.0
import Toou2D 1.0
import QtQuick.Controls 2.0
import QtQuick.Window 2.0
import Qt.labs.platform 1.1
import "qrc:/Widget"
import VideoPlayer 1.0

Rectangle {
    color: "#283046"

    Row {
        id: left_header
        width: parent.width / 2
        height: 62
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        spacing: 25
        TImageButton {
            id: menu_btn
            //property bool isReturn: false
            width: 24
            height: 24
            source: "qrc:/icon/menu"
            onClicked: {
                video_player.stop();
                root.goBrowse();
            }
        }
        Row {
            id: route
            width: contentItem.width
            height: 14
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

        }

        ComboBox {
            id: review_state
            width: 129
            height: 38
            anchors.right: sort_box.left
            anchors.rightMargin: 20
            hoverEnabled: true
            visible: fileModel.hasChecked()
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
                source: "qrc:/resources/icon/arrow_down.png"
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
                        var groupData = fileModel.groupChekedFile();
                        for (var content in groupData) {
                            for (var version in groupData[content]) {
                                var folders = groupData[content][version];
                                httpFunctions.postReviewStatus(content, version, folders, stageId);
                            }
                        }
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

    Row {
        id: right_header
        width: parent.width / 2
        height: 62
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        spacing: 25
        layoutDirection: "RightToLeft"

        UserCenter {
            width: 150
            hasBell: false
            hasRole: true
        }

        TIconButton {
            id: workstation_export
            width: 80
            height: 38
            icon.source: "qrc:/icon/export.svg"
            icon.color: "#B4B7BD"
            label.text: qsTr("导出")
            label.color: "#B4B7BD"
            background.color: "#30384D"
            border.width: 0
            onClicked: {
                workstation_export_dialog.open();
            }
        }
    }
    FileDialog {
        id: workstation_export_dialog
        fileMode: FileDialog.SaveFile
        onAccepted: {
            utils.exportCommentsJson(workstation_comment_display.currentComments, file);
        }
    }
    Rectangle {
        width: 1560
        height: 1018
        anchors.top: left_header.bottom
        anchors.left: parent.left
        color: "#161D31"

        VideoPlayer {
            id: video_player
            width: 1280
            height: 723
            anchors.top: parent.top
            anchors.topMargin: 28
            anchors.left: parent.left
            anchors.leftMargin: 160

            MouseArea {
                anchors.fill: parent
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
            anchors.centerIn: video_player
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
            id: play_mask
            width: video_player.width
            height: video_player.height
            anchors.centerIn: video_player
            color: "#AA000000"
            visible: video_player.state == VideoPlayer.PAUSE &&
                     replay_mask.visible == false
            TImageButton {
                id: play_mask_btn
                width: 48
                height: 48
                opacity: 0.8
                anchors.centerIn: parent
                source: "qrc:/icon/play"
                onClicked: {
                    video_player.play();
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
                    video_player.duration_frame
            value: media_controller.timeDisplay.currentIndex == 0 ?
                       video_player.currentTime :
                       video_player.current_frame
            stepSize: 1
            enabled: media_player.locked == 0 ? true : false
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
                    video_player.seekTime(value);
                }
            }
        }
        Rectangle {
            id: comment_timeline
            width: video_player.width
            height: 45
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
    CommentBrowser {
        width: 360
        height: 970
        anchors.top: right_header.bottom
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 8
    }
    Component.onCompleted: {
        video_player.setReplyParser(replyParser);
        video_player.setHttpFunctions(httpFunctions);
    }

    Connections {
        target: video_player
        onVideoFinished: {
            replay_mask.visible = true;
        }
    }

//    Connections {
//        target: replyParser
//        onM3u8ReplyDone: video_player.onM3u8ReplyDone()
//        onMediaInfoReplyDone: video_player.onMediaInfoReplyDone()
//    }
}

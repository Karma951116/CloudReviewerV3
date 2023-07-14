import QtQuick 2.0
import Toou2D 1.0
import QtQuick.Controls 2.0
import VideoPlayer 1.0
import RuntimeContext 1.0

TRectangle {
    id: media_toolbar
    width: parent.width
    height: 45
//    anchors.top: media_comment_stamp.bottom
//    anchors.left: parent.left
//    anchors.horizontalCenter: parent.horizontalCenter
//    enabled: true
        //media_player.locked == 0 ? true : false
    color: "#283046"
    radius: parent.isFullScreen ? 0 : 3

    property alias timeDisplay: media_time
    property alias playBtn: play_btn

    TRectangle {
        id: toolbar_rectmask
        width: parent.width
        height: 3
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#161D31"
        z: 1
    }

    TImageButton {
        id: play_btn
        width: 32
        height: 32
        anchors.left: parent.left
        anchors.leftMargin: 24
        anchors.top: parent.top
        anchors.topMargin: 5 + toolbar_rectmask.height
        source: video_player.state === VideoPlayer.PLAY ?
                    "qrc:/icon/pause" :
                    "qrc:/icon/play"

        onClicked: {
            switch (video_player.state) {
            case VideoPlayer.PLAY:
                video_player.pause();
                break;
            case VideoPlayer.PAUSE:
            case VideoPlayer.STOP:
                review.isPaintEnabled = false;
                video_player.play();
                break;
            }
        }
    }

    TImageButton {
        id: backward_btn
        width: 32
        height: 32
        anchors.left: play_btn.right
        anchors.leftMargin: 16
        anchors.top: parent.top
        anchors.topMargin: 5 + toolbar_rectmask.height
        source: "qrc:/icon/backward"

        onClicked: {
            switch(media_time.currentIndex) {
            case 0:
                video_player.seekTime(time_slider.value - 10);
                break;
            case 1:
                video_player.seekPreFrame();
                break;
            }
        }
    }

    TImageButton {
        id: forward_btn
        width: 32
        height: 32
        anchors.left: backward_btn.right
        anchors.top: parent.top
        anchors.topMargin: 5 + toolbar_rectmask.height
        source: "qrc:/icon/forward"

        onClicked: {
            switch(media_time.currentIndex) {
            case 0:
                video_player.seekTime(time_slider.value + 10);
                break;
            case 1:
                video_player.seekNextFrame();
                break;
            }
        }
    }

    ComboBox {
        id: media_time
        property string time_str: ""
        width: 150
        height: 42
        anchors.left: forward_btn.right
        anchors.leftMargin: 16
        anchors.top: parent.top
        anchors.topMargin: toolbar_rectmask.height

        displayText: currentIndex == 0 ?
             formatTransformer.sec2Time(video_player.currentTime) +
                         " / " + formatTransformer.sec2Time(video_player.durationTime) :
             video_player.currentFrame + " / " + video_player.durationFrame

        model: [
            "TIME",
            "FRAME"
        ]
        background: TRectangle {
            width: parent.width
            height: parent.height
            color: "transparent"
        }
        indicator: TImageButton {
            width: 16
            height: 16
            source: "qrc:/icon/media_time_indicator"
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
        }
        contentItem: Text {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: media_time.displayText
            font.pixelSize: 14
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: "#B4B7BD"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        delegate: ItemDelegate {
            id: mediaplayer_time_delegate_item
            width: media_time.width
            height: media_time.height

            highlighted: parent.highlightedIndex === index
            contentItem: Item {
                anchors.centerIn: parent
                Text {
                    id: time_name
                    width: parent.width
                    height: parent.height
                    anchors.centerIn: parent
                    text: modelData
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 14
                    font.family: local_font.name
                    font.weight: Font.DemiBold
                    color: "#B4B7BD"
                }
            }
            background: TRectangle {
                anchors.fill: parent
                color: media_time.highlightedIndex === index ?
                           "#5767F9" : "transparent"
            }
            onClicked: {
                //media_time.displayText = modelData;
            }
        }
        popup: Popup {
            width: parent.width
            height: contentItem.implicitHeight + 20
            x: -10
            y: parent.height + 20
            topPadding: 10
            bottomPadding: 10
            leftPadding: 0
            rightPadding: 0

            contentItem: ListView {
                width: parent.width
                implicitHeight: contentHeight
                anchors.centerIn: parent
                clip: true
                spacing: 18

                model: media_time.popup.visible ?
                           media_time.delegateModel : null
            }
            background: TRectangle {
                width: parent.width
                height: parent.height
                anchors.centerIn: parent
                color: "#161D31"
                border.width: 1
                border.color: "#283046"
                radius: 5
            }
        }
    }

    TImageButton {
        id: media_full_screen
        width: 16
        height: 16
        anchors.right: parent.right
        anchors.rightMargin: 24
        anchors.verticalCenter: parent.verticalCenter
        source: "qrc:/icon/fullscreen"
        onClicked: {
            review.fullScreen();
        }
    }

    TImageButton {
        id: media_volume
        width: 16
        height: 16
        anchors.right: media_full_screen.left
        anchors.rightMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        source: media_volume_slider.value === 0 ?
                    "qrc:/icon/volume_zero" :
                    "qrc:/icon/volume"
        onClicked: {
            if (media_volume_popup.visible) {
                media_volume_popup.visible = false;
            }
            else {
                media_volume_popup.visible = true;
            }
        }
    }

    TRectangle {
        id: media_volume_popup
        width: 30
        height: 160
        color: "#161D31"
        radius: 2
        border.color: "#283046"
        border.width: 1
        anchors.bottom: media_volume.top
        anchors.horizontalCenter: media_volume.horizontalCenter
        visible: false
        z: 2

        Text {
            id: media_volume_text
            width: 16
            height: 10
            anchors.bottom: media_volume_slider.top
            anchors.bottomMargin: 5
            anchors.horizontalCenter: media_volume_slider.horizontalCenter
            text: ((media_volume_slider.value / media_volume_slider.to) * 100).toFixed(0);
            horizontalAlignment: Text.Center
            color: "white"
            font.pixelSize: 12
            font.family: local_font.name
            font.weight: Font.DemiBold
        }

        Slider {
            id: media_volume_slider
            width: 12
            height: 135
            from: 0
            to: 128
            value: 64
            stepSize: 1
            orientation: Qt.Vertical
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            onValueChanged: {
                video_player.setVolume(value)
            }

            background: Rectangle {
                x: parent.leftPadding +
                   media_volume_slider.availableWidth / 2 -
                   width / 2
                y: parent.topPadding +
                   media_volume_slider.availableHeight / 2 -
                   height /2
                implicitWidth: 6
                implicitHeight: 122
                color: "#5767F9"
                radius: 5
                border.width: 0
                Rectangle {
                    width: parent.width
                    height: media_volume_slider.visualPosition * parent.height
                    color: "#1d2549"
                    radius: 5
                    border.width: 0
                }
            }
            handle: Rectangle {
                x: parent.leftPadding + media_volume_slider.availableWidth / 2 - width / 2
                y: parent.visualPosition * parent.availableHeight
                implicitWidth: 12
                implicitHeight: 12
                width: 12
                height: 12

                radius: 50
                color: "#283046"
                border.color: "#5767F9"
                border.width: 2
            }
        }
    }

    ComboBox {
        id: media_speed
        width: 60
        height: 42
        anchors.right: media_volume.left
        anchors.rightMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        displayText: currentIndex == 3 ? qsTr("倍速") : textAt(currentIndex)
        model: ["3.0x", "2.0x", "1.5x", "正常", "0.5x"]
        background: TRectangle {
            width: parent.width
            height: parent.height
            color: "transparent"
        }
        indicator: null
        contentItem: Text {
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            text: media_speed.displayText
            font.pixelSize: 14
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: "#B4B7BD"
            verticalAlignment: Text.AlignVCenter
        }
        delegate: ItemDelegate {
            id: mediaplayer_speed_delegate_item
            width: media_speed.width
            height: media_speed.height

            highlighted: parent.highlightedIndex === index
            contentItem: Item {
                anchors.centerIn: parent
                Text {
                    id: speed_name
                    width: parent.width
                    height: parent.height
                    anchors.centerIn: parent
                    text: modelData
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 14
                    font.family: local_font.name
                    font.weight: Font.DemiBold
                    color: "#B4B7BD"
                }
            }
            background: TRectangle {
                anchors.fill: parent
                color: media_speed.highlightedIndex === index ?
                           "#5767F9" : "transparent"
            }
            onClicked: {
                media_speed.displayText = modelData;
            }
        }
        popup: Popup {
            width: parent.width
            height: contentItem.implicitHeight + 20
            x: -10
            y: parent.height + 15
            topPadding: 10
            bottomPadding: 10
            leftPadding: 0
            rightPadding: 0
            z: 2

            contentItem: ListView {
                width: parent.width
                implicitHeight: contentHeight
                anchors.centerIn: parent
                clip: true
                spacing: 18

                model: media_speed.popup.visible ?
                           media_speed.delegateModel : null
            }
            background: TRectangle {
                width: parent.width
                height: parent.height
                anchors.centerIn: parent
                color: "#161D31"
                border.width: 1
                border.color: "#283046"
                radius: 5
            }
        }
        onCurrentIndexChanged:  {
            switch(currentIndex) {
            case 0 :
                video_player.setSpeed(3.0);
                break;
            case 1:
                video_player.setSpeed(2.0);
                break;
            case 2:
                video_player.setSpeed(1.5);
                break;
            case 3:
                video_player.setSpeed(1.0);
                break;
            case 4:
                video_player.setSpeed(0.5);
                break;
            }
        }
        Component.onCompleted: {
            currentIndex = 3;
        }
    }

    ComboBox {
        id: media_quality
        width: 60
        height: 42
        anchors.right: media_speed.left
        anchors.rightMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        displayText: qsTr("画质")
        model: ["原画质", "超清 1280P", "高清 720P"]
        background: TRectangle {
            width: parent.width
            height: parent.height
            color: "transparent"
        }
        indicator: null

        contentItem: Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: media_quality.displayText
            font.pixelSize: 14
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: "#B4B7BD"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        delegate: ItemDelegate {
            id: mediaplayer_quality_delegate_item
            width: media_quality.width * 1.5
            height: media_quality.height

            highlighted: parent.highlightedIndex === index
            contentItem: Item {
                anchors.fill: parent
                Text {
                    id: quality_name
                    width: parent.width
                    height: parent.height
                    anchors.centerIn: parent
                    text: modelData
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 14
                    font.family: local_font.name
                    font.weight: Font.DemiBold
                    color: "#B4B7BD"
                }
            }
            background: TRectangle {
                anchors.fill: parent
                color: media_quality.highlightedIndex === index ?
                           "#5767F9" : "transparent"
            }
            onClicked: {
                media_quality.displayText = modelData;
            }
        }
        popup: Popup {
            width: parent.width * 1.5
            height: contentItem.implicitHeight + 20
            x: -15
            y: parent.height + 15
            topPadding: 10
            bottomPadding: 10
            leftPadding: 0
            rightPadding: 0

            contentItem: ListView {
                width: parent.width
                implicitHeight: contentHeight
                anchors.centerIn: parent
                clip: true
                spacing: 18

                model: media_quality.popup.visible ?
                           media_quality.delegateModel : null
            }
            background: TRectangle {
                width: parent.width
                height: parent.height
                anchors.centerIn: parent
                color: "#161D31"
                border.width: 1
                border.color: "#283046"
                radius: 5
            }
        }
    }

    TButton {
        id: exit_comment_edit_mode
        width: 122
        height: 28
        anchors.right: media_quality.left
        anchors.rightMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        background.radius: 5
        background.color: "#30384D"
        border.width: 0
        label.text: qsTr("取消时间段批注")
        label.color: "#FFFFFF"
        label.font.pixelSize: 14
        label.font.family: local_font.name
        label.font.weight: Font.DemiBold
        visible: comment_timeline.editMode
        onClicked: {
            comment_timeline.editMode = false;
        }
    }

    Text {
        id: comment_edit_info
        width: 100
        height: 28
        anchors.right: exit_comment_edit_mode.left
        anchors.rightMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        text: formatTransformer.sec2Time(comment_timeline.start) +
              " - " +
              formatTransformer.sec2Time(comment_timeline.end)
        color: "#FFFFFF"
        font.pixelSize: 14
        font.family: local_font.name
        font.weight: Font.DemiBold
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        visible: comment_timeline.editMode
    }

    Connections {
        target: runtimeCtx
        onFileTypeChanged: {
            if (runtimeCtx.getFileType() === RuntimeContext.VIDEO) {
                media_time.indicator.visible = true;
                media_time.popup.enabled = true;
                media_quality.visible = true;
            }
            else if (runtimeCtx.getFileType() === RuntimeContext.AUDIO) {
                media_time.indicator.visible = false;
                media_time.popup.enabled = false;
                media_quality.visible = false;
            }
        }
    }
}

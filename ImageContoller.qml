import QtQuick 2.0
import Toou2D 1.0
import QtQuick 2.14

Rectangle {
    property int iconSize: 16
    property int stepping: 10
    property int imageScale: 100
    implicitWidth: 1280
    implicitHeight: 45
    color: "#283046"
    radius: parent.isFullScreen ? 0 : 3
    signal scaleValueChanged(var value)

    Row {
        anchors.right: parent.right
        anchors.rightMargin: 24
        anchors.verticalCenter: parent.verticalCenter
        spacing: 15
        TImageButton {
            id: expand
            width: iconSize
            height: iconSize
            source: "qrc:/icon/expend.svg"
            onClicked: {
            }
        }

        TImageButton {
            id: decrease
            width: iconSize
            height: iconSize
            source: "qrc:/icon/decrease.svg"
            onClicked: {
                if (imageScale - 10 < 0) {
                    return;
                }
                imageScale -= 10;
            }
        }

        TextInput {
            id: scale_value
            width: iconSize * 3
            height: iconSize
            rightPadding: iconSize
            text: imageScale
            font.pixelSize: 14
            color: "#B4B7BD"
            font.family: local_font.name
            font.weight: Font.DemiBold
            //verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            cursorVisible: false
            validator: IntValidator {
                bottom: 0
            }
            onEditingFinished: {
                var value = parseInt(text)
                if (value > 300) {
                    value = 300;
                }
                imageScale = value;
            }

            Text {
                width: iconSize
                height: iconSize
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: "%"
                font.pixelSize: 14
                color: "#B4B7BD"
                font.family: local_font.name
                font.weight: Font.DemiBold
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }

        TImageButton {
            id: increase
            width: iconSize
            height: iconSize
            source: "qrc:/icon/increase.svg"
            onClicked: {
                if (imageScale + 10 > 300) {
                    return;
                }
                imageScale += 10;
            }
        }

        TDividerLine {
            id: divider
            width: 1
            height: 20
            color: "#3B4253"
        }

        TImageButton {
            id: unknown_btn
            width: iconSize
            height: iconSize
            source: "qrc:/icon/book.svg"
            onClicked: {

            }
        }

        TImageButton {
            id: image_full_screen
            width: iconSize
            height: iconSize
            source: "qrc:/icon/fullscreen"
            onClicked: {
                review.fullScreen();
            }
        }
    }
    onImageScaleChanged: {
        scaleValueChanged(imageScale / 100);
        scale_value.update();
    }
}

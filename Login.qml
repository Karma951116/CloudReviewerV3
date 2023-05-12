import QtQuick 2.0
import "qrc:/Widget"

Item {
    property alias windowButtons: window_buttons
    property alias titlebar: titlebar
    Image {
        id: bg
        anchors.fill: parent
        source: "qrc:/texture/login_bg"
    }

    Image {
        id: logo
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 30
        anchors.leftMargin: 25
        source: "qrc:/icon/login_logo"
    }

    TitleBar {
        id: titlebar
        width: parent.width - 10
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 10
        bgColor: "transparent"
    }

    WindowButtonGroup {
        id: window_buttons
        anchors.right: titlebar.right
        anchors.verticalCenter: titlebar.verticalCenter
    }

    LoginPanel {
        id: login_input
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.654
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.242
        width: parent.width * 0.254
        height: parent.height * 0.514
    }
    onVisibleChanged: {
        if (visible) {
           login_input.reset();
        }
    }
}

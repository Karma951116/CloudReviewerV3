import QtQuick 2.0

MouseArea {
    property alias source: icon.source
    property var hoverColor: "#d9d9d9"
    property var pressColor: "#a6a6a6"
    property var normalColor: "transparent"
    property bool bHovered: false
    property bool bPressed: false
    property bool bText: false

    implicitWidth: 40
    implicitHeight: 25
    hoverEnabled: true

    Rectangle {
        id: bg
        anchors.fill: parent
        color: bPressed ? pressColor : bHovered ? hoverColor : normalColor
    }

    Image {
        id: icon
        width: height
        height: parent.height
        anchors.centerIn: parent
    }

    onEntered: {
        bHovered = true;
    }
    onExited: {
        bHovered = false;
    }
    onPressed: {
        bPressed = true;
    }
    onReleased: {
        bPressed = false;
    }
}

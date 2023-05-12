import QtQuick 2.0


// not useful

Rectangle {
    property var holder;
    property bool bPressed: false;
    property point clickPos: "0, 0";
    property bool resizeWidth: false;
    property bool resizeHeight: false;
    width: holder.width;
    height: holder.height;
    anchors.centerIn: parent;
    color: "transparent"
    border.color: "black";
    border.width: 1;

    MouseArea {
        anchors.fill: parent;
        hoverEnabled: true;
        onEntered: {
            // 在窗口左侧
            if (mouseX < 50) {
                resizeWidth = true;
                // 左上
                if (mouseY < 50) {
                    cursorShape = Qt.ArrowCursor;
                    resizeHeight = true;
                }
                // 左下
                else if (mouseY > height - 50) {
                    cursorShape = Qt.ArrowCursor;
                    resizeHeight = true;
                }
                else {
                    cursorShape = Qt.SizeHorCursor;
                    resizeHeight = false;
                }
            }
            // 窗口右侧
            else if (mouseX > width - 50) {
                resizeWidth = true;
                // 右上
                if (mouseY < 50) {
                    cursorShape = Qt.ArrowCursor;
                    resizeHeight = true;
                }
                // 右下
                else if (mouseY > height - 50) {
                    cursorShape = Qt.ArrowCursor;
                    resizeHeight = true;
                }
                else {
                    cursorShape = Qt.SizeHorCursor;
                    resizeHeight = false;
                }
            }
            else {
                resizeWidth = false;
                resizeHeight = true;
                cursorShape = Qt.SizeVerCursor;
            }
        }
        onExited: {
            cursorShape = Qt.ArrowCursor;
        }
        onPressed: {
            bPressed = true;
            clickPos = Qt.point(mouse.x, mouse.y);
        }
        onPositionChanged: {
            if (!bPressed) {
                return;
            }
            var deltaX = mouse.x - clickPos.x;
            var deltaY = mouse.y - clickPos.y;
            if (resizeWidth) holder.width += deltaX;
            if (resizeHeight) holder.height += deltaY;
            clickPos = Qt.point(mouse.x, mouse.y);
        }
        onReleased: {
            bPressed = false;
        }
    }
}

import QtQuick 2.14
import QtQuick.Window 2.14
import "qrc:/Base";
import "qrc:/Page"
import Toou2D 1.0

Window {
    id: root
    property int displayWidth: width - 2;
    property int displayHeight: height - 2;
    property bool bMaximized: false
    visible: true
    width: 1919
    height: 1079
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "transparent"

    T2DWorld {

    }
    FontLoader {
        id: local_font
        source: "qrc:/font/PingFang SC Regular.ttf"
    }

//    CusResizeComponent {
//        holder: root;
//    }

    Login {
        id: login_page
        anchors.fill: parent
        visible: true;
    }

    Browse {
        id: browse_page
        anchors.fill: parent
        visible: false
    }

    Review {
        id: review_page
        anchors.fill: parent
        visible: false
    }

    function goLogin () {
        login_page.visible = true;
        browse_page.visible = false;
        review_page.visible = false;
    }
    function goBrowse() {
        login_page.visible = false;
        browse_page.visible = true;
        review_page.visible = false;
    }
    function goReview() {
        login_page.visible = false;
        browse_page.visible = false;
        review_page.visible = true;
    }

    Connections {
        target: login_page.windowButtons
        onMinimized: {
            root.showMinimized();
        }
        onMaximized: {
            if (bMaximized) {
                root.showNormal();
            }
            else {
                root.showMaximized();
            }
            bMaximized = !bMaximized;
        }
        onClosed: {
            //aboutToClose();
            root.close();
        }
    }
    Connections {
        target: login_page.titlebar
        onMoved: {
            root.setX(root.x + deltaX);
            root.setY(root.y + deltaY);
        }
    }

    Connections {
        target: browse_page.windowButtons
        onMinimized: {
            root.showMinimized();
        }
        onMaximized: {
            if (bMaximized) {
                root.showNormal();
            }
            else {
                root.showMaximized();
            }
            bMaximized = !bMaximized;
        }
        onClosed: {
            //aboutToClose();
            root.close();
        }
    }


    Connections {
        target: replyParser
        onLoginReplyDone: {
            if (success) {
                goBrowse();
                httpFunctions.getUserCompany();
            }
        }
    }
}

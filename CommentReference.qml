import QtQuick 2.0
import Toou2D 1.0

Item {
    id: pic_refer
    //property alias imgSource: main_image.source

    implicitWidth: 72
    implicitHeight: 42

    TAvatar {
        id: main_image
        width: 64
        height: 36
        anchors.centerIn: parent
        source: filePath
        radius: 5
    }
    TImageButton {
        id: delete_btn
        width: 10
        height: 10
        anchors.top: parent.top
        anchors.right: parent.right
        source: "qrc:/icon/close_btn_ellipse_red"
        onClicked: {
            refer_btn.refers.remove(index);
        }
    }
}

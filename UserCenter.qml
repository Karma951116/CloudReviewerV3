import QtQuick 2.0
import Toou2D 1.0
import QtQuick.Controls 2.0

Item {
    implicitWidth: 250
    implicitHeight: 38

    property bool hasBell: true
    property bool hasRole: false

    TAvatar {
        id: avatar
        width: 38
        height: 38
        anchors.top: parent.top
        anchors.left: parent.left
        source: "qrc:/texture/login_bg"
        radius: width / 2
    }

    Text {
        id: user_name
        width: 80
        height: 19
        anchors.left: avatar.right
        anchors.leftMargin: 10
        y: hasRole ? 0 : 9.5
        text: clientUser.userName
        font.pixelSize: 14
        font.family: local_font.name
        font.weight: Font.DemiBold
        color: "#B4B7BD"
    }

    Text {
        id: role_name
        width: 80
        height: 19
        anchors.left: avatar.right
        anchors.leftMargin: 10
        anchors.top: user_name.bottom
        text: clientUser.role
        font.pixelSize: 14
        font.family: local_font.name
        font.weight: Font.DemiBold
        color: "#B4B7BD"
        visible: hasRole
    }

    TImageButton {
        id: logout_popbtn
        width: 12
        height: 12
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: user_name.right
        anchors.leftMargin: 5

        source: "qrc:/icon/arrow_down"
        onClicked: {
            logout_popup.open();
        }
    }
    Popup {
        id: logout_popup
        width: 200
        x: logout_popbtn.x - 200
        y: logout_popbtn.y + 10
        closePolicy: Popup.CloseOnPressOutside
        background: TRectangle {
            width: 200
            height: 300
            color: "#283046"
            TImage {
                width: parent.width
                height: parent.height / 3
                anchors.top: parent.top
                anchors.left: parent.left
                source: "qrc:/texture/login_bg"
                z: 1
            }
        }
        contentItem: Column {
            anchors.left: parent.left
            anchors.leftMargin: 50
            anchors.top: parent.top
            anchors.topMargin: 50
            spacing: 20

            TAvatar {
                width: 100
                height: 100
                radius: width / 2
                border.width: 4
                border.color: "#283046"
                source: "qrc:/texture/login_bg"
            }

            Text {
                width: 100
                height: 20
                text: clientUser.userName
                font.pixelSize: 16
                color: "#E8E8E8"
                font.family: local_font.name
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
            }

            TButton {
                id: logout_popup_logout
                width: 100
                height: 25
                label.text: "退出登录"
                label.font.pixelSize: 14
                label.font.weight: Font.DemiBold
                label.color: "#45E8E8E8"
                background.color: "#5767F9"
                background.radius: 5
                border.width: 0

                onClicked: {
                    logout_popup.close();
                    goLogin();
                }
            }
        }
    }

    TImageButton {
        id: message
        width: 20
        height: 20
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: logout_popbtn.right
        anchors.leftMargin: 24

        source: "qrc:/icon/bell"
        visible: hasBell
    }

}

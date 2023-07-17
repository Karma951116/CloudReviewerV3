import QtQuick 2.0
import Toou2D 1.0
import "qrc:/Base"
import "qrc:/Widget"

Rectangle {
    id: panel
    color: "#221E63"
    //TODO: border颜色与设计图不匹配，暂未使用
    //border.width: 1
    //border.color: "#3B4253"

    Text {
        width: 224
        height: 45
        x: 64
        y: 64
        text: qsTr("欢迎登录审片室")
        font.family: local_font.name
        font.pixelSize: 32
        font.weight: Font.Bold
        color: "#E8E8E8"
    }
    CusNavigator {
        id: navigator
        width: 360
        height: 32
        x: 64
        y: 133
    }

    Loader {
        id: input_loader
        width: 360
        height: sourceComponent == LoginMessageInput ? 155 : 193
        x: 64
        y: 187

        sourceComponent: msg_input
    }

    LoinPasswordInput {
        id: pwd_input
    }

    LoginMessageInput {
        id: msg_input
    }

    // 登录按钮和注册
    TButton{
        id: login_btn
        width: 360
        height: 46
        x: 64
        anchors.top: input_loader.bottom
        anchors.topMargin: 24
        label.text: qsTr("登录")
        label.font.family: local_font.name
        label.font.pixelSize: 14
        label.font.weight: Font.DemiBold
        label.color: "#FFFFFF"
        background.color: "#5767F9"
        background.radius: 5
        border.width: 0
        onClicked: {
            var userName = input_loader.item.userName;
            var pwd = input_loader.item.pwd;
            if (input_loader.sourceComponent == pwd_input) {
                if (!formatChecker.accountCheck(userName) || !formatChecker.pwdCheck(pwd)) {
                    TToast.showWarning(promptProvider.getPrompt("error", "login", "UserOrPwdFormatError"),
                                       TTimePreset.LongTime4s);
                    return;
                }
            }
            else {
                if (!formatChecker.phoneCheck(userName) || !formatChecker.pinCheck(pwd)) {
                    TToast.showWarning(promptProvider.getPrompt("error", "login", "PhoneOrPinFormatError"),
                                       TTimePreset.LongTime4s);
                    return;
                }
            }
            httpFunctions.postLogin(userName, pwd);
        }
    }

    Text {
        id: regist_text
        width: 126
        height: 21
        x: 153
        anchors.top: login_btn.bottom
        anchors.topMargin: 20
        text: qsTr("您还没有平台账号？")
        font.family: local_font.name
        font.pixelSize: 14
        font.weight: Font.DemiBold
        color: "#B4B7BD"
        verticalAlignment: Text.AlignVCenter
    }

    TButton{
        id: regist_btn
        width: 56
        height: 21
        anchors.left: regist_text.right
        anchors.leftMargin: 0
        anchors.verticalCenter: regist_text.verticalCenter
        label.text: qsTr("立即注册")
        label.font.family: local_font.name
        label.font.pixelSize: 14
        label.font.weight: Font.DemiBold
        label.color: "#5767F9"
        background.color: "transparent"
        border.width: 0
        onClicked: {

        }
    }

    function reset() {
        input_loader.item.userName = "";
        input_loader.item.pwd = "";
    }

    Connections {
        target: navigator
        onIndexChanged: {
            if (index == 0) {
                input_loader.sourceComponent = msg_input;
            }
            else {
                input_loader.sourceComponent = pwd_input;
            }
        }
    }
    Connections {
        target: replyParser
        onLoginReplyDone: {
            if (!success) {
                TToast.showError(promptProvider.getPrompt("error", "login", "LoginFailed"),
                                 TTimePreset.LongTime4s);
                return;
            }
            TToast.showSuccess(promptProvider.getPrompt("success", "login", "LoginSuccessed"),
                               TTimePreset.LongTime4s);
            if (input_loader.sourceComponent == pwd_input &&
                    input_loader.item.keepPwd) {
                console.log(input_loader.item.userName);
                config.setConfig("Login", "Account",
                                 formatTransformer.strToBase64(input_loader.item.userName));
                console.log(input_loader.item.pwd);
                config.setConfig("Login", "Pwd",
                                 formatTransformer.strToBase64(input_loader.item.pwd));
            }
            parent.loginCompleted();
        }
    }
}

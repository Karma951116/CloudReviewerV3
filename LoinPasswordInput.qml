import QtQuick 2.0
import Toou2D 1.0
//import HttpRequestTool 1.0
Component {
    TRectangle {
        property string userName: account_input.text
        property string pwd: password_input.text
        property alias keepPwd: remember_me.checked
        implicitWidth: 360
        implicitHeight: 193

        color: "transparent"
        Text {
            width: 42
            height: 18
            x: 0
            y: 0
            text: qsTr("账号")
            font.family: local_font.name
            font.pixelSize: 14
            font.weight: Font.DemiBold
            color: "#E8E8E8"
        }
        TInputField {
            id: account_input
            //property bool accountAvailable: false
            width: 360
            height: 46
            x: 0
            y: 22
            placeholderPosition: TPosition.Left;
            placeholderLabel.text: qsTr("请输入登录账号");
            label.font.family: local_font.name
            label.font.pixelSize: 14
            label.font.weight: Font.DemiBold
            label.color: "#45E8E8E8"
            background.color: "#353182"
            background.radius: 5
            border.width: 0
            selectByMouse: true
            qtInput.selectedTextColor: "blue"
            onInputingChanged: {   
                if (!inputing) {
                    var bFit = formatChecker.accountCheck(account_input.text);
                    if (!bFit) {
                        TToast.showError(promptProvider.getPrompt("error", "login", "UserNameFormatError"),
                                         TTimePreset.LongTime4s);
                    }
                }
            }
        }

        Text {
            width: 42
            height: 18
            x: 0
            y: 87
            text: qsTr("密码")
            font.family: local_font.name
            font.pixelSize: 14
            font.weight: Font.DemiBold
            color: "#E8E8E8"
        }

        TInputField {
            id: password_input
            width: 360
            height: 46
            x: 0
            y: 109
            placeholderPosition: TPosition.Left;
            placeholderLabel.text: qsTr("请输入登录密码");
            label.font.family: local_font.name
            label.font.pixelSize: 14
            label.font.weight: Font.DemiBold
            label.color: "#45E8E8E8"
            background.color: "#353182"
            background.radius: 5
            border.width: 0
            echoMode: TextInput.Password

            TImageButton{
                id: echo_mode
                anchors.right: password_input.right;
                anchors.rightMargin: 19;
                anchors.verticalCenter: password_input.verticalCenter;
                width: 18
                height: 18
                source: "qrc:/icon/pwd_hide"

                onClicked: {
                    if(parent.echoMode === TextInput.Password) {
                        parent.echoMode = TextInput.Normal
                        source = "qrc:/icon/pwd_display"
                    }
                    else {
                        parent.echoMode = TextInput.Password
                        source = "qrc:/icon/pwd_hide"
                    }
                }
            }
            onInputingChanged: {
                if (!inputing) {
                    var bFit = formatChecker.pwdCheck(password_input.text)
                    if (!bFit) {
                        TToast.showError(promptProvider.getPrompt("error", "login", "PwdFormatError"),
                                           TTimePreset.LongTime4s);
                    }
                }
            }
        }

        TCheckBox {
            id: remember_me
            width: 82
            height: 20
            x: 0
            y: 175
            label.text: qsTr("记住密码")
            label.font.family: local_font.name
            label.font.pixelSize: 14
            label.font.weight: Font.DemiBold
            label.color: "#B4B7BD"
            onCheckedChanged: {
                config.setConfig("Login", "KeepPwd", checked);
            }
        }

        TButton {
            id: forget_password
            width: 70
            height: 20
            x: 290
            y: 175
            label.text: qsTr("忘记密码？")
            label.font.family: local_font.name
            label.font.pixelSize: 14
            label.font.weight: Font.DemiBold
            label.color: "#5767F9"
            border.width: 0
            background.color: "transparent"
        }
        Component.onCompleted: {
            var remember = config.getBoolConfig("Login", "KeepPwd");
            remember_me.checked = remember;
            if (remember) {
                var account_base64 = config.getStrConfig("Login", "Account");
                var account = formatTransformer.base64ToStr(account_base64);
                var pwd_base64 = config.getStrConfig("Login", "Pwd");
                var pwd = formatTransformer.base64ToStr(pwd_base64);
                account_input.text = account;
                password_input.text = pwd;
            }
        }
    }
}


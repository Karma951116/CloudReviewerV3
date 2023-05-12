import QtQuick 2.0
import Toou2D 1.0

Component {
    Rectangle {
        property alias userName: phone_input.text
        property alias pwd: captcha_input.text
        implicitWidth: 360
        implicitHeight: 155
        color: "transparent"
        Text {
            width: 42
            height: 18
            x: 0
            y: 0
            text: qsTr("手机号")
            font.family: local_font.name
            font.pixelSize: 14
            font.weight: Font.DemiBold
            color: "#E8E8E8"
        }

        TInputField {
            id: phone_input
            width: 360
            height: 46
            x: 0
            y: 22
            placeholderPosition: TPosition.Left;
            placeholderLabel.text: qsTr("请输入手机号");
            label.font.family: local_font.name
            label.font.pixelSize: 14
            label.font.weight: Font.DemiBold
            label.color: "#45E8E8E8"
            background.color: "#353182"
            background.radius: 5
            border.width: 0
//            qtInput.validator: RegExpValidator {
//                regExp: /^1[3-9]\d{9}$/
//            }
            onInputingChanged: {
                if (inputing) {
                    var bFit = formatChecker.phoneCheck(phone_input.text);
                    if (!bFit) {
                        TToast.showWarning(promptProvider.getPrompt("error", "login", "PhoneFormatError"),
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
            text: qsTr("验证码")
            font.family: local_font.name
            font.pixelSize: 14
            font.weight: Font.DemiBold
            color: "#E8E8E8"
        }

        TInputField {
            id: captcha_input
            property bool phoneAvailable: false
            width: 254
            height: 46
            x: 0
            y: 109
            placeholderPosition: TPosition.Left;
            placeholderLabel.text: qsTr("请输入验证码");
            label.font.family: local_font.name
            label.font.pixelSize: 14
            label.font.weight: Font.DemiBold
            label.color: "#45E8E8E8"
            background.color: "#353182"
            background.radius: 5
            border.width: 0
//            qtInput.validator: RegExpValidator {
//                regExp: /^\d{6}$/
//            }
            onInputingChanged: {
                if (inputing) {
                    var bFit = formatChecker.pinCheck(captcha_input.text);
                    if (!bFit) {
                        TToast.showWarning(promptProvider.getPrompt("error", "login", "PinFormatError"),
                                           TTimePreset.LongTime4s);
                    }
                }
            }
        }
        TButton{
            width: 94
            height: 46
            x: 266
            y: 109
            anchors.verticalCenter: captcha_input.verticalCenter;
            label.text: qsTr("获取验证码")
            label.font.family: local_font.name
            label.font.pixelSize: 14
            label.font.weight: Font.DemiBold
            label.color: "#E8E8E8"
            background.color: "#5767F9"
            background.radius: 5
            border.width: 0
            onClicked: {
                var bFit = formatChecker.phoneCheck(phone_input.text);
                if (bFit) {
                    httpFunctions.postPin(phone_input.text);
                }
                else {
                    TToast.showWarning(promptProvider.getPrompt("error", "login", "PhoneFormatError"),
                                       TTimePreset.ShortTime2s);
                }
            }
        }

        Connections {
            target: httpFunctions
            onPinBack: {

            }
        }
    }
}


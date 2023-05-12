import QtQuick 2.0
import QtQuick.Controls 2.2
import Toou2D 1.0

ComboBox {
    id: company_switch

    implicitWidth: 260
    implicitHeight: 52
    hoverEnabled: true

    model: companyModel
    background: Rectangle {
        width: parent.width
        height: parent.height
        color: "transparent"
    }
    indicator: TImageButton {
        width: 16
        height: 16
        source: "qrc:/icon/company_switcher"
        anchors.right: parent.right
        anchors.rightMargin: 28
        anchors.verticalCenter: parent.verticalCenter
        onClicked: {
            if (company_switch.popup.opened) {
                company_switch.popup.close();
            }
            else {
                company_switch.popup.open();
                httpFunctions.getUserCompany();
            }
        }
    }
    contentItem: Item {
        id: company_main_item
        property alias itemSource: company_main_image.source
        property alias itemText: company_main_text.text
        width: company_switch.width - company_switch.indicator.width
        height: company_switch.height
        Image {
            id: company_main_image
            anchors.left: parent.left
            anchors.leftMargin: 31
            anchors.verticalCenter: parent.verticalCenter
            width: 24
            height: 24
            //需要接口提供图像
            source: "qrc:/icon/test_group_mark"
        }
        Text {
            id: company_main_text
            anchors.left: company_main_image.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            font.family: local_font.name
            text: "我的工作台"
            font.pixelSize: 14
            font.weight: Font.DemiBold
            color: "#D0D2D6"
        }
    }
    delegate: ItemDelegate {
        id: company_delegate_item
        width: company_switch.width
        height: company_switch.height

        highlighted: parent.highlightedIndex === index
        contentItem: Item {
            width: company_switch.width
            height: company_switch.height
            Item {
                width: 200
                height: company_switch.height
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    width: 24
                    height: 24
                    // 需要接口提供图像
                    source: ""
                }
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 34
                    anchors.verticalCenter: parent.verticalCenter
                    text: {
                        return companyModel.getRoleValue(index, "name");
                    }
                    font.family: local_font.name
                    font.weight: Font.DemiBold
                    font.pixelSize: 14
                    color: "#FFFFFF"
                }
            }
            Image {
                anchors.left: parent.left
                anchors.leftMargin: 215
                anchors.verticalCenter: parent.verticalCenter
                width: 30
                height: 30
                source: "qrc:/icon/company_checked"
                visible: {
                    var name = companyModel.getRoleValue(index, "name");
                    company_main_item.itemText === name;
                }
            }
        }
        background: TRectangle {
            anchors.fill: parent
            color: company_switch.highlightedIndex === index ?
                       "#5767F9" : "transparent"
        }
        onClicked: {
            var name = companyModel.getRoleValue(index, "name");
            var id = companyModel.getRoleValue(index, "id");
            var uuid = companyModel.getRoleValue(index, "uuid");
            console.log(name);
            company_main_item.itemText = name;
            httpFunctions.getCompanyProjects(id);

            runtimeCtx.setCompanyName(name);
            runtimeCtx.setCompanyUuid(uuid);
        }
    }
    popup: Popup {
        width: company_switch.width
        height: contentItem.implicitHeight + 20
        x: 0
        y: parent.height + 8
        topPadding: 10
        bottomPadding: 10
        leftPadding: 0
        rightPadding: 0

        contentItem: ListView {
            width: parent.width
            implicitHeight: contentHeight
            anchors.centerIn: parent
            clip: true
            spacing: 18

            model: company_switch.popup.visible ?
                       company_switch.delegateModel : null
        }
        background: TRectangle {
            width: parent.width
            height: parent.height
            anchors.centerIn: parent
            color: "#161D31"
            border.width: 1
            border.color: "#283046"
            radius: 5
        }
    }
}

import QtQuick 2.0
import QtQuick.Controls 2.0
import Toou2D 1.0

ComboBox {
    id: workstation_asset_version
    property bool isSwitching: false
    property var switchContentUuid;
    property var switchFolderUuid;
    property var switchVersionUuid;
    property var switchVersionName;
    property var switchFileFolder;
    width: 150
    height: 34
    anchors.left: workstation_route.right
    anchors.leftMargin: 25
    anchors.verticalCenter: parent.verticalCenter
    hoverEnabled: true
    model: ListModel {
        id: versions_model
    }
    background: TRectangle {
        width: parent.width
        height: parent.height
        radius: 5
        color: workstation_asset_version.hovered ? "#5767F9" : "#30384D"
    }
    indicator: TImageButton {
        width: 16
        height: 16
        source: "qrc:/resources/icon/arrow_down.png"
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.verticalCenter: parent.verticalCenter
    }
    contentItem: Text {
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        text: workstation_asset_version.displayText
        font.pixelSize: 14
        font.family: local_font.name
        font.weight: Font.DemiBold
        color: workstation_asset_version.hovered ? "white" : "#676D7D"
        verticalAlignment: Text.AlignVCenter
    }
    delegate: ItemDelegate {
        id: workstation_asset_version_delegate_item
        signal versionChange();
        width: 400
        height: 60
        highlighted: parent.highlightedIndex === index
        contentItem: Item {
            anchors.fill: parent
            //anchors.centerIn: parent

            Text {
                id: asset_version_name
                width: 40
                height: parent.height
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter
                text: versionName
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.DemiBold
                color: "#B4B7BD"
            }
            Image {
                id: asset_version_thumbnail
                width: 80
                height: 44
                anchors.left: asset_version_name.right
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                source: auditFileFolder["image"]
            }

            Column {
                id: asset_version_info
                width: 220
                height: 44
                anchors.left: asset_version_thumbnail.right
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 8
                spacing: 4

                Text {
                    width: parent.width
                    height: 20
                    text: auditFileFolder["name"]
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                    font.family: local_font.name
                    font.weight: Font.DemiBold
                    color: "#B4B7BD"
                }
                Text {
                    width: parent.width
                    height: 20
                    text: "2021/06/29 16:54:55"
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                    font.family: local_font.name
                    font.weight: Font.DemiBold
                    color: "#B4B7BD"
                }
            }
            Image {
                width: 20
                height: 20
                anchors.left: asset_version_info.right
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/resources/icon/checked.png"
                visible: workstation_asset_version.displayText === versionName ?
                    true : false
            }
        }
        background: TRectangle {
            anchors.fill: parent
            color: workstation_asset_version.highlightedIndex === index ?
                       "#5767F9" : "transparent"
        }
        onClicked: {
            workstation_asset_version.switchContentUuid = auditContentUuid;
            workstation_asset_version.switchFolderUuid = auditFileFolderUuid;
            workstation_asset_version.switchVersionUuid = versionUuid;
            workstation_asset_version.switchFileFolder = versionName;
            workstation_asset_version.isSwitching = true;
            versionChanged();
        }
    }
    popup: Popup {
        width: 400
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

            model: workstation_asset_version.popup.visible ?
                       workstation_asset_version.delegateModel : null
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

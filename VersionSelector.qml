import QtQuick 2.0
import QtQuick.Controls 2.0
import Toou2D 1.0

ComboBox {
    id: versions
    property bool isSwitching: false
    implicitWidth: 150
    implicitHeight: 34
    hoverEnabled: true
    model: versionModel
    background: Rectangle {
        width: parent.width
        height: parent.height
        radius: 5
        color: versions.hovered ? "#5767F9" : "#30384D"
    }
    indicator: TImageButton {
        width: 16
        height: 16
        source: "qrc:/icon/arrow_down"
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.verticalCenter: parent.verticalCenter
    }
    contentItem: Text {
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        text: versions.displayText
        font.pixelSize: 14
        font.family: local_font.name
        font.weight: Font.DemiBold
        color: versions.hovered ? "white" : "#676D7D"
        verticalAlignment: Text.AlignVCenter
    }
    delegate: ItemDelegate {
        id: versions_delegate_item
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
                source: imageUrl
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
                    text: name
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                    font.family: local_font.name
                    font.weight: Font.DemiBold
                    color: "#B4B7BD"
                }
                Text {
                    width: parent.width
                    height: 20
                    // 时间接口中没有
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
                source: "qrc:/icon/company_checked"
                visible: versionModel.currentVersion === versionUuid
            }
        }
        background: TRectangle {
            anchors.fill: parent
            color: versions.highlightedIndex === index ?
                       "#5767F9" : "transparent"
        }
        onClicked: {
            video_player.stop();
            httpFunctions.getFileInfo(auditFileFolderUuid);
            httpFunctions.getIndex(auditFileFolderUuid);
            runtimeCtx.setAuditContentUuid(auditContentUuid);
            runtimeCtx.setVersionUuid(versionUuid);
            runtimeCtx.setVersionName(versionName);
            runtimeCtx.setAuditFileFolderUuid(auditFileFolderUuid);
            runtimeCtx.setFileName(name);
            runtimeCtx.setFileType(fileSuffix);
            versionModel.setCurrentVersion(versionUuid);
            versions.displayText = versionName;
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

            model: versions.popup.visible ?
                       versions.delegateModel : null
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
    Connections {
        target: versionModel
        onVersionListRefreshed: {
            versionModel.setCurrentVersion(runtimeCtx.getVersionUuid());
            versions.displayText = runtimeCtx.getVersionName();
        }
    }
}

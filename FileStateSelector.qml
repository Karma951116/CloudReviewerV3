import QtQuick 2.0
import QtQuick.Controls 2.0
import Toou2D 1.0

ComboBox {
    id: review_state
    width: 129
    height: 38
    anchors.right: sort_box.left
    anchors.rightMargin: 20
    hoverEnabled: true
    visible: fileModel.hasChecked()
    model: ListModel {
        ListElement {
            stageId: "0"
            stage: "待审核";
            iconColor: "#5767F9"
        }
        ListElement {
            stageId: "1"
            stage: "审核中";
            iconColor: "#00CFE8";
        }
        ListElement {
            stageId: "2"
            stage: "审核通过";
            iconColor: "#28C76F";
        }
        ListElement {
            stageId: "3"
            stage: "审核不通过";
            iconColor: "#EA5455";
        }
    }
    background: TRectangle {
        width: parent.width
        height: parent.height
        radius: 5
        color: review_state.hovered ? "#5767F9" : "#283046"
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
        text: qsTr("设置审核状态")
        font.pixelSize: 14
        font.family: local_font.name
        font.weight: Font.Bold
        color: review_state.hovered ? "white" : "#676D7D"
        verticalAlignment: Text.AlignVCenter
    }
    delegate: ItemDelegate {
        id: review_stage_delegate_item
        width: parent.width
        highlighted: parent.highlightedIndex === index
        contentItem: Item {
            width: parent.width
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: review_stage_delegate_item.background.verticalCenter
            height: 36

            TRectangle {
                id: color_round
                width: 8
                height: 8
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                radius: width / 2
                color: iconColor
            }
            Text {
                anchors.left:color_round.right
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: stage
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.Bold
                color: "#B4B7BD"
            }
        }
        background: TRectangle {
            width: parent.width
            height: 36
            color: review_state.highlightedIndex === index ?
                       "#5767F9" : "transparent"
            radius: 5
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                var groupData = fileModel.groupChekedFile();
                for (var content in groupData) {
                    for (var version in groupData[content]) {
                        var folders = groupData[content][version];
                        httpFunctions.postReviewStatus(content, version, folders, stageId);
                    }
                }
            }
        }
    }
    popup: Popup {
        width: parent.width
        y: parent.height + 8
        height: 185
        contentItem: ListView {
            clip: true
            spacing: 18
            model: review_state.popup.visible ?
                       review_state.delegateModel : null
        }
        background: TRectangle {
            color: "#283046"
            radius: 5
        }
    }
}

import QtQuick 2.0
import QtQuick.Controls 2.0
import Toou2D 1.0

ComboBox {
    id: showroom_sort_box
    width: 169
    height: 38
    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
    hoverEnabled: true
    model: ["按指派时间正序", "按指派时间倒序"]
    contentItem: Text {
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
        text: showroom_sort_box.textAt(showroom_sort_box.currentIndex)
        font.pixelSize: 14
        font.family: local_font.name
        font.weight: Font.DemiBold
        color: showroom_sort_box.hovered ? "white" : "#676D7D"
        verticalAlignment: Text.AlignVCenter
    }
    background: TRectangle {
        width: parent.width
        height: parent.height
        radius: 5
        color: showroom_sort_box.hovered ? "#5767F9" :"#283046"
        border.color: showroom_sort_box.hovered ? "#5767F9" : "#3B4253"
        border.width: 1
    }
    indicator: TImageButton {
        width: 16
        height: 16
        source: "qrc:/icon/arrow_down"
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.verticalCenter: parent.verticalCenter
    }
    delegate: ItemDelegate {
        id: review_sort_delegate_item
        width: parent.width
        highlighted: parent.highlightedIndex === index
        contentItem: Item {
            width: parent.width
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: review_sort_delegate_item.background.verticalCenter
            height: 36

            Text {
                anchors.left:parent.left
                //anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                text: modelData
                font.pixelSize: 14
                font.family: local_font.name
                font.weight: Font.DemiBold
                color: showroom_sort_box.highlightedIndex === index ? "#FFFFFF" : "#676D7D"
            }
        }
        background: TRectangle {
            width: parent.width
            height: 36
            color: showroom_sort_box.highlightedIndex === index ?
                   "#5767F9" : "transparent"
            radius: 5
        }
    }
    popup: Popup {
        width: parent.width
        y: parent.height + 8
        height: 185
        contentItem: ListView {
            clip: true
            spacing: 18
            model: showroom_sort_box.popup.visible ?
            showroom_sort_box.delegateModel : null
        }
        background: TRectangle {
            color: "#283046"
            radius: 5
        }
    }
}

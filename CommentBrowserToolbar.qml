import QtQuick 2.0
import QtQuick.Controls 2.14
import Toou2D 1.0

Item {
    id: comment_toolbar
    implicitWidth: 360
    implicitHeight: 48

    property alias atMe: at_me_btn.checked
    property alias commentSortIndex: comment_sort.currentIndex

    ComboBox {
        id: comment_sort
        width: 145
        height: 20
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.verticalCenter: parent.verticalCenter
        displayText: textAt(currentIndex)
        model: ["按批注时间正序", "按批注时间倒序"]
        contentItem: Text {
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            text: comment_sort.textAt(comment_sort.currentIndex)
            font.pixelSize: 14
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: "#B4B7BD"
            verticalAlignment: Text.AlignVCenter
        }
        background: TRectangle {
            width: parent.width
            height: parent.height
            radius: 5
            color: "transparent"
            border.width: 0
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
            id: comment_sort_delegate_item
            width: parent.width
            highlighted: parent.highlightedIndex === index
            contentItem: Item {
                width: parent.width
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: comment_sort_delegate_item.background.verticalCenter
                height: 36

                Text {
                    anchors.left:parent.left
                    //anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData
                    font.pixelSize: 14
                    font.family: local_font.name
                    font.weight: Font.DemiBold
                    color: "#B4B7BD"
                }
            }
            background: TRectangle {
                width: parent.width
                height: 36
                color: comment_sort.highlightedIndex === index ?
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
                model: comment_sort.popup.visible ?
                comment_sort.delegateModel : null
            }
            background: TRectangle {
                color: "#283046"
                radius: 5
            }
        }
    }
    CheckBox {
        id: at_me_btn
        width: 51
        height: 20
        anchors.right: parent.right
        anchors.rightMargin: 18
        anchors.verticalCenter: parent.verticalCenter

        indicator: Rectangle {
            implicitWidth: 18
            implicitHeight: 18
            x: at_me_btn.leftPadding
            y: parent.height / 2 - height / 2
            radius: 3
            border.color: "#404656"
            color: "transparent"

            Rectangle {
                width: 8
                height: 8
                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                radius: 2
                color: "#B4B7BD"
                visible: at_me_btn.checked
            }
        }

        contentItem: Text {
            anchors.left: at_me_btn.indicator.right
            anchors.leftMargin: 6
            text: qsTr("@我")
            font.pixelSize: 14
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: "#B4B7BD"
            verticalAlignment: Text.AlignVCenter
        }
    }
    TDividerLine {
        color: "#3B4253"
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
    }
}

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Shapes 1.14
import Toou2D 1.0

Item {
    implicitWidth: 1604
    implicitHeight: 38
    property alias sortMode: sort_box.currentIndex

    Text {
        id: project_name
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        text: runtimeCtx.projectName
        font.family: local_font.name
        font.pixelSize: 24
        font.weight: Font.Bold
        color: "#D0D2D6"
    }

    TDividerLine {
        width: 1
        height: parent.height
        anchors.left: project_name.right
        anchors.leftMargin: 24
        anchors.verticalCenter: parent.verticalCenter
        color: "#3B4253"
    }

    CheckBox {
        id: check_all
        width: 56
        height: 22
        anchors.left: project_name.right
        anchors.leftMargin: 49
        anchors.verticalCenter: parent.verticalCenter
        indicator:  TRectangle {
            implicitWidth: 24
            implicitHeight: 24
            x: 0
            y: 0
            radius: 3
            color: "transparent"
            border.color: "#404656"
            border.width: 2
            TRectangle {
                anchors.fill: parent
                radius: 3
                color: "#5767F9"
                visible: check_all.checked
                Shape {
                    id: indicator_shape
                    anchors.fill: parent
                    ShapePath {
                        strokeWidth: 4
                        strokeColor: "white"
                        fillColor: "transparent"

                        capStyle: ShapePath.RoundCap
                        property int joinStyleIndex: 0

                        property variant styles: [
                            ShapePath.BevelJoin,
                            ShapePath.MiterJoin,
                            ShapePath.RoundJoin
                        ]

                        joinStyle: styles[joinStyleIndex]
                        startX: 0
                        startY: indicator_shape.height / 2
                        PathLine {
                            x: indicator_shape.width / 2
                            y: indicator_shape.height
                        }
                        PathLine {
                            x: indicator_shape.width
                            y: 0
                        }
                    }
                }

            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (check_all.checked) {
                        fileModel.uncheckAll();
                    }
                    else {
                        fileModel.checkAll();
                    }
                }
            }
        }
        contentItem: Text {
            text: qsTr("全选")
            color: "#B4B7BD"
            font.pixelSize:  16
            font.family: local_font.name
            verticalAlignment: Text.AlignVCenter
            anchors.left: parent.indicator.right
            anchors.leftMargin: 6
        }
    }

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
    ComboBox {
        id: sort_box
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
            text: sort_box.textAt(sort_box.currentIndex)
            font.pixelSize: 14
            font.family: local_font.name
            font.weight: Font.DemiBold
            color: sort_box.hovered ? "white" : "#676D7D"
            verticalAlignment: Text.AlignVCenter
        }
        background: TRectangle {
            width: parent.width
            height: parent.height
            radius: 5
            color: sort_box.hovered ? "#5767F9" :"#283046"
            border.color: sort_box.hovered ? "#5767F9" : "#3B4253"
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
                    color: sort_box.highlightedIndex === index ? "#FFFFFF" : "#676D7D"
                }
            }
            background: TRectangle {
                width: parent.width
                height: 36
                color: sort_box.highlightedIndex === index ?
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
                model: sort_box.popup.visible ?
                           sort_box.delegateModel : null
            }
            background: TRectangle {
                color: "#283046"
                radius: 5
            }
        }
    }
    Connections {
        target: fileModel
        onCheckedListChanged: {
            check_all.checked = fileModel.isCheckedAll();
            review_state.visible = fileModel.hasChecked();
        }
    }
}

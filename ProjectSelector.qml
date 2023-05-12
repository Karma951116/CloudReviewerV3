import QtQuick 2.0
import Toou2D 1.0

Item {
    id: group_btn_background
    width: 230
    height: group_btn.isFold ? 42 : 42 + project_col.height
    TRectangle {
        property bool isFold: true
        id: group_btn
        width: 230
        height: 42
        anchors.top: parent.top
        anchors.left: parent.left
        color: isFold ? "transparent" : "#161D31"
        radius: 5
        Image {
            id: group_btn_icon
            width: 16
            height: 16
            anchors.top: parent.top
            anchors.topMargin: 13
            anchors.left: parent.left
            anchors.leftMargin: 16
            source: groupName == "云审片" ? "qrc:/icon/monitor" : "qrc:/icon/video"
        }
        Text {
            id: group_name
            height: 20
            anchors.left: group_btn_icon.right
            anchors.leftMargin: 11
            anchors.verticalCenter: group_btn_icon.verticalCenter
            text: groupName
            font.family: local_font.name
            font.pixelSize: 14
            font.weight: Font.Bold
            color: "#FFFFFF"
            verticalAlignment: Text.AlignVCenter
        }

        Image {
            id: fold_btn
            width: 16
            height: 16
            anchors.top: parent.top
            anchors.topMargin: 13
            anchors.right: parent.right
            anchors.rightMargin: 13
            source: group_btn.isFold ? "qrc:/icon/arrow_right" :
                                       "qrc:/icon/arrow_down"
        }
        MouseArea {
            anchors.fill: parent
            z: 1
            onClicked: {
                group_btn.isFold = !group_btn.isFold
            }
        }
    }
    Column {
        id: project_col
        anchors.top: group_btn.bottom
        anchors.topMargin: 6
        anchors.left: group_btn.left
        visible: !group_btn.isFold

        Repeater {
            id: projects

            model: projectModel

            delegate: TRectangle {
                id: project_btn
                width: 230
                height: 42
                color: focus ? "#5767F9" : "transparent"
                radius: 5

                Image {
                    id: project_btn_icon
                    width: 10
                    height: 10
                    anchors.top: parent.top
                    anchors.topMargin: 16
                    anchors.left: parent.left
                    anchors.leftMargin: 43
                    source: "qrc:/icon/project_btn"
                }

                Text {
                    id: project_name
                    height: 20
                    anchors.left: project_btn_icon.right
                    anchors.leftMargin: 14
                    anchors.verticalCenter: project_btn_icon.verticalCenter
                    text: {
                        return projectModel.getRoleValue(index, "name");
                    }
                    font.family: local_font.name
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    color: "#FFFFFF"
                    verticalAlignment: Text.AlignVCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        parent.focus = true;
                        var projectUuid = projectModel.getRoleValue(index, "uuid");
                        var id = projectModel.getRoleValue(index, "id");
                        var name = projectModel.getRoleValue(index, "name")
                        httpFunctions.getProjectFiles(id, projectUuid, 1, 1, 100);
                        runtimeCtx.setProjectUuid(projectUuid);
                        runtimeCtx.setProjectName(name);
                        console.log(runtimeCtx.getProjectName());
                    }
                }
            }
        }
    }
}

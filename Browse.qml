import QtQuick 2.0
import "qrc:/Widget"
import "qrc:/Base"
import Toou2D 1.0


Rectangle {
    property alias windowButtons: window_buttons

    color: "#283046"
    CompanySelector {
        id: company_selector
        width: 260
        height: 62
        anchors.top: parent.top
        anchors.left: parent.left
    }
    ModuleSelector {
        width: 230
        height: 1018
        anchors.top: company_selector.bottom
        anchors.horizontalCenter: company_selector.horizontalCenter
    }

    TDividerLine {
        id: divider
        width: 1
        height: 20
        anchors.top: parent.top
        anchors.topMargin: (company_selector.height - height) / 2
        anchors.left: company_selector.right

        color: "#3B4253"
    }
    // 项目资产搜索
    FileSercher {
        id:file_sercher
        width: 840
        height: 62
        anchors.left: company_selector.right
        anchors.leftMargin: 25
        anchors.verticalCenter: company_selector.verticalCenter
    }
    UserCenter {
        width: 250
        height: 38
        anchors.verticalCenter: divider.verticalCenter
        anchors.right: window_buttons.left
        anchors.rightMargin: 48
    }

    TDividerLine {
        width: 1
        height: 20
        anchors.right: window_buttons.left
        anchors.rightMargin: 24
        anchors.verticalCenter: divider.verticalCenter
        color: "#3B4253"
        z: 1
    }

    WindowButtonGroup {
        id: window_buttons
        anchors.verticalCenter: divider.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 24
    }

    FileBrowser {
        width: 1660
        height: 1018
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}

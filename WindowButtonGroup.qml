import QtQuick 2.0
import "qrc:/Base"

Row {
    signal minimized();
    signal maximized();
    signal restored();
    signal closed();

    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
    spacing: 10

    CusIconButton {
        id: minimize_btn
        width: 16
        height: 16
        source: "qrc:/icon/titlebar_minimize"
        onClicked: {
            minimized();
        }
    }
    CusIconButton {
        id: maximize_btn
        width: 16
        height: 16
        source: "qrc:/icon/titlebar_maximize"
        onClicked: {
            maximized();
        }
    }
    CusIconButton {
        id: close_btn
        width: 16
        height: 16
        source: "qrc:/icon/titlebar_close"
        hoverColor: "#ff3333"
        pressColor: "#ff8080"
        onClicked: {
            closed();
        }
    }
}

import QtQuick 2.0
import "qrc:/Base"


ListView {
    interactive: false
    clip: true
    spacing: 20

    model: ListModel {
        ListElement {
            groupName: "云审片"
        }
        ListElement {
            groupName: "云监看"
        }
    }

    delegate: ProjectSelector {

    }
}

import QtQuick 2.0
import Toou2D 1.0
import QtQuick.Shapes 1.14
import VideoPlayer 1.0

Item{
    id: comment_timeline
    implicitWidth: 1280
    implicitHeight: 24

    property int maxTime
    property bool editMode
    readonly property int minTime: 0
    property double nodeScale
    property var scaleValues: new Array
    property bool isEditMoving: false
    property var start: start_handle_loader.status === Loader.Ready ?
                            start_handle_loader.item.editTime : null
    property var end: end_handle_loader.status === Loader.Ready ?
                          end_handle_loader.item.editTime : null

    property Component startHandle;
    property Component endHandle;
    property Component middleBar;
    property Item curHandle;
    signal clearNodes();

    startHandle: Shape {
        id: start_handle
        width: 6
        height: 18
        // handle指向的视频时间秒
        property int editTime;
        // 视频时间对应的刻度x
        property double editTimeX;
        ShapePath {
            strokeWidth: 0
            strokeStyle: ShapePath.SolidLine
            strokeColor: "transparent"
            startX: 0
            startY: 18
            PathLine { x: 6; y: 18 }
            PathLine { x: 6; y: 6 }
            PathLine { x: 0; y: 0 }
            fillColor: Qt.rgba(217, 217, 217, 0.80)
        }
    }

    endHandle: Shape {
        id: end_handle
        width: 6
        height: 18
        property int editTime;
        property double editTimeX;
        ShapePath {
            strokeWidth: 0
            strokeStyle: ShapePath.SolidLine
            strokeColor: "transparent"
            startX: 0
            startY: 18
            PathLine { x: 6; y: 18 }
            PathLine { x: 6; y: 0 }
            PathLine { x: 0; y: 6 }

            fillColor: Qt.rgba(217, 217, 217, 0.80)
        }
    }

    middleBar: TRectangle {
        id: middle_bar
        width: 0
        height: 12
        color: "transparent"

        TRectangle {
            id: middle_bar_handle
            implicitWidth: 12
            implicitHeight: 12
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            radius: 6
            color: "#5767F9"
            visible: middle_bar.width > 12 ? true : false
        }
        TRectangle {
            id: middle_bar_slider
            width: parent.width
            height: 4
            color: "#5767F9"
            visible: middle_bar.width > 0 ? true : false
        }

    }


    MouseArea {
        id: timeline_area
        anchors.fill: parent
        enabled: maxTime > 0 ? true : false
        onClicked: {
            editMode = true;
            if (start_handle_loader.status != Loader.Ready) {
                start_handle_loader.sourceComponent = startHandle;
                curHandle = start_handle_loader;
            }
            else if (end_handle_loader.status != Loader.Ready) {
                end_handle_loader.sourceComponent = endHandle;
                curHandle = end_handle_loader;
            }
            var calculateData = findScaleVal(mouseX);
            // 若为结束点计算，则时间需在起始点之后, 否则回到操作起始点状态
            if (curHandle === end_handle_loader &&
                    calculateData[0] < start_handle_loader.item.editTime) {
                end_handle_loader.sourceComponent = "";
                curHandle = start_handle_loader;
                mouse.accepted = true;
                return;
            }
            if (curHandle.status === Loader.Ready) {
                curHandle.item.editTime = calculateData[0]
                curHandle.item.editTimeX = calculateData[1];
                if (calculateData[1] + curHandle.item.width >
                        scaleValues[scaleValues.length - 1]) {
                    curHandle.x =
                            scaleValues[scaleValues.length - 1] -
                            curHandle.item.width;
                }
                else {
                    curHandle.x = calculateData[1];
                }
            }
        }

        onPressAndHold: {
            isEditMoving = true;
        }

        onMouseXChanged: {
            if (curHandle === null || !isEditMoving) {
                return;
            }
            var data = findScaleVal(mouseX);
            if (curHandle === end_handle_loader &&
                    data[0] < start_handle_loader.item.editTime) {
                mouse.accepted = true;
                return;
            }
            curHandle.item.editTime = data[0];
            curHandle.item.editTimeX = data[1];
            if (data[1] + curHandle.item.width >
                    scaleValues[scaleValues.length - 1]) {
                curHandle.x =
                        scaleValues[scaleValues.length - 1] -
                        curHandle.item.width;
            }
            else {
                curHandle.x = data[1];
            }

        }
        onReleased: {
            isEditMoving = false;
        }
    }
    // 计算与posX最近的刻度值
    function findScaleVal(posX) {
        var left = 0;
        var right = scaleValues.length - 1;
        if (posX > scaleValues[right]) {
            return [right, scaleValues[right]];
        }
        if (posX < scaleValues[left]) {
            return [left, scaleValues[left]];
        }

        while ((right - left) > 1) {
            var mid = Math.floor((left + right) / 2) ;
            if (posX <= scaleValues[mid]) {
                right = mid;
            }
            else if (posX >= scaleValues[mid]){
                left = mid;
            }
        }
        var left_diff = Math.abs(scaleValues[left] - posX);
        var right_diff = Math.abs(scaleValues[right] - posX);
        return left_diff < right_diff ?
                    [left, scaleValues[left]] : [right, scaleValues[right]]
    }

    function getTimeLineMouseX() {
        return timeline_area.mouseX;
    }

    Loader {
        id: start_handle_loader
        sourceComponent: undefined
        anchors.verticalCenter: parent.verticalCenter
        z: 2
    }
    Loader {
        id: end_handle_loader
        sourceComponent: undefined
        anchors.verticalCenter: parent.verticalCenter
        z: 2
    }
    Loader {
        id: middle_bar_loader
        sourceComponent: undefined
        anchors.verticalCenter: parent.verticalCenter
        z: 2
    }

    TRectangle {
        id: bg
        anchors.fill: parent
        color: "#283046"
    }

    onEditModeChanged: {
        if (!editMode) {
            start_handle_loader.sourceComponent = undefined;
            end_handle_loader.sourceComponent = undefined;
            middle_bar_loader.sourceComponent = undefined;
        }
    }
    Connections {
        target: video_player
        onDurationTimeReady: {
            //计算每秒像素宽度并填充刻度值列表
            comment_timeline.maxTime = video_player.durationTime;
            comment_timeline.nodeScale = (comment_timeline.width / (comment_timeline.maxTime + 1));
            for (var j = 0; j < maxTime + 1; j++) {
                scaleValues.push(j * comment_timeline.nodeScale)
            }

            for(var i = 0; i < commentModel.commentCount(); i++) {
                var start = formatTransformer.time2Sec(commentModel.getRoleValue(i, "start"))
                var end = formatTransformer.time2Sec(commentModel.getRoleValue(i, "end"))
                var start_scaled = start * nodeScale;
                var width_scaled = (end - start) * nodeScale < 22 ?
                            22 : (end - start) * nodeScale;

                var component = Qt.createComponent("CommentTimelineNode.qml");
                if (component.status === Component.Ready) {
                    var node = component.createObject(comment_timeline,
                                                      {commentRow: i,
                                                          width: width_scaled,
                                                          height:24,
                                                          x: start_scaled,
                                                          y: 0});
                }
            }
        }
        onStateChanged: {
            if (video_player.getState() === VideoPlayer.STOP) {
                clearNodes();
            }
        }
    }
}


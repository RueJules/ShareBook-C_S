import QtQuick
import Qt5Compat.GraphicalEffects
Item {
    Rectangle{
        anchors.fill: parent
        color:"#009688"

        Connections {
            target: control
            function onGetNewNotes(newNotes){
                grid.model=newNotes;
            }
        }

        GridView {
            id:grid
            anchors.fill: parent
//            model: control.receiveNotes()
            leftMargin:5
            cellWidth: (parent.width-grid.leftMargin)/2
            cellHeight:parent.height*2/5
            property bool needReflesh:false
            property bool needLoadMore:false
            delegate: Rectangle {
                property int id_:modelData[0]
                radius: 10
                width: grid.cellWidth-5
                height:grid.cellHeight-5
                //OpacityMask使用时用到的rectangle和image必须在同一层级
                Image {
                    id: material
                    source: modelData[3]===""?"qrc:/UI/default/2991e08f0e0926a5424c8cb73bca7742.png":"image://material/" + modelData[3];
                    width: parent.width
                    height:parent.height-115
                    fillMode: Image.PreserveAspectFit //缩放以适应屏幕
                    visible: false

                }
                Rectangle {
                    id: mask
                    color: "black"
                    radius: 10
                    width: grid.cellWidth-5
                    height:grid.cellHeight-5
                    visible: false
                }
                OpacityMask {
                    anchors.fill: material
                    source: material
                    maskSource: mask
                    visible: true
                }
                Rectangle{
                    id:titleRec
                    width: parent.width
                    height:60
                    anchors.top:material.bottom
                    Text {
                        id: titleText
                        width: parent.width
                        anchors.verticalCenter:titleRec.verticalCenter
                        text: modelData[2]
                        padding: 10
                        wrapMode:Text.Wrap
                        font.pixelSize: 18
                    }
                }
                Image {
                    id: profileImg
                    source:"image://material/" + modelData[5];
                    width: 50
                    height: 50
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.top:titleRec.bottom
                    visible: false
                }
                Rectangle {
                    id: _mask
                    color: "black"
                    width: 50
                    height: 50
                    radius: 50
                    anchors.top:titleRec.bottom
                    visible: false
                }
                OpacityMask {
                    anchors.fill: profileImg
                    source: profileImg
                    maskSource: _mask
                    visible: true
                }
                Rectangle{
                    id:nickRec
                    height: 50
                    anchors.left: profileImg.right
                    anchors.top:titleRec.bottom
                    Text {
                        id: nicknameText
                        text: modelData[4]
                        padding: 10
                        font.pixelSize: 18
                        anchors.verticalCenter:nickRec.verticalCenter
                    }
                }
                TapHandler{
                    onTapped: {
                        control.requestNoteDetail();
                        var detail_data=[profileImg.source,nicknameText.titleText.text,id_,material.source]
                        stack.push("NoteDetailPage.qml",{"data":detail_data})
                    }
                }


            }
            onContentYChanged: {
                //下拉刷新判断逻辑：已经到头了，还下拉一定距离
                if (contentY < originY){
                    var top_offset = contentY - originY;
                    if (top_offset < -10){
                        needReflesh=true;
                    }
                }else{
                    needReflesh=false
                }
                //上拉加载判断逻辑：还差两条笔记就刷完，还上拉一定距离
                if (contentHeight>height && contentY-originY > (contentHeight-grid.cellHeight)-height){
                    needLoadMore = true;
                }else{
                    needLoadMore = false;
                }
            }
            onNeedRefleshChanged: {
                if(needReflesh){
                    console.log("刷新")
                    control.getNotes()
                }
            }
            onNeedLoadMoreChanged: {
                if(needLoadMore){
                    console.log("加载更多")
                    //control.getNotes()
                }


            }
        }
    }
}

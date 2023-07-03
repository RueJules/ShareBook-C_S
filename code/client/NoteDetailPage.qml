import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
Page {
    id:pge
    property var detail
    property alias data:pge.detail
    background: Rectangle{
        anchors.fill: parent
        color:"#fdfbfb"
    }
    Connections {
        target: control
        function onGetNoteDetial(noteDetial){
            //grid.model=newNotes;
        }
    }
    visible: true
    header:Item{
        id:itm
        height: root.height/20
        RowLayout {
            id:row
            anchors.fill: parent
            spacing: -40
            Button{
                id:btn
               // width:ite
                Layout.maximumHeight: itm.height
                Layout.minimumHeight: itm.height
                Layout.maximumWidth: itm.height
                Layout.minimumWidth: itm.height
                Layout.fillHeight: true
                Layout.fillWidth: true
//                Layout.rightMargin: 1
                Layout.leftMargin:5
                Layout.alignment: Qt.AlignLeft
                flat: true
                icon.source: "qrc:/UI/icon/return.svg"
                icon.width: itm.height * 0.7
                icon.height: itm.height * 0.7
                onClicked: {
                    stack.clear()
                }
            }
            Item{
                id:rec
                Layout.maximumHeight: itm.height
                Layout.minimumHeight: itm.height
                Layout.maximumWidth: itm.height
                Layout.minimumWidth: itm.height
                //Layout.leftMargin: 1
                //Layout.alignment: Qt.AlignLeft
                Image {
                    id: d_profile
                    source: detail[0]
                    anchors.fill: parent
                    visible: false
                }
                Rectangle {
                    id: d_mask
                    color: "black"
                    //anchors.fill: parent
                    width: itm.height
                    height: itm.height
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    radius: 50
                    visible: false
                    //Layout.leftMargin: 1
                }
                OpacityMask {
                    //anchors.fill: d_profile
                    width: itm.height
                    height: itm.height
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    //Layout.leftMargin: 1
                    source: d_profile
                    maskSource: d_mask
                    visible: true
                    //Layout.alignment: Qt.AlignLeft
                }
            }
            Text{
                id:nickname
                Layout.leftMargin: 3
                //Layout.alignment: Qt.AlignLeft
                text:detail[1]
            }
            Button{
                id:gofollow
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 20
                flat: true
                text: qsTr("follow")
                contentItem: Text {
                       id: text2
                       font: gofollow.font
                       text: gofollow.text
                       color: "red"
                       horizontalAlignment: Text.AlignHCenter
                       verticalAlignment: Text.AlignVCenter
                   }
                background: Rectangle{
                    id:bgd
                    implicitWidth: itm.height*2.5
                    implicitHeight: itm.height * 0.7
                    border.color: "red"
                    radius: 20
                }
                onClicked: {
                    text2.text = text2.text === "follow" ? "followed" : "follow"
                }
            }
        }
    }

    ScrollView{
        id:sv
        anchors.fill: parent
        anchors.topMargin: 3
        contentHeight: view.height + indicator.height + titleArea.height + contentArea.height + note_time.height + pge.height*0.2
        SwipeView {
            id:view
            width: parent.width
            height: 700
            Image{
                fillMode: Image.PreserveAspectFit
                source:detail[6]
            }
            Repeater{
                model:control.getNoteDetails(Number(detail[5]))  //c++端传来的素材路径
                Image{
                    fillMode: Image.PreserveAspectFit
                    source:modelData
                }
            }
        }
        //只有一张图片就不要这个
        PageIndicator {
            id: indicator
            count: view.count
            currentIndex: view.currentIndex
            anchors.top: view.bottom
            anchors.margins: 10
            anchors.horizontalCenter: parent.horizontalCenter
            visible: count===1?false:true
        }
        Text{
            id:titleArea
            width: parent.width
            anchors.top: indicator.bottom
            anchors.topMargin: 10
            leftPadding: 20
            rightPadding: 20
            font.bold: true
            font.pointSize: 20
            wrapMode:Text.Wrap
            text:detail[2]
        }

        Text{
            id:contentArea
            anchors.top: titleArea.bottom
            anchors.topMargin: 10
            width: parent.width
            leftPadding: 20
            rightPadding: 20
            font.pointSize: 15
            wrapMode:Text.Wrap
            text:detail[3]

        }
        Text{
            id:note_time
            width: parent.width
            color: "#4d4747"
            anchors.top: contentArea.bottom
            anchors.topMargin: 20
            leftPadding: 20
            rightPadding: 20
            font.family: "Courier"
            font.pointSize: 15
            wrapMode:Text.Wrap
            text:detail[4]
        }
    }
}

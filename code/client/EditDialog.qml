import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
Page {
    id:edit
//    color:"#fdfbfb"
    background: Rectangle{
        anchors.fill: parent
        color:"#fdfbfb"
    }
    property var pics: ["qrc:/resource_file/materials/mmexport1683778025723.jpg", "qrc:/resource_file/materials/mmexport1684237281162.jpg", "qrc:/resource_file/materials/mmexport1684937628890.jpg", "qrc:/resource_file/materials/mmexport1685247537325.jpg", "qrc:/resource_file/materials/mmexportf2b0d6afdc7183cb80fa7005dfe2e261_1668357584729.jpeg"]

    Rectangle{
        id:title
        color: "#fdfbfb"
        width:parent.width
        height: 60
        anchors.top: parent.top
        Text {
            id: name
            anchors.centerIn: parent
            color:"#596164"
            text: qsTr("发布笔记")
        }
        MyButton{
            id:publish
            background: Rectangle{
                anchors.fill: parent
                radius:10
                color: "#fdfbfb"
            }
            anchors.right: parent.right
            width: 40
            show:"qrc:/UI/icon/checkmark.svg"
            icon_color:"#009688"
            onClicked: {
                console.log(textfield_title.text)
//                if(textarea_content.text===""&&素材也为空)------>提示用户必须选择一个输入
                //如果只有图片，自动添加标题“分享图片”
                if(textfield_title.text===""&&textarea_content.text==="")
                {
                    //textfield_title.text="分享图片"
                }
                //如果有内容但没有标题，标题为内容的前二十个字
                else{
                    if(textfield_title.text===""){
                        textfield_title.text=textarea_content.text.substring(0,20)
                    }
                    control.requestPublish(textfield_title.text,textarea_content.text,[]);
                    stack.clear()
                }
            }
        }
        MyButton{
            id:exit
            background: Rectangle{
                anchors.fill: parent
                radius:10
                color: "#fdfbfb"
            }
            anchors.left: parent.left
            width: 40
            show:"qrc:/UI/icon/cross.svg"
            icon_color:"#596164"
            onClicked: {
                stack.clear();
            }
        }
    }
    //存放选择的素材，是横着的list
    ListView{
        id:materialsView
        anchors.top: title.bottom
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.leftMargin: 15
        //anchors.right: parent.right
        spacing: 5
        width: parent.width
        height:edit.width / 3.3
        orientation:ListView.Horizontal
        model:mymodel
        delegate: Rectangle {
            id:dgt
            radius: 7
            border.color: "lightgrey"
            color: "#fdfbfb"
            width: edit.width / 3.3
            height:edit.width / 3.3
            Image {
                id: material
                source: modelData
                width: parent.width
                height:parent.height
                fillMode: Image.PreserveAspectCrop //缩放以适应屏幕
                visible: false
            }
            Rectangle {
                id: mask
                color: "black"
                radius: 7
                width: dgt.width
                height:dgt.height
                visible: false
            }
            OpacityMask {
                anchors.fill: material
                source: material
                maskSource: mask
                visible: true
        }
     }
        footer: Button{
            //radius: 10
            //border.color: "grey"
            width: edit.width / 3.3
            height:edit.width / 3.3
            background: Rectangle{
                anchors.fill: parent
                border.width: 0
                color: "#F8F8F8"
            }
            Image {
                id: add
                source: "qrc:/UI/icon/add-rectangle.svg"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width / 2
                height: parent.width / 2
            }
            onClicked: {
                var source1 = randomPcture()
                mymodel.append({"source":source1})
                //真是实现应该是加载（显示）本地图库里面的素材供网民选取
            }
        }
    }
    TextField{
        id: textfield_title
        width: parent.width
        anchors.top: materialsView.bottom
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
//        border:
        padding: 15
        //无输入时显示的文本
        placeholderText: "输入标题（20个字）"
        placeholderTextColor: "gray"
        selectByMouse: true
        //选中文本的颜色
        selectedTextColor: "white"
        //选中文本背景色
        selectionColor: "#009688"
        font{
            family: "SimSun"
            pixelSize: 23
            bold:true
        }
        //设置最多可输入的字符
        maximumLength:20
        //默认Text.QtRendering看起来比较模糊
        renderType: Text.NativeRendering
        background: Rectangle{
            anchors.fill: parent
            opacity: 0
            border.width: 0
        }
    }
    Rectangle{
        id:line2
        width: parent.width-30
        anchors.horizontalCenter: parent.horizontalCenter
        height: 3
        anchors.top:textfield_title.bottom
        color: "#53B7AF"
        opacity: 0.8
    }
    ScrollView{
        id:textfield_content
        width: parent.width
        height: parent.height / 4
        anchors.top: line2.bottom
        TextArea{
            padding: 15
            //无输入时显示的文本
            placeholderText: "输入文本内容（500个字）"
            placeholderTextColor: "gray"
            selectByMouse: true
            //选中文本的颜色
            selectedTextColor: "white"
            //选中文本背景色
            selectionColor: "#009688"
            font{
                family: "SimSun"
                pixelSize: 23
            }
            //maximumLength:500//超出可输入的文本有提示
            wrapMode:TextEdit.Wrap
       }
    }
    //井号后边的变色
    TextField{
        id:keyWords
        width: parent.width
        padding: 20
        anchors.top: textfield_content.bottom
        font{
            family: "SimSun"
            pixelSize: 23
        }
        placeholderText: "#关键字"
        placeholderTextColor: "gray"
        selectByMouse: true
        background: Rectangle{
            anchors.fill: parent
            opacity: 0
            border.width: 0
        }
    }
    Rectangle{
        id:line3
        width: parent.width
        height: 3
        anchors.top:keyWords.bottom
        color: "#53B7AF"
        opacity: 0.8
    }
    ListModel{
        id:mymodel

    }
    function randomPcture(){
        var random = Math.floor(Math.random()*pics.length)
        console.log(pics[random])
        return pics[random]

    }
}

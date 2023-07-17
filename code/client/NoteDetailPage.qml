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
//    Component.onCompleted: {
//        console.log("查看评论\n\n\n")
//        control.requestCommentDetail(detail[3],0);
//    }

    Connections {
        target: control
        function onGetNoteDetail(noteDetail){
            contentArea.text=noteDetail[0][0];
            repeater.model=noteDetail[1];
            control.requestCommentDetail(detail[3],0)
        }
        function onGetCommentDetail(commentDetial){
            //commentListView
            commentListView.model=commentDetial
        }
    }
    InfoDialog {
        id:errorDialog
        width: root.width*0.6
        height: root.width*0.5
        visible:false
        onClosed: {
            errorDialog.visible=false;
        }
    }
    visible: true
    //头部有博主的头像、昵称和关注的渠道
    header:Item{
        id:itm
        height: root.height/20
        RowLayout {
            id:row
            anchors.fill: parent
            spacing: -40
            Button{
                id:btn
                Layout.maximumHeight: itm.height
                Layout.minimumHeight: itm.height
                Layout.maximumWidth: itm.height
                Layout.minimumWidth: itm.height
                Layout.fillHeight: true
                Layout.fillWidth: true
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
                Image {
                    id: d_profile
                    source: detail[0]
                    anchors.fill: parent
                    visible: false
                }
                Rectangle {
                    id: d_mask
                    color: "black"
                    width: itm.height
                    height: itm.height
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    radius: 50
                    visible: false
                }
                OpacityMask {
                    width: itm.height
                    height: itm.height
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    source: d_profile
                    maskSource: d_mask
                    visible: true
                }
                //点击头像会跳转到个人主页
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        //loader.source="PersonalPage.qml"

                    }
                }
            }
            Text{
                id:nickname
                Layout.leftMargin: 3
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
    //笔记详细信息，素材、标题、内容、时间
    ScrollView{
        id:sv
        anchors.fill: parent
        anchors.topMargin: 3
        contentHeight: view.height + indicator.height + titleArea.height + contentArea.height + note_time.height +commentListView.height
        SwipeView {
            id:view
            width: parent.width
            height: 700
            currentIndex: indicator.currentIndex
            Image{
                width: view.width
                height: 700
                fillMode: Image.PreserveAspectFit
                source:detail[4]
            }

            Repeater{
                id:repeater
                Image{
                    width: view.width
                    height: 700
                    fillMode: Image.PreserveAspectFit
                    source:"image://material/" + modelData;
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
            text:detail[5]
        }
        ListView{
            id:commentListView
            property bool needLoadMore:false
            anchors.top: note_time.bottom
            anchors.topMargin: 50
            anchors.left: parent.left
            anchors.leftMargin: 20
            spacing: 10
            width: root.width-20
            height: root.height

            delegate: Rectangle {
                color: "#fdfbfb"
                width: parent.width
                height: root.height/20
                Image {
                    id: comment_netizen_profile
                    source: "image://profile/"+modelData[0]
                    width: parent.height
                    height:parent.height
                    fillMode: Image.PreserveAspectCrop //缩放以适应屏幕
                    visible: false
                }
                Rectangle {
                    id: comment_mask
                    color: "black"
                    radius: 50
                    width: parent.height
                    height:parent.height
                    visible: false
                }
                OpacityMask {
                    anchors.fill: comment_netizen_profile
                    source: comment_netizen_profile
                    maskSource: comment_mask
                    visible: true
                }
                Text{
                    id:comment_netizen_nickname
                    text:modelData[1]
                    font.pixelSize: 12
                    color:"#4d4747"
                    anchors.left: comment_netizen_profile.right
                    anchors.leftMargin: 5
                    anchors.top:parent.top
                    anchors.topMargin: 10
                }
                Text{
                    id:comment_content
                    text:modelData[2]
                    font.pixelSize: 12
                    anchors.left: comment_netizen_profile.right
                    anchors.leftMargin: 5
                    anchors.bottom:parent.bottom
                    anchors.bottomMargin: 10
                }
                Text{
                    id:comment_time
                    text:modelData[3]
                    font.pixelSize: 8
                    color: "#4d4747"
                    anchors.left: comment_content.right
                    anchors.leftMargin: 20
                    anchors.bottom:parent.bottom
                    anchors.bottomMargin: 10
                }

    //            //一个填充头像部分，点击头像跳转到个人主页
    //            MouseArea{
    //            }
    //            //一个填充评论内容部分，点击内容回复该条评论
    //            MouseArea{
    //                onClicked: {
    //                    reply_popup.open();
    //            textarea_reply.text: "回复@"+comment_netizen_nickname.text
    //                }
    //            }
            }

            //下拉到底端的时候再次获取评论，control->requestCommentDetail(detail[3],commentListView.count)
            onContentYChanged: {
                //上拉加载判断逻辑：还差两条笔记就刷完，还上拉一定距离
                if (contentHeight>height && contentY-originY > (contentHeight-root.height/20)-height){
                    needLoadMore = true;
                }else{
                    needLoadMore = false;
                }
            }
            onNeedLoadMoreChanged: {
                if(needLoadMore){
                    console.log("加载更多");
                    control.requestCommentDetail(detail[3],commentListView.count)
                }
            }
        }
    }
    //评论列表

    //评论输入框
    Popup {
        id: comment_popup
        //anchors.bottom:parent.bottom
        //height: root.height/20
        width: parent.width
        height: 160
        modal: true
        focus: true
        closePolicy: Popup.CloseOnPressOutside
        x:0
        y:parent.height-comment_popup.height+detail_footer.height
        background: Rectangle{
            anchors.fill: parent
            color: "white"
        }

        ScrollView{
            id:textfield_comment
            width: parent.width*0.7
            height:parent.height-50
            anchors.left: parent.left
            anchors.margins: 20
            anchors.verticalCenter: parent.verticalCenter
            TextArea{
                id:textarea_comment
                background: Rectangle{
                    //anchors.fill: parent
                    color: "#eee"
                    radius: 10
                }
                padding: 15
                //无输入时显示的文本
                placeholderText: "输入文本内容（限300个字）"
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
        Button{
            id:publish_comment
            width: parent.width*0.2
            height: parent.height*0.3
            anchors.left: textfield_comment.right
            anchors.bottom: parent.bottom
            anchors.margins: 20
            background: Rectangle{
                anchors.fill: parent
                color: "#4197ec"
                radius: 20
            }
            Text {
                text: qsTr("发送")
                font{
                    family: "SimSun"
                    bold: true
                    pixelSize: 23
                }
                color: "#fbfbfb"
                anchors.centerIn: parent
            }
            onClicked: {
                //请求发布评论
                if(textarea_comment.text==="")//------>提示用户必须输入
                {
                    errorDialog.text="评论的内容不能为空";
                    errorDialog.visible=true;
                    return;
                }
                else
                {
                    control.requestPublishComment(textarea_comment.text,detail[3]);
                }
            }
        }
    }
    //回复输入框
    Popup {
        id: reply_popup
        //anchors.bottom:parent.bottom
        //height: root.height/20
        property var parent_id
        property var top_id
        width: parent.width
        height: 160
        modal: true
        focus: true
        closePolicy: Popup.CloseOnPressOutside
        x:0
        y:parent.height-reply_popup.height+detail_footer.height
        background: Rectangle{
            anchors.fill: parent
            color: "white"
        }

        ScrollView{
            id:textfield_reply
            width: parent.width*0.7
            height:parent.height-50
            anchors.left: parent.left
            anchors.margins: 20
            anchors.verticalCenter: parent.verticalCenter
            TextArea{
                id:textarea_reply
                background: Rectangle{
                    //anchors.fill: parent
                    color: "#eee"
                    radius: 10
                }
                padding: 15
                //无输入时显示的文本
                //placeholderText: "回复@"+
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
        Button{
            id:publish_reply
            width: parent.width*0.2
            height: parent.height*0.3
            anchors.left: textfield_reply.right
            anchors.bottom: parent.bottom
            anchors.margins: 20
            background: Rectangle{
                anchors.fill: parent
                color: "#4197ec"
                radius: 20
            }
            Text {
                text: qsTr("发送")
                font{
                    family: "SimSun"
                    bold: true
                    pixelSize: 23
                }
                color: "#fbfbfb"
                anchors.centerIn: parent
            }
            onClicked: {
                //请求发布评论
                if(textarea_reply.text==="")//------>提示用户必须输入
                {
                    errorDialog.text="回复的内容不能为空";
                    errorDialog.visible=true;
                    return;
                }
                else
                {
                    //control.requestPublishReply(textarea_comment.text,detail[3],parent_id,top_id);
                }
            }
        }
    }
    footer:Rectangle{
        id:detail_footer
        height: root.height/20
        Button{
            id:input_comment
            width: parent.width*0.4
            height: parent.height-10
            anchors.leftMargin: 10
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            background: Rectangle{
                anchors.fill: parent
                color: "#eee"
                radius: 20
            }
            Image{
                id:comment_image
                height: parent.height-20
                width: height
                source: "qrc:/UI/icon/pencil.svg"
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 20
                anchors.left: parent.left
            }
            ColorOverlay {
                anchors.fill: comment_image
                source: comment_image
                color: "#6a6a6a"
            }
            Text {
                text: qsTr("说点什么")
                font{
                    family: "SimSun"
                    pixelSize: 18
                }
                color: "#6a6a6a"
                anchors.left: comment_image.right
                anchors.leftMargin: 5
                anchors.verticalCenter: parent.verticalCenter
            }

            onClicked: {
                //弹出评论输入框
                comment_popup.open()
            }
        }

        MyButton{
            id:like
            anchors.left: input_comment.right
            background: Rectangle{
                anchors.fill: parent
                radius:10
                color: "#fdfbfb"
            }
            width: parent.width*0.1
            show:"qrc:/UI/icon/heart.svg"
            onClicked: {
                //该笔记的点赞数量加一
               icon_color="red"
            }
        }
        Text{
            id:like_count
            anchors.left: like.right
            //点赞的数量
            width: parent.width*0.1
            text:"0"
            color:"#596164"
            font.pointSize: 18
            anchors.verticalCenter: parent.verticalCenter
        }

        MyButton{
            id:collect
            anchors.left: like_count.right
            background: Rectangle{
                anchors.fill: parent
                radius:10
                color: "#fdfbfb"
            }
            width: parent.width*0.1
            show:"qrc:/UI/icon/star-full.svg"
            onClicked: {
                //该笔记的收藏数量加一
               icon_color="yellow"
            }
        }
        Text{
            //收藏的数量
            id:collect_count
            anchors.left: collect.right
            width: parent.width*0.1
            text:"0"
            color:"#596164"
            font.pointSize: 18
            anchors.verticalCenter: parent.verticalCenter
        }
        MyButton{
            id:comment
            anchors.left: collect_count.right
            background: Rectangle{
                anchors.fill: parent
                radius:10
                color: "#fdfbfb"
            }
            width: parent.width*0.1
            show:"qrc:/UI/icon/bubble.svg"
//                onClicked: {
//                    stack.clear();
//                }
        }
        Text{
            //评论的数量
            id:comment_count
            anchors.left: comment.right
            width: parent.width*0.1
            text:"0"
            color:"#596164"
            font.pointSize: 18
            anchors.verticalCenter: parent.verticalCenter
        }

    }
    //发布评论结果提示框
    Popup{
        id:publishComment_result
        width: 200
        height:160
        anchors.centerIn: parent
        modal: true
        focus: true
        //closePolicy: Popup.CloseOnPressOutside
        background: Rectangle{
            anchors.fill: parent
            radius: 20
            color: "white"
        }
        Text{

            id:publishComment_result_text
            color: "gray"
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

        }

        Timer {
            id:comment_timer
            interval: 2000 // 设置5秒钟的超时时间
            repeat: true
            onTriggered:{
                publishComment_result.close() // 触发时关闭Popup
                comment_timer.running=false
                if(publishComment_result_text.text==="发布成功"){
                    publishComment_result_text.text="";
                    textarea_comment.clear();
                    comment_popup.close()
                }
            }
            running: true // 启动计时器
        }
    }
    //发布回复结果提示框
    Popup{
        id:publishReply_result
        width: 200
        height:160
        anchors.centerIn: parent
        modal: true
        focus: true
        //closePolicy: Popup.CloseOnPressOutside
        background: Rectangle{
            anchors.fill: parent
            radius: 20
            color: "white"
        }
        Text{

            id:publishReply_result_text
            color: "gray"
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

        }

        Timer {
            id:reply_timer
            interval: 2000 // 设置5秒钟的超时时间
            repeat: true
            onTriggered:{
                publishReply_result.close() // 触发时关闭Popup
                reply_timer.running=false
                if(publishReply_result_text.text==="发布成功"){
                    publishReply_result_text.text="";
                    textarea_reply.clear();
                    reply_popup.close()
                }
            }
            running: true // 启动计时器
        }
    }

    Connections {
        target: control
        function onGetPublishCommentResult(res){
            if(res){
                //提示发布成功，关闭发布笔记的编辑框
                publishComment_result_text.text="发布成功"
                publishComment_result.open();
                comment_timer.running=true

            }else{
                //提示发布失败，停留在发布笔记的编辑框
                publishComment_result_text.text="发布失败"
                publishComment_result.open();
                comment_timer.running=true
            }
        }
        function onGetPublishReplyResult(res){
            if(res){
                //提示发布成功，关闭发布笔记的编辑框
                publishReply_result_text.text="发布成功"
                publishReply_result.open();
                reply_timer.running=true

            }else{
                //提示发布失败，停留在发布笔记的编辑框
                publishReply_result_text.text="发布失败"
                publishReply_result.open();
                reply_timer.running=true
            }
        }
    }

}

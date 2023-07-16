import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id:root
    width: 543
    height: 1000
    visible: true
    title: qsTr("享书---享你所想")
    background: Rectangle{
        id: loginFrame
        anchors.fill: parent
        gradient:Gradient {
            id: gradient
            GradientStop { position: 0.0; color: "#FFB6C1" }
            GradientStop { position: 1.0; color: "#87CEEB" }
        }
    }
    Loader {
        id: login_loader
        anchors.fill: parent
        asynchronous: true
        source: "Main.qml"
        visible: false
    }
    StackView{
        id:stack
        anchors.fill: parent
        transform: Translate{ id:translate }
        PropertyAnimation {
            id:enter
            target: translate
            property: "y"
            from: 1000
            to: 0
            duration: 300
        }
    }
    Column {
        id:column
        spacing: 20
        anchors.centerIn: parent
        TextField {
            id: usernameInput
            placeholderText: "请输入账号"
            placeholderTextColor: "gray"
            width: root.width*0.8
            height: root.width*0.1

            font.pixelSize: 23
            background: Rectangle{
                anchors.fill: parent
                color: "white"
                border.width: 0
                radius: 10
            }
            maximumLength:20
        }

        TextField {
            id: passwordInput
            placeholderText: "请输入密码"
            placeholderTextColor: "gray"
            width: root.width*0.8
            height: root.width*0.1

            font.pixelSize: 23
            background: Rectangle{
                anchors.fill: parent
                color: "white"
                border.width: 0
                radius: 10
            }
            echoMode: TextInput.Password
            maximumLength:20
        }

        Button {
            id: button
            text: "登录"
            width: root.width*0.8
            height: root.width*0.1
            font.pixelSize: 18
            background: Rectangle{
                anchors.fill: parent
                color: "#FFB6C1"
                radius: 10
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: { button.state = "pressed" }
                    onReleased: { button.state = "normal" }
                }

                states: [
                    State {
                        name: "normal"
                        PropertyChanges { target: button; opacity: 1 }
                    },

                    State {
                        name: "pressed"
                        when: button.pressed
                        PropertyChanges { target: button; opacity: 0.6 }
                    }
                ]
            }
            onClicked: {
                // 登录逻辑处理
                if(usernameInput.text!==""&passwordInput.text!==""){
                    var nickname=usernameInput.text;
                    var password=passwordInput.text;
                    control.requestLogin(nickname,password);

                }
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

    }
    Connections {
        target: control
        function onGetAccountInfo(res){
            if(res){
                //跳转首页
                column.destroy();
                login_loader.visible=true;
                control.requestNotes()
            }else{
                //提示信息错误
                errorDialog.text="用户名或密码输入错误"
                errorDialog.visible=true;
            }
        }
    }
}

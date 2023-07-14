import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    Loader {
        id: loader
        anchors.fill: parent
        asynchronous: true
        source:"Home.qml"

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

    footer:Rectangle{
            width: root.width
            height:50
            color:"white"
            TabBar {
            id:tabBar
            anchors.fill: parent
            currentIndex:0
            MyButton{
                id:home_btn
                show:"qrc:/UI/icon/home.svg"
                icon_color: tabBar.currentIndex===0?"#009688":"#596164"
                onClicked:{
                    loader.source="Home.qml";
                    stack.clear();
                }
            }
            MyButton{
                id:video_btn
                show:"qrc:/UI/icon/film.svg"
                icon_color: tabBar.currentIndex===1?"#009688":"#596164"
                onClicked:{
                    loader.source="Video.qml";
                    stack.clear();
                }
            }
            MyButton{
                id:new_btn
                show:"qrc:/UI/icon/plus.svg"
                icon_color: "#ffffff"
                background: Rectangle{
                    anchors.margins: 5
                    anchors.fill: parent
                    color: "#009688"
                    radius: 10
                }
                onClicked:{
                    stack.push("EditDialog.qml")
                    enter.start()
                }
            }
            MyButton{
                id:message_btn
                show:"qrc:/UI/icon/envelop.svg"
                icon_color: tabBar.currentIndex===3?"#009688":"#596164"
                onClicked:{
                    loader.source="Message.qml";
                    stack.clear();
                }
            }
            MyButton{
                id:personal_btn
                show:"qrc:/UI/icon/evil.svg"
                icon_color: tabBar.currentIndex===4?"#009688":"#596164"
                onClicked:{
                    loader.source="PersonalPage.qml";
                    stack.clear();
                }
            }
        }
    }
}

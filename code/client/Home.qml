import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
Page{
    id:home
    header:
        Rectangle{
            width: home.width
            height:40
            color:"white"
            RowLayout{
                anchors.fill: parent
                Button{
                    id:menu_list
                    Layout.fillHeight:true
                    Layout.alignment: Qt.AlignLeft
                    Image{
                        height: parent.height-15
                        width: parent.height-15
                        anchors.centerIn: parent
                        source:"qrc:/UI/icon/list2.svg"
                    }
                    onClicked:{
                        //loader.source="Video.qml";
                    }

                }
                TabBar{
                    id:tabBar
                    currentIndex: 1
                    Layout.fillHeight:true
                    Layout.alignment:Qt.AlignHCenter
                    MyButton{
                        id:concern
                        text: "关注"
                        //color: tabBar.currentIndex===0?"#009688":"#596164"
                    }
                    MyButton{
                        id:recommend
                        text:"推荐"
                    }
                    MyButton{
                        id:nearby
                        text:"附近"
                    }

                }

                Button{
                    id:search
                    Layout.fillHeight:true
                    Layout.alignment:Qt.AlignRight
                    Image{
                        height: parent.height-15
                        width: parent.height-15
                        anchors.centerIn: parent
                        source:"qrc:/UI/icon/search.svg"
                    }
                    onClicked:{
                        //loader.source="Video.qml";
                    }

                }

            }
        }

    SwipeView {
        id: view
        currentIndex: tabBar.currentIndex
        anchors.fill: parent
        onCurrentIndexChanged: {
            tabBar.currentIndex = currentIndex
        }
            //第一页
            Concerned{
//                width: view.width-7
//                height: view.height
//                color:"red"
            }
            //第二页
            Recommend{
//                width: view.width
//                height: view.height
            }

            //第三页
            Nearby{

//                color:"green"
            }
        }


}

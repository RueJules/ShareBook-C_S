import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


Dialog {
    id:dialog
    property string contentText: "用户名或密码输入错误"
    property color backgroundColor: "#404040"
    property color textColor: "white"

    anchors.centerIn: Overlay.overlay   /* 居中展现 */

    title: qsTr("提示")
    modal: true /* 非模态对话框 */

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    /* 背景 */
    background: Rectangle {
        color: dialog.backgroundColor
        border.color: "#404040"
        border.width: 2
        radius: 8
    }

    /* 标题 */
    header: Item {
        height:parent.height*0.2
        Text {
            id: titleLabel
            anchors.centerIn: parent
            text: dialog.title
            color: dialog.textColor
            font.pointSize: 15
            verticalAlignment: Text.AlignVCenter
            topPadding: 4
            bottomPadding: topPadding
        }

        /* 水平分隔线 */
        Rectangle {
            id: horizontalLine
            width: parent.width
            height: 1
            anchors.top: titleLabel.bottom
            color: "#606060"
        }
    }

    /* 内容 */

    Text {
        text: dialog.contentText
        color: dialog.textColor
        anchors.centerIn: parent
        font.pointSize: 10
        lineHeight: 1.5
        wrapMode: Text.WordWrap
        Layout.maximumWidth: 300
    }


    /* 对话框按钮 */
    footer: DialogButtonBox {
        buttonLayout: DialogButtonBox.WinLayout
        alignment: Qt.AlignHCenter
        Button {
            id: control
            text: qsTr("确定")
            /** 文本颜色 */
            property color textColor: "#4CC5EF"

            /** 边框颜色 */
            property color borderColor: "white"

            /** 背景色 */
            property color bgColor: "transparent"

            leftPadding: 16 * 0.2
            rightPadding: 16 * 0.2
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            contentItem: Text {
                text: control.text
                font: control.font
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: control.textColor
                elide: Text.ElideRight
                opacity: enabled ? 1.0 : 0.6
                leftPadding: control.leftPadding
                rightPadding: control.rightPadding
            }

            background: Rectangle {
                color: control.bgColor
                border.width: control.hovered ? 1.5 : 1
                border.color: control.borderColor
                radius: height * 0.1
                opacity: enabled ? 0.8 : 0.5
            }
        }


        background: Rectangle {
            color: dialog.backgroundColor
            radius: 8
        }
    }

    onAccepted: {
        dialog.close();   /* 点击关闭 */
    }
}

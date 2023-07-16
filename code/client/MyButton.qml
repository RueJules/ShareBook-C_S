import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

TabButton{
    property alias show: image.source
    property alias icon_color:overlay.color
    anchors.top: parent.top
    anchors.bottom:parent.bottom
    Image{
        id:image
        height: parent.height-20
        width: parent.height-20
        anchors.centerIn: parent
    }
    ColorOverlay {
        id:overlay
        anchors.fill: image
        source: image
        color: "#596164"
    }
}

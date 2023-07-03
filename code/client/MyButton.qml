import QtQuick

import QtQuick 2.0
import QtQuick.Controls 2.2
import Qt5Compat.GraphicalEffects

TabButton{
    property alias show: image.source
    property alias icon_color:overlay.color
    anchors.top: parent.top
    anchors.bottom:parent.bottom
    Image{
        id:image
        height: parent.height-30
        width: parent.height-30
        anchors.centerIn: parent
    }
    ColorOverlay {
        id:overlay
        anchors.fill: image
        source: image
        color: "#596164"
    }
}

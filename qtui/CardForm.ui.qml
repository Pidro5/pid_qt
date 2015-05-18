import QtQuick 2.4

Item {
    id: item
    // Actual size of the PNG files is 224 x 312
    width: 112
    height: 156

    property alias source: image.source

    Image {
        id: image
        clip: false
        anchors.fill: parent
    }
}


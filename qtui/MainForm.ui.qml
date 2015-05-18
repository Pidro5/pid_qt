import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    id: form
    width: 1280
    height: 960

    property alias button3: button3
    property alias button2: button2
    property alias button1: button1

    Rectangle {
        id: board
        color: "#45f422"
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

        Rectangle {
            id: rectangleAlice
            x: 0
            width: 150
            color: "#f71a1a"
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            Text {
                id: textAlice
                text: qsTr("Alice")
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 150
                font.pixelSize: 20
            }
        }

        Rectangle {
            id: rectangleCarlos
            x: 919
            width: 150
            color: "#073aea"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.right: rectangleScore.left
            anchors.rightMargin: 0

            Text {
                id: textCarlos
                text: qsTr("Carlos")
                anchors.top: parent.top
                anchors.topMargin: 150
                font.pixelSize: 20
            }
        }

        Rectangle {
            id: rectangleBob
            height: 150
            color: "#37801b"
            anchors.left: rectangleAlice.right
            anchors.leftMargin: 0
            anchors.right: rectangleCarlos.left
            anchors.rightMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0

            Text {
                id: textBob
                text: qsTr("Bob")
                font.pixelSize: 20
            }
        }

        Rectangle {
            id: rectangleVictor
            y: 623
            height: 150
            color: "#dd07c5"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.right: rectangleCarlos.left
            anchors.rightMargin: 0
            anchors.left: rectangleAlice.right
            anchors.leftMargin: 0

            Text {
                id: textVictor
                text: qsTr("Victor")
                font.pixelSize: 20
            }
        }

        RowLayout {
            x: 156
            y: 228
            anchors.top: parent.top
            anchors.topMargin: 121
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 337
            anchors.right: parent.right
            anchors.rightMargin: 280
            anchors.left: parent.left
            anchors.leftMargin: 126

            Button {
                id: button1
                text: qsTr("Press Me 1")
            }

            Button {
                id: button2
                text: qsTr("Press Me 2")
            }

            Button {
                id: button3
                text: qsTr("Press Me 3")
            }
        }

    Rectangle {
        id: rectangleScore
        x: 927
        width: 300
        color: "#ffffff"
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
    }

    }
}

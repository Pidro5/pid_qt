import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    id: form
    width: 1280
    height: 960

    property alias buttonExit: buttonExit
    property alias buttonAbout: buttonAbout

    property alias rectangleAlice: rectangleAlice
    property alias rectangleAliceHiddenCards: rectangleAliceHiddenCards

    property alias rectangleBob: rectangleBob
    property alias rectangleBobHiddenCards: rectangleBobHiddenCards

    property alias rectangleCarlos: rectangleCarlos
    property alias rectangleCarlosHiddenCards: rectangleCarlosHiddenCards

    property alias rectangleVictor: rectangleVictor
    property alias rectangleVictorHiddenCards: rectangleVictorHiddenCards

    property alias bid: rowLayoutBid;

    property alias bidBob: bidBob;
    property alias bidAlice: bidAlice;
    property alias bidCarlos: bidCarlos;


    Rectangle {
        id: board
        color: "#2db612"
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
            color: "#2db612"
            opacity: 1
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

            Rectangle {
                id: rectangleAliceHiddenCards
                x: 0
                y: 189
                width: 68
                height: 565
                color: "#2db612"
                clip: false
                opacity: 1
            }

            Rectangle {
                id: rectangleAliceVisibleCards
                x: 74
                y: 189
                width: 68
                height: 565
                color: "#2db612"
                opacity: 1
            }
        }

        Rectangle {
            id: rectangleCarlos
            x: 919
            width: 150
            color: "#2db612"
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

            Rectangle {
                id: rectangleCarlosHiddenCards
                x: 80
                y: 189
                width: 68
                height: 565
                color: "#2db612"
                opacity: 1
            }

            Rectangle {
                id: rectangleCarlosVisibleCards
                x: 0
                y: 189
                width: 68
                height: 565
                color: "#2db612"
                opacity: 1
            }
        }

        Rectangle {
            id: rectangleBob
            height: 150
            color: "#2db612"
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

            Rectangle {
                id: rectangleBobHiddenCards
                x: 41
                y: 8
                width: 639
                height: 63
                color: "#2db612"
                opacity: 1
            }

            Rectangle {
                id: rectangleBobVisibleCards
                x: 41
                y: 87
                width: 639
                height: 63
                color: "#2db612"
                opacity: 1
            }
        }

        Rectangle {
            id: rectangleVictor
            y: 623
            height: 150
            color: "#2db612"
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

            Rectangle {
                id: rectangleVictorHiddenCards
                x: 56
                y: 80
                width: 624
                height: 62
                color: "#2db612"
                opacity: 1
            }

            Rectangle {
                id: rectangleVictorVisibleCards
                x: 59
                y: 8
                width: 624
                height: 66
                color: "#2db612"
                opacity: 1
            }
        }

    Rectangle {
        id: rectangleScore
        x: 927
        width: 300
        color: "#2db612"
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

        Button {
            id: buttonExit
            x: 204
            y: 22
            text: qsTr("Exit")
        }

        Button {
            id: buttonAbout
            x: 46
            y: 22
            text: qsTr("About")
        }
    }

    RowLayout {
        id: rowLayoutBid
        y: 756
        height: 54
        anchors.bottom: rectangleVictor.top
        anchors.bottomMargin: 0
        anchors.right: rectangleCarlos.left
        anchors.rightMargin: 0
        anchors.left: rectangleAlice.right
        anchors.leftMargin: 0

        property alias buttonPass : buttonPass;
        property alias button6 : button6;
        property alias button7 : button7;
        property alias button8 : button8;
        property alias button9 : button9;
        property alias button10 : button10;
        property alias button11 : button11;
        property alias button12 : button12;
        property alias button13 : button13;
        property alias button14 : button14;

        Button {
            id: buttonPass
            width: 40
            height: 23
            text: qsTr("-")
        }

        Button {
            id: button6
            width: 40
            height: 23
            text: qsTr("6")
        }

        Button {
            id: button7
            width: 40
            text: qsTr("7")
        }

        Button {
            id: button8
            width: 40
            text: qsTr("8")
            clip: false
            visible: true
        }

        Button {
            id: button9
            width: 40
            text: qsTr("9")
        }

        Button {
            id: button10
            width: 40
            text: qsTr("10")
        }

        Button {
            id: button11
            width: 40
            text: qsTr("11")
        }

        Button {
            id: button12
            width: 40
            text: qsTr("12")
        }

        Button {
            id: button13
            width: 40
            text: qsTr("13")
        }

        Button {
            id: button14
            width: 40
            text: qsTr("14")
        }
    }

    Text {
        id: bidAlice
        y: 473
        text: qsTr("")
        anchors.left: rectangleAlice.right
        anchors.leftMargin: 0
        font.pixelSize: 26
    }

    Text {
        id: bidCarlos
        x: 774
        y: 473
        text: qsTr("")
        anchors.right: rectangleCarlos.left
        anchors.rightMargin: 6
        font.pixelSize: 26
    }

    Text {
        id: bidBob
        x: 487
        text: qsTr("")
        anchors.top: rectangleBob.bottom
        anchors.topMargin: 0
        font.pixelSize: 26
    }

    }
}

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import pidro.components 1.0

ApplicationWindow {
    id: appWindow
    title: qsTr("Hello World")
    width: 1290
    height: 960
    visible: true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(player.hello);
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }

    Card {
        id: card
        source: "file:cards/4H.png"
    }

    MainForm {
        anchors.fill: parent
        buttonAbout.onClicked: messageDialog.show(qsTr("About Pidro"))
        buttonExit.onClicked: Qt.quit();
        property int event: Player.GAME_OVER;

        Component.onCompleted: {
            var Card = Qt.createComponent("Card.qml");
            if (Card) {
                console.log("Created Card");

                for (var i = 0; i < 14; ++i) {
                    var card = Card.createObject(rectangleAliceHiddenCards);

                    card.source = "file:cards/BACK_BLUE1.png"
                    card.visible = true;
                    card.y = i * 20;
                }
            } else {
                console.log("NOT created")
            }
        }


    }

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
}

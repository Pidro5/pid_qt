import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import pidro.components 1.0
import "CardFactory.js" as CardFactory

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
//        player.onEvent: messageDialog.show("EVENT");

        Component.onCompleted: {
            var Card = Qt.createComponent("Card.qml");
            if (Card) {
                console.log("Created Card");

                /*
                for (var i = 0; i < 14; ++i) {
                    var card = Card.createObject(rectangleAliceHiddenCards);

                    card.source = "file:cards/BACK_BLUE1.png"
                    card.visible = true;
                    card.y = i * 20;
                }
                */
            } else {
                console.log("NOT created")
            }

            player.event1.connect(handleEvent1);
            player.event2.connect(handleEvent2);
            player.event3.connect(handleEvent3);
            player.event4.connect(handleEvent4);

        }

        function handleEvent1(event) {
            console.log("EVENT FROM QML");
        }

        function handleEvent2(event, position) {
            console.log("EVENT FROM QML: " + position);
        }

        function handleEvent3(event, position, value) {
            console.log("EVENT FROM QML: " + position + ", " + value);

            var rectangle;
            var isHorizontal;

            switch (position) {
            case 0:
                rectangle = rectangleAliceHiddenCards;
                isHorizontal = false;
                break;

            case 1:
                rectangle = rectangleBobHiddenCards;
                isHorizontal = true;
                break;

            case 2:
                rectangle = rectangleCarlosHiddenCards;
                isHorizontal = false;
                break;

            case 3:
                rectangle = rectangleVictorHiddenCards;
                isHorizontal = true;
                break;

            default:
                console.error("Unexpected position: " + position)
            }

            console.log("CHILDREN: " + rectangle.children.length);

            var count = rectangle.children.length;

            for (var i = 0 ; i < value; ++i) {
                var card = CardFactory.createCard(rectangle);

                card.source = "file:cards/BACK_BLUE1.png"
                card.visible = true;
                card.x = isHorizontal ? count * 20 : 0;
                card.y = isHorizontal ? 0 : count * 20;
                ++count;
            }
        }

        function handleEvent4(event, position, cards) {
            var rectangle;
            var isHorizontal;

            switch (position) {
            case 0:
                rectangle = rectangleAliceHiddenCards;
                isHorizontal = false;
                break;

            case 1:
                rectangle = rectangleBobHiddenCards;
                isHorizontal = true;
                break;

            case 2:
                rectangle = rectangleCarlosHiddenCards;
                isHorizontal = false;
                break;

            case 3:
                rectangle = rectangleVictorHiddenCards;
                isHorizontal = true;
                break;

            default:
                console.error("Unexpected position: " + position)
            }

            console.log("CHILDREN: " + rectangle.children.length);

            var count = rectangle.children.length;

            for (var i = 0 ; i < cards.length; ++i) {
                var c = cards[i];
                var name = "" + c.value + c.suit;

                var card = CardFactory.createCard(rectangle);

                card.source = "file:cards/" + name + ".png";
                card.visible = true;
                card.x = isHorizontal ? count * 30 : 0;
                card.y = isHorizontal ? 0 : count * 30;
                ++count;
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

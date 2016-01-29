import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import pidro.components 1.0
import "CardFactory.js" as CardFactory

ApplicationWindow {
    id: appWindow
    title: qsTr("Pidro")
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
        source: "cards/4H.png"
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

                    card.source = "cards/BACK_BLUE1.png"
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
            player.giveBid.connect(handleGiveBid);
            player.giveSuit.connect(handleGiveSuit);

            bid.buttonPass.clicked.connect(buttonPassedClicked);
            bid.button6.clicked.connect(button6Clicked);
            bid.button7.clicked.connect(button7Clicked);
            bid.button8.clicked.connect(button8Clicked);
            bid.button9.clicked.connect(button9Clicked);
            bid.button10.clicked.connect(button10Clicked);
            bid.button11.clicked.connect(button11Clicked);
            bid.button12.clicked.connect(button12Clicked);
            bid.button13.clicked.connect(button13Clicked);
            bid.button14.clicked.connect(button14Clicked);
        }

        function buttonClicked(value) {
            bid.visible = false;
            player.setIntResult(value);
        }

        function buttonPassedClicked() {
            console.log("PASS");
            buttonClicked(0)
        }

        function button6Clicked() {
            console.log("6");
            buttonClicked(6);
        }

        function button7Clicked() {
            console.log("7");
            buttonClicked(7);
        }

        function button8Clicked() {
            console.log("8");
            buttonClicked(8);
        }

        function button9Clicked() {
            console.log("9");
            buttonClicked(9);
        }

        function button10Clicked() {
            console.log("10");
            buttonClicked(10);
        }

        function button11Clicked() {
            console.log("11");
            buttonClicked(11);
        }

        function button12Clicked() {
            console.log("12");
            buttonClicked(12);
        }

        function button13Clicked() {
            console.log("13");
            buttonClicked(13);
        }

        function button14Clicked() {
            console.log("14");
            buttonClicked(14);
        }

        function handleGiveBid(minimum) {
            console.log("GIVE BID: " + minimum);
            bid.visible = true;
            bid.buttonPass.enabled = true;

            switch (minimum) {
            case 14:
            case 13:
                bid.button13.enabled = false;
            case 12:
                bid.button12.enabled = false;
            case 11:
                bid.button11.enabled = false;
            case 10:
                bid.button10.enabled = false;
            case 9:
                bid.button9.enabled = false;
            case 8:
                bid.button8.enabled = false;
            case 7:
                bid.button7.enabled = false;
            case 6:
                bid.button6.enabled = false;
                break;
            case 5:
                bid.buttonPass.enabled = false;
                break;
            default:
                console.log("Don't know what to do with a minimum of: " + minimum);
            }
        }

        function handleGiveSuit() {
            console.log("NOW ARRANGE THAT COLOR CAN BE CHOSEN");
        }

        function handleEvent1(event) {
            console.log("1 EVENT FROM QML");
        }

        function handleEvent2(event, position) {
            console.log("2 EVENT FROM QML: " + position);
        }

        function handleEvent3(event, position, value) {
            console.log("3 EVENT FROM QML: " + position + ", " + value);

            var rectangle;
            var isHorizontal;
            var bid;

            switch (position) {
            case 0:
                rectangle = rectangleVictorHiddenCards;
                isHorizontal = true;
                break;

            case 1:
                rectangle = rectangleAliceHiddenCards;
                isHorizontal = false;
                bid = bidAlice;
                break;

            case 2:
                rectangle = rectangleBobHiddenCards;
                isHorizontal = true;
                bid = bidBob;
                break;

            case 3:
                rectangle = rectangleCarlosHiddenCards;
                isHorizontal = false;
                bid = bidCarlos;
                break;

            default:
                console.error("Unexpected position: " + position)
            }

            switch (event) {
            case Player.DEAL_CARD:
                console.log("DEAL_CARD");
                var count = rectangle.children.length;

                for (var i = 0 ; i < value; ++i) {
                    var card = CardFactory.createCard(rectangle);

                    card.source = "cards/BACK_BLUE1.png"
                    card.visible = true;
                    card.x = isHorizontal ? count * 20 : 0;
                    card.y = isHorizontal ? 0 : count * 20;
                    ++count;
                }
                break;

            case Player.BID_PLACED:
                if (bid) {
                    if (value === 0) {
                        value = "Pass";
                    }

                    bid.text = value;
                } else {
                    console.warn("Bid is null");
                }

                break;

            default:
                console.warn("3 Unhandled event: " + event);
            }
        }

        function handleEvent4(event, position, cards) {
            console.log("4 EVENT FROM QML: " + event + ", " + position + ", " + cards);

            var rectangle;
            var isHorizontal;

            switch (position) {
            case 0:
                rectangle = rectangleVictorHiddenCards;
                isHorizontal = true;
                break;

            case 1:
                rectangle = rectangleAliceHiddenCards;
                isHorizontal = false;
                break;

            case 2:
                rectangle = rectangleBobHiddenCards;
                isHorizontal = true;
                break;

            case 3:
                rectangle = rectangleCarlosHiddenCards;
                isHorizontal = false;
                break;

            default:
                console.error("Unexpected position: " + position)
            }

            switch (event) {
            case Player.DEAL_CARD:
                var count = rectangle.children.length;

                for (var i = 0 ; i < cards.length; ++i) {
                    var c = cards[i];
                    var name = "" + c.value + c.suit;

                    var card = CardFactory.createCard(rectangle);

                    card.source = "cards/" + name + ".png";
                    card.visible = true;
                    card.x = isHorizontal ? count * 30 : 0;
                    card.y = isHorizontal ? 0 : count * 30;
                    ++count;
                }
                break;

            default:
                console.warn("4: Unhandled event: " + event);
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

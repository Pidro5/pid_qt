var Card;

function createCard(parent) {
    if (!Card) {
        Card = Qt.createComponent("Card.qml");

        if (Card.status !== Component.Ready) {
            console.error("CardFactory: Card not immediately ready. Exception imminent.");
        }
    }

    return Card.createObject(parent)
}

Text {
    id: lineBreak%1;
    width: rctItem.width - parent.rightPadding - parent.leftPadding;
    height: 1;

    Rectangle {
        visible: %2
        border.width: dp(1);
        border.color: "yellow";
        color: "transparent";
        width: parent.width;
        height: parent.height;
    }
}

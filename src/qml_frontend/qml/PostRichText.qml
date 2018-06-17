Text {
    id: dynTxtPost%1;
//   width: rctItem.width - parent.rightPadding - parent.leftPadding;

    color: '%2';

    font.bold: %3;
    font.italic: %4;
    font.underline: %5;
    font.strikeout: %6;
    font.pointSize: 14;

    text: '%7';
    textFormat: Text.PlainText;

    elide: Text.ElideRight;
    wrapMode: Text.WordWrap;

    Rectangle {
        visible: %8;

        border.width: dp(1);
        border.color: "red";
        color: "transparent";
        width: parent.width;
        height: parent.height;
    }
}

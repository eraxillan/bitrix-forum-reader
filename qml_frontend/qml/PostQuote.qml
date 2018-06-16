Rectangle {
    id: rctQuote%1;
    color: "white";
    width: rctItem.width - parent.rightPadding - parent.leftPadding - dp(20);
    height: rctQuoteTitle%1.height + txtQuoteBody%1.height + txtQuoteSourceRef%1.height + 2*dp(5);

    border.width: dp(2);
    border.color: "silver";

    Rectangle {
        id: rctQuoteTitle%1;
        color: "silver";
        width: parent.width;
        height: txtQuoteTitle%1.height;

        Column {
            width: parent.width;
            height: parent.height;
            spacing: dp(5);

            Text {
                id: txtQuoteTitle%1;

                leftPadding: dp(10);
                verticalAlignment: Text.AlignVCenter;

                width: parent.width;

                font.pointSize: 14;
                text: '%2';
            }

            Row {
                id: txtQuoteSourceRef%1;

                leftPadding: dp(10);

                Text { font.pointSize: 14; font.bold: true; text: '%3  '; }
                %4
                Text { visible: %6; font.pointSize: 14; text: ':'; }
            }
            Flow {
                id: txtQuoteBody%1;

                leftPadding: dp(10);

                %5
                width: parent.width;
            }
        }
    }
}

import QtQuick 2.10;

Rectangle {
    id: rctQuote_d965b5e7c48a416aaeea7772888d02e4;
    color: "white";
    width: rctItem.width - parent.rightPadding - parent.leftPadding - dp(20);
    height: rctQuoteTitle_d965b5e7c48a416aaeea7772888d02e4.height + txtQuoteBody_d965b5e7c48a416aaeea7772888d02e4.height + txtQuoteSourceRef_d965b5e7c48a416aaeea7772888d02e4.height + 2*dp(5);

    border.width: dp(2);
    border.color: "silver";

    Rectangle {
        id: rctQuoteTitle_d965b5e7c48a416aaeea7772888d02e4;
        color: "silver";
        width: parent.width;
        height: txtQuoteTitle_d965b5e7c48a416aaeea7772888d02e4.height;

        Column {
            width: parent.width;
            height: parent.height;
            spacing: dp(5);

            Text {
                id: txtQuoteTitle_d965b5e7c48a416aaeea7772888d02e4;

                leftPadding: dp(10);
                verticalAlignment: Text.AlignVCenter;

                width: parent.width;

                font.pointSize: 14;
                text: '_2ca6e4441d074fbebef439a882585113';
            }

            Row {
                id: txtQuoteSourceRef_d965b5e7c48a416aaeea7772888d02e4;

                leftPadding: dp(10);

                Text { font.pointSize: 14; font.bold: true; text: '_81318a4c46474759b9ebd6b0e94873aa  '; }
                state: _e265fb5cb7544ee88130089cc4d0353f;
                Text { visible: _af0014871b304e238b0d6e37d90eb5c5; font.pointSize: 14; text: ':'; }
            }
            Flow {
                id: txtQuoteBody_d965b5e7c48a416aaeea7772888d02e4;

                leftPadding: dp(10);

                state: _c8f7635c38ed49168187d56654807c9c;
                width: parent.width;
            }
        }
    }
}

Rectangle {
    id: rctSpoiler%1;
    color: "white";
    width: rctItem.width - parent.rightPadding - parent.leftPadding - dp(20);
    height: rctQuoteTitle%1.height + txtSpoilerBody%1.height + 2*dp(5);

    border.width: dp(2);
    border.color: "silver";

    Rectangle {
        id: rctQuoteTitle%1;
        color: "silver";
        width: parent.width;
        height: txtQuoteTitle%1.height;

        Component.onCompleted: {
            rctQuoteTitle%1MA.onClicked(null);
            rctQuoteTitle%1MA.onClicked(null);
            rctQuoteTitle%1MA.onClicked(null);
        }

        MouseArea {
            id: rctQuoteTitle%1MA;

            anchors.fill: parent;
            onClicked: {
                txtSpoilerBody%1.visible = !txtSpoilerBody%1.visible;

                if (txtSpoilerBody%1.height > 0) {
                    console.log("Collapse spoiler...");

                    txtSpoilerBody%1.height = 0;
                    txtQuoteTitle%1.text = '%2 \u25BC';
                    rctSpoiler%1.height = Qt.binding(function() { return rctQuoteTitle%1.height + txtSpoilerBody%1.height - 2*dp(5); });
                } else {
                    console.log("Expand spoiler...");

                    txtSpoilerBody%1.height = txtSpoilerBody%1.childrenRect.height;
                    txtQuoteTitle%1.text = '%2 \u25B2';
                    rctSpoiler%1.height = Qt.binding(function() { return rctQuoteTitle%1.height + txtSpoilerBody%1.height + 2*dp(5); });
                }
            }
        }

        Column {
            width: parent.width;
            spacing: dp(5);

            Text {
                id: txtQuoteTitle%1;

                leftPadding: dp(10);
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;

                width: parent.width;

                font.pointSize: 14;
                font.bold: true;
                text: '%2';
            }

            Flow {
                id: txtSpoilerBody%1;

                leftPadding: dp(10);

                width: parent.width;

                %3
            }
        }
    }
}

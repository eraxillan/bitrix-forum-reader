import QtQuick 2.10;

Rectangle {
    id: rctSpoiler_fcdde97089594b218fbc925097a2f982;
    color: "white";
    width: rctItem.width - parent.rightPadding - parent.leftPadding - dp(20);
    height: rctQuoteTitle_fcdde97089594b218fbc925097a2f982.height + txtSpoilerBody_fcdde97089594b218fbc925097a2f982.height + 2*dp(5);

    border.width: dp(2);
    border.color: "silver";

    Rectangle {
        id: rctQuoteTitle_fcdde97089594b218fbc925097a2f982;
        color: "silver";
        width: parent.width;
        height: txtQuoteTitle_fcdde97089594b218fbc925097a2f982.height;

        Component.onCompleted: {
            rctQuoteTitle_fcdde97089594b218fbc925097a2f982MA.onClicked(null);
            rctQuoteTitle_fcdde97089594b218fbc925097a2f982MA.onClicked(null);
            rctQuoteTitle_fcdde97089594b218fbc925097a2f982MA.onClicked(null);
        }

        MouseArea {
            id: rctQuoteTitle_fcdde97089594b218fbc925097a2f982MA;

            anchors.fill: parent;
            onClicked: {
                txtSpoilerBody_fcdde97089594b218fbc925097a2f982.visible = !txtSpoilerBody_fcdde97089594b218fbc925097a2f982.visible;

                if (txtSpoilerBody_fcdde97089594b218fbc925097a2f982.height > 0) {
                    console.log("Collapse spoiler...");

                    txtSpoilerBody_fcdde97089594b218fbc925097a2f982.height = 0;
                    txtQuoteTitle_fcdde97089594b218fbc925097a2f982.text = '_4d4282062caf43929173ac37159d230f \u25BC';
                    rctSpoiler_fcdde97089594b218fbc925097a2f982.height = Qt.binding(function() { return rctQuoteTitle_fcdde97089594b218fbc925097a2f982.height + txtSpoilerBody_fcdde97089594b218fbc925097a2f982.height - 2*dp(5); });
                } else {
                    console.log("Expand spoiler...");

                    txtSpoilerBody_fcdde97089594b218fbc925097a2f982.height = txtSpoilerBody_fcdde97089594b218fbc925097a2f982.childrenRect.height;
                    txtQuoteTitle_fcdde97089594b218fbc925097a2f982.text = '_4d4282062caf43929173ac37159d230f \u25B2';
                    rctSpoiler_fcdde97089594b218fbc925097a2f982.height = Qt.binding(function() { return rctQuoteTitle_fcdde97089594b218fbc925097a2f982.height + txtSpoilerBody_fcdde97089594b218fbc925097a2f982.height + 2*dp(5); });
                }
            }
        }

        Column {
            width: parent.width;
            spacing: dp(5);

            Text {
                id: txtQuoteTitle_fcdde97089594b218fbc925097a2f982;

                leftPadding: dp(10);
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;

                width: parent.width;

                font.pointSize: 14;
                font.bold: true;
                text: '_4d4282062caf43929173ac37159d230f';
            }

            Flow {
                id: txtSpoilerBody_fcdde97089594b218fbc925097a2f982;

                leftPadding: dp(10);

                width: parent.width;

                state: _d664d2ca198c40819fb4850253db6886;
            }
        }
    }
}

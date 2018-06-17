/*
Text {
    id: dynTxtHyperlink%1;
//  width: rctItem.width - parent.rightPadding - parent.leftPadding;

    font.pointSize: 14;
    renderType: Text.NativeRendering;

    text: '%2';
    textFormat: Text.RichText;
    onLinkActivated: Qt.openUrlExternally(link);

    clip: false;
    elide: Text.ElideRight;
    wrapMode: Text.WordWrap;

    Rectangle {
        visible: %3
        border.width: dp(1);
        border.color: "red";
        color: "transparent";
        width: parent.width;
        height: parent.height;
    }
}
*/

Text {
    id: dynTxtHyperlink%1;
//   width: rctItem.width - parent.rightPadding - parent.leftPadding;

    color: 'blue';
    font.underline: true;
    font.pointSize: 14;
    renderType: Text.NativeRendering;

    text: '%2';
    textFormat: Text.PlainText;

    clip: false;
    elide: Text.ElideRight;
    wrapMode: Text.WordWrap;

    MouseArea {
        anchors.fill: parent;
        onClicked: { console.log("Opening external URL ", '%3'); Qt.openUrlExternally('%3'); }
    }

    Rectangle {
        visible: %4;

        border.width: dp(1);
        border.color: "red";
        color: "transparent";
        width: parent.width;
        height: parent.height;
    }
}

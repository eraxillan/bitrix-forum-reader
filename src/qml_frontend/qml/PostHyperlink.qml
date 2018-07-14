import QtQuick 2.10;

Text {
    id: dynTxtHyperlink_0947683f69d64f0b9e37d78a2e2a9925;
//   width: rctItem.width - parent.rightPadding - parent.leftPadding;

    color: 'blue';
    font.underline: true;
    font.pointSize: 14;
    renderType: Text.NativeRendering;

    text: '_8f07025844744bbc84bf2c1868f58abd';
    textFormat: Text.PlainText;

    clip: false;
    elide: Text.ElideRight;
    wrapMode: Text.WordWrap;

    MouseArea {
        anchors.fill: parent;
        onClicked: { console.log("Opening external URL ", '_b93596dcbae045a18be44f67d45f22a8'); Qt.openUrlExternally('_b93596dcbae045a18be44f67d45f22a8'); }
    }

    Rectangle {
        visible: _63a25cb5bf444bc19d127626ca9a9b3f;

        border.width: dp(1);
        border.color: "red";
        color: "transparent";
        width: parent.width;
        height: parent.height;
    }
}

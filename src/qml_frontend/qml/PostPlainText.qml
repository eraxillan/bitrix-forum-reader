import QtQuick 2.10;

Text {
    id: dynTxtPost_63f18ed6e6c84c7c803ca7bd2b7c8a43;

    // NOTE: of course Text has contentWidth property, but we cannot bind Text.width to Text.contentWidth:
    //       this will cause binding loop; currently i've found just one workaround - measure text width using another way
    TextMetrics {
        id: textMetrics_63f18ed6e6c84c7c803ca7bd2b7c8a43;

        font: dynTxtPost_63f18ed6e6c84c7c803ca7bd2b7c8a43.font;
        text: dynTxtPost_63f18ed6e6c84c7c803ca7bd2b7c8a43.text;
    }

    Component.onCompleted: {
        this.width = Qt.binding(function() {
            var postWidth = parent.width - 2*parent.rightPadding - 2*parent.leftPadding;
            return ((textMetrics_63f18ed6e6c84c7c803ca7bd2b7c8a43.width < postWidth) ? textMetrics_63f18ed6e6c84c7c803ca7bd2b7c8a43.width + dp(20) : postWidth);   // tightBoundingRect.width
        });
    }

    font.pointSize: 14;

    text: '_da5eb852c7b64ceca937ddb810b0bcdc';
    textFormat: Text.PlainText;

    elide: Text.ElideRight;
    wrapMode: Text.WordWrap;

    Rectangle {
        visible: _7fc091fe66ce4db193a4267004716245;

        border.width: dp(1);
        border.color: "red";
        color: "transparent";
        width: parent.width;
        height: parent.height;
    }
}

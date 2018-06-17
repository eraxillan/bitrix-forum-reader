Text {
    id: dynTxtPost%1;

    // NOTE: of course Text has contentWidth property, but we cannot bind Text.width to Text.contentWidth:
    //       this will cause binding loop; currently i've found just one workaround - measure text width using another way
    TextMetrics {
        id: textMetrics%1;

        font: dynTxtPost%1.font;
        text: dynTxtPost%1.text;
    }

    Component.onCompleted: {
        this.width = Qt.binding(function() {
            var postWidth = parent.width - 2*parent.rightPadding - 2*parent.leftPadding;
            return ((textMetrics%1.width < postWidth) ? textMetrics%1.width + dp(20) : postWidth);   // tightBoundingRect.width
        });
    }

    font.pointSize: 14;

    text: '%2';
    textFormat: Text.PlainText;

    elide: Text.ElideRight;
    wrapMode: Text.WordWrap;

    Rectangle {
        visible: %3;

        border.width: dp(1);
        border.color: "red";
        color: "transparent";
        width: parent.width;
        height: parent.height;
    }
}

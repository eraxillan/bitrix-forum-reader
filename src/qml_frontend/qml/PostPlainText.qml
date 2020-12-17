/*
 * This file is part of Bitrix Forum Reader.
 *
 * Copyright (C) 2016-2020 Alexander Kamyshnikov <axill777@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
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

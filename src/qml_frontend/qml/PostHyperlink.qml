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
import QtQuick 2.15

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

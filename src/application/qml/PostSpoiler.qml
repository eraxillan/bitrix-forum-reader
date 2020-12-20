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

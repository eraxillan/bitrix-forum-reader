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
import QtMultimedia 5.8;
import QtQuick 2.10;
import QtQuick.Layouts 1.3;
import QtQuick.Window 2.2;
import QtQuick.Controls 2.15;
import QtQuick.Controls.Material 2.3;
import QtQuick.Controls.Universal 2.3;

Column {
    id: clmnPost;

    spacing: dp(10);
    leftPadding: dp(10);
    rightPadding: dp(10);

    Text {
        id: txtPostDateTime;
        width: rctItem.width;

        topPadding: dp(5);
        padding: dp(0);
        horizontalAlignment: Text.AlignLeft;
        verticalAlignment: Text.AlignVCenter;
        clip: false;

        font.pointSize: 14;
        text: Qt.formatDateTime(model.postDateTime);
    }

    Rectangle {
        width: rctItem.width;
        height: dp(2);

        border.width: dp(0);
        color: "lightslategrey";
    }

    Flow {
        width: rctItem.width;

        padding: dp(10);
        spacing: dp(10);

        state: _a130f037750e40c69eb7d4ffc572822a;
    }

    Text {
        id: txtLastEdit_05ea9fa84e5148fdaa80754067b2ddad;
        visible: model.lastEdit !== "";
        width: rctItem.width - parent.rightPadding - parent.leftPadding;

        color: "lightslategrey";
        font.italic: true;
        font.pointSize: 14;

        renderType: Text.NativeRendering;

        text: model.postLastEdit;
        textFormat: Text.RichText;
        onLinkActivated: Qt.openUrlExternally(link);

        clip: false;
        elide: Text.ElideRight;
        wrapMode: Text.WordWrap;
    }

    Rectangle {
        visible: model.authorSignature !== "";
        width: rctItem.width - parent.rightPadding - parent.leftPadding;
        height: dp(1);
        border.width: dp(0);
        color: "lightslategrey";
    }

    Text {
        id: txtPostAuthorSignature_05ea9fa84e5148fdaa80754067b2ddad;
        visible: model.authorSignature !== "";
        width: rctItem.width - parent.rightPadding - parent.leftPadding;

        color: "lightslategrey";
        font.italic: true;
        font.pointSize: 14;

        renderType: Text.NativeRendering;

        text: model.authorSignature;
        textFormat: Text.RichText;
        onLinkActivated: Qt.openUrlExternally(link);

        clip: false;
        elide: Text.ElideRight;
        wrapMode: Text.WordWrap;
    }

    Rectangle {
        visible: model.postLikeCount > 0;
        width: rctItem.width - parent.rightPadding - parent.leftPadding;
        height: dp(1);
        border.width: dp(0);
        color: "lightslategrey";
    }

    Text {
        id: txtPostLikeCounter_05ea9fa84e5148fdaa80754067b2ddad;

        visible: model.postLikeCount > 0;
        width: rctItem.width - parent.rightPadding - parent.leftPadding;
        color: "lightslategrey";

        font.bold: true;
        font.pointSize: 14;
        text: model.postLikeCount + " like(s)";
    }

}    // QML Column end

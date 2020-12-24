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
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Window 2.15

Column {
    id: clmnUserInfo_6afb4d56214b4bb69281bf9bbf60396b;
    spacing: dp(2);
    padding: dp(5);
    // FIXME: calculate appropriate for current device width automatically
    width: dp(250)
    // NOTE: height will be calculated automatically

    Text {
        id: txtUserName_6afb4d56214b4bb69281bf9bbf60396b;
        text: "<b>" + '_14c1808e13d44b64accf01b6b8a6be25' + "</b>";
        color: "blue";

        font.pointSize: 14;

        elide: Text.ElideRight;
        wrapMode: Text.WordWrap;
    }

    AnimatedImage {
        id: imgUserAvatar_6afb4d56214b4bb69281bf9bbf60396b;
        source: reader.convertToUrl('_c75e18958b0f43fa9e7881315afccc54');
        visible: '_c75e18958b0f43fa9e7881315afccc54' !== "";

        width:  _1e4e440acb92478197b992539899e5de === -1 ? dp(100) : dp(_1e4e440acb92478197b992539899e5de);
        height: _3860f825cb2a426f82730cc2ca35e6ac === -1 ? dp(100) : dp(_3860f825cb2a426f82730cc2ca35e6ac);
    }

    Text {
        id: txtAuthorPostCount_6afb4d56214b4bb69281bf9bbf60396b;
        text: qsTranslate("User", "Posts: ") + _fcfaa58e3305486e872ae962537bdc8e;

        font.pointSize: 14;
    }

    Text {
       id: txtAuthorRegistrationDate_6afb4d56214b4bb69281bf9bbf60396b;
       text: qsTranslate("User", "Registered:\n") + '_e0aefa26825e44b58c74a54b5bad32b8';

       font.pointSize: 14;
    }

    Text {
        id: txtAuthorReputation_6afb4d56214b4bb69281bf9bbf60396b;
        text: qsTranslate("User", "Reputation: ") + _875abbe700a0408891e5c6f73b7e27be;

        font.pointSize: 14;
    }

    Text {
        id: txtAuthorCity_6afb4d56214b4bb69281bf9bbf60396b;
        visible: '_3cf1b6db9ccf4404b672f7c9d6c78211' !== "";
        text: qsTranslate("User", "City:\n") + '_3cf1b6db9ccf4404b672f7c9d6c78211';

        font.pointSize: 14;
    }
}

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
import QtMultimedia 5.15
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Window 2.15
import Qt.labs.settings 1.0

//import Fluid.Core 1.0 as FluidCore
import Fluid.Controls 1.0 as FluidControls

import name.eraxillan.bfr 1.0
import "../qml" as GenericUi


ApplicationWindow {
    id:      wndMain
    visible: true
    width:   Screen.width
    height:  Screen.height
    title:   qsTr("Bitrix Forum Reader: Desktop version")
    color:   "white"

    function dp(x) { return x; }
//    function sp(x) { return x * (displayDpi / 160) * textScaleFactor; }

    property bool qmlInit: false
    property bool pageLoaded: false
    property int totalPageCount: 1
    property int currentPageIndex: 1
    property int postIndex: 1;

    FluidControls.SnackBar {
         id: snbrMain
         //onClicked: console.log("Snack bar button clicked")
    }

    ForumThreadUrl {
        id: testThreadUrl;

        sectionId: 22;
        threadId: 358149;
    }

    ForumReader {
        id: reader

        onPageContentParseProgressRange: {
            pbPage.value = minimum
            pbPage.from  = minimum
            pbPage.to    = maximum
        }

        onPageContentParseProgress: {
            pbPage.value = value
        }

        onPageCountParsed: {
            console.log("Page count: ", pageCount)
            totalPageCount = pageCount

            // Cleanup
            pageLoaded = false;
            dataModel.clear();

            // Parse the page HTML data
            reader.startPageParseAsync(testThreadUrl, pageCount);
        }

        onPageContentParsed: {
            totalPageCount = pageCount;
            currentPageIndex = pageNo;

            // Fill the post list
            for (var i = 0; i < posts.length; i++) {
                var aPost = posts[i];

                dataModel.append( {  "color"                  : "lightgrey",
                                     "postAuthorQml"          : aPost["authorQml"],
                                     "authorSignature"        : aPost["authorSignature"],
                                     "postDateTime"           : aPost["date"],
                                     "postText"               : aPost["contentQml"],
                                     "postLastEdit"           : aPost["lastEdit"],
                                     "postLikeCount"          : aPost["likeCount"]
                                  } );
            }

            qmlInit = true;
            pageLoaded = true;

            snbrMain.open(qsTr("Page has been loaded"));
        }
    }

    ProgressBar {
        id: pbPage

        anchors.margins: dp(10)
        anchors.fill: parent
        spacing: dp(5)

        visible: !pageLoaded

        indeterminate: false
        value: 0
        from: 0
        to: 0
    }

    ListModel {
        id: dataModel
    }

    GenericUi.PostList {}

    Component.onCompleted: {
        reader.startPageCountAsync(testThreadUrl);
    }
}

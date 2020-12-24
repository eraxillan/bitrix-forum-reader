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

import "../../qml" as GenericUi
import "../../qml/+android" as AndroidUi


FluidControls.ApplicationWindow {
    id:      wndMain
    visible: true
    width:   Screen.width
    height:  Screen.height
    title:   qsTranslate("Android_Main", "Bitrix Forum Reader: Android version")
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

    // Application navigation panel sliding from the left side
    AndroidUi.NavigationPanel { id: navPanel; }

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

        // FIXME: process loading error

        onPageCountParsed: {
            console.log("Page count: ", pageCount);
            totalPageCount = pageCount;

            // Cleanup
            pageLoaded = false;
            dataModel.clear();

            // Parse the last page HTML data
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

            snbrMain.open(qsTranslate("Android_Main", "Page has been loaded"));
        }
    }

    ProgressBar {
        id: pbPage;

        anchors.margins: dp(10);
        anchors.fill: parent;
        spacing: dp(5);

        visible: !pageLoaded;

        indeterminate: false;
        value: 0;
        from: 0;
        to: 0;
    }

    ListModel {
        id: dataModel;
    }

    initialPage: FluidControls.Page {
        title: qsTranslate("Android_Main", "Page: ") + currentPageIndex;

        leftAction: FluidControls.Action {
            icon.source: FluidControls.Utils.iconUrl("navigation/menu");
            onTriggered: navPanel.open();
        }

        FluidControls.InputDialog {
            id: dlgPageNumber;

            x: (parent.width - width) / 2;
            y: (parent.height - height) / 2;
            //width: 280

            title: qsTranslate("Android_Main", "Go to page");
            text: qsTranslate("Android_Main", "Please enter the page number");
            textField.inputMask: "D99";
            textField.placeholderText: qsTranslate("Android_Main", "Type a number from range 1..999");

            onAccepted: {
                currentPageIndex = parseInt(dlgPageNumber.textField.text);
            }
        }

        appBar.maxActionCount: 5

        actions: [
            FluidControls.Action {
                enabled: totalPageCount > 0;

                text: qsTranslate("Android_Main", "First page");
                toolTip: qsTranslate("Android_Main", "Go to first page");
                onTriggered: currentPageIndex = 1;

                // Icon name from the Google Material Design icon pack
                icon.source: FluidControls.Utils.iconUrl("navigation/first_page");
            },

            FluidControls.Action {
                enabled: (totalPageCount > 0) && (currentPageIndex >= 2);

                text: qsTranslate("Android_Main", "Previous page");
                toolTip: qsTranslate("Android_Main", "Go to previous page");
                onTriggered: {
                    if (currentPageIndex >= 2)
                        currentPageIndex = currentPageIndex - 1;
                }

                icon.source: FluidControls.Utils.iconUrl("navigation/chevron_left");
            },

            FluidControls.Action {
                enabled: (totalPageCount > 0) && (currentPageIndex < totalPageCount);

                text: qsTranslate("Android_Main", "Next page");
                toolTip: qsTranslate("Android_Main", "Go to next page");
                onTriggered: {
                    if (currentPageIndex <= totalPageCount - 1)
                        currentPageIndex = currentPageIndex + 1;
                }

                icon.source: FluidControls.Utils.iconUrl("navigation/chevron_right");
            },

            FluidControls.Action {
                enabled: totalPageCount > 0;

                text: qsTranslate("Android_Main", "Last page");
                toolTip: qsTranslate("Android_Main", "Go to last page");
                onTriggered: currentPageIndex = totalPageCount;

                icon.source: FluidControls.Utils.iconUrl("navigation/last_page");
            },

            FluidControls.Action {
                enabled: totalPageCount > 0;

                text: qsTranslate("Android_Main", "Go to page");
                toolTip: qsTranslate("Android_Main", "Go to specified page");
                onTriggered: {
                    console.log("Go to specified page action called");
                    dlgPageNumber.open();
                }

                icon.source: FluidControls.Utils.iconUrl("action/input");
            }
        ]

        GenericUi.PostList { id: lstPosts; }
    }

    onCurrentPageIndexChanged: {
        console.log("Loading page number ", currentPageIndex)

        // Parse the page HTML data
        if (qmlInit) {
            pageLoaded = false;
            dataModel.clear();

            reader.startPageParseAsync(testThreadUrl, currentPageIndex);
        }
    }

    Component.onCompleted: {
        reader.startPageCountAsync(testThreadUrl);
    }
}

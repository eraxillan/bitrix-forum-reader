import QtMultimedia 5.8
import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Window 2.2
import Qt.labs.settings 1.0

import Fluid.Core 1.0 as FluidCore
import Fluid.Controls 1.0 as FluidControls

import ru.banki.reader 1.0
import "../qml" as BFR

FluidControls.ApplicationWindow {
    id:      wndMain
    visible: true
    width:   Screen.width
    height:  Screen.height
    title:   qsTr("Bitrix Forum Reader: Android version")
    color:   "white"

    function dp(x) { return x; }
//    function sp(x) { return x * (displayDpi / 160) * textScaleFactor; }

    property bool qmlInit: false
    property bool pageLoaded: false
    property int totalPageCount: 1
    property int currentPageIndex: 1
    property int postIndex: 1;

    onCurrentPageIndexChanged: {
        console.log("Loading page number ", currentPageIndex)

        // Parse the page HTML data
        if (qmlInit) {
            pageLoaded = false;
            dataModel.clear();

            reader.startPageParseAsync("http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1="
                                       + currentPageIndex.toString() + "#forum-message-list", currentPageIndex)
        }
    }


    FluidControls.SnackBar {
         id: snbrMain
         //onClicked: console.log("Snack bar button clicked")
     }

    // -------------------------------------------------
    FluidControls.NavigationListView {
        id: navDrawer

        topContent: Image {
            source: ""

            Layout.fillWidth: true
            Layout.preferredHeight: 200 /*window.header.height */
        }

        actions: [
            FluidControls.Action {
                text: qsTr("Action 1")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                onTriggered: console.log("action1 triggered")
            },
            FluidControls.Action {
                text: qsTr("Action 2")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                hasDividerAfter: true
                onTriggered: console.log("action2 triggered")
            },
            FluidControls.Action {
                text: qsTr("Action 3")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                onTriggered: console.log("action3 triggered")
            },
            FluidControls.Action {
                text: qsTr("Action 4")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                onTriggered: console.log("action4 triggered")
            },
            FluidControls.Action {
                text: qsTr("Action 5")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                visible: false
                onTriggered: console.log("action5 triggered")
            }
        ]
    }
    // -------------------------------------------------

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
            console.log("Page count: ", pageCount)
            totalPageCount = pageCount

            // Cleanup
            pageLoaded = false;
            dataModel.clear();

            // Parse the page HTML data
            reader.startPageParseAsync("http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1=" + pageCount.toString() + "#forum-message-list", pageCount)
        }

        onPageContentParsed: {
            // Fill the page counter
            totalPageCount = reader.pageCount();
            currentPageIndex = pageNo;

            // Fill the post list
            for (var i = 0; i < reader.postCount(); i++)
            {
                dataModel.append( {  "color"                  : "lightgrey",
                                     "postAuthorQml"          : reader.postAuthorQml(i),
                                     "postDateTime"           : reader.postDateTime(i),
                                     "postText"               : reader.postText(i),
                                     "postLastEdit"           : reader.postLastEdit(i),
                                     "postLikeCount"          : reader.postLikeCount(i),
                                     "authorSignature"        : reader.postAuthorSignature(i)
                                  } );
            }

            qmlInit = true;
            pageLoaded = true;

            snbrMain.open(qsTr("Page has been loaded"));
        }
    }

    Component.onCompleted: {
        reader.startPageCountAsync("http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1=1#forum-message-list");
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
        title: qsTr("Page: ") + currentPageIndex;

        leftAction: FluidControls.Action {
            icon.source: FluidControls.Utils.iconUrl("navigation/menu");
            onTriggered: navDrawer.open();
        }

        FluidControls.InputDialog {
            id: dlgPageNumber;

            x: (parent.width - width) / 2;
            y: (parent.height - height) / 2;
            //width: 280

            title: qsTr("Go to page");
            text: qsTr("Please enter the page number");
            textField.inputMask: "D99";
            textField.placeholderText: qsTr("Type a 3 digits number");

            onAccepted: {
                currentPageIndex = parseInt(dlgPageNumber.textField.text);
            }
        }

        appBar.maxActionCount: 5

        actions: [
            // TODO: manage enable/disable state as in desktop version

            FluidControls.Action {
                text: qsTr("First page");
                toolTip: qsTr("Go to first page");
                onTriggered: currentPageIndex = 1;

                // Icon name from the Google Material Design icon pack
                icon.source: FluidControls.Utils.iconUrl("navigation/first_page");
            },

            FluidControls.Action {
                text: qsTr("Previous page");
                toolTip: qsTr("Go to previous page");
                onTriggered: {
                    if (currentPageIndex >= 2)
                        currentPageIndex = currentPageIndex - 1;
                }

                icon.source: FluidControls.Utils.iconUrl("navigation/chevron_left");
            },

            FluidControls.Action {
                text: qsTr("Next page");
                toolTip: qsTr("Go to next page");
                onTriggered: {
                    if (currentPageIndex <= totalPageCount - 1)
                        currentPageIndex = currentPageIndex + 1;
                }

                icon.source: FluidControls.Utils.iconUrl("navigation/chevron_right");
            },

            FluidControls.Action {
                text: qsTr("Last page");
                toolTip: qsTr("Go to last page");
                onTriggered: currentPageIndex = totalPageCount;

                icon.source: FluidControls.Utils.iconUrl("navigation/last_page");
            },

            FluidControls.Action {
                text: qsTr("Go to page");
                toolTip: qsTr("Go to specified page");
                onTriggered: {
                    console.log("Go to specified page action called");
                    dlgPageNumber.open();
                }

                icon.source: FluidControls.Utils.iconUrl("action/input");
            }
        ]

        BFR.PostList {}
    }
}

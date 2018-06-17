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
        }
    }

    Component.onCompleted: {
        reader.startPageCountAsync("http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1=1#forum-message-list" )
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

    initialPage: FluidControls.Page {
        title: qsTr("BFR")

        leftAction: FluidControls.Action {
            icon.source: FluidControls.Utils.iconUrl("navigation/menu")
            onTriggered: navDrawer.open()
        }

        actions: [
            FluidControls.Action {
                text: qsTr("Print")
                toolTip: qsTr("Print")
                onTriggered: console.log("Print action called")

                // Icon name from the Google Material Design icon pack
                icon.source: FluidControls.Utils.iconUrl("action/print")
            }
        ]

        BFR.PostList {}
    }
}

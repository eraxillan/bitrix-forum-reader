import QtMultimedia 5.8
import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Window 2.2
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
    title:   qsTr("Bitrix Forum Reader: Android version")
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

            snbrMain.open(qsTr("Page has been loaded"));
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
        title: qsTr("Page: ") + currentPageIndex;

        leftAction: FluidControls.Action {
            icon.source: FluidControls.Utils.iconUrl("navigation/menu");
            onTriggered: navPanel.open();
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
            FluidControls.Action {
                enabled: totalPageCount > 0;

                text: qsTr("First page");
                toolTip: qsTr("Go to first page");
                onTriggered: currentPageIndex = 1;

                // Icon name from the Google Material Design icon pack
                icon.source: FluidControls.Utils.iconUrl("navigation/first_page");
            },

            FluidControls.Action {
                enabled: (totalPageCount > 0) && (currentPageIndex >= 2);

                text: qsTr("Previous page");
                toolTip: qsTr("Go to previous page");
                onTriggered: {
                    if (currentPageIndex >= 2)
                        currentPageIndex = currentPageIndex - 1;
                }

                icon.source: FluidControls.Utils.iconUrl("navigation/chevron_left");
            },

            FluidControls.Action {
                enabled: (totalPageCount > 0) && (currentPageIndex < totalPageCount);

                text: qsTr("Next page");
                toolTip: qsTr("Go to next page");
                onTriggered: {
                    if (currentPageIndex <= totalPageCount - 1)
                        currentPageIndex = currentPageIndex + 1;
                }

                icon.source: FluidControls.Utils.iconUrl("navigation/chevron_right");
            },

            FluidControls.Action {
                enabled: totalPageCount > 0;

                text: qsTr("Last page");
                toolTip: qsTr("Go to last page");
                onTriggered: currentPageIndex = totalPageCount;

                icon.source: FluidControls.Utils.iconUrl("navigation/last_page");
            },

            FluidControls.Action {
                enabled: totalPageCount > 0;

                text: qsTr("Go to page");
                toolTip: qsTr("Go to specified page");
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

import QtMultimedia 5.8
import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Window 2.2
import Qt.labs.settings 1.0

import ru.banki.reader 1.0
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

    ForumThreadUrl {
        id: testThreadUrl;

        sectionId: 22;
        threadId: 74420;
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

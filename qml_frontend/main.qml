import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import Qt.labs.settings 1.0

import ru.banki.reader 1.0

ApplicationWindow
{
    id:      wndMain
    visible: true
    width:   Screen.width
    height:  Screen.height
    title:   qsTr("banki.ru forum viewer")
    color:   "white"

    function dp(x) { return x * (displayDpi / 160); }
//    function sp(x) { return x * (displayDpi / 160) * textScaleFactor; }

    property bool qmlInit: false
    property bool pageLoaded: false
    property int totalPageCount: 1
    property int currentPageIndex: 1

    ForumReader
    {
        id: reader

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
            pageLoaded = false;
            dataModel.clear();

            // Fill the page counter
            totalPageCount = reader.pageCount();
            currentPageIndex = pageNo;

            // Fill the post list
            for (var i = 0; i < reader.postCount(); i++)
            {
                dataModel.append( {  "color"                  : "lightgrey",
                                     "postAuthor"             : reader.postAuthor(i),
                                     "postAvatar"             : reader.postAvatarUrl(i),
                                     "postAvatarWidth"        : reader.postAvatarWidth(i),
                                     "postAvatarHeight"       : reader.postAvatarHeight(i),
                                     "postDateTime"           : reader.postDateTime(i),
                                     "postText"               : reader.postText(i),
                                     "postLastEdit"           : reader.postLastEdit(i),
                                     "postLikeCount"          : reader.postLikeCount(i),
                                     "postFooterQml"          : reader.postFooterQml(),
                                     "authorPostCount"        : reader.postAuthorPostCount(i),
                                     "authorRegistrationDate" : reader.postAuthorRegistrationDate(i),
                                     "authorReputation"       : reader.postAuthorReputation(i),
                                     "authorCity"             : reader.postAuthorCity(i),
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

    BusyIndicator
    {
        anchors.centerIn: parent
        running: !pageLoaded
    }

    ListModel
    {
        id: dataModel
    }

    ListView
    {
        id: view

        anchors.margins: dp(10)
        anchors.fill: parent
        spacing: dp(5)
        model: dataModel

        clip: true

        // Current item highlighting setup
        highlight: Rectangle {
            color: "skyblue"
        }
        highlightFollowsCurrentItem: true

        delegate:
        Item
        {
            property var view: ListView.view
            property var isCurrent: ListView.isCurrentItem

            width: view.width
            height: Math.max( clmnUserInfo.height, clmnPost.height ) + clmnUserInfo.padding

            Row
            {
                anchors.fill: parent
                spacing: 0
                padding: 0

                Rectangle
                {
                    id: rectUserInfo
                    width: clmnUserInfo.width + 2*clmnUserInfo.padding
                    height: parent.height

                    radius: 0
                    color: isCurrent ? "skyblue" : model.color
                    border {
                        color: "black"
                        width: 1
                    }

                    Column
                    {
                        id: clmnUserInfo
                        spacing: dp(2)
                        padding: dp(5)
                        // NOTE: width will be calculated automatically

                        Text
                        {
                            id: txtUserName
                            text: "<b>" + model.postAuthor + "</b>"
                            color: "blue"

                            font.pointSize: 14
                        }

                        AnimatedImage
                        {
                            id: imgUserAvatar
                            source: reader.convertToUrl( model.postAvatar )
                            visible: model.postAvatar !== ""

                            width:  model.postAvatarWidth === -1  ? dp(100) : dp(model.postAvatarWidth)
                            height: model.postAvatarHeight === -1 ? dp(100) : dp(model.postAvatarHeight)
                        }

                        Text
                        {
                            id: txtAuthorPostCount
                            text: "Post count: " + model.authorPostCount

                            font.pointSize: 14
                        }

                        Text
                        {
                            id: txtAuthorRegistrationDate
                            text: "Registered: " + Qt.formatDate( model.authorRegistrationDate )

                            font.pointSize: 14
                        }

                        Text
                        {
                            id: txtAuthorReputation
                            text: "Reputation: " + model.authorReputation

                            font.pointSize: 14
                        }

                        Text
                        {
                            id: txtAuthorCity
                            visible: model.authorCity !== ""
                            text: "City: " + model.authorCity

                            font.pointSize: 14
                        }
                    }
                }

                Rectangle
                {
                    id: rctItem
                    width: parent.width - clmnUserInfo.width - 2*clmnUserInfo.padding
                    height: parent.height

                    radius: 0
                    color: isCurrent ? "skyblue" : model.color
                    border {
                        color: "black"
                        width: 1
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: view.currentIndex = model.index
                    }

                    Column
                    {
                        id: clmnPost

                        spacing: dp(5)
                        leftPadding: dp(10)
                        rightPadding: dp(10)

                        property int postIndex: -1;
                        function createItem() {
                            if (!pageLoaded) return;
                            if (model.postText === "") return;

                            postIndex++;
                            var postQmlFileName = reader.applicationDirPath() + "page_" + currentPageIndex + "_post_" + postIndex;
                            var postObj = Qt.createQmlObject(model.postText, clmnPost, postQmlFileName);

                            var postFooterObj = Qt.createQmlObject(model.postFooterQml, clmnPost, "dynamicPostAdditionalInfo");

                            if (postObj === null)
                            {
                                console.log("Post QML object creation FAILED:");
                                console.log(">>>--------------------------------------------------------");
                                console.log(model.postText);
                                console.log("<<<--------------------------------------------------------");
                            }
                        }
                        Component.onCompleted: clmnPost.createItem();

                        Text
                        {
                            id: txtPostDateTime
                            width: rctItem.width - parent.rightPadding - parent.leftPadding
                            topPadding: dp(5)
                            padding: dp(0)
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            clip: false

                            font.pointSize: 14
                            text: Qt.formatDateTime(model.postDateTime)
                        }

                        Rectangle
                        {
                            width: rctItem.width - parent.rightPadding - parent.leftPadding
                            height: dp(1)
                            border.width: dp(0)
                            color: "lightslategrey"
                        }
                    }
                }
            }
        }

        header: RowLayout {
            enabled: pageLoaded

            width: view.width
            height: dp(50)
            spacing: dp(5)

            property int childCount: 3
            property int childHeight: height

            Button {
                id: btnFirstPage
                enabled: parent.enabled && (cmbPage.value >= 2)

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.bottomMargin: dp(5)

                text: "<<"

                onClicked: cmbPage.value = 1;
            }

            SpinBox {
                id: cmbPage

                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.bottomMargin: dp(5)

                editable: true
                from: 1
                to: totalPageCount

                value: currentPageIndex
                stepSize: 1

                onValueChanged: {
                    // Parse the page HTML data
                    if (qmlInit)
                    {
                        pageLoaded = false;
                        dataModel.clear();

                        reader.startPageParseAsync("http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1="
                                                   + cmbPage.value.toString() + "#forum-message-list", cmbPage.value)
                    }
                }
            }

            Button {
                id: btnLastPage
                enabled: parent.enabled && (cmbPage.value <= totalPageCount - 1)

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.bottomMargin: dp(5)

                text: ">>"

                onClicked: cmbPage.value = totalPageCount;
            }
        }

        footer: Rectangle {
            width: view.width
            height: dp(40)
            border {
                color: "black"
                width: 1
            }

            Text {
                anchors.centerIn: parent
                renderType: Text.NativeRendering
                text: "Footer"
            }
        }
    }
}

import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Dialogs 1.2
import ru.banki.reader 1.0

ApplicationWindow
{
    visible: true
    width: 1280
    height: 720
    title: qsTr("banki.ru forum viewer")
    color: "white"

    ForumReader
    {
        id: reader
    }

    Component.onCompleted: {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {
            if (xhr.readyState == XMLHttpRequest.DONE ) {
                var resp = xhr.responseText;

                // Parse the page HTML data
                reader.parseForumPage( resp )

                // Fill the post list
                var i = 0;
                for( ; i < reader.postCount(); i++ )
                {
                    dataModel.append( {  "color"                  : "lightgrey",
                                         "postAuthor"             : reader.postAuthor(i),
                                         "postAvatar"             : reader.postAvatarUrl(i),
                                         "postAvatarWidth"        : reader.postAvatarWidth(i),
                                         "postAvatarHeight"       : reader.postAvatarHeight(i),
                                         "postDateTime"           : reader.postDateTime(i),
                                         "postText"               : reader.postText(i),
                                         "postLikeCount"          : reader.postLikeCount(i),
                                         "authorPostCount"        : reader.postAuthorPostCount(i),
                                         "authorRegistrationDate" : reader.postAuthorRegistrationDate(i),
                                         "authorReputation"       : reader.postAuthorReputation(i),
                                         "authorCity"             : reader.postAuthorCity(i),
                                         "authorSignature"        : reader.postAuthorSignature(i)
                                     } );
                }
            }
        };
        xhr.open("GET", "http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1=30#forum-message-list")
        xhr.send()
    }

    ListModel
    {
        id: dataModel
    }

    ListView
    {
        id: view

        anchors.margins: 10
        anchors.fill: parent
        spacing: 10
        model: dataModel

        clip: true

        highlight: Rectangle {
            color: "skyblue"
        }
        highlightFollowsCurrentItem: true

        delegate: Item {
            property var view: ListView.view
            property var isCurrent: ListView.isCurrentItem

            width: view.width
            height: 300

            Row
            {
                spacing: 10
                anchors.fill: parent

                Column
                {
                    spacing: 5

                    Text
                    {
                        id: txtUserName
                        text: "<b>" + model.postAuthor + "</b>"
                        color: "blue"
                        font.family: "Helvetica"
                        font.pointSize: 12

                        width: 100
                        height: 30
                    }

                    Image
                    {
                        id: imgUserAvatar
                        source: reader.convertToUrl( model.postAvatar )

                        width:  model.postAvatarWidth === -1  ? 100 : model.postAvatarWidth
                        height: model.postAvatarHeight === -1 ? 100 : model.postAvatarHeight
                    }

                    Text
                    {
                        id: txtAuthorPostCount
                        text: "Post count: " + model.authorPostCount
                    }

                    Text
                    {
                        id: txtAuthorRegistrationDate
                        text: "Registration date: " + Qt.formatDate( model.authorRegistrationDate )
                    }

                    Text
                    {
                        id: txtAuthorReputation
                        text: "Reputation: " + model.authorReputation
                    }

                    Text
                    {
                        id: txtAuthorCity
                        visible: model.authorCity !== ""
                        text: "City: " + model.authorCity
                    }
                }

                Rectangle
                {
                    id: rctItem

                    width: parent.width - imgUserAvatar.width - parent.spacing
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
                        spacing: 10
                        leftPadding: 20
                        rightPadding: 20

                        Text
                        {
                            id: txtPostDateTime
                            width: rctItem.width
                            //height: 20
                            padding: 5

                            //renderType: Text.NativeRendering
                            text: Qt.formatDateTime( model.postDateTime )
                        }

                        Rectangle
                        {
                            width: rctItem.width - 100
                            height: 2
                            border.width: 0
                            color: "lightslategrey"
                        }

                        Text
                        {
                            id: txtPost
                            width: rctItem.width - 100
                            height: rctItem.height - 200
                            padding: 5

                            font.pixelSize: 14

                            //renderType: Text.NativeRendering
                            text: model.postText
                            // FIXME: implement nice QML-based quote, HTML one is UGLY
                            //textFormat: Text.RichText
                            textFormat: Text.PlainText
                            clip: false
                            elide: Text.ElideRight
                            wrapMode: Text.WordWrap
                        }

                        Rectangle
                        {
                            visible: model.authorSignature !== ""
                            width: rctItem.width - 100
                            height: 2
                            border.width: 0
                            color: "lightslategrey"
                        }

                        Text
                        {
                            id: txtPostAuthorSignature
                            visible: model.authorSignature !== ""

                            width: rctItem.width - 100
                            height: 20
                            padding: 5

                            color: "lightslategrey"
                            font.italic: true

                            renderType: Text.NativeRendering
                            text: model.authorSignature
                            textFormat: Text.PlainText
                            clip: false
                            elide: Text.ElideRight
                            wrapMode: Text.WordWrap
                        }

                        Rectangle
                        {
                            visible: model.postLikeCount > 0
                            width: rctItem.width - 100
                            height: 2
                            border.width: 0
                            color: "lightslategrey"
                        }

                        Text
                        {
                            id: txtPostLikeCounter
                            visible: model.postLikeCount > 0

                            color: "black"
                            font.bold: true
                            font.pointSize: 10

                            width: rctItem.width - 100
                            height: 50
                            padding: 5

                            text: model.postLikeCount + " like(s)"
                        }
                    }
                }
            }
        }

        header: Rectangle {
            width: view.width
            height: 40
            border {
                color: "black"
                width: 1
            }

            Text {
                anchors.centerIn: parent
                renderType: Text.NativeRendering
                text: "Header"
            }
        }

        footer: Rectangle {
            width: view.width
            height: 40
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

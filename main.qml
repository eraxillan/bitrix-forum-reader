import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import ru.banki.reader 1.0

ApplicationWindow
{
    visible: true
    width: 1280
    height: 720
    title: qsTr("banki.ru forum viewer")
    color: "grey"

    ForumReader
    {
        id: reader
    }

    Component.onCompleted: {
        // Fill the post list
        var i = 0;
        for( ; i < reader.postCount(); i++ )
        {
            dataModel.append( {  "color"      : "lightgrey",
                                 "text"       : "first",
                                 "postAuthor" : reader.postAuthor(i),
                                 "postAvatar" : reader.postAvatarUrl(i),
                                 "postText"   : reader.postText(i)
                             } );
        }
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
            height: 150

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
                        source: reader.avatarUrlForUser( model.postAuthor )

                        width: 100
                        height: 100
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

                    Text {
                        textFormat: Text.RichText
                        anchors.fill: parent
                        anchors.centerIn: parent
                        renderType: Text.NativeRendering
                        text: model.postText
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: view.currentIndex = model.index
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

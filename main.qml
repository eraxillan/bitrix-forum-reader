import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import ru.banki.reader 1.0
//import QtWebKit 3.0

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
        // Add all user avatars
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

    /* ScrollView
    {
        width: 1280
        height: 720

        WebView
        {
            id: webview
            url: "http://qt-project.org"
            anchors.fill: parent
            onNavigationRequested:
            {
                // detect URL scheme prefix, most likely an external link
                var schemaRE = /^\w+:/;
                if (schemaRE.test(request.url)) {
                    request.action = WebView.AcceptRequest;
                } else {
                    request.action = WebView.IgnoreRequest;
                    // delegate request.url here
                }
            }
        }
    }*/
}
/*
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtWebKit 3.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    MainForm {
        anchors.fill: parent
        button1.onClicked: messageDialog.show(qsTr("Button 1 pressed"))
        button2.onClicked: messageDialog.show(qsTr("Button 2 pressed"))
    }

    WebView {
        id: postBrowser
        url: "http://qt-project.org"
        anchors.fill: parent
        onNavigationRequested: {
            // detect URL scheme prefix, most likely an external link
            var schemaRE = /^\w+:/;
            if (schemaRE.test(request.url)) {
                request.action = WebView.AcceptRequest;
            } else {
                request.action = WebView.IgnoreRequest;
                // delegate request.url here
            }
        }
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("May I have your attention, please?")

        function show(caption) {
            messageDialog.text = caption;
            messageDialog.open();
        }
    }
}
*/

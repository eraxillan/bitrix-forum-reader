import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

ListView {
    id: view

    anchors.margins: dp(5)
    anchors.fill: parent
    spacing: dp(5)
    model: dataModel

    clip: true

    ScrollIndicator.vertical: ScrollIndicator {}

    // Current item highlighting setup
    highlight: Rectangle {
        color: "skyblue"
    }
    highlightFollowsCurrentItem: true

    delegate:
    Item {
        property var view: ListView.view
        property var isCurrent: ListView.isCurrentItem

        id: listViewItem
        width: view.width

        MouseArea {
            anchors.fill: parent
            onClicked: view.currentIndex = model.index
        }

        Row {
            id: rowPost
            anchors.fill: parent
            spacing: 0
            padding: 0

            function createAuthorItem() {
                if (!pageLoaded) return;
                if (model.postAuthorQml === "") return;

                var postQmlFileName = reader.applicationDirPath() + "page_" + currentPageIndex + "_post_" + postIndex;
                var postAuthorObj = Qt.createQmlObject(model.postAuthorQml, rectUserInfo, postQmlFileName);
                if (!postAuthorObj) {
                    console.log("User QML object creation FAILED:");
                    console.log(">>>QML BEGIN--------------------------------------------------------");
                    console.log(model.postAuthorQml);
                    console.log("<<<QML END--------------------------------------------------------");
                }

                var postTextObj = Qt.createQmlObject(model.postText, rctItem, postQmlFileName);
                if (!postTextObj) {
                    console.log("User QML object creation FAILED:");
                    console.log(">>>QML BEGIN--------------------------------------------------------");
                    console.log(model.postText);
                    console.log("<<<QML END--------------------------------------------------------");
                }

                listViewItem.height = Qt.binding(function() { return Math.max(postAuthorObj.height, postTextObj.height) + postAuthorObj.padding; });
                rectUserInfo.width = Qt.binding(function() { return postAuthorObj.width + 2*postAuthorObj.padding; });
                rctItem.width = Qt.binding(function() { return parent.width - postAuthorObj.width - 2*postAuthorObj.padding; });
            }
            Component.onCompleted: rowPost.createAuthorItem();

            Rectangle {
                id: rectUserInfo
                height: parent.height

                radius: 0
                color: isCurrent ? "skyblue" : model.color
                border {
                    color: "black"
                    width: 1
                }
            }

            Rectangle {
                id: rctItem
                height: parent.height
                //width: <dynamic binding>

                radius: 0
                color: isCurrent ? "skyblue" : model.color
                border {
                    color: "black"
                    width: 1
                }
            }
        }
    }
}

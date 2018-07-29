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
                color: isCurrent ? "skyblue" : (model.color === undefined ? "lightgrey" : model.color)
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
                color: isCurrent ? "skyblue" : (model.color === undefined ? "lightgrey" : model.color)
                border {
                    color: "black"
                    width: 1
                }
            }
        }
    }

    header: RowLayout {
        enabled: pageLoaded

        width: view.width
        height: dp(30)
        spacing: dp(5)

        Button {
            id: btnFirstPage
            enabled: parent.enabled && (cmbPage.value >= 2)

            text: "<<"

            onClicked: cmbPage.value = 1;

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.bottomMargin: dp(5)
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
                if (qmlInit) {
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

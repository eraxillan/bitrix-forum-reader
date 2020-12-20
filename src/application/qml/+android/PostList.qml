/*
 * This file is part of Bitrix Forum Reader.
 *
 * Copyright (C) 2016-2020 Alexander Kamyshnikov <axill777@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
import QtQuick 2.15
import QtQuick.Controls 2.15

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
}

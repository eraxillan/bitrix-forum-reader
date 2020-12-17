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
import QtMultimedia 5.15
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Window 2.15
import Qt.labs.settings 1.0

import name.eraxillan.bfr 1.0

ApplicationWindow {
    id:      wndMain
    visible: true
    width:   Screen.width
    height:  Screen.height
    title:   qsTr("Bitrix Forum Reader: iOS/tvOS/watchOS version")
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

        onPageCountParsed: {
            console.log("Page count: ", pageCount);
            totalPageCount = pageCount;

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

    ListView {
        id: view

        anchors.margins: dp(5)
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

                        reader.startPageParseAsync(testThreadUrl, cmbPage.value);
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

    Component.onCompleted: {
        reader.startPageCountAsync(testThreadUrl);
    }
}

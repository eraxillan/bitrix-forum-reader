import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Window 2.2
import Qt.labs.settings 1.0

//import Fluid.Core 1.0 as FluidCore
import Fluid.Controls 1.0 as FluidControls

Dialog {
    id: dlgUsers

    x: (parent.width - width) / 2;
    y: (parent.height - height) / 2;
    width: parent.width / 2;
    height: parent.height / 2;

//            property alias textField: textField
//            property alias text: dialogLabel.text

    focus: true
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: {
        // TODO:
    }

    onRejected: {
        // TODO:
    }

    ColumnLayout {
        anchors {
            left: parent.left
            top: parent.top
        }
        width: parent.width

        FluidControls.DialogLabel {
            id: dialogLabel
            wrapMode: Text.Wrap
            text: qsTr("Manage users");

            Layout.fillWidth: true
        }

        TextField {
            id: textField
            focus: true
            onAccepted: dialog.accept()

            Layout.fillWidth: true
        }

        ListView {
            id: lstUsers;

            // FIXME: calculate automatically using binding!
            height: 300;

            Layout.fillWidth: true;
            anchors.margins: dp(5);
            spacing: dp(5);
            clip: true;

            ScrollIndicator.vertical: ScrollIndicator {}

            model: userModel;

            // Current item highlighting setup
            highlight: Rectangle {
                color: "skyblue";
            }
            highlightFollowsCurrentItem: true;

            delegate: Item {
                id: lstUsersItem

                property var view: ListView.view
                property var isCurrent: ListView.isCurrentItem

                width: view.width
                // FIXME: calculate automatically using binding!
                height: 200
                //

                MouseArea {
                    anchors.fill: parent
                    onClicked: view.currentIndex = model.index
                }

                Row {
                    id: rowPost;
                    anchors.fill: parent;
                    spacing: 0;
                    padding: 0;

                    Rectangle {
                        id: rctUserInfo_2;
                        height: parent.height;
                        //
                        //width: parent.width;
                        width: 200
                        //

                        radius: 0;
                        color: isCurrent ? "skyblue" : /*model.color*/ "white";
                        border {
                            color: "black";
                            width: 1;
                        }

                        Column {
                            id: clmnUserInfo_2;
                            spacing: dp(2);
                            padding: dp(5);
                            // FIXME: calculate appropriate for current device width automatically
                            width: dp(250)
                            // NOTE: height will be calculated automatically

                            Text {
                                id: txtUserName_2;
                                text: "<b>" + model.name + "</b>";
                                color: "blue";

                                font.pointSize: 14;

                                elide: Text.ElideRight;
                                wrapMode: Text.WordWrap;
                            }

                            AnimatedImage {
                                id: imgUserAvatar_2;
                                source: reader.convertToUrl(model.avatarUrl);
                                visible: model.avatarUrl !== "";

                                //width:  %4 === -1 ? dp(100) : dp(%4);
                                //height: %5 === -1 ? dp(100) : dp(%5);
                            }
                        }
                    }

                    Column {
                        Text {
                           id: txtAuthorRegistrationDate_2;
                           text: "Registered: " + model.registrationDate;

                           font.pointSize: 14;
                        }

                        Text {
                            id: txtAuthorCity_2;
                            visible: '%9' !== "";
                            text: "City: " + model.city;

                            font.pointSize: 14;
                        }

                        Text {
                            id: txtAuthorPostCount_2;
                            text: "Total posts: " + model.globalPostCount;

                            font.pointSize: 14;
                        }

                        Text {
                            id: txtAuthorThreadPostCount_2;
                            text: "Thread posts: " + model.threadPostCount;

                            font.pointSize: 14;
                        }

                        Text {
                            id: txtAuthorReputation_2;
                            text: "Total reputation: " + model.globalReputation;

                            font.pointSize: 14;
                        }

                        Text {
                            id: txtAuthorThreadReputation_2;
                            text: "Thread reputation: " + model.threadReputation;

                            font.pointSize: 14;
                        }

                        Text {
                            id: txtAuthorRatio_2;
                            text: "Ratio: " + model.ratio;

                            font.pointSize: 14;
                        }

                        Text {
                            id: txtAuthorThreadRatio_2;
                            text: "Thread ratio: " + model.threadRatio;

                            font.pointSize: 14;
                        }
                    }
                }
            }
        }
    }
}

import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import QtQuick.Window 2.2
import Qt.labs.settings 1.0

import Fluid.Core 1.0 as FluidCore
import Fluid.Controls 1.0 as FluidControls


FluidControls.NavigationDrawer {
    id: navDrawer

    /*topContent: Image {
            source: ""

            Layout.fillWidth: true
            Layout.preferredHeight: 200 // window.header.height
        } */

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 0;

        FluidControls.TitleLabel {
            text: qsTr("Basic settings");

            //Layout.alignment: Qt.AlignHCenter;
            Layout.leftMargin: dp(20);
        }

        Switch {
            id: swtImages;

            //Layout.alignment: Qt.AlignHCenter;
            Layout.leftMargin: dp(30);

            enabled: true;
            checked: true;
            text: qsTr("Show images");
        }

        Switch {
            id: swtSavePage;

            //Layout.alignment: Qt.AlignHCenter;
            Layout.leftMargin: dp(30);

            enabled: true;
            checked: true;
            text: qsTr("Save page locally");
        }
    }

    /*FluidControls.ListItem {
            icon.source: FluidControls.Utils.iconUrl("content/inbox")
            text: qsTr("Images")
        }

        FluidControls.ListItem {
            icon.source: FluidControls.Utils.iconUrl("content/archive")
            text: "Archive"
        }

        FluidControls.ListItem {
            icon.source: FluidControls.Utils.iconUrl("action/settings")
            text: "Settings"
            showDivider: true
        }*/

    /*
        actions: [
            FluidControls.Action {
                text: qsTr("Action 1")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                onTriggered: console.log("action1 triggered")
            },
            FluidControls.Action {
                text: qsTr("Action 2")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                hasDividerAfter: true
                onTriggered: console.log("action2 triggered")
            },
            FluidControls.Action {
                text: qsTr("Action 3")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                onTriggered: console.log("action3 triggered")
            },
            FluidControls.Action {
                text: qsTr("Action 4")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                onTriggered: console.log("action4 triggered")
            },
            FluidControls.Action {
                text: qsTr("Action 5")
                icon.source: FluidControls.Utils.iconUrl("action/info")
                visible: false
                onTriggered: console.log("action5 triggered")
            }
        ]*/
}

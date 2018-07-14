import QtMultimedia 5.8;
import QtQuick 2.10;
import QtQuick.Layouts 1.3;
import QtQuick.Window 2.2;
import QtQuick.Controls 2.3;
import QtQuick.Controls.Material 2.3;
import QtQuick.Controls.Universal 2.3;

Column {
    id: clmnPost;

    spacing: dp(10);
    leftPadding: dp(10);
    rightPadding: dp(10);

    Text {
        id: txtPostDateTime;
        width: rctItem.width;

        topPadding: dp(5);
        padding: dp(0);
        horizontalAlignment: Text.AlignLeft;
        verticalAlignment: Text.AlignVCenter;
        clip: false;

        font.pointSize: 14;
        text: Qt.formatDateTime(model.postDateTime);
    }

    Rectangle {
        width: rctItem.width;
        height: dp(2);

        border.width: dp(0);
        color: "lightslategrey";
    }

    Flow {
        width: rctItem.width;

        padding: dp(10);
        spacing: dp(10);

        state: _a130f037750e40c69eb7d4ffc572822a;
    }

    Text {
        id: txtLastEdit_05ea9fa84e5148fdaa80754067b2ddad;
        visible: model.lastEdit !== "";
        width: rctItem.width - parent.rightPadding - parent.leftPadding;

        color: "lightslategrey";
        font.italic: true;
        font.pointSize: 14;

        renderType: Text.NativeRendering;

        text: model.postLastEdit;
        textFormat: Text.RichText;
        onLinkActivated: Qt.openUrlExternally(link);

        clip: false;
        elide: Text.ElideRight;
        wrapMode: Text.WordWrap;
    }

    Rectangle {
        visible: model.authorSignature !== "";
        width: rctItem.width - parent.rightPadding - parent.leftPadding;
        height: dp(1);
        border.width: dp(0);
        color: "lightslategrey";
    }

    Text {
        id: txtPostAuthorSignature_05ea9fa84e5148fdaa80754067b2ddad;
        visible: model.authorSignature !== "";
        width: rctItem.width - parent.rightPadding - parent.leftPadding;

        color: "lightslategrey";
        font.italic: true;
        font.pointSize: 14;

        renderType: Text.NativeRendering;

        text: model.authorSignature;
        textFormat: Text.RichText;
        onLinkActivated: Qt.openUrlExternally(link);

        clip: false;
        elide: Text.ElideRight;
        wrapMode: Text.WordWrap;
    }

    Rectangle {
        visible: model.postLikeCount > 0;
        width: rctItem.width - parent.rightPadding - parent.leftPadding;
        height: dp(1);
        border.width: dp(0);
        color: "lightslategrey";
    }

    Text {
        id: txtPostLikeCounter_05ea9fa84e5148fdaa80754067b2ddad;

        visible: model.postLikeCount > 0;
        width: rctItem.width - parent.rightPadding - parent.leftPadding;
        color: "lightslategrey";

        font.bold: true;
        font.pointSize: 14;
        text: model.postLikeCount + " like(s)";
    }

}    // QML Column end

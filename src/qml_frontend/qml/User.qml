import QtQuick 2.10;
import QtQuick.Layouts 1.3;
import QtQuick.Controls 2.3;
import QtQuick.Controls.Material 2.3;
import QtQuick.Controls.Universal 2.3;
import QtQuick.Window 2.2;

Column {
    id: clmnUserInfo%1;
    spacing: dp(2);
    padding: dp(5);
    // FIXME: calculate appropriate for current device width automatically
    width: dp(250)
    // NOTE: height will be calculated automatically

    Text {
        id: txtUserName%1;
        text: "<b>" + '%2' + "</b>";
        color: "blue";

        font.pointSize: 14;

        elide: Text.ElideRight;
        wrapMode: Text.WordWrap;
    }

    AnimatedImage {
        id: imgUserAvatar%1;
        source: reader.convertToUrl('%3');
        visible: '%3' !== "";

        width:  %4 === -1 ? dp(100) : dp(%4);
        height: %5 === -1 ? dp(100) : dp(%5);
    }

    Text {
        id: txtAuthorPostCount%1;
        text: "Posts: " + %6;

        font.pointSize: 14;
    }

    Text {
       id: txtAuthorRegistrationDate%1;
       text: "Registered:\n" + '%7';

       font.pointSize: 14;
    }

    Text {
        id: txtAuthorReputation%1;
        text: "Reputation: " + %8;

        font.pointSize: 14;
    }

    Text {
        id: txtAuthorCity%1;
        visible: '%9' !== "";
        text: "City:\n" + '%9';

        font.pointSize: 14;
    }
}

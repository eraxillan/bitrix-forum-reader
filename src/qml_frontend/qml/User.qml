import QtQuick 2.10;
import QtQuick.Layouts 1.3;
import QtQuick.Controls 2.15;
import QtQuick.Controls.Material 2.3;
import QtQuick.Controls.Universal 2.3;
import QtQuick.Window 2.2;

Column {
    id: clmnUserInfo_6afb4d56214b4bb69281bf9bbf60396b;
    spacing: dp(2);
    padding: dp(5);
    // FIXME: calculate appropriate for current device width automatically
    width: dp(250)
    // NOTE: height will be calculated automatically

    Text {
        id: txtUserName_6afb4d56214b4bb69281bf9bbf60396b;
        text: "<b>" + '_14c1808e13d44b64accf01b6b8a6be25' + "</b>";
        color: "blue";

        font.pointSize: 14;

        elide: Text.ElideRight;
        wrapMode: Text.WordWrap;
    }

    AnimatedImage {
        id: imgUserAvatar_6afb4d56214b4bb69281bf9bbf60396b;
        source: reader.convertToUrl('_c75e18958b0f43fa9e7881315afccc54');
        visible: '_c75e18958b0f43fa9e7881315afccc54' !== "";

        width:  _1e4e440acb92478197b992539899e5de === -1 ? dp(100) : dp(_1e4e440acb92478197b992539899e5de);
        height: _3860f825cb2a426f82730cc2ca35e6ac === -1 ? dp(100) : dp(_3860f825cb2a426f82730cc2ca35e6ac);
    }

    Text {
        id: txtAuthorPostCount_6afb4d56214b4bb69281bf9bbf60396b;
        text: "Posts: " + _fcfaa58e3305486e872ae962537bdc8e;

        font.pointSize: 14;
    }

    Text {
       id: txtAuthorRegistrationDate_6afb4d56214b4bb69281bf9bbf60396b;
       text: "Registered:\n" + '_e0aefa26825e44b58c74a54b5bad32b8';

       font.pointSize: 14;
    }

    Text {
        id: txtAuthorReputation_6afb4d56214b4bb69281bf9bbf60396b;
        text: "Reputation: " + _875abbe700a0408891e5c6f73b7e27be;

        font.pointSize: 14;
    }

    Text {
        id: txtAuthorCity_6afb4d56214b4bb69281bf9bbf60396b;
        visible: '_3cf1b6db9ccf4404b672f7c9d6c78211' !== "";
        text: "City:\n" + '_3cf1b6db9ccf4404b672f7c9d6c78211';

        font.pointSize: 14;
    }
}

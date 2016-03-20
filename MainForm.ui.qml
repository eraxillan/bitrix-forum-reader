import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtWebKit 3.0

Item {
    id: item1
    width: 640
    height: 480


    RowLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent

        Flickable {
            id: flckPosts
            Layout.fillHeight: false
            anchors.bottomMargin: 20
            anchors.topMargin: 20
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
        }
    }
}

/*Video {
   id: video%1;
   width : dp(400);
   height : dp(400);
   source: '%2';

   Rectangle {
       id: rctVideo%1;
       anchors.fill: parent;
       color: "black";

//    FIXME: draw circle button with inner triangle (i.e. play button)
//    Rectangle { id: rctPlayButton%1; width: parent.width/2; height: parent.height/2; anchors.centerIn: parent; color:"white"; radius: parent.width/2; }

       MouseArea {
           anchors.fill: parent;
           onClicked: {
               rctVideo%1.visible = false;
               video%1.play();
           }
       }
   }

   focus: true;
   Keys.onSpacePressed: video%1.playbackState == MediaPlayer.PlayingState ? video%1.pause() : video%1.play();
   Keys.onLeftPressed: video%1.seek(video%1.position - 5000);
   Keys.onRightPressed: video%1.seek(video%1.position + 5000);
}
*/

Rectangle {
   id: video%1;
   width: dp(400);
   height: dp(400);
   color: "black";
   MouseArea {
       anchors.fill: parent;
       onClicked: { console.log("Opening video URL ", '%2'); Qt.openUrlExternally('%2'); }
   }
}

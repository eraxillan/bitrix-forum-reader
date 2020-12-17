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
   id: video_2f0a985471e44cd69925f80d37de946a;

   width: dp(400);
   height: dp(400);

   color: "black";

   MouseArea {
       anchors.fill: parent;
       onClicked: { console.log("Opening video URL ", '_c66d930f7e324345ba9ae5741f3d4142'); Qt.openUrlExternally('_c66d930f7e324345ba9ae5741f3d4142'); }
   }
}

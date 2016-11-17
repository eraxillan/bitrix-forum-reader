#include "websiteinterface.h"

namespace BankiRuForum
{

IPostObject::~IPostObject()
{
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostQuote

PostQuote::PostQuote()
{
}

bool PostQuote::isValid() const
{
    return !m_data.isEmpty();
}

QString PostQuote::getQmlString() const
{
    const QString qmlStr =
            "Rectangle {\n"
            "   id: rctQuote%1;\n"
            "   color: \"white\";\n"
            "   width: rctItem.width - parent.rightPadding - parent.leftPadding;\n"
            "   height: rctQuoteTitle%1.height + txtQuoteBody%1.height;\n"
            "\n"
            "   Rectangle {\n"
            "       id: rctQuoteTitle%1;\n"
            "       color: \"silver\";\n"
            "       width: parent.width;\n"
            "       height: dp(50);\n"
            "\n"
            "       Column {\n"
            "           width: parent.width;\n"
            "           height: parent.height;\n"
            "           spacing: dp(2);\n"
            "\n"
            "           Text {\n"
            "               id: txtQuoteTitle%1;\n"
            "\n"
            "               leftPadding: dp(20);\n"
            "               verticalAlignment: Text.AlignVCenter;\n"
            "\n"
            "               width: parent.width;\n"
            "               height: parent.height;\n"
            "\n"
            "               font.pixelSize: sp(2);\n"
            "               text: \"%2\";\n"
            "           }\n"
            "\n"
            "           Column { id: txtQuoteBody%1; %3 }"
            "       }\n"
            "   }\n"
            "}\n";

    QString quoteQml;
    IPostObjectList::const_iterator iObj = m_data.begin();
    for (; iObj != m_data.end(); ++iObj)
    {
        quoteQml += (*iObj)->getQmlString();
    }

    return qmlStr.arg(qrand()).arg(m_title).arg(quoteQml);
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostImage

PostImage::PostImage()
{}

PostImage::PostImage(QString url, int width, int height, int border, QString altName, QString id, QString className)
    : m_url(url), m_width(width), m_height(height), m_border(border), m_altName(altName), m_id(id), m_className(className)
{
}

bool PostImage::isValid() const
{
    return !m_url.isEmpty() && (m_width > 0 && m_height > 0);
}

QString PostImage::getQmlString() const
{
    // FIXME: use other fields e.g. width and height
    const QString qmlStr =
            "AnimatedImage { id: imgSmile%1; source: \"%2\"; }";
    return qmlStr.arg(qrand()).arg(m_url);
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostPlainText

PostPlainText::PostPlainText()
{
}

PostPlainText::PostPlainText(QString text)
    : m_text(text)
{
}

bool PostPlainText::isValid() const
{
    return !m_text.isEmpty();
}

QString PostPlainText::getQmlString() const
{
    const QString qmlStr =
            "Text {\n"
            "   id: dynTxtPost%1;\n"
            "   width: rctItem.width - parent.rightPadding - parent.leftPadding;\n"
            "\n"
            "   font.pixelSize: sp(2);\n"
            "\n"
            "   text: '%2';\n"
            "   textFormat: Text.PlainText;\n"
            "\n"
            "   elide: Text.ElideRight;\n"
            "   wrapMode: Text.WordWrap;\n"
            "\n"
#ifdef RBR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
            "   Rectangle {\n"
            "       border.width: dp(1);\n"
            "       border.color: \"red\";\n"
            "       color: \"transparent\";\n"
            "       width: parent.width;\n"
            "       height: parent.height;\n"
            "   }\n"
#endif
            "}";
    return qmlStr.arg(qrand()).arg(m_text);
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostRichText

PostRichText::PostRichText()
{
}

PostRichText::PostRichText(QString text, bool isBold, bool isItalic, bool isUnderlined)
    : m_text(text), m_isBold(isBold), m_isItalic(isItalic), m_isUnderlined(isUnderlined)
{
}

bool PostRichText::isValid() const
{
    return !m_text.isEmpty();
}

QString PostRichText::getQmlString() const
{
    const QString qmlStr =
            "Text {\n"
            "   id: dynTxtPost%1;\n"
            "   width: rctItem.width - parent.rightPadding - parent.leftPadding;\n"
            "\n"
            "   font.bold: %2;\n"
            "   font.italic: %3;\n"
            "   font.underline: %4;\n"
            "   font.pixelSize: sp(2);\n"
            "\n"
            "   text: '%5';\n"
            "   textFormat: Text.PlainText;\n"
            "\n"
            "   elide: Text.ElideRight;\n"
            "   wrapMode: Text.WordWrap;\n"
            "\n"
#ifdef RBR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
            "   Rectangle {\n"
            "       border.width: dp(1);\n"
            "       border.color: \"red\";\n"
            "       color: \"transparent\";\n"
            "       width: parent.width;\n"
            "       height: parent.height;\n"
            "   }\n"
#endif
            "}";
    return  qmlStr.arg(qrand())
            .arg(m_isBold ? "true" : "false").arg(m_isItalic ? "true" : "false").arg(m_isUnderlined ? "true" : "false")
            .arg(m_text);
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostVideo

PostVideo::PostVideo()
{
}

PostVideo::PostVideo(QString urlStr)
    : m_urlStr(urlStr), m_url(urlStr)
{
    Q_ASSERT(m_url.isValid());

    // FIXME: currently only YouTube videos are supported
    // Use youtube-dl to download all kind of network videos here!
    Q_ASSERT(urlStr.contains("youtube"));

    // Example of YouTube URL: https://www.youtube.com/watch?v=PI9o3v4nttU
    QUrlQuery urlQuery(m_url);
    QString videoId = urlQuery.queryItemValue("v");

    // FIXME: replace this ugly hardcoded path with e.g. environment varible
    QProcess youtubeDlProcess;
    youtubeDlProcess.start("C:\\Users\\2\\Downloads\\youtube-dl.exe", QStringList() << "--get-url" << videoId);
    if (!youtubeDlProcess.waitForStarted()) Q_ASSERT(0);
    youtubeDlProcess.closeWriteChannel();
    if (!youtubeDlProcess.waitForFinished()) Q_ASSERT(0);

    QByteArray result = youtubeDlProcess.readAll();
    if (!result.isEmpty())
    {
        result = result.trimmed();
        m_urlStr = QString::fromUtf8(result);
        m_url = QUrl(m_urlStr);
        Q_ASSERT(m_url.isValid());
    }
    else qDebug() << "ERROR: unable to get full URL of video " << videoId;
}

bool PostVideo::isValid() const
{
    return !m_urlStr.isEmpty() && m_url.isValid();
}

QString PostVideo::getQmlString() const
{
    const QString qmlStr =
            "Video {\n"
            "   id: video%1;\n"
            "   width : dp(400);\n"
            "   height : dp(400);\n"
            "   source: '%2';\n"
            "\n"
            "   Rectangle {\n"
            "       id: rctVideo%1;\n"
            "       anchors.fill: parent;\n"
            "       color: \"black\";\n"
            // FIXME: draw circle button with inner triangle (i.e. play button)
            //"       Rectangle { id: rctPlayButton%1; width: parent.width/2; height: parent.height/2; anchors.centerIn: parent; color:\"white\"; radius: parent.width/2; }"
            "\n"
            "       MouseArea {\n"
            "           anchors.fill: parent;\n"
            "           onClicked: {\n"
            "               rctVideo%1.visible = false;\n"
            "               video%1.play();\n"
            "           }\n"
            "       }\n"
            "   }\n"
            "\n"
            "   focus: true;\n"
            "   Keys.onSpacePressed: video%1.playbackState == MediaPlayer.PlayingState ? video%1.pause() : video%1.play();\n"
            "   Keys.onLeftPressed: video%1.seek(video%1.position - 5000);\n"
            "   Keys.onRightPressed: video%1.seek(video%1.position + 5000);\n"
            "}\n";
    return qmlStr.arg(qrand()).arg(m_urlStr);
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostHyperLink

PostHyperlink::PostHyperlink()
{
}

PostHyperlink::PostHyperlink(QString urlStr, QString title, QString tip, QString rel)
    : m_urlStr(urlStr), m_url(urlStr), m_title(title), m_tip(tip), m_rel(rel)
{
    if (m_title.isEmpty()) m_title = m_urlStr;
    Q_ASSERT(m_url.isValid());
}

bool PostHyperlink::isValid() const
{
    return !m_urlStr.isEmpty() && m_url.isValid();
}

QString PostHyperlink::getQmlString() const
{
    const QString qmlStr =
            "Text\n"
            "{\n"
            "   id: dynTxtPost%1;\n"
            "   width: rctItem.width - parent.rightPadding - parent.leftPadding;\n"
            "\n"
            "   font.pixelSize: sp(2);\n"
            "   renderType: Text.NativeRendering\n"
            "\n"
            "   text: '%2';\n"
            "   textFormat: Text.RichText;\n"
            "   onLinkActivated: Qt.openUrlExternally(link);\n"
            "\n"
            "   clip: false;\n"
            "   elide: Text.ElideRight;\n"
            "   wrapMode: Text.WordWrap;\n"
            "\n"
#ifdef RBR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
            "   Rectangle {\n"
            "       border.width: dp(1);\n"
            "       border.color: \"red\";\n"
            "       color: \"transparent\";\n"
            "       width: parent.width;\n"
            "       height: parent.height;\n"
            "   }\n"
#endif
            "}\n";
    return qmlStr.arg(qrand()).arg("<a href=\"" + m_urlStr + "\">" + m_title + "</a>");
}

// ----------------------------------------------------------------------------------------------------------------------------
// IForumPageReader

IForumPageReader::~IForumPageReader() {}

}

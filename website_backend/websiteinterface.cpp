#include "websiteinterface.h"

namespace BankiRuForum
{

IPostObject::~IPostObject()
{
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostSpoiler
PostSpoiler::PostSpoiler()
{
}

bool PostSpoiler::isValid() const
{
    return !m_data.isEmpty();
}

uint PostSpoiler::getHash(uint seed) const
{
    return qHash(m_title, seed) ^ qHash(m_data, seed);
}

QString PostSpoiler::getQmlString(int randomSeed) const
{
    const QString qmlStr =
            "Rectangle {\n"
            "   id: rctSpoiler%1;\n"
            "   color: \"white\";\n"
            "   width: rctItem.width - parent.rightPadding - parent.leftPadding - dp(20);\n"
            "   height: rctQuoteTitle%1.height + txtSpoilerBody%1.height + 2*dp(5);\n"
            "\n"
            "   border.width: dp(2);\n"
            "   border.color: \"silver\";\n"
            "\n"
            "   Rectangle {\n"
            "       id: rctQuoteTitle%1;\n"
            "       color: \"silver\";\n"
            "       width: parent.width;\n"
            "       height: txtQuoteTitle%1.height;\n"
            "\n"
            "       property int txtSpoilerBodyHeight: 0;\n"
            "\n"
            "       Component.onCompleted: {\n"
            "           rctQuoteTitle%1MA.onClicked(null);\n"
            "           rctQuoteTitle%1MA.onClicked(null);\n"
            "           rctQuoteTitle%1MA.onClicked(null);\n"
            "       }\n"
            "\n"
            "       MouseArea {\n"
            "           id: rctQuoteTitle%1MA;\n"
            "\n"
            "           anchors.fill: parent;\n"
            "           onClicked: {\n"
            "               txtSpoilerBody%1.visible = !txtSpoilerBody%1.visible;\n"
            "\n"
            "               if (txtSpoilerBody%1.height > 0) {\n"
            "                   rctQuoteTitle%1.txtSpoilerBodyHeight = txtSpoilerBody%1.height;\n"
            "                   txtSpoilerBody%1.height = 0;\n"
            "                   txtQuoteTitle%1.text = '%2 \u25BC';\n"
            "                   rctSpoiler%1.height = Qt.binding(function() { return rctQuoteTitle%1.height + txtSpoilerBody%1.height - 2*dp(5); });\n"
            "               } else {\n"
            "                   txtSpoilerBody%1.height = rctQuoteTitle%1.txtSpoilerBodyHeight;\n"
            "                   txtQuoteTitle%1.text = '%2 \u25B2';\n"
            "                   rctSpoiler%1.height = Qt.binding(function() { return rctQuoteTitle%1.height + txtSpoilerBody%1.height + 2*dp(5); });\n"
            "               }\n"
            "           }\n"
            "       }\n"
            "\n"
            "       Column {\n"
            "           width: parent.width;\n"
            "           height: parent.height;\n"
            "           spacing: dp(5);\n"
            "\n"
            "           Text {\n"
            "               id: txtQuoteTitle%1;\n"
            "\n"
            "               leftPadding: dp(10);\n"
            "               horizontalAlignment: Text.AlignHCenter;\n"
            "               verticalAlignment: Text.AlignVCenter;\n"
            "\n"
            "               width: parent.width;\n"
            "\n"
            "               font.pointSize: 14;\n"
            "               font.bold: true;\n"
            "               text: '%2';\n"
            "           }\n"
            "\n"
            "           Flow {\n"
            "               id: txtSpoilerBody%1;\n"
            "\n"
            "               leftPadding: dp(10);\n"
            "\n"
            "               width: parent.width;\n"
            "\n"
            "               %3\n"
            "           }\n"
            "       }\n"
            "   }\n"
            "}\n";

    QString quoteQml;
    IPostObjectList::const_iterator iObj = m_data.begin();
    for (; iObj != m_data.end(); ++iObj)
    {
        quoteQml += (*iObj)->getQmlString(qrand());
    }
    return qmlStr.arg(QString::number(randomSeed), m_title /*+ " \u25B2"*/, quoteQml);
}

// PostQuote

PostQuote::PostQuote()
{
}

bool PostQuote::isValid() const
{
    return !m_data.isEmpty();
}

uint PostQuote::getHash(uint seed) const
{
    return qHash(m_title, seed) ^ qHash(m_userName, seed) ^ qHash(m_url, seed) ^ qHash(m_data, seed);
}

QString PostQuote::getQmlString(int randomSeed) const
{
    const QString qmlStr =
            "Rectangle {\n"
            "   id: rctQuote%1;\n"
            "   color: \"white\";\n"
            "   width: rctItem.width - parent.rightPadding - parent.leftPadding - dp(20);\n"
            "   height: rctQuoteTitle%1.height + txtQuoteBody%1.height + txtQuoteSourceRef%1.height + 2*dp(5);\n"
            "\n"
            "   border.width: dp(2);\n"
            "   border.color: \"silver\";\n"
            "\n"
            "   Rectangle {\n"
            "       id: rctQuoteTitle%1;\n"
            "       color: \"silver\";\n"
            "       width: parent.width;\n"
            "       height: txtQuoteTitle%1.height;\n"
            "\n"
            "       Column {\n"
            "           width: parent.width;\n"
            "           height: parent.height;\n"
            "           spacing: dp(5);\n"
            "\n"
            "           Text {\n"
            "               id: txtQuoteTitle%1;\n"
            "\n"
            "               leftPadding: dp(10);\n"
            "               verticalAlignment: Text.AlignVCenter;\n"
            "\n"
            "               width: parent.width;\n"
            "\n"
            "               font.pointSize: 14;\n"
            "               text: '%2';\n"
            "           }\n"
            "\n"
            "           Row {\n"
            "               id: txtQuoteSourceRef%1;\n"
            "\n"
            "               leftPadding: dp(10);\n"
            "\n"
            "               Text { font.pointSize: 14; font.bold: true; text: '%3  '; }\n"
            "               %4\n"
            "               Text { visible: %6; font.pointSize: 14; text: ':'; }\n"
            "           }\n"
            "           Flow {\n"
            "               id: txtQuoteBody%1;\n\n"
            "\n"
            "               leftPadding: dp(10);\n"
            "\n"
            "               %5\n"
            "               width: parent.width;\n"
            "           }\n"
            "       }\n"
            "   }\n"
            "}\n";

    QString quoteQml;
    IPostObjectList::const_iterator iObj = m_data.begin();
    for (; iObj != m_data.end(); ++iObj)
    {
        quoteQml += (*iObj)->getQmlString(qrand());
    }

    QString titleEsc = QString(m_title).replace("'", "\\'");
    QString userNameEsc = QString(m_userName).replace("'", "\\'");
    QString urlText = m_url.isValid() ? PostHyperlink(m_url.toString(), QUOTE_WRITE_VERB).getQmlString(randomSeed) : QString();

    return qmlStr.arg(
                QString::number(randomSeed),
                titleEsc,
                userNameEsc,
                urlText,
                quoteQml,
                !userNameEsc.isEmpty() ? "true" : "false");
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostImage

PostImage::PostImage()
{
}

PostImage::PostImage(QString url, int width, int height, int border, QString altName, QString id, QString className)
    : m_url(url), m_width(width), m_height(height), m_border(border), m_altName(altName), m_id(id), m_className(className)
{
}

bool PostImage::isValid() const
{
    // NOTE: image with only correct URL specified is still valid
    return !m_url.isEmpty() /*&& (m_width > 0 && m_height > 0)*/;
}

uint PostImage::getHash(uint seed) const
{
    // NOTE: m_id is auto-generated each time page reloaded, so it is useless for hashing purposes
    return qHash(m_url, seed) ^ qHash(m_width, seed) ^ qHash(m_height, seed) ^ qHash(m_border, seed)
            ^ qHash(m_altName, seed) /* ^ qHash(m_id, seed) */ ^ qHash(m_className, seed);
}

QString PostImage::getQmlString(int randomSeed) const
{
    if (!m_url.endsWith(".gif"))
    {
        return QString("Image { id: img%1; source: '%2' }")
                .arg(QString::number(randomSeed), m_url);
    }

    // NOTE: implement other fields usage if they will be implemented on the forum side
    Q_ASSERT(m_width == -1);
    Q_ASSERT(m_height == -1);
    return QString("AnimatedImage { id: imgSmile%1; source: '%2'; }").arg(QString::number(randomSeed), m_url);
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostLineBreak
PostLineBreak::PostLineBreak()
{
}

bool PostLineBreak::isValid() const
{
    return true;
}

uint PostLineBreak::getHash(uint seed) const
{
    return qHash(0, seed);
}

QString PostLineBreak::getQmlString(int randomSeed) const
{
    const QString qmlStr =
            "Text {\n"
            "   id: lineBreak%1\n"
            "   width: rctItem.width - parent.rightPadding - parent.leftPadding;\n\n"
            "   height: 1;\n"
        #ifdef RBR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
            "   Rectangle {\n"
            "       border.width: dp(1);\n"
            "       border.color: \"yellow\";\n"
            "       color: \"transparent\";\n"
            "       width: parent.width;\n"
            "       height: parent.height;\n"
            "   }\n"
        #endif
            "}\n";
    return qmlStr.arg(randomSeed);
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

uint PostPlainText::getHash(uint seed) const
{
    return qHash(m_text, seed);
}

QString PostPlainText::getQmlString(int randomSeed) const
{
    const QString qmlStr =
            "Text {\n"
            "   property int postWidth: rctItem.width - parent.rightPadding - parent.leftPadding - dp(20);\n"
            "\n"
            "   id: dynTxtPost%1;\n"
            "   Component.onCompleted: { width = contentWidth >= postWidth ? postWidth : contentWidth; }\n"
            "\n"
            "   font.pointSize: 14;\n"
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
            "}\n";
    QString textEsc = QString(m_text).replace("'", "\\'");
    return qmlStr.arg(QString::number(randomSeed), textEsc);
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostRichText

PostRichText::PostRichText()
{
}

PostRichText::PostRichText(QString text, QString color, bool isBold, bool isItalic, bool isUnderlined, bool isStrikedOut)
    : m_text(text), m_color(color), m_isBold(isBold), m_isItalic(isItalic), m_isUnderlined(isUnderlined), m_isStrikedOut(isStrikedOut)
{
}

bool PostRichText::isValid() const
{
    return !m_text.isEmpty();
}

uint PostRichText::getHash(uint seed) const
{
    return qHash(m_text, seed) ^ qHash(m_color, seed) ^ qHash(m_isBold, seed)
            ^ qHash(m_isItalic, seed) ^ qHash(m_isUnderlined, seed) ^ qHash(m_isStrikedOut, seed);
}

QString PostRichText::getQmlString(int randomSeed) const
{
    const QString qmlStr =
            "Text {\n"
            "   id: dynTxtPost%1;\n"
//            "   width: rctItem.width - parent.rightPadding - parent.leftPadding;\n"
            "\n"
            "   color: '%2';\n"
            "\n"
            "   font.bold: %3;\n"
            "   font.italic: %4;\n"
            "   font.underline: %5;\n"
            "   font.strikeout: %6;\n"
            "   font.pointSize: 14;\n"
            "\n"
            "   text: '%7';\n"
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
            "}\n";

    QString textEsc = QString(m_text).replace("'", "\\'");
    return  qmlStr.arg(
                QString::number(randomSeed),
                m_color,
                m_isBold ? "true" : "false",
                m_isItalic ? "true" : "false",
                m_isUnderlined ? "true" : "false",
                m_isStrikedOut ? "true" : "false",
                textEsc);
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostVideo

PostVideo::PostVideo()
{
}

namespace {
#ifndef Q_OS_IOS
static bool findBestVideoUrl(QByteArray aJsonData, QString& aVideoUrlStr)
{
    aVideoUrlStr = "";

    int maxVideoWidth = -1;
//    int maxVideoHeight = -1;

    QJsonDocument jsonDoc(QJsonDocument::fromJson(aJsonData));
    QJsonObject jsonRootObject = jsonDoc.object();
    QJsonArray jsonFormatsArray = jsonRootObject["formats"].toArray();
    for (int i = 0; i < jsonFormatsArray.size(); ++i)
    {
        QJsonObject jsonFormatObject = jsonFormatsArray[i].toObject();
        Q_ASSERT(!jsonFormatObject.isEmpty());

        Q_ASSERT(jsonFormatObject.value("vcodec").isString());
        Q_ASSERT(jsonFormatObject.value("acodec").isString());
        Q_ASSERT(jsonFormatObject.value("ext").isString());
        Q_ASSERT(jsonFormatObject.value("format_note").isString());
        // FIXME: determine type of these values
        //Q_ASSERT(jsonFormatObject.value("width").isDouble());
        //Q_ASSERT(jsonFormatObject.value("height").isDouble());
        Q_ASSERT(jsonFormatObject.value("url").isString());

        QString videoCodecStr = jsonFormatObject.value("vcodec").toString();
//        QString audioCodecStr = jsonFormatObject.value("acodec").toString();
//        QString videoFileExt = jsonFormatObject.value("ext").toString();
        QString videoFormatNote = jsonFormatObject.value("format_note").toString();
        int videoWidth = jsonFormatObject.value("width").toInt(-1);
        int videoHeight = jsonFormatObject.value("height").toInt(-1);
        QString videoUrlStr = jsonFormatObject.value("url").toString();

        if (videoWidth < 0 || videoHeight < 0 || videoCodecStr.compare("none", Qt::CaseInsensitive) == 0) continue;
        if (videoUrlStr.isEmpty()) continue;
        if (videoFormatNote.startsWith("DASH")) continue;

        if (videoWidth > maxVideoWidth)
        {
            maxVideoWidth = videoWidth;
//            maxVideoHeight = videoHeight;

            aVideoUrlStr = videoUrlStr;
        }

#ifdef RBR_PRINT_DEBUG_OUTPUT
        qDebug() << "--------------------------------------------------";
        qDebug() << "Video width:" << videoWidth;
        qDebug() << "Video height:" << videoHeight;
        qDebug() << "Video codec:" << videoCodecStr;
        qDebug() << "Audio codec:" << audioCodecStr;
        qDebug() << "File format:" << videoFormatNote;
        qDebug() << "File extension:" << videoFileExt;
        qDebug() << "--------------------------------------------------";
#endif
    }

#ifdef RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "Maximum resolution: " << maxVideoWidth << " x " << maxVideoHeight;
#endif

    return true;
}
#endif
}

PostVideo::PostVideo(QString urlStr)
    : m_urlStr(urlStr), m_url(urlStr)
{
    Q_ASSERT(m_url.isValid());

    // Example of YouTube URL: https://www.youtube.com/watch?v=PI9o3v4nttU
    QUrlQuery urlQuery(m_url);
    QString videoId = urlQuery.queryItemValue("v");

    // FIXME: replace this ugly hardcoded path with e.g. environment varible
#if !defined(Q_OS_IOS)
    QProcess youtubeDlProcess;
#if defined(Q_OS_WIN)
    youtubeDlProcess.start("C:\\Users\\2\\Downloads\\youtube-dl.exe", QStringList() << "-J" << "--skip-download" << videoId);
#elif defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_OSX)
    youtubeDlProcess.start("youtube-dl", QStringList() << "-J" << "--skip-download" << videoId);
#elif defined(Q_OS_OSX)
    youtubeDlProcess.start("/usr/local/bin/youtube-dl", QStringList() << "-J" << "--skip-download" << videoId);
#elif defined(Q_OS_ANDROID)
    // FIXME: embed youtube-dl executable to APK
    m_url.clear();
    m_urlStr.clear();
    return;
#else
#error "Unsupported OS"
#endif

    if (!youtubeDlProcess.waitForStarted()) Q_ASSERT(0);
    youtubeDlProcess.closeWriteChannel();
    if (!youtubeDlProcess.waitForFinished()) Q_ASSERT(0);

    QByteArray result = youtubeDlProcess.readAll();
    findBestVideoUrl(result, m_urlStr);
#else
#pragma message("iOS do not support QProcess")
    m_url.clear();
    m_urlStr.clear();
#endif
}

bool PostVideo::isValid() const
{
    return !m_urlStr.isEmpty() && m_url.isValid();
}

uint PostVideo::getHash(uint seed) const
{
    return qHash(m_urlStr, seed) ^ qHash(m_url, seed);
}

QString PostVideo::getQmlString(int randomSeed) const
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
    return qmlStr.arg(QString::number(randomSeed), m_urlStr);
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

uint PostHyperlink::getHash(uint seed) const
{
    return qHash(m_urlStr, seed) ^ qHash(m_url, seed) ^ qHash(m_title, seed)
            ^ qHash(m_tip, seed) ^ qHash(m_rel, seed);
}

QString PostHyperlink::getQmlString(int randomSeed) const
{
    const QString qmlStr =
            "Text {\n"
            "   id: dynTxtPost%1;\n"
//            "   width: rctItem.width - parent.rightPadding - parent.leftPadding;\n"
            "\n"
            "   font.pointSize: 14;\n"
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
    return qmlStr.arg(QString::number(randomSeed), "<a href=\"" + m_urlStr + "\">" + m_title + "</a>");
}

// ----------------------------------------------------------------------------------------------------------------------------
// IForumPageReader

IForumPageReader::~IForumPageReader() {}

}

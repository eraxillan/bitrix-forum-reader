#include "forumreader.h"

#include "website_backend/gumboparserimpl.h"
#include "common/filedownloader.h"

namespace {
template <typename T>
void dumpFutureObj(QFuture<T> future, QString name)
{
#ifdef RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "----------------------------------------------------";
    qDebug() << "Future name:"        << name;
    qDebug() << "Future is started:"  << future.isStarted();
    qDebug() << "Future is running:"  << future.isRunning();
    qDebug() << "Future is finished:" << future.isFinished();
    qDebug() << "Future is paused:"   << future.isPaused();
    qDebug() << "Future is canceled:" << future.isCanceled();
    qDebug() << "Future has result:"  << future.isResultReadyAt(0);
    qDebug() << "----------------------------------------------------";
#else
    Q_UNUSED(future);
    Q_UNUSED(name);
#endif
}
}

#ifdef RBR_DUMP_GENERATED_QML_IN_FILES
namespace {
static bool WriteTextFile(QString fileName, QString fileContents)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << fileContents;
    return true;
}
}
#endif

ForumReader::ForumReader() :
    m_forumPageCountWatcher(),
    m_forumPageParserWatcher(),
    m_pagePosts(),
    m_pageCount(0),
    m_pageNo(0),
    m_lastError(ResultCode::S_OK)
{
    connect(&m_forumPageCountWatcher, &IntFutureWatcher::finished, this, &ForumReader::onForumPageCountParsed);
    connect(&m_forumPageCountWatcher, &IntFutureWatcher::canceled, this, &ForumReader::onForumPageCountParsingCanceled);

    connect(&m_forumPageParserWatcher, &ParserFutureWatcher::finished, this, &ForumReader::onForumPageParsed);
    connect(&m_forumPageParserWatcher, &ParserFutureWatcher::canceled, this, &ForumReader::onForumPageParsingCanceled);
}

ForumReader::~ForumReader()
{
}

QString ForumReader::applicationDirPath() const
{
    QString result = qApp->applicationDirPath();
    if (!result.endsWith("/")) result += "/";
    return result;
}

QUrl ForumReader::convertToUrl(QString urlStr) const
{
    return QUrl(urlStr);
}

int ForumReader::parsePageCount(QString urlStr)
{
    // Cleanup
    m_pagePosts.clear();
    m_pageCount = 0;

    // 1) Download the first forum web page
    QByteArray htmlRawData;
    if (!FileDownloader::downloadUrl(urlStr, htmlRawData)) { Q_ASSERT(0); return 0; }

    // 2) Parse the page HTML to get the page number
    BankiRuForum::ForumPageParser fpp;
    int resultFpp = fpp.getPageCount(htmlRawData, m_pageCount);
    Q_ASSERT(resultFpp == 0); if (resultFpp != 0) { m_pageCount = 0; return 0; }
    return m_pageCount;
}

namespace {
int parsePageCountAsync(QString urlStr)
{
    int result = 0;

    // 1) Download the first forum web page
    QByteArray htmlRawData;
    if (!FileDownloader::downloadUrl(urlStr, htmlRawData))
    {
        Q_ASSERT(0);
        return result;
    }

    // 2) Parse the page HTML to get the page number
    BankiRuForum::ForumPageParser fpp;
    int resultFpp = fpp.getPageCount(htmlRawData, result);
    if (resultFpp != 0)
    {
        Q_ASSERT(0);
        result = 0;
        return result;
    }

    return result;
}
}

void ForumReader::startPageCountAsync(QString urlStr)
{
    dumpFutureObj(m_forumPageCountWatcher.future(), "m_forumPageCountWatcher");

    // Wait for previous operation finish (if any)
    // NOTE: QtConcurrent::run() return future that cannot be canceled
    //m_forumPageCountWatcher.cancel();
    m_forumPageCountWatcher.waitForFinished();

    auto forumPageCountFuture = QtConcurrent::run(std::bind(parsePageCountAsync, urlStr));
    m_forumPageCountWatcher.setFuture(forumPageCountFuture);
}

bool ForumReader::parseForumPage(QString urlStr, int pageNo)
{
    // Cleanup
    m_pagePosts.clear();
    m_pageCount = 0;
    m_pageNo = pageNo;

    // 1) Download the first forum web page
    QByteArray htmlRawData;
    if (!FileDownloader::downloadUrl(urlStr, htmlRawData)) { Q_ASSERT(0); return false; }

    // 2) Parse the page HTML to get the page number
    BankiRuForum::ForumPageParser fpp;
    int resultFpp = fpp.getPageCount(htmlRawData, m_pageCount);
    Q_ASSERT(resultFpp == 0); if (resultFpp != 0) { m_pageCount = 0; return false; }

    // 3) Parse the page HTML to get the page user posts
    resultFpp = fpp.getPagePosts(htmlRawData, m_pagePosts);
    Q_ASSERT(resultFpp == 0); if (resultFpp != 0) { m_pagePosts.clear(); return false; }
    return true;
}

namespace {
// NOTE: QtConcurrent require to return collection; return result code will be much more straightforward to reader
BankiRuForum::UserPosts parsePageAsync(QString urlStr, ResultCode& errorCode)
{
    BankiRuForum::UserPosts result;
    errorCode = ResultCode::S_OK;

    // 1) Download the first forum web page
    QByteArray htmlRawData;
    if (!FileDownloader::downloadUrl(urlStr, htmlRawData))
    {
        Q_ASSERT(0);
        errorCode = ResultCode::E_NETWORK;
        return result;
    }

    // 2) Parse the page HTML to get the page number
    // FIXME: implement updatePageCount() method
    BankiRuForum::ForumPageParser fpp;
    /*int resultFpp = fpp.getPageCount(htmlRawData, m_pageCount);
    if (resultFpp != 0)
    {
        Q_ASSERT(0);
        ok = false;
        return result;
    }*/

    // 3) Parse the page HTML to get the page user posts
    // FIXME: rewrite with 'enum class'
    errorCode = (ResultCode)fpp.getPagePosts(htmlRawData, result);
    if (errorCode != ResultCode::S_OK)
    {
        Q_ASSERT(0);
        return result;
    }

    return result;
}
}

void ForumReader::startPageParseAsync(QString urlStr, int pageNo)
{
    dumpFutureObj(m_forumPageParserWatcher.future(), "m_forumPageParserWatcher");

    // Cleanup
    m_pagePosts.clear();
    // FIXME: implement updatePageNumber() method and call it here
//    m_pageCount = 0;
    m_pageNo = pageNo;

    auto forumPageParseFuture = QtConcurrent::run(std::bind(parsePageAsync, urlStr, std::ref(m_lastError)));
    m_forumPageParserWatcher.setFuture(forumPageParseFuture);
}

void ForumReader::onForumPageCountParsed()
{
    dumpFutureObj(m_forumPageCountWatcher.future(), "m_forumPageCountWatcher");

    int pageCount = m_forumPageCountWatcher.result();
    Q_ASSERT(pageCount > 0);
    m_pageCount = pageCount;
    emit pageCountParsed(pageCount);
}

void ForumReader::onForumPageCountParsingCanceled()
{
    Q_ASSERT_X(0, Q_FUNC_INFO, "QtConcurrent::run result cannot be canceled");
}

void ForumReader::onForumPageParsed()
{
    dumpFutureObj(m_forumPageParserWatcher.future(), "m_forumPageParserWatcher");

    m_pagePosts = m_forumPageParserWatcher.result();
    emit pageContentParsed(m_pageNo);
}

void ForumReader::onForumPageParsingCanceled()
{
    Q_ASSERT_X(0, Q_FUNC_INFO, "QFuture returned by QtConcurrent::run() cannot be canceled");
}

// ----------------------------------------------------------------------------------------------------------------------------------------

int ForumReader::pageCount() const
{
    return m_pageCount;
}

int ForumReader::postCount() const
{
    return m_pagePosts.size();
}

QString ForumReader::postAuthor(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].first.m_userName;
}

QString ForumReader::postAvatarUrl(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    if (m_pagePosts[index].first.m_userAvatar.isNull()) return QString();
    return m_pagePosts[index].first.m_userAvatar->m_url;
}

int ForumReader::postAvatarWidth(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    if (m_pagePosts[index].first.m_userAvatar.isNull()) return -1;
    return m_pagePosts[index].first.m_userAvatar->m_width;
}

int ForumReader::postAvatarHeight(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    if (m_pagePosts[index].first.m_userAvatar.isNull()) return -1;
    return m_pagePosts[index].first.m_userAvatar->m_height;
}

int ForumReader::postAvatarMaxWidth() const
{
    int maxWidth = 100;
    for(int i = 0; i < m_pagePosts.size(); ++i)
    {
        if (m_pagePosts[i].first.m_userAvatar.isNull()) continue;
        int width = m_pagePosts[i].first.m_userAvatar->m_width;
        if(width > maxWidth) maxWidth = width;
    }
    return maxWidth;
}

QDateTime ForumReader::postDateTime(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].second.m_date;
}

QString ForumReader::postText(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());
    if (m_pagePosts[index].second.m_data.empty()) return QString();

    QString qmlStr =
            "import QtMultimedia 5.6;\n"
            "import QtQuick 2.6;\n"
            "import QtQuick.Window 2.2;\n"
            "import QtQuick.Controls 1.5;\n"
            "import QtQuick.Dialogs 1.2;\n\n";

    int randomSeed = 0;
    int validItemsCount = 0;
    BankiRuForum::IPostObjectList::const_iterator iObj = m_pagePosts[index].second.m_data.begin();
    for (; iObj != m_pagePosts[index].second.m_data.end(); ++iObj)
    {
        randomSeed = qrand();
        if (!(*iObj)->isValid() || (*iObj)->getQmlString(randomSeed).isEmpty()) continue;

        validItemsCount++;
    }

    if (validItemsCount == 0) return QString();
    if (validItemsCount == 1)
    {
        randomSeed = qrand();
        qmlStr += m_pagePosts[index].second.m_data[0]->getQmlString(randomSeed);
    }
    else
    {
        iObj = m_pagePosts[index].second.m_data.begin();
        qmlStr += "Flow {\n";
        qmlStr += "    width: rctItem.width;\n";
        for (; iObj != m_pagePosts[index].second.m_data.end(); ++iObj)
        {            
            randomSeed = qrand();
            qmlStr += (*iObj)->getQmlString(randomSeed);
            qmlStr = qmlStr.trimmed();
        }
        qmlStr += "}\n";
    }

#ifdef RBR_DUMP_GENERATED_QML_IN_FILES
    QDir appRootDir(qApp->applicationDirPath());
    Q_ASSERT(appRootDir.isReadable());
    Q_ASSERT(appRootDir.cd(RBR_QML_OUTPUT_DIR));

    QString fullDirPath = appRootDir.path();
    if (!fullDirPath.endsWith("/")) fullDirPath += "/";

    Q_ASSERT(WriteTextFile(fullDirPath + "page_" + QString::number(m_pageNo) + "_post_" + QString::number(index) + ".qml", qmlStr));
#endif

    return qmlStr;
}

QString ForumReader::postLastEdit(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].second.m_lastEdit;
}

int ForumReader::postLikeCount(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].second.m_likeCounter;
}

int ForumReader::postAuthorPostCount(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].first.m_postCount;
}

QDate ForumReader::postAuthorRegistrationDate(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].first.m_registrationDate;
}

int ForumReader::postAuthorReputation(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].first.m_reputation;
}

QString ForumReader::postAuthorCity(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].first.m_city;
}

QString ForumReader::postAuthorSignature(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].second.m_userSignature;
}

QString ForumReader::postFooterQml() const
{
    const QString qmlStr =
            "import QtQuick 2.6;\n"
            "Column {\n"
            "   Text {\n"
            "       id: txtLastEdit\n"
            "       visible: model.lastEdit !== \"\"\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "\n"
            "       color: \"lightslategrey\"\n"
            "       font.italic: true\n"
            "       font.pointSize: 14\n"
            "\n"
            "       renderType: Text.NativeRendering\n"
            "\n"
            "       text: model.postLastEdit\n"
            "       textFormat: Text.RichText\n"
            "       onLinkActivated: Qt.openUrlExternally(link)\n"
            "\n"
            "       clip: false\n"
            "       elide: Text.ElideRight\n"
            "       wrapMode: Text.WordWrap\n"
            "   }\n"
            "\n"
            "   Rectangle {\n"
            "       visible: model.authorSignature !== \"\"\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "       height: dp(1)\n"
            "       border.width: dp(0)\n"
            "       color: \"lightslategrey\"\n"
            "   }\n"
            "\n"
            "   Text {\n"
            "       id: txtPostAuthorSignature\n"
            "       visible: model.authorSignature !== \"\"\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "\n"
            "       color: \"lightslategrey\"\n"
            "       font.italic: true\n"
            "       font.pointSize: 14\n"
            "\n"
            "       renderType: Text.NativeRendering\n"
            "\n"
            "       text: model.authorSignature\n"
            "       textFormat: Text.RichText\n"
            "       onLinkActivated: Qt.openUrlExternally(link)\n"
            "\n"
            "       clip: false\n"
            "       elide: Text.ElideRight\n"
            "       wrapMode: Text.WordWrap\n"
            "   }\n"
            "\n"
            "   Rectangle {\n"
            "       visible: model.postLikeCount > 0\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "       height: dp(1)\n"
            "       border.width: dp(0)\n"
            "       color: \"lightslategrey\"\n"
            "   }\n"
            "\n"
            "   Text {\n"
            "       id: txtPostLikeCounter\n"
            "       visible: model.postLikeCount > 0\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "       color: \"lightslategrey\"\n"
            "\n"
            "       font.bold: true\n"
            "       font.pointSize: 14\n"
            "       text: model.postLikeCount + \" like(s)\"\n"
            "   }\n"
            "}\n";
    return qmlStr;
}

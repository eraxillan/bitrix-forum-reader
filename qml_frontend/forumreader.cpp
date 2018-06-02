#include "forumreader.h"

#include "common/logger.h"
#include "common/filedownloader.h"
#include "website_backend/gumboparserimpl.h"

namespace {
template <typename T>
void dumpFutureObj(QFuture<T> future, QString name)
{
#ifdef BFR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("----------------------------------------------------");
    ConsoleLogger->info("Future name: {}", name);
    ConsoleLogger->info("Future is started: {}",  future.isStarted());
    ConsoleLogger->info("Future is running: {}",  future.isRunning());
    ConsoleLogger->info("Future is finished: {}", future.isFinished());
    ConsoleLogger->info("Future is paused: {}",   future.isPaused());
    ConsoleLogger->info("Future is canceled: {}", future.isCanceled());
    ConsoleLogger->info("Future has result: {}",  future.isResultReadyAt(0));
    ConsoleLogger->info("----------------------------------------------------");
#else
    Q_UNUSED(future);
    Q_UNUSED(name);
#endif
}
}

ForumReader::ForumReader() :
    m_forumPageCountWatcher(),
    m_forumPageParserWatcher(),
    m_pagePosts(),
    m_pageCount(0),
    m_pageNo(0),
    m_lastError(result_code::Type::Ok)
{
    connect(&m_downloader, &FileDownloader::downloadProgress, this, &ForumReader::onForumPageDownloadProgress);
    connect(&m_downloader, &FileDownloader::downloadFinished, this, &ForumReader::onForumPageDownloaded);
    connect(&m_downloader, &FileDownloader::downloadFailed,   this, &ForumReader::onForumPageDownloadFailed);

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

#ifdef BITRIX_FORUM_READER_SYNC_API
int ForumReader::parsePageCount(QString urlStr)
{
    // Cleanup
    m_pagePosts.clear();
    m_pageCount = 0;

    // 1) Download the first forum web page
    QByteArray htmlRawData;
    if (!FileDownloader::downloadUrl(urlStr, htmlRawData)) { Q_ASSERT(0); return 0; }

    // 2) Parse the page HTML to get the page number
    bfr::ForumPageParser fpp;
    result_code::Type resultFpp = fpp.getPageCount(htmlRawData, m_pageCount);
    Q_ASSERT(result_code::succeeded(resultFpp)); if (result_code::failed(resultFpp)) { m_pageCount = 0; return 0; }
    return m_pageCount;
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
    bfr::ForumPageParser fpp;
    result_code::Type resultFpp = fpp.getPageCount(htmlRawData, m_pageCount);
    Q_ASSERT(result_code::succeeded(resultFpp)); if (result_code::failed(resultFpp)) { m_pageCount = 0; return false; }

    // 3) Parse the page HTML to get the page user posts
    resultFpp = fpp.getPagePosts(htmlRawData, m_pagePosts);
    Q_ASSERT(result_code::succeeded(resultFpp)); if (result_code::failed(resultFpp)) { m_pagePosts.clear(); return false; }
    return true;
}
#endif

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
    bfr::ForumPageParser fpp;
    result_code::Type resultFpp = fpp.getPageCount(htmlRawData, result);
    if (result_code::failed(resultFpp))
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

namespace {
// NOTE: QtConcurrent require to return collection; return result code will be much more straightforward to reader
bfr::UserPosts parsePageAsync(QByteArray rawHtmlData, int& pageCount, result_code::Type& errorCode)
{
    bfr::UserPosts result;
    errorCode = result_code::Type::Ok;

    // 2) Parse the page HTML to get the page number
    bfr::ForumPageParser fpp;
    errorCode = fpp.getPageCount(rawHtmlData, pageCount);
    if (errorCode != result_code::Type::Ok)
    {
        Q_ASSERT(0);
        return result;
    }

    // 3) Parse the page HTML to get the page user posts
    errorCode = fpp.getPagePosts(rawHtmlData, result);
    if (errorCode != result_code::Type::Ok)
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

    m_downloader.startDownloadAsync(urlStr);

    // FIXME: a better way? server don't return Content-Length header;
    //        a HTML page size is unknown, and the only way to get it - download the entire page;
    //        however we know forum HTML page average size - it is around 400 Kb
    emit pageContentParseProgressRange(0, 400000);
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

void ForumReader::onForumPageDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
#ifdef BFR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("{}: {} bytes received, from {} bytes total", Q_FUNC_INFO, bytesReceived, bytesTotal);
#endif

    // NOTE: currently banki.ru server don't return Content-Length header
    Q_ASSERT(bytesTotal <= 0);

    // NOTE: HTML page size should not exceed 2^32 bytes, i hope :)
    emit pageContentParseProgress((int)bytesReceived);
}

void ForumReader::onForumPageDownloaded()
{
    m_pageData = m_downloader.downloadedData();
    Q_ASSERT(!m_pageData.isEmpty());

    // Wait for previous operation finish (if any)
    // NOTE: QtConcurrent::run() return future that cannot be canceled
    //m_forumPageParserWatcher.cancel();
    m_forumPageParserWatcher.waitForFinished();

    auto forumPageParseFuture = QtConcurrent::run(
                std::bind(parsePageAsync, m_pageData, std::ref(m_pageCount), std::ref(m_lastError)));
    m_forumPageParserWatcher.setFuture(forumPageParseFuture);
}

void ForumReader::onForumPageDownloadFailed(result_code::Type code)
{
    // FIXME: implement
    Q_UNUSED(code);
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

QString ForumReader::postAuthorQml(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].first.getQmlString(qrand());
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

    return m_pagePosts[index].second.getQmlString(qrand());
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

QString ForumReader::postAuthorSignature(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index].second.m_userSignature;
}

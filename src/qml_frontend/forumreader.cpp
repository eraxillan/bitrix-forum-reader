#include "forumreader.h"

#include "common/logger.h"
#include "common/filedownloader.h"
#include "website_backend/gumboparserimpl.h"
#include "parser_frontend/forumthreadpool.h"

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
    m_forumThreadParserWatcher(),
    m_pagePosts(),
    m_pageCount(0),
    m_pageNo(0),
    m_lastError(result_code::Type::Ok)
{
    connect(&ForumThreadPool::globalInstance(), &ForumThreadPool::downloadProgress, this, &ForumReader::onForumPageDownloadProgress);
    connect(&ForumThreadPool::globalInstance(), &ForumThreadPool::threadParseProgress, this, &ForumReader::threadContentParseProgress);

    connect(&m_forumPageCountWatcher, &ResultCodeFutureWatcher::finished, this, &ForumReader::onForumPageCountParsed);
    connect(&m_forumPageCountWatcher, &ResultCodeFutureWatcher::canceled, this, &ForumReader::onForumPageCountParsingCancelled);

    connect(&m_forumPageParserWatcher, &ResultCodeFutureWatcher::finished, this, &ForumReader::onForumPageParsed);
    connect(&m_forumPageParserWatcher, &ResultCodeFutureWatcher::canceled, this, &ForumReader::onForumPageParsingCancelled);

    connect(&m_forumThreadParserWatcher, &ResultCodeFutureWatcher::finished, this, &ForumReader::onForumThreadParsed);
    connect(&m_forumThreadParserWatcher, &ResultCodeFutureWatcher::canceled, this, &ForumReader::onForumThreadParsingCancelled);
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

void ForumReader::startPageCountAsync(ForumThreadUrl *url)
{
    dumpFutureObj(m_forumPageCountWatcher.future(), "m_forumPageCountWatcher");

    // Wait for previous operation finish (if any)
    // NOTE: QtConcurrent::run() return future that cannot be canceled
    //m_forumPageCountWatcher.cancel();
    m_forumPageCountWatcher.waitForFinished();

    auto countFuture = QtConcurrent::run(
                std::bind(&ForumThreadPool::getForumThreadPageCount, &ForumThreadPool::globalInstance(),
                          url->data(), std::ref(m_pageCount)));
    m_forumPageCountWatcher.setFuture(countFuture);
}

void ForumReader::startPageParseAsync(ForumThreadUrl *url, int pageNo)
{
    dumpFutureObj(m_forumPageParserWatcher.future(), "m_forumPageParserWatcher");

    // Cleanup
    m_pagePosts.clear();
    // FIXME: implement updatePageNumber() method and call it here
//    m_pageCount = 0;
    m_pageNo = pageNo;

    auto parseFuture = QtConcurrent::run(
                std::bind(&ForumThreadPool::getForumPagePosts, &ForumThreadPool::globalInstance(),
                          url->data(), pageNo, std::ref(m_pagePosts)));
    m_forumPageParserWatcher.setFuture(parseFuture);

    // FIXME: a better way? server don't return Content-Length header;
    //        a HTML page size is unknown, and the only way to get it - download the entire page;
    //        however we know forum HTML page average size - it is around 400 Kb
    emit pageContentParseProgressRange(0, 400000);
}

void ForumReader::startThreadParseAsync(ForumThreadUrl *url)
{
    // FIXME: ensure pageCount is already determined
    Q_ASSERT(m_pageCount > 0);

    dumpFutureObj(m_forumThreadParserWatcher.future(), "m_forumThreadParserWatcher");

    m_pagePosts.clear();

    auto parseFuture = QtConcurrent::run(
                std::bind(&ForumThreadPool::getForumThreadPosts, &ForumThreadPool::globalInstance(), url->data(), std::ref(m_threadPosts)));
    m_forumThreadParserWatcher.setFuture(parseFuture);

    emit threadContentParseProgressRange(0, m_pageCount);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

// FIXME: use m_lastError here

void ForumReader::onForumPageCountParsed()
{
    dumpFutureObj(m_forumPageCountWatcher.future(), "m_forumPageCountWatcher");

    Q_ASSERT(result_code::succeeded(m_forumPageCountWatcher.result()));
    Q_ASSERT(m_pageCount > 0);

    emit pageCountParsed(m_pageCount);
}

void ForumReader::onForumPageCountParsingCancelled()
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

void ForumReader::onForumPageParsed()
{
    dumpFutureObj(m_forumPageParserWatcher.future(), "m_forumPageParserWatcher");

    Q_ASSERT(result_code::succeeded(m_forumPageParserWatcher.result()));

    emit pageContentParsed(m_pageNo);
}

void ForumReader::onForumPageParsingCancelled()
{
    Q_ASSERT_X(0, Q_FUNC_INFO, "QFuture returned by QtConcurrent::run() cannot be canceled");
}

void ForumReader::onForumThreadParsed()
{
    dumpFutureObj(m_forumThreadParserWatcher.future(), "m_forumThreadParserWatcher");

    Q_ASSERT(result_code::succeeded(m_forumThreadParserWatcher.result()));

    // NOTE: ForumThreadUrl object will be destroyed in ForumReader dtor
    emit threadContentParsed(new ForumThreadUrl(this));
}

void ForumReader::onForumThreadParsingCancelled()
{
    Q_ASSERT_X(0, Q_FUNC_INFO, "QFuture returned by QtConcurrent::run() cannot be canceled");
}

// ----------------------------------------------------------------------------------------------------------------------------------------

// FIXME: add state checks, e.g. whether the page loaded

int ForumReader::pageCount() const
{
    return m_pageCount;
}

int ForumReader::pagePostCount() const
{
    return m_pagePosts.size();
}

QString ForumReader::postAuthorQml(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index]->m_author->getQmlString(qrand());
}

int ForumReader::postAvatarMaxWidth() const
{
    int maxWidth = 100;
    for(int i = 0; i < m_pagePosts.size(); ++i)
    {
        if (m_pagePosts[i]->m_author->m_userAvatar.isNull()) continue;
        int width = m_pagePosts[i]->m_author->m_userAvatar->m_width;
        if(width > maxWidth) maxWidth = width;
    }
    return maxWidth;
}

QDateTime ForumReader::postDateTime(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index]->m_date;
}

QString ForumReader::postText(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());
    if (m_pagePosts[index]->m_data.empty()) return QString();

    return m_pagePosts[index]->getQmlString(qrand());
}

QString ForumReader::postLastEdit(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index]->m_lastEdit;
}

int ForumReader::postLikeCount(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index]->m_likeCounter;
}

QString ForumReader::postAuthorSignature(int index) const
{
    Q_ASSERT(index >= 0 && index < m_pagePosts.size());

    return m_pagePosts[index]->m_userSignature;
}

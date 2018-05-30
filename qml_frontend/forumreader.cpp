#include "forumreader.h"

#include "common/logger.h"
#include "common/filedownloader.h"
#include "website_backend/gumboparserimpl.h"

namespace {
template <typename T>
void dumpFutureObj(QFuture<T> future, QString name)
{
#ifdef RBR_PRINT_DEBUG_OUTPUT
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

#ifdef HAVE_QX_ORM
result_code::Type ForumReader::openDatabase()
{
    QString appDataDirStr = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir appDataDir(appDataDirStr);
    // FIXME: generate database name from forum topic URL
    QString fullDbPath = appDataDir.filePath("forum_page_X_cache.sqlite");
    ConsoleLogger->info("Trying to open database file '{}'", fullDbPath);

    if (!appDataDir.exists())
    {
        if (!appDataDir.mkpath(appDataDirStr))
        {
            ConsoleLogger->error("Unable to create application local data writeable directory");
            return result_code::Type::IoError;
        }
    }
    else
        ConsoleLogger->info("Application local data directory already exists");

    if (QFile::exists(fullDbPath))
    {
        ConsoleLogger->info("Database file exists");

        // FIXME: remove this code after testing complete
        if (QFile::remove(fullDbPath))
            ConsoleLogger->info("Database file was removed");
        else
            ConsoleLogger->info("Unable to remove database file");
    }
    else
        ConsoleLogger->info("Database file is absent");

    // Parameters to connect to database
    qx::QxSqlDatabase::getSingleton()->setDriverName("QSQLITE");
    qx::QxSqlDatabase::getSingleton()->setDatabaseName(fullDbPath);
    qx::QxSqlDatabase::getSingleton()->setHostName("localhost");
    qx::QxSqlDatabase::getSingleton()->setUserName("root");
    qx::QxSqlDatabase::getSingleton()->setPassword("");

    // Only for debug purpose : assert if invalid offset detected fetching a relation
#ifdef QT_DEBUG
    qx::QxSqlDatabase::getSingleton()->setVerifyOffsetRelation(true);
#endif

    // Create all tables in database
    QSqlError daoError = qx::dao::create_table<BankiRuForum::User>();
    if (daoError.isValid())
    {
        ConsoleLogger->error("Unable to create table 'User': '{}'", daoError.text());
        return result_code::Type::DatabaseError;
    }

    daoError = qx::dao::create_table<BankiRuForum::Post>();
    if (daoError.isValid())
    {
        ConsoleLogger->error("Unable to create table 'Post': '{}'", daoError.text());
        return result_code::Type::DatabaseError;
    }

    // FIXME: implement
    /*
    QSqlError daoError = qx::dao::create_table<author>();
    daoError = qx::dao::create_table<comment>();
    daoError = qx::dao::create_table<category>();
    daoError = qx::dao::create_table<blog>();
    */
    //

    return result_code::Type::Ok;
}

result_code::Type ForumReader::closeDatabase()
{
    //
    return result_code::Type::Ok;
}

result_code::Type ForumReader::serializeToDatabase(/*const*/ BankiRuForum::PostCollection &posts)
{
    // QSqlError daoError = qx::dao::insert(posts);
    QSqlError daoError = qx::dao::insert_with_all_relation(posts);
    if (daoError.isValid())
        ConsoleLogger->error("Unable to insert new posts to database: '{}'", daoError.text());
    else
        ConsoleLogger->info("New posts were inserted in teh database");

    return (daoError.isValid() ? result_code::Type::DatabaseError : result_code::Type::Ok);

    // Create a scope to destroy temporary connexion to database
    /*{
        // Open a transaction to database
        QSqlDatabase db = qx::QxSqlDatabase::getDatabase();
        bool bCommit = db.transaction();

        // Insert 3 categories into database, use 'db' parameter for the transaction
        //daoError = qx::dao::insert(posts, &db);
        bCommit = (bCommit && ! daoError.isValid());

        qAssert(bCommit);

        // Terminate transaction => commit or rollback if there is error
        if (bCommit) { db.commit(); }
        else { db.rollback(); }
    }*/
    // End of scope : 'db' is destroyed
}

result_code::Type ForumReader::deserializeFromDatabase(BankiRuForum::PostCollection &posts)
{
    /*
    // Fetch drug with id '3' into a new variable
    drug_ptr d; d.reset(new drug());
    d->id = 3;
    QSqlError daoError = qx::dao::fetch_by_id(d);
    */

    /*
// Fetch blog into a new variable with all relation : 'author', 'comment' and 'category'
   blog_ptr blog_tmp; blog_tmp.reset(new blog());
   blog_tmp->m_id = blog_1->m_id;
   daoError = qx::dao::fetch_by_id_with_all_relation(blog_tmp);

   qAssert(blog_tmp->m_commentX.count() == 2);
   qAssert(blog_tmp->m_categoryX.count() == 2);
   qAssert(blog_tmp->m_text == "update blog_text_1");
   qAssert(blog_tmp->m_author && blog_tmp->m_author->m_id == "author_id_2");

   // Dump 'blog_tmp' result from database (XML or JSON serialization)
   qx::dump(blog_tmp);
    */

    QSqlError daoError;
    daoError = qx::dao::fetch_all_with_all_relation(posts);
    if (daoError.isValid())
        ConsoleLogger->error("Unable to fetch posts from database: '{}'", daoError.text());
    else
        ConsoleLogger->info("Posts were fetched from teh database");

    return (daoError.isValid() ? result_code::Type::DatabaseError : result_code::Type::Ok);
}
#endif

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
    BankiRuForum::ForumPageParser fpp;
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
    BankiRuForum::ForumPageParser fpp;
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
    BankiRuForum::ForumPageParser fpp;
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
BankiRuForum::PostCollection parsePageAsync(QByteArray rawHtmlData, int& pageCount, result_code::Type& errorCode)
{
    BankiRuForum::PostCollection result;
    errorCode = result_code::Type::Ok;

    // 2) Parse the page HTML to get the page number
    BankiRuForum::ForumPageParser fpp;
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
#ifdef RBR_PRINT_DEBUG_OUTPUT
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

    // FIXME: save to SQLite database
#ifdef HAVE_QX_ORM
    if (result_code::succeeded(openDatabase()))
    {
        if (result_code::succeeded(serializeToDatabase(m_pagePosts)))
        {
            BankiRuForum::PostCollection test;
            if (result_code::succeeded(deserializeFromDatabase(test)))
            {
                ConsoleLogger->info("Fetched {} records", test.size());
            }
        }
    }
#endif

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
    Q_ASSERT(index >= 0 && index < static_cast<int>(m_pagePosts.size()));

    return m_pagePosts[index]->m_author->getQmlString(qrand());
}

int ForumReader::postAvatarMaxWidth() const
{
    int maxWidth = 100;
    for(int i = 0; i < static_cast<int>(m_pagePosts.size()); ++i)
    {
        if (!m_pagePosts[i]->m_author->m_userAvatar)
            continue;

        int width = m_pagePosts[i]->m_author->m_userAvatar->m_width;
        if(width > maxWidth) maxWidth = width;
    }
    return maxWidth;
}

QDateTime ForumReader::postDateTime(int index) const
{
    Q_ASSERT(index >= 0 && index < static_cast<int>(m_pagePosts.size()));

    return m_pagePosts[index]->m_date;
}

QString ForumReader::postText(int index) const
{
    Q_ASSERT(index >= 0 && index < static_cast<int>(m_pagePosts.size()));
    if (m_pagePosts[index]->m_data.empty())
        return QString();

    return m_pagePosts[index]->getQmlString(qrand());
}

QString ForumReader::postLastEdit(int index) const
{
    Q_ASSERT(index >= 0 && index < static_cast<int>(m_pagePosts.size()));

    return m_pagePosts[index]->m_lastEdit;
}

int ForumReader::postLikeCount(int index) const
{
    Q_ASSERT(index >= 0 && index < static_cast<int>(m_pagePosts.size()));

    return m_pagePosts[index]->m_likeCounter;
}

QString ForumReader::postAuthorSignature(int index) const
{
    Q_ASSERT(index >= 0 && index < static_cast<int>(m_pagePosts.size()));

    return m_pagePosts[index]->m_userSignature;
}

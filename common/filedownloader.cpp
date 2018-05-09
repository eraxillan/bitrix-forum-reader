#include "filedownloader.h"
#include "common/logger.h"

#ifndef USE_QT_NAM
#include <curl/curl.h>
#endif

FileDownloader::FileDownloader(QObject *parent) : QObject(parent)
{
}

FileDownloader::~FileDownloader()
{
}

// Async API

#ifndef USE_QT_NAM
namespace {
int downloadFileProgressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    Q_UNUSED(ultotal);
    Q_UNUSED(ulnow);

    FileDownloader* thisObj = reinterpret_cast<FileDownloader*>(clientp);
    Q_ASSERT(thisObj);

    if (dltotal > 0 || dlnow > 0)
    {
#ifdef RBR_PRINT_DEBUG_OUTPUT
        ConsoleLogger->info("Download progress: {} of {} bytes", dlnow, dltotal);
#endif

        emit thisObj->downloadProgress(dlnow, dltotal);
    }
    return CURLE_OK;
}

size_t downloadFileWriteCallback(void *ptr, size_t size, size_t nmemb, QByteArray* data)
{
    QCoreApplication::processEvents();

    data->append((char*)ptr, (int)(size * nmemb));
    return size * nmemb;
}

QByteArray downloadFileAsync(QString urlStr, FileDownloader* thisObj, QByteArray& resultData, result_code::Type& resultCode)
{
#ifdef RBR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("libcurl version: {}", LIBCURL_VERSION);
#endif

    // FIXME: curl_global_init is not thread-safe, so call it manually
    //        to prevent automatic call from curl_easy_init;
    //        ensure what such call prevent from multi-threading problems!
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK)
    {
        ConsoleLogger->error("curl_global_init() failed");
        ConsoleLogger->error("Error code: {}", result);
        ConsoleLogger->error("Error string: {}", curl_easy_strerror(result));

        resultCode = result_code::Type::CurlError;
        emit thisObj->downloadFailed(result_code::Type::NetworkError);
        return QByteArray();
    }

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        ConsoleLogger->error("curl_easy_init() failed");

        resultCode = result_code::Type::CurlError;
        emit thisObj->downloadFailed(result_code::Type::NetworkError);
        return QByteArray();
    }

    curl_easy_setopt(curl, CURLOPT_URL, urlStr.toLocal8Bit().constData());

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, downloadFileProgressCallback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, thisObj);

//    curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.51.0");
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    QByteArray response_string;
    QByteArray header_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadFileWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

    result = curl_easy_perform(curl);
    if (result != CURLE_OK)
    {
        ConsoleLogger->error("curl_easy_perform() failed");
        ConsoleLogger->error("Error code: {}", result);
        ConsoleLogger->error("Error string: {}", curl_easy_strerror(result));

        resultCode = result_code::Type::NetworkError;
        emit thisObj->downloadFailed(result_code::Type::NetworkError);
        return QByteArray();
    }

#ifdef RBR_PRINT_DEBUG_OUTPUT
    char* url;
    long response_code;
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

    ConsoleLogger->info("libcurl response:");
    ConsoleLogger->info("Elapsed time: {}", elapsed);
    ConsoleLogger->info("Redirected URL: {}", url);
    ConsoleLogger->info("Response code: {}", response_code);
    ConsoleLogger->info("Response header: {}", header_string.toStdString());
    ConsoleLogger->info("Response string: {}", response_string.toStdString());
#endif

    curl_easy_cleanup(curl);
    curl = NULL;

    resultData = response_string;
    resultCode = result_code::Type::NetworkError;
    emit thisObj->downloadFinished();
    return response_string;
}
}
#endif

void FileDownloader::startDownloadAsync(QUrl url)
{
    m_lastError = result_code::Type::Invalid;
    m_downloadedData.clear();

#ifdef USE_QT_NAM
    m_reply = m_webCtrl.get(QNetworkRequest(url));
    Q_CHECK_PTR(m_reply); if (!m_reply) return;
    connect(m_reply, &QNetworkReply::downloadProgress, this, &FileDownloader::onDownloadProgress);
    connect(m_reply, &QNetworkReply::finished,         this, &FileDownloader::onDownloadFinished);
    // NOTE: don't work due to overload issues
    //connect(m_reply, &QNetworkReply::error,            this, &FileDownloader::onDownloadFailed);
    connect(m_reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onDownloadFailed(QNetworkReply::NetworkError)));
#else
    // FIXME: test what happens when we will call this method again BEFORE previous call finishes
//    m_downloadedDataWatcher.waitForFinished();

    m_downloadedDataWatcher.setFuture(QtConcurrent::run(std::bind(downloadFileAsync, url.toString(), this, std::ref(m_downloadedData), std::ref(m_lastError))));
#endif
}

QByteArray FileDownloader::downloadedData() const
{
    return m_downloadedData;
}

result_code::Type FileDownloader::lastError() const
{
    return m_lastError;
}

#ifdef USE_QT_NAM
void FileDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_lastError = result_code::Type::InProgress;

    emit downloadProgress(bytesReceived, bytesTotal);
}

void FileDownloader::onDownloadFinished()
{
    Q_CHECK_PTR(m_reply); if (!m_reply) return;

    m_lastError = result_code::Type::Ok;

    m_downloadedData = m_reply->readAll();
    m_reply->deleteLater();

    emit downloadFinished();
}

void FileDownloader::onDownloadFailed(QNetworkReply::NetworkError code)
{
    m_lastError = result_code::Type::NetworkError;

    // FIXME: map the NetworkError to the ResultCode
    emit downloadFailed(result_code::Type::NetworkError/*, code*/);
}
#endif

//-----------------------------------------------------------------------------
// Sync API

#ifdef USE_QT_NAM
bool FileDownloader::downloadUrl(QString urlStr, QByteArray &data)
{
    FileDownloader fd;
    fd.startDownloadAsync(urlStr);

    Q_ASSERT(fd.lastError() == result_code::Type::Invalid);
    while (fd.lastError() == result_code::Type::Invalid || fd.lastError() == result_code::Type::InProgress)
        qApp->processEvents();

    data = fd.downloadedData();
    return result_code::succeeded(fd.lastError());
}
#else
bool FileDownloader::downloadUrl(QString urlStr, QByteArray &data)
{
    data.clear();

#ifdef RBR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("libcurl version: {}", LIBCURL_VERSION);
#endif

    // FIXME: curl_global_init is not thread-safe, so call it manually
    //        to prevent automatic call from curl_easy_init;
    //        ensure what such call prevent from multi-threading problems!
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK)
    {
        ConsoleLogger->error( "curl_global_init() failed");
        ConsoleLogger->error("Error code: {}", result);
        ConsoleLogger->error("Error string: {}", curl_easy_strerror(result));
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        ConsoleLogger->error("curl_easy_init() failed");
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, urlStr.toLocal8Bit().constData());
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

//    curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.51.0");
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    QByteArray response_string;
    QByteArray header_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadFileWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

    result = curl_easy_perform(curl);
    if (result != CURLE_OK)
    {
        ConsoleLogger->error("curl_easy_perform() failed");
        ConsoleLogger->error("Error code: {}", result);
        ConsoleLogger->error("Error string: {}", curl_easy_strerror(result));
        return false;
    }

#ifdef RBR_PRINT_DEBUG_OUTPUT
    char* url;
    long response_code;
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

    ConsoleLogger->info("libcurl response:");
    ConsoleLogger->info("Elapsed time: {}", elapsed);
    ConsoleLogger->info("Redirected URL: {}", url);
    ConsoleLogger->info("Response code: {}", response_code);
    ConsoleLogger->info("Response header: {}", header_string.toStdString());
    ConsoleLogger->info("Response string: {}", response_string.toStdString());
#endif

    curl_easy_cleanup(curl);
    curl = NULL;

    data = response_string;
    return true;
}
#endif

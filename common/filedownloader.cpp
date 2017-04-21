#include "filedownloader.h"

#ifndef Q_OS_WIN
#include <curl/curl.h>
#endif

FileDownloader::FileDownloader(QObject *parent) : QObject(parent)
{
}

FileDownloader::~FileDownloader()
{
}

// Async API

#ifndef Q_OS_WIN
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
        qDebug("Download progress: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T "\r\n", dlnow, dltotal);
#endif

        emit thisObj->downloadProgress(dlnow, dltotal);
    }
    return CURLE_OK;
}

size_t downloadFileWriteCallback(void *ptr, size_t size, size_t nmemb, QByteArray* data)
{
    QCoreApplication::processEvents();

    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

QByteArray downloadFileAsync(QString urlStr, FileDownloader* thisObj, QByteArray& resultData, ResultCode& resultCode)
{
#ifdef RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "libcurl version: " << LIBCURL_VERSION;
#endif

    // FIXME: curl_global_init is not thread-safe, so call it manually
    //        to prevent automatic call from curl_easy_init;
    //        ensure what such call prevent from multi-threading problems!
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK)
    {
        qDebug() << "curl_global_init() failed";
        qDebug() << "Error code: " << result;
        qDebug() << "Error string: " << curl_easy_strerror(result);
        resultCode = ResultCode::E_CURL;
        emit thisObj->downloadFailed(ResultCode::E_NETWORK);
        return QByteArray();
    }

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "curl_easy_init() failed";
        resultCode = ResultCode::E_CURL;
        emit thisObj->downloadFailed(ResultCode::E_NETWORK);
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
        qDebug() << "curl_easy_perform() failed";
        qDebug() << "Error code:" << result;
        qDebug() << "Error string: " << curl_easy_strerror(result);
        resultCode = ResultCode::E_NETWORK;
        emit thisObj->downloadFailed(ResultCode::E_NETWORK);
        return QByteArray();
    }

#ifdef RBR_PRINT_DEBUG_OUTPUT
    char* url;
    long response_code;
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
    qDebug() << "libcurl response:";
    qDebug() << "Elapsed time: " << elapsed;
    qDebug() << "Redirected URL: " << url;
    qDebug() << "Response code: " << response_code;
    qDebug() << "Response header: " << header_string;
    qDebug() << "Response string: " << response_string;
#endif

    curl_easy_cleanup(curl);
    curl = NULL;

    resultData = response_string;
    resultCode = ResultCode::S_OK;
    emit thisObj->downloadFinished();
    return response_string;
}
}
#endif

void FileDownloader::startDownloadAsync(QUrl url)
{
    m_lastError = ResultCode::S_OK;
    m_downloadedData.clear();

#ifdef Q_OS_WIN
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

#ifdef Q_OS_WIN
void FileDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

void FileDownloader::onDownloadFinished()
{
    Q_CHECK_PTR(m_reply); if (!m_reply) return;

    m_downloadedData = m_reply->readAll();
    m_reply->deleteLater();

    emit downloadFinished();
}

void FileDownloader::onDownloadFailed(QNetworkReply::NetworkError code)
{
    // FIXME: map the NetworkError to the ResultCode
    emit downloadFailed(ResultCode::E_NETWORK);
}
#endif

//-----------------------------------------------------------------------------
// Sync API

#ifdef Q_OS_WIN
bool FileDownloader::downloadUrl(QString urlStr, QByteArray &data)
{
    throw std::runtime_error("not implemented");
    return false;
}
#else
bool FileDownloader::downloadUrl(QString urlStr, QByteArray &data)
{
    data.clear();

#ifdef RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "libcurl version: " << LIBCURL_VERSION;
#endif

    // FIXME: curl_global_init is not thread-safe, so call it manually
    //        to prevent automatic call from curl_easy_init;
    //        ensure what such call prevent from multi-threading problems!
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK)
    {
        qDebug() << "curl_global_init() failed";
        qDebug() << "Error code: " << result;
        qDebug() << "Error string: " << curl_easy_strerror(result);
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        qDebug() << "curl_easy_init() failed";
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
        qDebug() << "curl_easy_perform() failed";
        qDebug() << "Error code:" << result;
        qDebug() << "Error string: " << curl_easy_strerror(result);
        return false;
    }

#ifdef RBR_PRINT_DEBUG_OUTPUT
    char* url;
    long response_code;
    double elapsed;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
    qDebug() << "libcurl response:";
    qDebug() << "Elapsed time: " << elapsed;
    qDebug() << "Redirected URL: " << url;
    qDebug() << "Response code: " << response_code;
    qDebug() << "Response header: " << header_string;
    qDebug() << "Response string: " << response_string;
#endif

    curl_easy_cleanup(curl);
    curl = NULL;

    data = response_string;
    return true;
}
#endif

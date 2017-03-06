#include "filedownloader.h"

#if defined(Q_OS_MACOS)
#include <curl/curl.h>
#else
#error "Unsupported platform, porting needed"
#endif

// Async API

FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) : QObject(parent)
{
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(imageUrl);
    m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader()
{
}

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    m_DownloadedData = pReply->readAll();

    // Emit a signal
    pReply->deleteLater();
    emit downloaded();
}

QByteArray FileDownloader::downloadedData() const
{
    return m_DownloadedData;
}

//-----------------------------------------------------------------------------
// Sync API

namespace {
static size_t writeFunction(void *ptr, size_t size, size_t nmemb, QByteArray* data)
{
    QCoreApplication::processEvents();

    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}
}

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
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
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

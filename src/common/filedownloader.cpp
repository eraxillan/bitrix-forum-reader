#include "filedownloader.h"
#include "common/logger.h"

#ifndef USE_QT_NAM
#include <curl/curl.h>

const char *FileDownloader::CACertificatesPath { /*"/system/etc/security/cacerts_google"*/ "/system/etc/security/cacerts" };
#endif

namespace {
static const char *bfrUserAgent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36";
}

FileDownloader::FileDownloader(QObject *parent)
	: QObject(parent)
	, m_nm(new QNetworkAccessManager(this))
	, m_reply(nullptr)
	, m_progressCb(nullptr)
	, m_downloadedData()
	, m_lastError(result_code::Type::Invalid) { }

// Async API

#ifndef USE_QT_NAM
namespace {
int downloadFileProgressCallback(
	void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	Q_UNUSED(ultotal);
	Q_UNUSED(ulnow);

	FileDownloader *thisObj = reinterpret_cast<FileDownloader *>(clientp);
	Q_ASSERT(thisObj);

	if (dltotal > 0 || dlnow > 0) {
#ifdef BFR_PRINT_DEBUG_OUTPUT
		ConsoleLogger->info("Download progress: {} of {} bytes", dlnow, dltotal);
#endif

		emit thisObj->downloadProgress(dlnow, dltotal);
	}
	return CURLE_OK;
}

int downloadFileProgressCallback_2(
	void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	Q_UNUSED(ultotal);
	Q_UNUSED(ulnow);

	if (dltotal > 0 || dlnow > 0) {
#ifdef BFR_PRINT_DEBUG_OUTPUT
		ConsoleLogger->info("Download progress: {} of {} bytes", dlnow, dltotal);
#endif

		if (clientp) {
			FileDownloader::ProgressCallback *progressCb
				= reinterpret_cast<FileDownloader::ProgressCallback *>(clientp);
			if (*progressCb)
				(*progressCb)(dlnow, dltotal);
		}
	}
	return CURLE_OK;
}

size_t downloadFileWriteCallback(void *ptr, size_t size, size_t nmemb, QByteArray *data) {
	QCoreApplication::processEvents();

	data->append((char *)ptr, (int)(size * nmemb));
	return size * nmemb;
}

QByteArray downloadFileAsync(QString urlStr, FileDownloader* thisObj, QByteArray& resultData, result_code::Type& resultCode)
{
#ifdef BFR_PRINT_DEBUG_OUTPUT
	ConsoleLogger->info("libcurl version: {}", LIBCURL_VERSION);
#endif

	// FIXME: curl_global_init is not thread-safe, so call it manually
	//        to prevent automatic call from curl_easy_init;
	//        ensure what such call prevent from multi-threading problems!
	CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
	if (result != CURLE_OK) {
		ConsoleLogger->error("curl_global_init() failed");
		ConsoleLogger->error("Error code: {}", result);
		ConsoleLogger->error("Error string: {}", curl_easy_strerror(result));

		resultCode = result_code::Type::CurlError;
		emit thisObj->downloadFailed(result_code::Type::NetworkError);
		return QByteArray();
	}

	CURL *curl = curl_easy_init();
	if (!curl) {
		ConsoleLogger->error("curl_easy_init() failed");

		resultCode = result_code::Type::CurlError;
		emit thisObj->downloadFailed(result_code::Type::NetworkError);
		return QByteArray();
	}

	curl_easy_setopt(curl, CURLOPT_URL, urlStr.toLocal8Bit().constData());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, CURL_TRUE);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, CURL_TRUE);

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, downloadFileProgressCallback);
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, thisObj);

	//    curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
	curl_easy_setopt(curl, bfrUserAgent);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

	QByteArray response_string;
	QByteArray header_string;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadFileWriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

	// FIXME HACK: need corect cert stuff setup instead of ignoring them
#ifdef Q_OS_ANDROID
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, CURL_FALSE);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, CURL_FALSE);
#endif

	result = curl_easy_perform(curl);
	if (result != CURLE_OK) {
		ConsoleLogger->error("curl_easy_perform() failed");
		ConsoleLogger->error("Error code: {}", result);
		ConsoleLogger->error("Error string: {}", curl_easy_strerror(result));

		resultCode = result_code::Type::NetworkError;
		emit thisObj->downloadFailed(result_code::Type::NetworkError);
		return QByteArray();
	}

#ifdef BFR_PRINT_DEBUG_OUTPUT
	char *url;
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

#ifdef USE_QT_NAM
void FileDownloader::startDownloadSync(const QUrl &url) {
	m_lastError = result_code::Type::Invalid;
	m_downloadedData.clear();

	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", bfrUserAgent);

	// FIXME: move to separate method and call on program start
	//m_nm->connectToHostEncrypted(url.host());

	QEventLoop loop;
	connect(m_nm, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()), Qt::DirectConnection);

	m_reply = m_nm->get(request);
	if (!m_reply) {
		SPDLOG_ERROR("GET request failed for URL '{}'", url.toString());
		return;
	}

	connect(m_reply, &QNetworkReply::metaDataChanged, this, &FileDownloader::onMetadataChanged, Qt::DirectConnection);
	connect(m_reply, &QNetworkReply::downloadProgress, this, &FileDownloader::onDownloadProgress, Qt::DirectConnection);
	connect(m_reply, &QNetworkReply::finished, this, &FileDownloader::onDownloadFinished, Qt::DirectConnection);
	connect(m_reply, &QNetworkReply::errorOccurred, this, &FileDownloader::onDownloadFailed, Qt::DirectConnection);
	connect(m_reply, &QNetworkReply::sslErrors, this, &FileDownloader::onSslErrors, Qt::DirectConnection);

	loop.exec();
}
#endif

void FileDownloader::startDownloadAsync(const QUrl &url) {
	m_lastError = result_code::Type::Invalid;
	m_downloadedData.clear();

#ifdef USE_QT_NAM
	QNetworkRequest request;
	request.setUrl(url);
	request.setRawHeader("User-Agent", bfrUserAgent);

	// FIXME: move to separate method and call on program start
	//m_nm->connectToHostEncrypted(url.host());

	m_reply = m_nm->get(request);
	if (!m_reply) {
		SPDLOG_ERROR("GET request failed for URL '{}'", url.toString());
		return;
	}

	connect(m_reply, &QNetworkReply::metaDataChanged, this, &FileDownloader::onMetadataChanged, Qt::DirectConnection);
	connect(m_reply, &QNetworkReply::downloadProgress, this, &FileDownloader::onDownloadProgress, Qt::DirectConnection);
	connect(m_reply, &QNetworkReply::finished, this, &FileDownloader::onDownloadFinished, Qt::DirectConnection);
	connect(m_reply, &QNetworkReply::errorOccurred, this, &FileDownloader::onDownloadFailed, Qt::DirectConnection);
	connect(m_reply, &QNetworkReply::sslErrors, this, &FileDownloader::onSslErrors, Qt::DirectConnection);
#else
	// FIXME: test what happens when we will call this method again BEFORE previous call finishes
	//    m_downloadedDataWatcher.waitForFinished();

	m_downloadedDataWatcher.setFuture(QtConcurrent::run(
		std::bind(downloadFileAsync, url.toString(), this, std::ref(m_downloadedData), std::ref(m_lastError))));
#endif
}

QByteArray FileDownloader::downloadedData() const { return m_downloadedData; }

result_code::Type FileDownloader::lastError() const { return m_lastError; }

#ifdef USE_QT_NAM
void FileDownloader::onMetadataChanged() {
	SPDLOG_INFO("bytesAvail: {}", m_reply->bytesAvailable());
	SPDLOG_INFO("has content-length: {}", m_reply->hasRawHeader("Content-Length"));
}

void FileDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
	SPDLOG_INFO("received {} bytes from total {}", bytesReceived, bytesTotal);

	m_lastError = result_code::Type::InProgress;

	if (m_progressCb)
		m_progressCb(bytesReceived, bytesTotal);

	emit downloadProgress(bytesReceived, bytesTotal);
}

void FileDownloader::onDownloadFinished() {
	if (!m_reply) {
		SPDLOG_ERROR("logic error: m_reply is null");
		m_lastError = result_code::Type::NetworkError;
		return;
	}

	m_lastError = result_code::Type::Ok;

	m_downloadedData = m_reply->readAll();
	m_reply->deleteLater();

	emit downloadFinished();
}

void FileDownloader::onDownloadFailed(QNetworkReply::NetworkError code) {
	m_lastError = result_code::Type::NetworkError;

	// FIXME: map the NetworkError to the ResultCode
	Q_UNUSED(code);
	emit downloadFailed(result_code::Type::NetworkError /*, code*/);
}

void FileDownloader::onSslErrors(const QList<QSslError> &errors) {
	m_lastError = result_code::Type::NetworkError;

	Q_UNUSED(errors);
	for (const auto &error : errors)
		SPDLOG_ERROR("SSL error: '{}'", error.errorString());
}
#endif

//-----------------------------------------------------------------------------
// Sync API

#ifdef USE_QT_NAM
bool FileDownloader::downloadUrl(QString urlStr, QByteArray &data, ProgressCallback progressCb) {

	QScopedPointer<FileDownloader> fd(new FileDownloader);
	fd->m_progressCb = progressCb;
	fd->startDownloadSync(QUrl(urlStr));

	data = fd->downloadedData();
	return result_code::succeeded(fd->lastError());
}
#else
bool FileDownloader::downloadUrl(QString urlStr, QByteArray &data, ProgressCallback progressCb) {
	data.clear();

#ifdef BFR_PRINT_DEBUG_OUTPUT
	ConsoleLogger->info("libcurl version: {}", LIBCURL_VERSION);
#endif

	// FIXME: curl_global_init is not thread-safe, so call it manually
	//        to prevent automatic call from curl_easy_init;
	//        ensure what such call prevent from multi-threading problems!
	CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
	if (result != CURLE_OK) {
		ConsoleLogger->error("curl_global_init() failed");
		ConsoleLogger->error("Error code: {}", result);
		ConsoleLogger->error("Error string: {}", curl_easy_strerror(result));
		return false;
	}

	CURL *curl = curl_easy_init();
	if (!curl) {
		ConsoleLogger->error("curl_easy_init() failed");
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, urlStr.toLocal8Bit().constData());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, CURL_TRUE);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, CURL_TRUE);

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, downloadFileProgressCallback_2);
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progressCb);

	//    curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
	curl_easy_setopt(curl, CURLOPT_USERAGENT, bfrUserAgent);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

	QByteArray response_string;
	QByteArray header_string;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadFileWriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

	// FIXME HACK: need corect cert stuff setup instead of ignoring them
#ifdef Q_OS_ANDROID
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, CURL_FALSE);
	//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, CURL_FALSE);

	// #define BFR_RETURN_VALUE_IF(cond, returnValue, msg)

	BFR_RETURN_VALUE_IF(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1) != CURLE_OK, false, "setting CURLOPT_SSL_VERIFYPEER failed");
	BFR_RETURN_VALUE_IF(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2) != CURLE_OK, false, "setting CURLOPT_SSL_VERIFYHOST failed");
	BFR_RETURN_VALUE_IF(curl_easy_setopt(curl, CURLOPT_CAPATH, CACertificatesPath) != CURLE_OK, false, "setting CURLOPT_CAPATH failed");
#endif

	result = curl_easy_perform(curl);
	if (result != CURLE_OK) {
		ConsoleLogger->error("curl_easy_perform() failed");
		ConsoleLogger->error("Error code: {}", result);
		ConsoleLogger->error("Error string: {}", curl_easy_strerror(result));
		return false;
	}

#ifdef BFR_PRINT_DEBUG_OUTPUT
	char *url;
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

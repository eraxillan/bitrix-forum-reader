/*
 * This file is part of Bitrix Forum Reader.
 *
 * Copyright (C) 2016-2020 Alexander Kamyshnikov <axill777@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#ifndef __BFR_FILEDOWNLOADER_H__
#define __BFR_FILEDOWNLOADER_H__

// NOTE: code was copy-pasted from https://wiki.qt.io/Download_Data_from_URL
//       modifications:
//       - optional use of powerful libcurl instead of Qt Network framework
//       - progress tracking support
//       - async API support

// FIXME: implement proxy auth

#include <QtCore/QByteArray>
#include <QtCore/QUrl>
#include <QtConcurrent/QtConcurrent>

#ifdef USE_QT_NAM
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#else
#define CURL_TRUE 1
#define CURL_FALSE 0
#endif

#include "common/resultcode.h"

class FileDownloader : public QObject {
	Q_OBJECT

public:
	explicit FileDownloader(QObject *parent = nullptr);
	virtual ~FileDownloader() = default;

	// Async API
	void startDownloadAsync(const QUrl &url);
	QByteArray downloadedData() const;
	result_code::Type lastError() const;

	// Sync API
	//using ProgressCallback = void (*)(qint64 /*bytesReceived*/, qint64 /*bytesTotal*/);
	using ProgressCallback = std::function<void(qint64, qint64)>;
	static bool downloadUrl(QString urlStr, QByteArray &data, ProgressCallback progressCb = nullptr);

signals:
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void downloadFinished();
	void downloadFailed(result_code::Type code);

#ifdef USE_QT_NAM
private slots:
	void onMetadataChanged();
	void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void onDownloadFinished();
	void onDownloadFailed(QNetworkReply::NetworkError code);
	void onSslErrors(const QList<QSslError> &errors);
#endif

private:
#ifdef USE_QT_NAM
	QPointer<QNetworkAccessManager> m_nm;
	QPointer<QNetworkReply> m_reply;
	ProgressCallback m_progressCb;
#endif

	QByteArray m_downloadedData;
	result_code::Type m_lastError;

#ifndef USE_QT_NAM
	// Async API
	QFutureWatcher<QByteArray> m_downloadedDataWatcher;
#endif
private:
#ifndef USE_QT_NAM
	static const char *CACertificatesPath;
#else
	void startDownloadSync(const QUrl &url);
#endif
};

#endif // __BFR_FILEDOWNLOADER_H__

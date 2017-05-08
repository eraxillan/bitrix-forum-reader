#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

// NOTE: code was copy-pasted from https://wiki.qt.io/Download_Data_from_URL
//       modifications:
//       - optional use of powerful libcurl instead of Qt Network framework
//       - progress tracking support
//       - async API support

// FIXME: implement proxy auth

#include <QtCore/QCoreApplication>
#include <QtCore/QByteArray>
#include <QtCore/QUrl>

#include <QtConcurrent/QtConcurrent>

//#define USE_QT_NAM

#ifdef USE_QT_NAM
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#endif

#include "common/resultcode.h"

class FileDownloader : public QObject
{
    Q_OBJECT

public:
    explicit FileDownloader(QObject *parent = nullptr);
    virtual ~FileDownloader();

    // Async API
    void startDownloadAsync(QUrl url);
    QByteArray downloadedData() const;

    // Sync API
    static bool downloadUrl(QString urlStr, QByteArray& data);

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadFailed(ResultCode code);

#ifdef USE_QT_NAM
private slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    void onDownloadFailed(QNetworkReply::NetworkError code);
#endif

private:
#ifdef USE_QT_NAM
    QNetworkAccessManager m_webCtrl;
    QPointer<QNetworkReply> m_reply;
#endif

    QByteArray m_downloadedData;
    ResultCode m_lastError;

    // Async API
    QFutureWatcher<QByteArray> m_downloadedDataWatcher;
};

#endif // FILEDOWNLOADER_H

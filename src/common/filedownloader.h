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
    result_code::Type lastError() const;

    // Sync API
    //using ProgressCallback = void (*)(qint64 /*bytesReceived*/, qint64 /*bytesTotal*/);
    using ProgressCallback = std::function<void(qint64, qint64)>;
    static bool downloadUrl(QString urlStr, QByteArray& data, ProgressCallback progressCb = nullptr);

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadFailed(result_code::Type code);

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
    result_code::Type m_lastError;

    // Async API
    QFutureWatcher<QByteArray> m_downloadedDataWatcher;
};

#endif // __BFR_FILEDOWNLOADER_H__

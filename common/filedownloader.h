#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

// NOTE: code was copy-pasted from https://wiki.qt.io/Download_Data_from_URL

// FIXME: implement proxy auth

#include <QCoreApplication>
#include <QByteArray>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
    Q_OBJECT

public:
    // Async API
    explicit FileDownloader(QUrl imageUrl, QObject *parent = 0);
    virtual ~FileDownloader();
    QByteArray downloadedData() const;

    // Sync API
    static bool downloadUrl(QString urlStr, QByteArray& data);

signals:
    void downloaded();

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    QNetworkAccessManager m_WebCtrl;
    QByteArray m_DownloadedData;
};

#endif // FILEDOWNLOADER_H

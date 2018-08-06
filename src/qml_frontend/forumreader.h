#ifndef __BFR_FORUMREADER_H__
#define __BFR_FORUMREADER_H__

#include "task.h"

#include "common/resultcode.h"
#include "common/filedownloader.h"
#include "common/forumthreadurl.h"
#include "website_backend/websiteinterface.h"
#include "concurrentqueue/blockingconcurrentqueue.h"

class ForumReader : public QObject
{
    Q_OBJECT

    moodycamel::BlockingConcurrentQueue<BfrTask> m_tasks;
    std::atomic<int> m_pendingTaskCount;
    std::thread m_producerThread;
    std::atomic<bool> m_timeToExit;

public:
    ForumReader();
    ~ForumReader();

    // Helper functions
    Q_INVOKABLE QString   applicationDirPath() const;
    Q_INVOKABLE QUrl      convertToUrl(QString urlStr) const;

    // Forum HTML page parser async API (use Qt signal-slots system)
    Q_INVOKABLE void      startPageCountAsync(ForumThreadUrl *url);
    Q_INVOKABLE void      startPageParseAsync(ForumThreadUrl *url, int pageNo);
    Q_INVOKABLE void      startThreadUsersParseAsync(ForumThreadUrl *url);

signals:
    // Forum parser signals
    void pageCountParsed(int pageCount);
    void pageContentParsed(int pageCount, int pageNo, QVariantList posts);
    void threadUsersParsed(ForumThreadUrl *url, QVariantList users);

    void pageContentParseProgressRange(int minimum, int maximum);
    void pageContentParseProgress(int value);

    void threadContentParseProgressRange(int minimum, int maximum);
    void threadContentParseProgress(int value);

private slots:
    // Forum page downloader slots
    void onForumPageDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // __BFR_FORUMREADER_H__

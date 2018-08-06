#ifndef __BFR_FORUMTHREADURL_H__
#define __BFR_FORUMTHREADURL_H__

#include <QtCore/QtCore>

struct ForumThreadUrlData
{
    int m_sectionId;
    int m_threadId;

    ForumThreadUrlData();
    ForumThreadUrlData(int sectionId, int threadId);
    ForumThreadUrlData(const ForumThreadUrlData &other);
    ~ForumThreadUrlData();
};

class ForumThreadUrl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int sectionId READ sectionId WRITE setSectionId NOTIFY sectionIdChanged)
    Q_PROPERTY(int threadId READ threadId WRITE setThreadId NOTIFY threadIdChanged)

    ForumThreadUrlData m_data;

public:
    explicit ForumThreadUrl(QObject *parent = nullptr);
    explicit ForumThreadUrl(QObject *parent, ForumThreadUrlData urlData);
    ForumThreadUrl(int sectionId, int threadId);
    ~ForumThreadUrl();

    ForumThreadUrlData data() const;
    int sectionId() const;
    int threadId() const;
    void setSectionId(int sectionId);
    void setThreadId(int threadId);

    // "http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1=1#forum-message-list"
    Q_INVOKABLE QString firstPageUrl() const;

    // "http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1=215#forum-message-list"
    Q_INVOKABLE QString pageUrl(int pageNumber) const;

signals:
    void sectionIdChanged();
    void threadIdChanged();
};

Q_DECLARE_METATYPE(ForumThreadUrlData)

inline bool operator < (const ForumThreadUrlData &url1, const ForumThreadUrlData &url2)
{
    if (url1.m_sectionId != url2.m_sectionId)
        return url1.m_sectionId < url2.m_sectionId;
    return url1.m_threadId < url2.m_threadId;
}

#endif // __BFR_FORUMTHREADURL_H__

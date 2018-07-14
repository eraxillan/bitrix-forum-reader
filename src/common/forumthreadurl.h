#ifndef __BFR_FORUMTHREADURL_H__
#define __BFR_FORUMTHREADURL_H__

#include <QtCore/QtCore>


class ForumThreadUrl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int sectionId READ sectionId WRITE setSectionId NOTIFY sectionIdChanged)
    Q_PROPERTY(int threadId READ threadId WRITE setThreadId NOTIFY threadIdChanged)

    int m_sectionId;
    int m_threadId;

public:
    explicit ForumThreadUrl(QObject *parent = nullptr);

    ForumThreadUrl(int sectionId, int threadId);

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

#endif // __BFR_FORUMTHREADURL_H__

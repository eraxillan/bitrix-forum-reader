#include "forumthreadurl.h"

ForumThreadUrl::ForumThreadUrl(QObject *parent) : QObject(parent)
{
    setSectionId(-1);
    setThreadId(-1);
}

ForumThreadUrl::ForumThreadUrl(int sectionId, int threadId)
{
    setSectionId(sectionId);
    setThreadId(threadId);
}

int ForumThreadUrl::sectionId() const
{
    return m_sectionId;
}

int ForumThreadUrl::threadId() const
{
    return m_threadId;
}

void ForumThreadUrl::setSectionId(int sectionId)
{
    m_sectionId = sectionId;
}

void ForumThreadUrl::setThreadId(int threadId)
{
    m_threadId = threadId;
}

QString ForumThreadUrl::firstPageUrl() const
{
    return QString("http://www.banki.ru/forum/?PAGE_NAME=read&FID=" + QString::number(m_sectionId)
                   + "&TID=" + QString::number(m_threadId) + "&PAGEN_1=1#forum-message-list");
}

QString ForumThreadUrl::pageUrl(int pageNumber) const
{
    return QString("http://www.banki.ru/forum/?PAGE_NAME=read&FID=" + QString::number(m_sectionId)
                   + "&TID=" + QString::number(m_threadId)
                   + "&PAGEN_1=" + QString::number(pageNumber) + "#forum-message-list");
}

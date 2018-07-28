#include "forumthreadurl.h"

ForumThreadUrlData::ForumThreadUrlData() : m_sectionId(-1), m_threadId(-1)
{
}

ForumThreadUrlData::ForumThreadUrlData(int sectionId, int threadId) : m_sectionId(sectionId), m_threadId(threadId)
{
}

ForumThreadUrlData::ForumThreadUrlData(const ForumThreadUrlData &other)
{
    m_sectionId = other.m_sectionId;
    m_threadId = other.m_threadId;
}

ForumThreadUrlData::~ForumThreadUrlData()
{
}

// --------------------------------------------------------------------------------------------------------------------------------------------------

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

ForumThreadUrl::~ForumThreadUrl()
{
}

ForumThreadUrlData ForumThreadUrl::data() const
{
    return m_data;
}

int ForumThreadUrl::sectionId() const
{
    return m_data.m_sectionId;
}

int ForumThreadUrl::threadId() const
{
    return m_data.m_threadId;
}

void ForumThreadUrl::setSectionId(int sectionId)
{
    m_data.m_sectionId = sectionId;
}

void ForumThreadUrl::setThreadId(int threadId)
{
    m_data.m_threadId = threadId;
}

QString ForumThreadUrl::firstPageUrl() const
{
    return QString("http://www.banki.ru/forum/?PAGE_NAME=read&FID=" + QString::number(sectionId())
                   + "&TID=" + QString::number(threadId()) + "&PAGEN_1=1#forum-message-list");
}

QString ForumThreadUrl::pageUrl(int pageNumber) const
{
    return QString("http://www.banki.ru/forum/?PAGE_NAME=read&FID=" + QString::number(sectionId())
                   + "&TID=" + QString::number(threadId())
                   + "&PAGEN_1=" + QString::number(pageNumber) + "#forum-message-list");
}

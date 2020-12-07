#include "forumthreadurl.h"

// --------------------------------------------------------------------------------------------------------------------------------------------------

ForumThreadUrl::ForumThreadUrl(QObject *parent)
	: QObject(parent) {
	setSectionId(-1);
	setThreadId(-1);
}

ForumThreadUrl::ForumThreadUrl(QObject *parent, ForumThreadUrlData urlData)
	: QObject(parent) {
	setSectionId(urlData.m_sectionId);
	setThreadId(urlData.m_threadId);
}

ForumThreadUrl::ForumThreadUrl(int sectionId, int threadId) {
	setSectionId(sectionId);
	setThreadId(threadId);
}

ForumThreadUrl::~ForumThreadUrl()
{
}

ForumThreadUrlData ForumThreadUrl::data() const { return m_data; }

int ForumThreadUrl::sectionId() const { return m_data.m_sectionId; }

int ForumThreadUrl::threadId() const { return m_data.m_threadId; }

void ForumThreadUrl::setSectionId(int sectionId) { m_data.m_sectionId = sectionId; }

void ForumThreadUrl::setThreadId(int threadId) { m_data.m_threadId = threadId; }

QString ForumThreadUrl::firstPageUrl() const {
	// https://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=358149
	return QString("https://www.banki.ru/forum/?PAGE_NAME=read&FID=%1&TID=%2")
		.arg(sectionId())
		.arg(threadId());
}

QString ForumThreadUrl::pageUrl(int pageNumber) const {
	// https://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=358149&PAGEN_1=14#forum-message-list
	return QString("https://www.banki.ru/forum/?PAGE_NAME=read&FID=%1&TID=%2&PAGEN_1=%3#forum-message-list")
		.arg(sectionId())
		.arg(threadId())
		.arg(pageNumber);
}

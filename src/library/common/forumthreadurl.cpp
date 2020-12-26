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
#include "forumthreadurl.h"

// --------------------------------------------------------------------------------------------------------------------------------------------------

ForumThreadUrl::ForumThreadUrl(QObject *parent)
	: QObject(parent) {
	setSectionId(-1);
	setThreadId(-1);
}

ForumThreadUrl::ForumThreadUrl(QObject *parent, const ForumThreadUrlData &urlData)
	: QObject(parent) {
	setSectionId(urlData.m_sectionId);
	setThreadId(urlData.m_threadId);
}

ForumThreadUrl::ForumThreadUrl(const int sectionId, const int threadId) {
	setSectionId(sectionId);
	setThreadId(threadId);
}

const ForumThreadUrlData &ForumThreadUrl::data() const { return m_data; }

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

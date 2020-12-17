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
#ifndef __BFR_FORUMREADER_H__
#define __BFR_FORUMREADER_H__

#include "task.h"

#include <common/resultcode.h>
#include <common/filedownloader.h>
#include <common/forumthreadurl.h>
#include <website_backend/websiteinterface.h>

#include <concurrentqueue/blockingconcurrentqueue.h>

class ForumReader : public QObject {
	Q_OBJECT

	moodycamel::BlockingConcurrentQueue<BfrTask> m_tasks;
	std::atomic<int> m_pendingTaskCount;
	std::thread m_producerThread;
	std::atomic<bool> m_timeToExit;

public:
	ForumReader();
	~ForumReader();

	// Helper functions
	Q_INVOKABLE QString applicationDirPath() const;
	Q_INVOKABLE QUrl convertToUrl(QString urlStr) const;

	// Forum HTML page parser async API (use Qt signal-slots system)
	Q_INVOKABLE void startPageCountAsync(ForumThreadUrl *url);
	Q_INVOKABLE void startPageParseAsync(ForumThreadUrl *url, int pageNo);
	Q_INVOKABLE void startThreadUsersParseAsync(ForumThreadUrl *url);

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

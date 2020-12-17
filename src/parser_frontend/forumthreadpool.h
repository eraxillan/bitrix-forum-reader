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
#ifndef __BFR_FORUMTHREADPOOL_H__
#define __BFR_FORUMTHREADPOOL_H__

#include <common/resultcode.h>
#include <common/logger.h>
#include <common/filedownloader.h>
#include <common/forumthreadurl.h>
#include <website_backend/websiteinterface_fwd.h>

#include <functional>

class ForumThreadPool : public QObject {
	Q_OBJECT

protected:
	using PagePostMap = QMap<int /*pageNo*/, bfr::PostList /*pagePosts*/>;
	using ThreadPagePostMap = QMap<ForumThreadUrlData /*forumThreadUrl*/, PagePostMap /*forumThreadPagesPosts*/>;
	using ThreadPageCountMap = QMap<ForumThreadUrlData /*forumThreadUrl*/, int /*pageCount*/>;

	ThreadPageCountMap m_threadPageCountCollection;
	ThreadPagePostMap m_threadPagePostCollection;

	explicit ForumThreadPool(QObject *parent = nullptr);
	~ForumThreadPool();

	size_t pageCountCacheSize() const;
	size_t pagePostsCacheSize() const;

	void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

public:
	static ForumThreadPool &globalInstance();

	// Delete copy and move constructors and assign operators
	ForumThreadPool(ForumThreadPool const &) = delete; // Copy construct
	ForumThreadPool(ForumThreadPool &&) = delete; // Move construct
	ForumThreadPool &operator=(ForumThreadPool const &) = delete; // Copy assign
	ForumThreadPool &operator=(ForumThreadPool &&) = delete; // Move assign

public:
	// FIXME: implement
	//enum class Policy { Invalid = -1, CachedOnly, PreferCached, PreferNew, NewOnly, Count };
	//void setPolicy(Policy policy);
	// FIXME: implement
	//void setMaximumMemoryUsage(quint64 maxCacheMem);

	/*SYNC*/ result_code::Type getForumThreadPageCount(ForumThreadUrlData urlData, int &pageCount);

	/*SYNC*/ result_code::Type getForumPagePosts(ForumThreadUrlData urlData, int pageNo, bfr::PostList &posts);

	/*SYNC*/ result_code::Type getForumThreadPosts(ForumThreadUrlData urlData, bfr::PostList &posts);

signals:
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	//    void downloadFinished();
	//    void downloadFailed(result_code::Type code);

	void threadParseProgress(int pageNo, int pageCount);
};

#endif // __BFR_FORUMTHREADPOOL_H__

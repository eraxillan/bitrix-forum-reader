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
#include "forumreader.h"

#include <QtCore/QFuture>

#include <common/logger.h>
#include <common/filedownloader.h>
#include <website_backend/gumboparserimpl.h>
#include <website_backend/websiteinterface_qt.h>
#include <parser_frontend/forumthreadpool.h>

namespace {
template <typename T>
void dumpFutureObj(QFuture<T> future, QString name)
{
#ifdef BFR_PRINT_DEBUG_OUTPUT
	SystemLogger->info("----------------------------------------------------");
	SystemLogger->info("Future name: {}", name);
	SystemLogger->info("Future is started: {}", future.isStarted());
	SystemLogger->info("Future is running: {}", future.isRunning());
	SystemLogger->info("Future is finished: {}", future.isFinished());
	SystemLogger->info("Future is paused: {}", future.isPaused());
	SystemLogger->info("Future is canceled: {}", future.isCanceled());
	SystemLogger->info("Future has result: {}", future.isResultReadyAt(0));
	SystemLogger->info("----------------------------------------------------");
#else
	Q_UNUSED(future);
	Q_UNUSED(name);
#endif
}
}

ForumReader::ForumReader()
	: m_pendingTaskCount(0)
	, m_timeToExit(false) {
	connect(&ForumThreadPool::globalInstance(), &ForumThreadPool::downloadProgress, this,
		&ForumReader::onForumPageDownloadProgress);
	connect(&ForumThreadPool::globalInstance(), &ForumThreadPool::threadParseProgress, this,
		&ForumReader::threadContentParseProgress);

	m_producerThread = std::thread([&, this, &m_tasks = m_tasks]() {
		SystemLogger->info("Producer thread started");

		ForumThreadPool &pool = ForumThreadPool::globalInstance();

		result_code::Type result = result_code::Type::Invalid;
		BfrTask task;
		while (!m_timeToExit.load(std::memory_order_acquire)) {
			// NOTE: without timeout queue will wait for new item forever, and thread will never finish!
			if (m_tasks.wait_dequeue_timed(task, std::chrono::milliseconds(100))) {
				// Process task
				SystemLogger->info("Processing task");

				switch (task.action()) {
					case BfrTask::Action::ParseForumThreadPageCount: {
						int pageCount = -1;
						result = pool.getForumThreadPageCount(task.url(), pageCount);
						if (result_code::succeeded(result)) {
							emit this->pageCountParsed(pageCount);
						}
						break;
					}
					case BfrTask::Action::ParseForumThreadPagePosts: {
						int pageCount = -1;
						result = pool.getForumThreadPageCount(task.url(), pageCount);
						if (result_code::succeeded(result)) {
							bfr::PostList posts;
							result = pool.getForumPagePosts(task.url(), task.pageNo(), posts);
							if (result_code::succeeded(result)) {
								QVariantList postsVrnt;
								for (const auto &post : posts) {
									QVariant var;
									PostQtWrapper pw(post);
									var.setValue(pw);

									postsVrnt.push_back(var);
								}

								emit this->pageContentParsed(pageCount, task.pageNo(), postsVrnt);
							}
						}
						break;
					}
					case BfrTask::Action::ExtractForumThreadUsers: {
						// TODO: add  `emit threadContentParseProgressRange(0, m_pageCount);`

						bfr::PostList threadPosts;
						result = pool.getForumThreadPosts(task.url(), threadPosts);
						if (result_code::succeeded(result)) {
							// Extract users
							QMap<QString, bfr::UserPtr> threadUsersMap;
							for (const auto &post : threadPosts) {
								threadUsersMap.insert(post->m_author->m_userName, post->m_author);
							}

							// Wrap users collection to Qt-compatible container
							QVariantList usersVrnt;
							bfr::UserList threadUsersList = threadUsersMap.values();
							for (const auto &user : threadUsersList) {
								QVariant var;
								UserQtWrapper uw(user);
								var.setValue(uw);

								usersVrnt.push_back(var);
							}

							// NOTE: ForumThreadUrl object will be destroyed in ForumReader dtor
							emit this->threadUsersParsed(new ForumThreadUrl(this, task.url()), usersVrnt);
						}
						break;
					}
					default: {
						SystemLogger->error("Invalid task action got: {}", static_cast<int>(task.action()));
						break;
					}
				}

				m_pendingTaskCount.fetch_add(-1, std::memory_order_release);
			}
		}

		SystemLogger->info("Producer thread finished");
	});
}

ForumReader::~ForumReader() {
	SystemLogger->info("ForumReader dtor started");

	m_timeToExit = true;
	m_producerThread.join();

	SystemLogger->info("ForumReader dtor finished");
}

QString ForumReader::applicationDirPath() const {
	QString result = qApp->applicationDirPath();
	if (!result.endsWith("/"))
		result += "/";
	return result;
}

QUrl ForumReader::convertToUrl(QString urlStr) const { return QUrl(urlStr); }

void ForumReader::startPageCountAsync(ForumThreadUrl *url) {
	SystemLogger->info("Enqueue forum thread page count parse task");
	m_pendingTaskCount.fetch_add(1, std::memory_order_release);
	m_tasks.enqueue(BfrTask(BfrTask::Action::ParseForumThreadPageCount, url->data()));
}

void ForumReader::startPageParseAsync(ForumThreadUrl *url, int pageNo) {
	SystemLogger->info("Enqueue forum thread page posts parse task");
	m_pendingTaskCount.fetch_add(1, std::memory_order_release);
	m_tasks.enqueue(BfrTask(BfrTask::Action::ParseForumThreadPagePosts, url->data(), pageNo));

	// FIXME: a better way? server don't return Content-Length header;
	//        a HTML page size is unknown, and the only way to get it - download the entire page;
	//        however we know forum HTML page average size - it is around 400 Kb
	emit pageContentParseProgressRange(0, 400000);
}

void ForumReader::startThreadUsersParseAsync(ForumThreadUrl *url) {
	SystemLogger->info("Enqueue forum thread users parse task");
	m_pendingTaskCount.fetch_add(1, std::memory_order_release);
	m_tasks.enqueue(BfrTask(BfrTask::Action::ExtractForumThreadUsers, url->data()));

	//    emit threadContentParseProgressRange(0, m_pageCount);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

void ForumReader::onForumPageDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
#ifdef BFR_PRINT_DEBUG_OUTPUT
	SystemLogger->info("{}: {} bytes received, from {} bytes total", Q_FUNC_INFO, bytesReceived, bytesTotal);
#endif

	// NOTE: this field is unreliable source of data length, often it is just -1
	//Q_ASSERT(bytesTotal <= 0);
	Q_UNUSED(bytesTotal);

	// NOTE: HTML page size should not exceed 2^32 bytes, i hope :)
	emit pageContentParseProgress((int)bytesReceived);
}

// ----------------------------------------------------------------------------------------------------------------------------------------

#if 0
int ForumReader::postAvatarMaxWidth() const
{
    int maxWidth = 100;
    for(int i = 0; i < m_pagePosts.size(); ++i)
    {
        if (m_pagePosts[i]->m_author->m_userAvatar.isNull()) continue;
        int width = m_pagePosts[i]->m_author->m_userAvatar->m_width;
        if(width > maxWidth) maxWidth = width;
    }
    return maxWidth;
}
#endif

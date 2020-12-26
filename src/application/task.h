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
#ifndef __BFR_TASK_H__
#define __BFR_TASK_H__

#include <common/forumthreadurl.h>

class BfrTask
{
public:
	enum class Action {
		Invalid = -1,
		ParseForumThreadPageCount, // Input: URL           | Output: URL, int
		ParseForumThreadPagePosts, // Input: URL, pageNo   | Output: URL, PostList
		ExtractForumThreadUsers, // Input: URL           | Output: URL, UserList
		// AnalyzeForumThreadUsers,    // Input: URL, UserList | Output: URL, UserList
		Count
	};

private:
	static const int INVALID_PAGENO = -1;

	// Input data
	Action m_action;
	ForumThreadUrlData m_url;
	int m_pageNo;

	// Output data
	// int m_pageCount; // Action: ParseForumThreadPageCount
	// bfr::PostList m_posts; // Action: ParseForumThreadPagePosts
	// bfr::UserList m_users; // Action: ExtractForumThreadUsers, AnalyzeForumThreadUsers

public:
	BfrTask();
	BfrTask(const Action action, const ForumThreadUrlData &url);
	BfrTask(const Action action, const ForumThreadUrlData &url, const int pageNo);
	~BfrTask() = default;

	bool isValid() const;
	Action action() const;
	const ForumThreadUrlData &url() const;
	int pageNo() const;
};

#endif // __BFR_TASK_H__

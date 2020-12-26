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
#include "task.h"

BfrTask::BfrTask()
	: m_action(Action::Invalid)
	, m_url()
	, m_pageNo(INVALID_PAGENO) { }

BfrTask::BfrTask(const BfrTask::Action action, const ForumThreadUrlData &url)
	: m_action(action)
	, m_url(url)
	, m_pageNo(INVALID_PAGENO) { }

BfrTask::BfrTask(const BfrTask::Action action, const ForumThreadUrlData &url, const int pageNo)
	: m_action(action)
	, m_url(url)
	, m_pageNo(pageNo) { }

bool BfrTask::isValid() const {
	// TODO: implement and use
	return (m_action != Action::Invalid);
}

BfrTask::Action BfrTask::action() const { return m_action; }

const ForumThreadUrlData &BfrTask::url() const { return m_url; }

int BfrTask::pageNo() const { return m_pageNo; }

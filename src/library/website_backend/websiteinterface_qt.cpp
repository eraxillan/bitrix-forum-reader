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
#include "websiteinterface_qt.h"

#include <website_backend/websiteinterface.h>

PostQtWrapper::PostQtWrapper() : m_authorQml(), m_authorSignature(), m_date(), m_contentQml(), m_lastEdit(), m_likeCount(0) {}

PostQtWrapper::PostQtWrapper(bfr::PostPtr post) {

	m_authorQml = post->m_author->getQmlString(QRandomGenerator::global()->generate());
	m_authorSignature = post->m_userSignature;
	m_date = post->m_date;
	m_contentQml = post->getQmlString(QRandomGenerator::global()->generate());
	m_lastEdit = post->m_lastEdit;
	m_likeCount = post->m_likeCounter;
}

QString PostQtWrapper::authorQml() const { return m_authorQml; }

QString PostQtWrapper::authorSignature() const { return m_authorSignature; }

QDateTime PostQtWrapper::date() const { return m_date; }

QString PostQtWrapper::contentQml() const { return m_contentQml; }

QString PostQtWrapper::lastEdit() const { return m_lastEdit; }

int PostQtWrapper::likeCount() const { return m_likeCount; }

UserQtWrapper::UserQtWrapper()
	: m_name()
	, m_avatarUrl()
	, m_registrationDate()
	, m_city()
	, m_postCount(0)
	, m_threadPostCount(0)
	, m_reputation(0)
	, m_threadReputation(0)
	, m_ratio(0)
	, m_threadRatio(0) { }

UserQtWrapper::UserQtWrapper(bfr::UserPtr user) {
	m_name = user->m_userName;
	m_avatarUrl = user->m_userAvatar ? user->m_userAvatar->m_url : QString();
	m_registrationDate = user->m_registrationDate.startOfDay();
	m_city = user->m_city;
	m_postCount = user->m_postCount;
	m_threadPostCount = user->m_threadPostCount;
	m_reputation = user->m_reputation;
	m_threadReputation = user->m_threadReputation;
	m_ratio = user->m_ratio;
	m_threadRatio = user->m_threadRatio;
}

QString UserQtWrapper::name() const { return m_name; }

QString UserQtWrapper::avatarUrl() const { return m_avatarUrl; }

QDateTime UserQtWrapper::registrationDate() const { return m_registrationDate; }

QString UserQtWrapper::city() const { return m_city; }

int UserQtWrapper::postCount() const { return m_postCount; }

int UserQtWrapper::threadPostCount() const { return m_threadPostCount; }

qreal UserQtWrapper::reputation() const { return m_reputation; }

qreal UserQtWrapper::threadReputation() const { return m_threadReputation; }

qreal UserQtWrapper::ratio() const { return m_ratio; }

qreal UserQtWrapper::threadRatio() const { return m_threadRatio; }

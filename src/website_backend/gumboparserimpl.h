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
#ifndef __BFR_GUMBOPARSERIMPL_H__
#define __BFR_GUMBOPARSERIMPL_H__

#include "websiteinterface.h"
#include "qtgumbodocument.h"

namespace bfr {

class ForumPageParser : public IForumPageReader {
	struct UserBaseInfo {
		int m_id = -1;
		QString m_name;
		QUrl m_profileUrl;
	};

	struct UserAdditionalInfo {
		QUrl m_allPostsUrl;
		int m_postCount = -1;
		QDate m_registrationDate;
		int m_reputation = -1;
		QString m_city;
	};

	QtGumboDocumentPtr m_htmlDocument;

	mutable bool m_textQuoteFlag = false;

private:
	void printTagsRecursively(QtGumboNodePtr node, int &level);
	void findMsdivNodesRecursively(QtGumboNodePtr node, QVector<QtGumboNodePtr> &msdivNodes);
	void findPageCount(QString rawData, int &pageCount);
	UserBaseInfo getUserBaseInfo(QtGumboNodePtr userInfoNode);
	UserAdditionalInfo getUserAdditionalInfo(QtGumboNodePtr userInfoNode);
	PostImagePtr getUserAvatar(QtGumboNodePtr userInfoNode);
	UserPtr getPostUser(QtGumboNodePtr trNode1);
	PostPtr getPostValue(QtGumboNodePtr trNode1);
	QString getPostLastEdit(QtGumboNodePtr postEntryNode);
	QString getPostUserSignature(QtGumboNodePtr postEntryNode);
	IPostObjectList getPostAttachments(QtGumboNodePtr postEntryNode);
	int getLikeCounterValue(QtGumboNodePtr trNode2);
	int getPostId(QtGumboNodePtr msdivNode);
	void fillPostList(QtGumboNodePtr node, PostList &posts);

	PostHyperlinkPtr parseHyperlink(QtGumboNodePtr aNode) const;
	PostImagePtr parseImage(QtGumboNodePtr imgNode) const;
	PostQuotePtr parseQuote(QtGumboNodePtr tableNode) const;
	PostSpoilerPtr parseSpoiler(QtGumboNodePtr tableNode) const;

	void parseMessage(QtGumboNodes nodes, IPostObjectList &postObjects) const;

public:
	// IForumPageReader implementation
	result_code::Type getPageCount(QByteArray rawData, int &pageCount) override;
	result_code::Type getPagePosts(QByteArray rawData, PostList &userPosts) override;
};
}

#endif // __BFR_GUMBOPARSERIMPL_H__

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
#ifndef __BFR_WEBSITEINTERFACE_H__
#define __BFR_WEBSITEINTERFACE_H__

#include <QtCore/QUrl>
#include <QtCore/QDateTime>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QRandomGenerator>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#ifdef BFR_DUMP_GENERATED_QML_IN_FILES
#include <QtCore/QCoreApplication>
#endif

#include <common/resultcode.h>
#include <website_backend/websiteinterface_fwd.h>

#ifdef BFR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
#define BFR_DEBUG_FRAME_VISIBLE "true"
#else
#define BFR_DEBUG_FRAME_VISIBLE "false"
#endif

namespace bfr {
const QString QUOTE_WRITE_VERB = "пишет";

#ifdef BFR_SERIALIZATION_ENABLED
enum PostObjectType {
	InvalidType = -1,
	SpoilerType = 0,
	QuoteType,
	ImageType,
	LineBreakType,
	PlainTextType,
	RichTextType,
	VideoType,
	HyperlinkType,
	PostType,
	UserType,
	PostObjectTypeCount
};

result_code::Type serializePosts(const bfr::PostList &posts);
result_code::Type deserializePosts(bfr::PostList &posts);
#endif  // #ifdef BFR_SERIALIZATION_ENABLED

struct IPostObject {
	virtual ~IPostObject();

	virtual bool isValid() const = 0;
	virtual uint getHash(uint seed) const = 0;
	virtual QString getQmlString(quint32 randomSeed) const = 0;

#ifdef BFR_SERIALIZATION_ENABLED
	virtual QDataStream &serialize(QDataStream &stream) const = 0;
	virtual QDataStream &deserialize(QDataStream &stream) = 0;
#endif
};

// NOTE: spoiler text is HTML
struct PostSpoiler : IPostObject {
	QString m_title;
	IPostObjectList m_data;

	PostSpoiler();

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

// NOTE: quote text is HTML
struct PostQuote : IPostObject {
	QString m_title;
	QString m_userName;
	QUrl m_url;
	IPostObjectList m_data;

	PostQuote();

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

struct PostImage : IPostObject {
	QString m_url;
	int m_width = -1;
	int m_height = -1;
	int m_border = -1;
	QString m_altName;
	QString m_id;
	QString m_className;

	PostImage();
	PostImage(QString url, int width, int height, int border = 0, QString altName = QString(), QString id = QString(),
		QString className = QString());

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

struct PostLineBreak : IPostObject {
	PostLineBreak();

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

struct PostPlainText : IPostObject {
	QString m_text;

	PostPlainText();
	PostPlainText(QString text);

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

struct PostRichText : IPostObject {
	QString m_text;
	QString m_color = "black";
	bool m_isBold = false;
	bool m_isItalic = false;
	bool m_isUnderlined = false;
	bool m_isStrikedOut = false;

	PostRichText();
	PostRichText(QString text, QString color, bool isBold, bool isItalic, bool isUnderlined, bool isStrikedOut);

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

struct PostVideo : IPostObject {
	// URL
	QString m_urlStr;
	QUrl m_url;

	PostVideo();
	PostVideo(QString urlStr);

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

struct PostHyperlink : IPostObject {
	// URL
	QString m_urlStr;
	QUrl m_url;

	// Display name
	QString m_title;

	// Balloon tip
	QString m_tip;

	QString m_rel;

	PostHyperlink();
	PostHyperlink(QString urlStr, QString title, QString tip = QString(), QString rel = QString());

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

// ----------------------------------------------------------------------------------------------------------------

struct Post : IPostObject {
	int m_id = -1;
	//      int m_postNumber;
	int m_likeCounter = -1;

	//QString m_text;
	IPostObjectList m_data;

	//		QString m_style;
	QString m_lastEdit;
	QString m_userSignature;
	QDateTime m_date;
	//		QUrl m_permalink;

	UserPtr m_author;

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

struct User : IPostObject {
	// Base info
	int m_userId = -1;
	QString m_userName;
	QUrl m_userProfileUrl;

	// Avatar info
	PostImagePtr m_userAvatar;

	// Additional info
	// TODO: extend thread-bound variables to collection,
	//       to store user ratio etc. for several forum threads
	QUrl m_allPostsUrl;
	int m_postCount = -1;
	int m_threadPostCount = 0;
	QDate m_registrationDate;
	int m_reputation = -1;
	int m_threadReputation = 0;
	double m_ratio = 0.0;
	double m_threadRatio = 0.0;
	QString m_city;

	bool isValid() const Q_DECL_OVERRIDE;
	uint getHash(uint seed) const Q_DECL_OVERRIDE;
	QString getQmlString(quint32 randomSeed) const Q_DECL_OVERRIDE;

#ifdef BFR_SERIALIZATION_ENABLED
	QDataStream &serialize(QDataStream &stream) const Q_DECL_OVERRIDE;
	QDataStream &deserialize(QDataStream &stream) Q_DECL_OVERRIDE;
#endif
};

//---------------------------------------------------------------------------------------------
// Interfaces

class IForumPageReader {
public:
	virtual ~IForumPageReader();

	virtual result_code::Type getPageCount(QByteArray rawData, int &pageCount) = 0;
	virtual result_code::Type getPagePosts(QByteArray rawData, PostList &userPosts) = 0;
};

} // namespace bfr

//---------------------------------------------------------------------------------------------

inline uint qHash(const bfr::IPostObjectList &key, uint seed) {
	uint dataHash = 0;
	for (auto obj : key)
		dataHash ^= obj->getHash(seed);
	return dataHash;
}

inline uint qHash(const bfr::Post &key, uint seed) { return key.getHash(seed); }

inline uint qHash(const bfr::User &key, uint seed) { return key.getHash(seed); }

//---------------------------------------------------------------------------------------------

inline bool operator==(const bfr::IPostObject &a, const bfr::IPostObject &b) { return a.getHash(0) == b.getHash(0); }

inline bool operator!=(const bfr::IPostObject &a, const bfr::IPostObject &b) { return !(a == b); }

inline bool operator==(const bfr::PostSpoiler &a, const bfr::PostSpoiler &b) { return (a.getHash(0) == b.getHash(0)); }

inline bool operator!=(const bfr::PostSpoiler &a, const bfr::PostSpoiler &b) { return !(a == b); }

inline bool operator==(const bfr::PostQuote &a, const bfr::PostQuote &b) { return (a.getHash(0) == b.getHash(0)); }

inline bool operator!=(const bfr::PostQuote &a, const bfr::PostQuote &b) { return !(a == b); }

inline bool operator==(const bfr::PostImage &a, const bfr::PostImage &b) { return (a.getHash(0) == b.getHash(0)); }

inline bool operator!=(const bfr::PostImage &a, const bfr::PostImage &b) { return !(a == b); }

inline bool operator==(const bfr::PostLineBreak &a, const bfr::PostLineBreak &b) {
	return (a.getHash(0) == b.getHash(0));
}

inline bool operator!=(const bfr::PostLineBreak &a, const bfr::PostLineBreak &b) { return !(a == b); }

inline bool operator==(const bfr::PostPlainText &a, const bfr::PostPlainText &b) {
	return (a.getHash(0) == b.getHash(0));
}

inline bool operator!=(const bfr::PostPlainText &a, const bfr::PostPlainText &b) { return !(a == b); }

inline bool operator==(const bfr::PostRichText &a, const bfr::PostRichText &b) {
	return (a.getHash(0) == b.getHash(0));
}

inline bool operator!=(const bfr::PostRichText &a, const bfr::PostRichText &b) { return !(a == b); }

inline bool operator==(const bfr::PostVideo &a, const bfr::PostVideo &b) { return (a.getHash(0) == b.getHash(0)); }

inline bool operator!=(const bfr::PostVideo &a, const bfr::PostVideo &b) { return !(a == b); }

inline bool operator==(const bfr::PostHyperlink &a, const bfr::PostHyperlink &b) {
	return (a.getHash(0) == b.getHash(0));
}

inline bool operator!=(const bfr::PostHyperlink &a, const bfr::PostHyperlink &b) { return !(a == b); }

inline bool operator==(const bfr::User &a, const bfr::User &b) { return (a.getHash(0) == b.getHash(0)); }

inline bool operator!=(const bfr::User &a, const bfr::User &b) { return !(a == b); }

inline bool operator==(const bfr::Post &a, const bfr::Post &b) { return (a.getHash(0) == b.getHash(0)); }

inline bool operator!=(const bfr::Post &a, const bfr::Post &b) { return !(a == b); }

inline bool comparePostLists(const bfr::PostList &a, const bfr::PostList &b) {
	if (a.size() != b.size())
		return false;

	for (int i = 0; i < a.size(); i++) {
		if ((*a[i]) != (*b[i]))
			return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------------

#ifdef BFR_SERIALIZATION_ENABLED

QDataStream &operator << (QDataStream &stream, const bfr::IPostObject &obj);
QDataStream &operator >> (QDataStream &stream, bfr::IPostObject &obj);

QDataStream &operator << (QDataStream &stream, const bfr::IPostObjectList &obj);
QDataStream &operator >> (QDataStream &stream, bfr::IPostObjectList &obj);

QDataStream &operator << (QDataStream &stream, const bfr::PostList &obj);
QDataStream &operator >> (QDataStream &stream, bfr::PostList &obj);

#endif // #ifdef BFR_SERIALIZATION_ENABLED

#endif // __BFR_WEBSITEINTERFACE_H__

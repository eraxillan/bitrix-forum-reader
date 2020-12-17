#include "websiteinterface.h"
#include "common/logger.h"

#ifdef BFR_SERIALIZATION_ENABLED

namespace {
static const quint32 BFR_SERIALIZATION_MAGIC = 0xBF6EADE6;
static const quint32 BFR_SERIALIZATION_VERSION = 0x00000001;
}

QDataStream &operator<<(QDataStream &stream, const bfr::IPostObject &obj) { return obj.serialize(stream); }
QDataStream &operator>>(QDataStream &stream, bfr::IPostObject &obj) { return obj.deserialize(stream); }

QDataStream &operator<<(QDataStream &stream, const bfr::IPostObjectList &postList) {
	stream << postList.size();
	foreach (const auto &postObj, postList)
		postObj->serialize(stream);

	return stream;
}
QDataStream &operator>>(QDataStream &stream, bfr::IPostObjectList &postList) {
	int size = 0;
	stream >> size;
	Q_ASSERT(size >= 0);
	for (int i = 0; i < size; ++i) {
		static_assert(bfr::PostObjectTypeCount == 10, "FIXME: implement new PostObject types first");
		int postObjectType = bfr::InvalidType;
		stream >> postObjectType;
		switch (postObjectType) {
			case bfr::SpoilerType: {
				bfr::PostSpoilerPtr spoiler(new bfr::PostSpoiler);
				spoiler->deserialize(stream);
				postList.push_back(spoiler);
				break;
			}
			case bfr::QuoteType: {
				bfr::PostQuotePtr quote(new bfr::PostQuote);
				quote->deserialize(stream);
				postList.push_back(quote);
				break;
			}
			case bfr::ImageType: {
				bfr::PostImagePtr image(new bfr::PostImage);
				image->deserialize(stream);
				postList.push_back(image);
				break;
			}
			case bfr::LineBreakType: {
				bfr::PostLineBreakPtr linebreak(new bfr::PostLineBreak);
				linebreak->deserialize(stream);
				postList.push_back(linebreak);
				break;
			}
			case bfr::PlainTextType: {
				bfr::PostPlainTextPtr plaintext(new bfr::PostPlainText);
				plaintext->deserialize(stream);
				postList.push_back(plaintext);
				break;
			}
			case bfr::RichTextType: {
				bfr::PostRichTextPtr richtext(new bfr::PostRichText);
				richtext->deserialize(stream);
				postList.push_back(richtext);
				break;
			}
			case bfr::VideoType: {
				bfr::PostVideoPtr video(new bfr::PostVideo);
				video->deserialize(stream);
				postList.push_back(video);
				break;
			}
			case bfr::HyperlinkType: {
				bfr::PostHyperlinkPtr hyperlink(new bfr::PostHyperlink);
				hyperlink->deserialize(stream);
				postList.push_back(hyperlink);
				break;
			}
			case bfr::PostType: {
				bfr::PostPtr post(new bfr::Post);
				post->deserialize(stream);
				postList.push_back(post);
				break;
			}
			case bfr::UserType: {
				bfr::UserPtr user(new bfr::User);
				user->deserialize(stream);
				postList.push_back(user);
				break;
			}
			default:
				Q_ASSERT(0);
		}
	}

	return stream;
}

QDataStream &operator<<(QDataStream &stream, const bfr::PostList &obj) {
	stream << obj.size();
	foreach (const auto &post, obj)
		post->serialize(stream);

	return stream;
}
QDataStream &operator>>(QDataStream &stream, bfr::PostList &obj) {
	int size = 0;
	stream >> size;
	for (int i = 0; i < size; i++) {
		int type = bfr::InvalidType;
		stream >> type;
		Q_ASSERT(type == bfr::PostType);

		bfr::PostPtr postObj(new bfr::Post);
		postObj->deserialize(stream);
		obj.push_back(postObj);
	}

	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::PostSpoiler &obj)
QDataStream &bfr::PostSpoiler::serialize(QDataStream &stream) const {
	// QString m_title;
	// IPostObjectList m_data;
	//
	stream << SpoilerType;
	stream << m_title;
	stream << m_data;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::PostSpoiler &obj)
QDataStream &bfr::PostSpoiler::deserialize(QDataStream &stream) {
	stream >> m_title;
	stream >> m_data;
	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::PostQuote &obj)
QDataStream &bfr::PostQuote::serialize(QDataStream &stream) const {
	// QString m_title;
	// QString m_userName;
	// QUrl m_url;
	// IPostObjectList m_data;
	//
	stream << QuoteType;
	stream << m_title;
	stream << m_userName;
	stream << m_url;
	stream << m_data;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::PostQuote &obj)
QDataStream &bfr::PostQuote::deserialize(QDataStream &stream) {
	stream >> m_title;
	stream >> m_userName;
	stream >> m_url;
	stream >> m_data;
	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::PostImage &obj)
QDataStream &bfr::PostImage::serialize(QDataStream &stream) const {
	// QString m_url;
	// int m_width = -1;
	// int m_height = -1;
	// int m_border = -1;
	// QString m_altName;
	// QString m_id;
	// QString m_className;
	//
	stream << ImageType;
	stream << m_url;
	stream << m_width;
	stream << m_height;
	stream << m_border;
	stream << m_altName;
	stream << m_id;
	stream << m_className;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::PostImage &obj)
QDataStream &bfr::PostImage::deserialize(QDataStream &stream) {
	stream >> m_url;
	stream >> m_width;
	stream >> m_height;
	stream >> m_border;
	stream >> m_altName;
	stream >> m_id;
	stream >> m_className;
	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::PostLineBreak &obj)
QDataStream &bfr::PostLineBreak::serialize(QDataStream &stream) const {
	stream << LineBreakType;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::PostLineBreak &obj)
QDataStream &bfr::PostLineBreak::deserialize(QDataStream &stream) { return stream; }

// QDataStream &operator<<(QDataStream &stream, const bfr::PostPlainText &obj)
QDataStream &bfr::PostPlainText::serialize(QDataStream &stream) const {
	// QString m_text;
	//
	stream << PlainTextType;
	stream << m_text;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::PostPlainText &obj)
QDataStream &bfr::PostPlainText::deserialize(QDataStream &stream) {
	stream >> m_text;
	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::PostRichText &obj)
QDataStream &bfr::PostRichText::serialize(QDataStream &stream) const {
	// QString m_text;
	// QString m_color = "black";
	// bool m_isBold = false;
	// bool m_isItalic = false;
	// bool m_isUnderlined = false;
	// bool m_isStrikedOut = false;
	//
	stream << RichTextType;
	stream << m_text;
	stream << m_color;
	stream << m_isBold;
	stream << m_isItalic;
	stream << m_isUnderlined;
	stream << m_isStrikedOut;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::PostRichText &obj)
QDataStream &bfr::PostRichText::deserialize(QDataStream &stream) {
	stream >> m_text;
	stream >> m_color;
	stream >> m_isBold;
	stream >> m_isItalic;
	stream >> m_isUnderlined;
	stream >> m_isStrikedOut;
	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::PostVideo &obj)
QDataStream &bfr::PostVideo::serialize(QDataStream &stream) const {
	// QString m_urlStr;
	// QUrl m_url;
	//
	stream << VideoType;
	stream << m_urlStr;
	stream << m_url;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::PostVideo &obj)
QDataStream &bfr::PostVideo::deserialize(QDataStream &stream) {
	stream >> m_urlStr;
	stream >> m_url;
	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::PostHyperlink &obj)
QDataStream &bfr::PostHyperlink::serialize(QDataStream &stream) const {
	// QString m_urlStr;
	// QUrl m_url;
	// QString m_title;
	// QString m_tip;
	// QString m_rel;
	//
	stream << HyperlinkType;
	stream << m_urlStr;
	stream << m_url;
	stream << m_title;
	stream << m_tip;
	stream << m_rel;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::PostHyperlink &obj)
QDataStream &bfr::PostHyperlink::deserialize(QDataStream &stream) {
	stream >> m_urlStr;
	stream >> m_url;
	stream >> m_title;
	stream >> m_tip;
	stream >> m_rel;
	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::Post &obj)
QDataStream &bfr::Post::serialize(QDataStream &stream) const {
	// int m_id = -1;
	// int m_likeCounter = -1;
	// IPostObjectList m_data;
	// QString m_lastEdit;
	// QString m_userSignature;
	// QDateTime m_date;
	// UserPtr m_author;
	//
	stream << PostType;
	stream << m_id;
	stream << m_likeCounter;
	stream << m_data;
	stream << m_lastEdit;
	stream << m_userSignature;
	stream << m_date;
	stream << (*m_author);
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::Post &obj)
QDataStream &bfr::Post::deserialize(QDataStream &stream) {
	stream >> m_id;
	stream >> m_likeCounter;
	stream >> m_data;
	stream >> m_lastEdit;
	stream >> m_userSignature;
	stream >> m_date;
	m_author.reset(new User);
	int authorType = InvalidType;
	stream >> authorType;
	Q_ASSERT(authorType == UserType);
	stream >> (*m_author);
	return stream;
}

// QDataStream &operator<<(QDataStream &stream, const bfr::User &obj)
QDataStream &bfr::User::serialize(QDataStream &stream) const {
	// int m_userId = -1;
	// QString m_userName;
	// QUrl m_userProfileUrl;
	// PostImagePtr m_userAvatar;
	// QUrl m_allPostsUrl;
	// int m_postCount = -1;
	// int m_threadPostCount = 0;
	// QDate m_registrationDate;
	// int m_reputation = -1;
	// int m_threadReputation = 0;
	// double m_ratio = 0.0;
	// double m_threadRatio = 0.0;
	// QString m_city;
	//
	stream << UserType;
	stream << m_userId;
	stream << m_userName;
	stream << m_userProfileUrl;

	stream << (m_userAvatar ? true : false);
	if (m_userAvatar)
		stream << (*m_userAvatar);

	stream << m_allPostsUrl;
	stream << m_postCount;
	stream << m_threadPostCount;
	stream << m_registrationDate;
	stream << m_reputation;
	stream << m_threadReputation;
	stream << m_ratio;
	stream << m_threadRatio;
	stream << m_city;
	return stream;
}
// QDataStream &operator>>(QDataStream &stream, bfr::User &obj)
QDataStream &bfr::User::deserialize(QDataStream &stream) {
	stream >> m_userId;
	stream >> m_userName;
	stream >> m_userProfileUrl;

	bool haveAvatar = false;
	stream >> haveAvatar;
	if (haveAvatar) {
		int imageType = InvalidType;
		stream >> imageType;
		Q_ASSERT(imageType == ImageType);
		m_userAvatar.reset(new PostImage);
		stream >> (*m_userAvatar);
	}

	stream >> m_allPostsUrl;
	stream >> m_postCount;
	stream >> m_threadPostCount;
	stream >> m_registrationDate;
	stream >> m_reputation;
	stream >> m_threadReputation;
	stream >> m_ratio;
	stream >> m_threadRatio;
	stream >> m_city;
	return stream;
}

namespace bfr {

result_code::Type serializePosts(const bfr::PostList &posts) {
	QString localDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	if (!localDataDir.endsWith(QDir::separator()))
		localDataDir += QDir::separator();
	QDir localDataDirObj(localDataDir);
	if (!localDataDirObj.exists()) {
		ConsoleLogger->warn("Local data directory not exists, trying to create it...");
		if (!localDataDirObj.mkpath(localDataDir)) {
			ConsoleLogger->error("Unable to create local data directory");
			return result_code::Type::InputOutputError;
		}

		ConsoleLogger->info("Local data directory was successfully created");
	}

	QString filePath = localDataDir + "bfr.dat";
	ConsoleLogger->info("Forum posts database file path: {}", filePath.toStdString());

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		ConsoleLogger->error("Unable to create file '{}'", filePath);
		return result_code::Type::InputOutputError;
	}

	QDataStream out(&file);

	// Write a header with a "magic number" and a version
	out << (quint32)BFR_SERIALIZATION_MAGIC;
	out << (quint32)BFR_SERIALIZATION_VERSION;

	out.setVersion(QDataStream::Qt_5_10);

	// Write the data
	out << posts;

	return result_code::Type::Ok;
}

result_code::Type deserializePosts(bfr::PostList &posts) {
	QString localDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	if (!localDataDir.endsWith(QDir::separator()))
		localDataDir += QDir::separator();
	QDir localDataDirObj(localDataDir);
	if (!localDataDirObj.exists()) {
		ConsoleLogger->error("Local data directory not exists");
		return result_code::Type::InputOutputError;
	}

	QString filePath = localDataDir + "bfr.dat";
	ConsoleLogger->info("Forum posts database file path: {}", filePath.toStdString());

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		ConsoleLogger->error("Unable to open file '{}'", filePath);
		return result_code::Type::InputOutputError;
	}

	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_5_10);

	// Write a header with a "magic number" and a version
	quint32 magicNumber = 0;
	out >> magicNumber;
	if (magicNumber != BFR_SERIALIZATION_MAGIC) {
		ConsoleLogger->error("Invalid file format");
		return result_code::Type::InvalidFileFormat;
	}

	quint32 versionNumber = 0;
	out >> versionNumber;
	if (versionNumber != BFR_SERIALIZATION_VERSION) {
		ConsoleLogger->error("Invalid file version");
		return result_code::Type::InvalidFileVersion;
	}

	// Read the data
	out >> posts;
	return result_code::Type::Ok;
}

} // namespace bfr

#endif // #ifdef BFR_SERIALIZATION_ENABLED


#ifdef BFR_DUMP_GENERATED_QML_IN_FILES
namespace {
static bool WriteTextFile(QString fileName, QString fileContents) {
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QTextStream out(&file);
	out << fileContents;
	return true;
}
}
#endif  // #ifdef BFR_DUMP_GENERATED_QML_IN_FILES

namespace bfr
{

IPostObject::~IPostObject()
{
}

// ----------------------------------------------------------------------------------------------------------------------------
namespace {
QString readQmlFile(QString fileName) {
	BFR_DECLARE_DEFAULT_RETURN_TYPE(QString);

	QFile f(fileName);
	BFR_RETURN_DEFAULT_IF(!f.open(QIODevice::ReadOnly | QIODevice::Text), "Unable to open QML file");
	return f.readAll();
}
}

PostSpoiler::PostSpoiler()
{
}

bool PostSpoiler::isValid() const { return !m_data.isEmpty(); }

uint PostSpoiler::getHash(uint seed) const { return qHash(m_title, seed) ^ qHash(m_data, seed); }

QString PostSpoiler::getQmlString(quint32 randomSeed) const {
#ifndef BFR_SHOW_SPOILER
	Q_UNUSED(randomSeed);
	return QString("        Text { font.pointSize: 14; text: 'PostSpoiler'; }\n");
#else
	QString spoilerQml;
	for (const auto &iObj : m_data)
		spoilerQml += iObj->getQmlString(QRandomGenerator::global()->generate());

	QString qmlStr = readQmlFile("://qml/PostSpoiler.qml");

	qmlStr.replace("import QtQuick 2.10;", "");
	qmlStr.replace("_fcdde97089594b218fbc925097a2f982", QString::number(randomSeed));
	qmlStr.replace("_4d4282062caf43929173ac37159d230f", m_title);
	qmlStr.replace("state: _d664d2ca198c40819fb4850253db6886;", spoilerQml);

	return qmlStr;
#endif
}

// PostQuote

PostQuote::PostQuote()
{
}

bool PostQuote::isValid() const { return !m_data.isEmpty(); }

uint PostQuote::getHash(uint seed) const {
	return qHash(m_title, seed) ^ qHash(m_userName, seed) ^ qHash(m_url, seed) ^ qHash(m_data, seed);
}

QString PostQuote::getQmlString(quint32 randomSeed) const {
#ifndef BFR_SHOW_QUOTE
	Q_UNUSED(randomSeed);
	return QString("        Text { font.pointSize: 14; text: 'PostQuote'; }\n");
#else
	QString quoteQml;
	for (const auto &iObj : m_data)
		quoteQml += iObj->getQmlString(QRandomGenerator::global()->generate());

	QString titleEsc = QString(m_title).replace("'", "\\'");
	QString userNameEsc = QString(m_userName).replace("'", "\\'");
	QString urlText
		= m_url.isValid() ? PostHyperlink(m_url.toString(), QUOTE_WRITE_VERB).getQmlString(randomSeed) : QString();

	QString qmlStr = readQmlFile("://qml/PostQuote.qml");

	qmlStr.replace("import QtQuick 2.10;", "");
	qmlStr.replace("_d965b5e7c48a416aaeea7772888d02e4", QString::number(randomSeed));
	qmlStr.replace("_2ca6e4441d074fbebef439a882585113", titleEsc);
	qmlStr.replace("_81318a4c46474759b9ebd6b0e94873aa", userNameEsc);
	qmlStr.replace("state: _e265fb5cb7544ee88130089cc4d0353f;", urlText);
	qmlStr.replace("state: _c8f7635c38ed49168187d56654807c9c;", quoteQml);
	qmlStr.replace("_af0014871b304e238b0d6e37d90eb5c5", !userNameEsc.isEmpty() ? "true" : "false");

	return qmlStr;
#endif
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostImage

PostImage::PostImage()
{
}

PostImage::PostImage(QString url, int width, int height, int border, QString altName, QString id, QString className)
	: m_url(url)
	, m_width(width)
	, m_height(height)
	, m_border(border)
	, m_altName(altName)
	, m_id(id)
	, m_className(className) { }

bool PostImage::isValid() const {
	// NOTE: image with only correct URL specified is still valid
	return !m_url.isEmpty() /*&& (m_width > 0 && m_height > 0)*/;
}

uint PostImage::getHash(uint seed) const {
	// NOTE: m_id is auto-generated each time page reloaded, so it is useless for hashing purposes
	return qHash(m_url, seed) ^ qHash(m_width, seed) ^ qHash(m_height, seed) ^ qHash(m_border, seed)
		^ qHash(m_altName, seed) /* ^ qHash(m_id, seed) */ ^ qHash(m_className, seed);
}

QString PostImage::getQmlString(quint32 randomSeed) const {
#ifndef BFR_SHOW_IMAGE
	Q_UNUSED(randomSeed);
	return QString("        Text { font.pointSize: 14; text: 'PostImage'; }\n");
#else
	QString qmlStr;
	if (!m_url.endsWith(".gif")) {
		qmlStr = readQmlFile("://qml/PostImage.qml");
	} else {
		// NOTE: implement other fields usage if they will be implemented on the backend side
		Q_ASSERT(m_width == -1);
		Q_ASSERT(m_height == -1);

		qmlStr = readQmlFile("://qml/PostAnimatedImage.qml");
	}

	qmlStr.replace("import QtQuick 2.10;", "");
	qmlStr.replace("_6d63cf96fdc747d591311cfa352b26e4", QString::number(randomSeed));
	qmlStr.replace("_5b4898dbfff046a7ad6474e9b9155808", m_url);
	return qmlStr;
#endif
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostLineBreak
PostLineBreak::PostLineBreak()
{
}

bool PostLineBreak::isValid() const { return true; }

uint PostLineBreak::getHash(uint seed) const { return qHash(0, seed); }

QString PostLineBreak::getQmlString(quint32 randomSeed) const {
#ifndef BFR_SHOW_LINEBREAK
	Q_UNUSED(randomSeed);
	return QString("        Text { font.pointSize: 14; text: 'PostLineBreak'; }\n");
#else
	QString qmlStr = readQmlFile("://qml/PostLineBreak.qml");

	qmlStr.replace("import QtQuick 2.10;", "");
	qmlStr.replace("_0b2f8da4a11c4b9a8ba2e642cc9e113e", QString::number(randomSeed));
	qmlStr.replace("_51d55029cb9f49aabdb30ae957929ebc", BFR_DEBUG_FRAME_VISIBLE);
	return qmlStr;
#endif
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostPlainText

PostPlainText::PostPlainText()
{
}

PostPlainText::PostPlainText(QString text)
	: m_text(text) { }

bool PostPlainText::isValid() const { return !m_text.isEmpty(); }

uint PostPlainText::getHash(uint seed) const { return qHash(m_text, seed); }

QString PostPlainText::getQmlString(quint32 randomSeed) const {
#ifndef BFR_SHOW_PLAINTEXT
	Q_UNUSED(randomSeed);
	return QString("        Text { font.pointSize: 14; text: 'PostPlainText'; }\n");
#else
	QString qmlStr = readQmlFile("://qml/PostPlainText.qml");
	QString textEsc = QString(m_text).replace("'", "\\'");

	// NOTE: Qt Creator editor requires this import statement, but Qt.createQmlObject() call will fail on it
	qmlStr.replace("import QtQuick 2.10;", "");

	qmlStr.replace("_63f18ed6e6c84c7c803ca7bd2b7c8a43", QString::number(randomSeed));
	qmlStr.replace("_da5eb852c7b64ceca937ddb810b0bcdc", textEsc);
	qmlStr.replace("_7fc091fe66ce4db193a4267004716245", BFR_DEBUG_FRAME_VISIBLE);
	return qmlStr;
#endif
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostRichText

PostRichText::PostRichText()
{
}

PostRichText::PostRichText(
	QString text, QString color, bool isBold, bool isItalic, bool isUnderlined, bool isStrikedOut)
	: m_text(text)
	, m_color(color)
	, m_isBold(isBold)
	, m_isItalic(isItalic)
	, m_isUnderlined(isUnderlined)
	, m_isStrikedOut(isStrikedOut) { }

bool PostRichText::isValid() const { return !m_text.isEmpty(); }

uint PostRichText::getHash(uint seed) const {
	return qHash(m_text, seed) ^ qHash(m_color, seed) ^ qHash(m_isBold, seed) ^ qHash(m_isItalic, seed)
		^ qHash(m_isUnderlined, seed) ^ qHash(m_isStrikedOut, seed);
}

QString PostRichText::getQmlString(quint32 randomSeed) const {
#ifndef BFR_SHOW_RICHTEXT
	Q_UNUSED(randomSeed);
	return QString("        Text { font.pointSize: 14; text: 'PostRichText'; }\n");
#else
	QString qmlStr = readQmlFile("://qml/PostRichText.qml");
	QString textEsc = QString(m_text).replace("'", "\\'");

	qmlStr.replace("import QtQuick 2.10;", "");
	qmlStr.replace("_b48bb9229e2545d28a3024bffdbae97f", QString::number(randomSeed));
	qmlStr.replace("#FF00FF00", m_color);
	qmlStr.replace("_e0b18a71c2ea460c8229a2b8019490d7", m_isBold ? "true" : "false");
	qmlStr.replace("_8d2ac045ee8543dc8d4733fee0b852cb", m_isItalic ? "true" : "false");
	qmlStr.replace("_e24880192af74e8f9fa513b818bef3b8", m_isUnderlined ? "true" : "false");
	qmlStr.replace("_018c6d2a97cf494783da76292f1c932d", m_isStrikedOut ? "true" : "false");
	qmlStr.replace("_4a58cd3f7bf24fa38933fc7538be1d82", textEsc);
	qmlStr.replace("_2d8f971fbbf1456385834828253e21de", BFR_DEBUG_FRAME_VISIBLE);

	return qmlStr;
#endif
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostVideo

PostVideo::PostVideo()
{
}

/*
namespace {
#ifndef Q_OS_IOS
static bool findBestVideoUrl(QByteArray aJsonData, QString& aVideoUrlStr)
{
    aVideoUrlStr = "";

int maxVideoWidth = -1;
//    int maxVideoHeight = -1;

QJsonDocument jsonDoc(QJsonDocument::fromJson(aJsonData));
QJsonObject jsonRootObject = jsonDoc.object();
QJsonArray jsonFormatsArray = jsonRootObject["formats"].toArray();
for (int i = 0; i < jsonFormatsArray.size(); ++i)
{
	QJsonObject jsonFormatObject = jsonFormatsArray[i].toObject();
	Q_ASSERT(!jsonFormatObject.isEmpty());
	
	Q_ASSERT(jsonFormatObject.value("vcodec").isString());
	Q_ASSERT(jsonFormatObject.value("acodec").isString());
	Q_ASSERT(jsonFormatObject.value("ext").isString());
	Q_ASSERT(jsonFormatObject.value("format_note").isString());
	// FIXME: determine type of these values
	//Q_ASSERT(jsonFormatObject.value("width").isDouble());
	//Q_ASSERT(jsonFormatObject.value("height").isDouble());
	Q_ASSERT(jsonFormatObject.value("url").isString());
	
	QString videoCodecStr = jsonFormatObject.value("vcodec").toString();
	//        QString audioCodecStr = jsonFormatObject.value("acodec").toString();
	//        QString videoFileExt = jsonFormatObject.value("ext").toString();
	QString videoFormatNote = jsonFormatObject.value("format_note").toString();
	int videoWidth = jsonFormatObject.value("width").toInt(-1);
	int videoHeight = jsonFormatObject.value("height").toInt(-1);
	QString videoUrlStr = jsonFormatObject.value("url").toString();
	
	if (videoWidth < 0 || videoHeight < 0 || videoCodecStr.compare("none", Qt::CaseInsensitive) == 0) continue;
	if (videoUrlStr.isEmpty()) continue;
	if (videoFormatNote.startsWith("DASH")) continue;
	
	if (videoWidth > maxVideoWidth)
	{
		maxVideoWidth = videoWidth;
		//            maxVideoHeight = videoHeight;
		
		aVideoUrlStr = videoUrlStr;
	}

#ifdef BFR_PRINT_DEBUG_OUTPUT
	ConsoleLogger->info("--------------------------------------------------");
	ConsoleLogger->info("Video width: {}", videoWidth);
	ConsoleLogger->info("Video height: {}", videoHeight);
	ConsoleLogger->info("Video codec: {}", videoCodecStr);
	//        ConsoleLogger->info("Audio codec: {}", audioCodecStr);
	ConsoleLogger->info("File format: {}", videoFormatNote);
	//        ConsoleLogger->info("File extension: {}", videoFileExt);
	ConsoleLogger->info("--------------------------------------------------");
#endif
}

#ifdef BFR_PRINT_DEBUG_OUTPUT
//    ConsoleLogger->info("Maximum resolution: {} x {}", maxVideoWidth, maxVideoHeight);
#endif

return true;
}
#endif
}
*/

PostVideo::PostVideo(QString urlStr)
	: m_urlStr(urlStr)
	, m_url(urlStr) {
	Q_ASSERT(m_url.isValid());

	// Example of YouTube URL: https://www.youtube.com/watch?v=PI9o3v4nttU
	/* QUrlQuery urlQuery(m_url);
    QString videoId = urlQuery.queryItemValue("v");

	// FIXME: replace this ugly hardcoded path with e.g. environment varible
#if !defined(Q_OS_IOS)
	QProcess youtubeDlProcess;
#if defined(Q_OS_WIN)
	youtubeDlProcess.start("C:\\Users\\2\\Downloads\\youtube-dl.exe", QStringList() << "-J" << "--skip-download" << videoId);
#elif defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_OSX)
	youtubeDlProcess.start("youtube-dl", QStringList() << "-J" << "--skip-download" << videoId);
#elif defined(Q_OS_OSX)
	youtubeDlProcess.start("/usr/local/bin/youtube-dl", QStringList() << "-J" << "--skip-download" << videoId);
#elif defined(Q_OS_ANDROID)
	// FIXME: embed youtube-dl executable to APK
	m_url.clear();
	m_urlStr.clear();
	return;
#else
#error "Unsupported OS"
#endif
	
	if (!youtubeDlProcess.waitForStarted()) Q_ASSERT(0);
	youtubeDlProcess.closeWriteChannel();
	if (!youtubeDlProcess.waitForFinished()) Q_ASSERT(0);
	
	QByteArray result = youtubeDlProcess.readAll();
	findBestVideoUrl(result, m_urlStr);
#else
#pragma message("iOS do not support QProcess")
	m_url.clear();
	m_urlStr.clear();
#endif*/
}

bool PostVideo::isValid() const { return !m_urlStr.isEmpty() && m_url.isValid(); }

uint PostVideo::getHash(uint seed) const { return qHash(m_urlStr, seed) ^ qHash(m_url, seed); }

QString PostVideo::getQmlString(quint32 randomSeed) const {
#ifndef BFR_SHOW_VIDEO
	Q_UNUSED(randomSeed);
	return QString("        Text { font.pointSize: 14; text: 'PostVideo';}\n");
#else
	QString qmlStr = readQmlFile("://qml/PostVideo.qml");

	qmlStr.replace("import QtQuick 2.10;", "");
	qmlStr.replace("_2f0a985471e44cd69925f80d37de946a", QString::number(randomSeed));
	qmlStr.replace("_c66d930f7e324345ba9ae5741f3d4142", m_urlStr);
	return qmlStr;
#endif
}

// ----------------------------------------------------------------------------------------------------------------------------
// PostHyperLink

PostHyperlink::PostHyperlink()
{
}

PostHyperlink::PostHyperlink(QString urlStr, QString title, QString tip, QString rel)
	: m_urlStr(urlStr)
	, m_url(urlStr)
	, m_title(title)
	, m_tip(tip)
	, m_rel(rel) {
	if (m_title.isEmpty())
		m_title = m_urlStr;
	Q_ASSERT(m_url.isValid());
}

bool PostHyperlink::isValid() const { return !m_urlStr.isEmpty() && m_url.isValid(); }

uint PostHyperlink::getHash(uint seed) const {
	return qHash(m_urlStr, seed) ^ qHash(m_url, seed) ^ qHash(m_title, seed) ^ qHash(m_tip, seed) ^ qHash(m_rel, seed);
}

QString PostHyperlink::getQmlString(quint32 randomSeed) const {
#ifndef BFR_SHOW_HYPERLINK
	Q_UNUSED(randomSeed);
	return QString("        Text { font.pointSize: 14; text: 'PostHyperlink'; }\n");
#else
	QString qmlStr = readQmlFile("://qml/PostHyperlink.qml");

	qmlStr.replace("import QtQuick 2.10;", "");
	qmlStr.replace("_0947683f69d64f0b9e37d78a2e2a9925", QString::number(randomSeed));
	qmlStr.replace("_8f07025844744bbc84bf2c1868f58abd", m_title);
	qmlStr.replace("_b93596dcbae045a18be44f67d45f22a8", m_urlStr);
	qmlStr.replace("_63a25cb5bf444bc19d127626ca9a9b3f", BFR_DEBUG_FRAME_VISIBLE);
	return qmlStr;
#endif
}

// ----------------------------------------------------------------------------------------------------------------------------
// IForumPageReader

IForumPageReader::~IForumPageReader() {}

//void Post::addObject(IPostObjectPtr obj)
//{
//    m_data.append(obj);
//}

bool Post::isValid() const { return (m_id > 0) && (m_likeCounter >= 0) && !m_data.isEmpty() && m_date.isValid(); }

uint Post::getHash(uint seed) const {
	uint dataHash = 0;
	for (auto obj : m_data)
		dataHash ^= obj->getHash(seed);

	return qHash(m_id, seed) ^ qHash(m_likeCounter, seed) ^ dataHash ^ qHash(m_lastEdit, seed)
		^ qHash(m_userSignature, seed) ^ qHash(m_date, seed);
}

QString Post::getQmlString(quint32 randomSeed) const {
	QString qmlStr = readQmlFile("://qml/Post.qml");

	int validItemsCount = 0;
	for (auto iObj = m_data.begin(); iObj != m_data.end(); ++iObj) {
		if (!(*iObj)->isValid() || (*iObj)->getQmlString(randomSeed).isEmpty()) {
			continue;
		}

		validItemsCount++;
	}

	QString internalQml;
	if (validItemsCount == 0)
		return QString();
	if (validItemsCount == 1) {
		randomSeed = QRandomGenerator::global()->generate();
		internalQml += m_data[0]->getQmlString(randomSeed);
	} else {
		for (auto iObj = m_data.begin(); iObj != m_data.end(); ++iObj) {
			randomSeed = QRandomGenerator::global()->generate();
			internalQml += (*iObj)->getQmlString(randomSeed);
			// internalQml = internalQml.trimmed();
		}
	}

	qmlStr.replace("_05ea9fa84e5148fdaa80754067b2ddad", QString::number(randomSeed));
	qmlStr.replace("state: _a130f037750e40c69eb7d4ffc572822a;", internalQml);

#ifdef BFR_DUMP_GENERATED_QML_IN_FILES
#if defined(Q_OS_ANDROID)
	const QDir appDataDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
	BFR_RETURN_VALUE_IF(!appDataDir.isReadable(), QString(), "unable to get application local data directory path!");

	// FIXME: find a way to get access to the page and post number fields to the Post object;
	//        as an option - implement "Property" interface in "Post" object and set page number and post index as properties
	static const int pageNo = 130;
	static int index = 1;
	const QString qmlFilePath = appDataDir.absoluteFilePath("page_" + QString::number(pageNo) + "_post_" + QString::number(index) + ".qml");

	BFR_RETURN_VALUE_IF(!WriteTextFile(qmlFilePath, qmlStr), QString(), "unable to dump qml file!");
	index++;

#elif defined(Q_OS_IOS)
	// FIXME: implement if possible
#else
	QDir appDataDir(qApp->applicationDirPath());
	BFR_RETURN_VALUE_IF(!appDataDir.isReadable(), QString(), "unable to get application local data directory path!");

	// FIXME: find a way to get access to the page and post number fields to the Post object;
	//        as an option - implement "Property" interface in "Post" object and set page number and post index as properties
	static const int pageNo = 130;
	static int index = 1;
	const QString qmlFilePath = appDataDir.absoluteFilePath("page_" + QString::number(pageNo) + "_post_" + QString::number(index) + ".qml");

	BFR_RETURN_VALUE_IF(!WriteTextFile(qmlFilePath, qmlStr), QString(), "unable to dump qml file!");
	index++;
#endif
#endif

	return qmlStr;
}

bool User::isValid() const {
	return (m_userId > 0) && !m_userName.isEmpty() && m_userProfileUrl.isValid() && m_allPostsUrl.isValid()
		&& (m_postCount > 0) && m_registrationDate.isValid() && (m_reputation >= 0);
}

uint User::getHash(uint seed) const {
	return qHash(m_userId, seed) ^ qHash(m_userName, seed) ^ qHash(m_userProfileUrl, seed)
		^ (m_userAvatar ? m_userAvatar->getHash(seed) : 0) ^ qHash(m_allPostsUrl, seed) ^ qHash(m_postCount, seed)
		^ qHash(m_registrationDate, seed) ^ qHash(m_reputation, seed) ^ qHash(m_city, seed);
}

QString User::getQmlString(quint32 randomSeed) const {
	QString qmlStr = readQmlFile("://qml/User.qml");

	qmlStr.replace("_6afb4d56214b4bb69281bf9bbf60396b", QString::number(randomSeed));
	qmlStr.replace("_14c1808e13d44b64accf01b6b8a6be25", m_userName);
	qmlStr.replace("_c75e18958b0f43fa9e7881315afccc54", m_userAvatar ? m_userAvatar->m_url : "");
	qmlStr.replace("_1e4e440acb92478197b992539899e5de",
		m_userAvatar ? QString::number(m_userAvatar->m_width) : QString::number(0));
	qmlStr.replace("_3860f825cb2a426f82730cc2ca35e6ac",
		m_userAvatar ? QString::number(m_userAvatar->m_height) : QString::number(0));
	qmlStr.replace("_fcfaa58e3305486e872ae962537bdc8e", QString::number(m_postCount));
	qmlStr.replace("_e0aefa26825e44b58c74a54b5bad32b8", m_registrationDate.toString("yyyy"));
	qmlStr.replace("_875abbe700a0408891e5c6f73b7e27be", QString::number(m_reputation));
	qmlStr.replace("_3cf1b6db9ccf4404b672f7c9d6c78211", m_city);
	return qmlStr;
}

}   // namespace bfr

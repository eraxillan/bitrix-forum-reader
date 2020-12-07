#ifndef __BFR_FORUMTHREADURL_H__
#define __BFR_FORUMTHREADURL_H__

#include <QtCore/QtCore>

struct ForumThreadUrlData {
	int m_sectionId;
	int m_threadId;

	constexpr ForumThreadUrlData() noexcept
		: m_sectionId(-1)
		, m_threadId(-1) { }

	constexpr ForumThreadUrlData(int sectionId, int threadId) noexcept
		: m_sectionId(sectionId)
		, m_threadId(threadId) { }

	constexpr ForumThreadUrlData(const ForumThreadUrlData &other) noexcept
		: m_sectionId(other.m_sectionId)
		, m_threadId(other.m_threadId) { }

	constexpr ForumThreadUrlData &operator=(ForumThreadUrlData arg) noexcept {
		// NOTE: std::swap will be marked as `constexpr` only in C++20
		//std::swap(m_sectionId, arg.m_sectionId);
		//std::swap(m_threadId, arg.m_threadId);

		auto swapInt = [](int &first, int &second) {
			int temp = first;
			first = second;
			second = temp;
		};
		swapInt(m_sectionId, arg.m_sectionId);
		swapInt(m_threadId, arg.m_threadId);

		return *this;
	}
};

class ForumThreadUrl : public QObject {
	Q_OBJECT

	Q_PROPERTY(int sectionId READ sectionId WRITE setSectionId NOTIFY sectionIdChanged)
	Q_PROPERTY(int threadId READ threadId WRITE setThreadId NOTIFY threadIdChanged)

	ForumThreadUrlData m_data;

public:
	explicit ForumThreadUrl(QObject *parent = nullptr);
	explicit ForumThreadUrl(QObject *parent, ForumThreadUrlData urlData);
	ForumThreadUrl(int sectionId, int threadId);
	~ForumThreadUrl();

	ForumThreadUrlData data() const;
	int sectionId() const;
	int threadId() const;
	void setSectionId(int sectionId);
	void setThreadId(int threadId);

	// "https://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=358149"
	Q_INVOKABLE QString firstPageUrl() const;

	// "https://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=358149&PAGEN_1=14#forum-message-list"
	Q_INVOKABLE QString pageUrl(int pageNumber) const;

signals:
	void sectionIdChanged();
	void threadIdChanged();
};

Q_DECLARE_METATYPE(ForumThreadUrlData)

inline bool operator<(const ForumThreadUrlData &url1, const ForumThreadUrlData &url2) {
	if (url1.m_sectionId != url2.m_sectionId)
		return url1.m_sectionId < url2.m_sectionId;
	return url1.m_threadId < url2.m_threadId;
}

#endif // __BFR_FORUMTHREADURL_H__

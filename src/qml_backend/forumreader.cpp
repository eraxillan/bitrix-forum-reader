#include "forumreader.h"

#include "src/website_backend/gumboparserimpl.h"

ForumReader::ForumReader()
{
    // NOTE: for testing purposes
    //BankiRuForum::ForumPageParser fpp;
    //fpp.getPagePosts( QUrl::fromLocalFile("C:/Projects/__DATA/test_page_30.html"), m_userPosts );
}

ForumReader::~ForumReader()
{
}

ForumReader::ForumReader(BankiRuForum::UserPosts userPosts, QObject *parent) : QObject(parent), m_userPosts(userPosts), m_pageCount(0)
{
}

QUrl ForumReader::convertToUrl(QString urlStr) const
{
    return QUrl(urlStr);
}

bool ForumReader::parseForumPage(QString forumPageRawHtml)
{
    m_userPosts.clear();
    m_pageCount = 0;

    BankiRuForum::ForumPageParser fpp;
    int result = fpp.getPagePosts(forumPageRawHtml, m_userPosts, m_pageCount);
    return (result == 0);
}

int ForumReader::pageCount() const
{
    return m_pageCount;
}

int ForumReader::postCount() const
{
    return m_userPosts.size();
}

QString ForumReader::postAuthor(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_userName;
}

QString ForumReader::postAvatarUrl(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    if (m_userPosts[index].first.m_userAvatar.isNull()) return QString();
    return m_userPosts[index].first.m_userAvatar->m_url;
}

int ForumReader::postAvatarWidth(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    if (m_userPosts[index].first.m_userAvatar.isNull()) return -1;
    return m_userPosts[index].first.m_userAvatar->m_width;
}

int ForumReader::postAvatarHeight(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    if (m_userPosts[index].first.m_userAvatar.isNull()) return -1;
    return m_userPosts[index].first.m_userAvatar->m_height;
}

int ForumReader::postAvatarMaxWidth() const
{
    int maxWidth = 100;
    for(int i = 0; i < m_userPosts.size(); ++i)
    {
        if (m_userPosts[i].first.m_userAvatar.isNull()) continue;
        int width = m_userPosts[i].first.m_userAvatar->m_width;
        if(width > maxWidth) maxWidth = width;
    }
    return maxWidth;
}

QDateTime ForumReader::postDateTime(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_date;
}

QString ForumReader::postText(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());
    if (m_userPosts[index].second.m_data.empty()) return "";

    QString qmlStr;
    qmlStr =
            "import QtMultimedia 5.6;\n"
            "import QtQuick 2.6;\n"
            "import QtQuick.Window 2.2;\n"
            "import QtQuick.Controls 1.5;\n"
            "import QtQuick.Dialogs 1.2;\n\n";

    BankiRuForum::IPostObjectList::const_iterator iObj = m_userPosts[index].second.m_data.begin();
    int validItemsCount = 0;
    for (; iObj != m_userPosts[index].second.m_data.end(); ++iObj)
    {
        if (!(*iObj)->isValid() || (*iObj)->getQmlString().isEmpty()) continue;

        validItemsCount++;
    }

    if (validItemsCount == 0) return QString();
    if (validItemsCount == 1) return qmlStr + m_userPosts[index].second.m_data[0]->getQmlString();

    iObj = m_userPosts[index].second.m_data.begin();
    qmlStr += "Column {\n";
    for (; iObj != m_userPosts[index].second.m_data.end(); ++iObj)
    {
        qmlStr += (*iObj)->getQmlString();
        qmlStr = qmlStr.trimmed();
    }
    qmlStr += "}\n";
    return qmlStr;
}

QString ForumReader::postLastEdit(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_lastEdit;
}

int ForumReader::postLikeCount(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_likeCounter;
}

int ForumReader::postAuthorPostCount(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_postCount;
}

QDate ForumReader::postAuthorRegistrationDate(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_registrationDate;
}

int ForumReader::postAuthorReputation(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_reputation;
}

QString ForumReader::postAuthorCity(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_city;
}

QString ForumReader::postAuthorSignature(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_userSignature;
}

QString ForumReader::postFooterQml() const
{
    const QString qmlStr =
            "import QtQuick 2.6;\n"
            "Column {\n"
            "   Text {\n"
            "       id: txtLastEdit\n"
            "       visible: model.lastEdit !== \"\"\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "\n"
            "       color: \"lightslategrey\"\n"
            "       font.italic: true\n"
            "       font.pixelSize: sp(2)\n"
            "\n"
            "       renderType: Text.NativeRendering\n"
            "\n"
            "       text: model.postLastEdit\n"
            "       textFormat: Text.RichText\n"
            "       onLinkActivated: Qt.openUrlExternally(link)\n"
            "\n"
            "       clip: false\n"
            "       elide: Text.ElideRight\n"
            "       wrapMode: Text.WordWrap\n"
            "   }\n"
            "\n"
            "   Rectangle {\n"
            "       visible: model.authorSignature !== \"\"\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "       height: dp(1)\n"
            "       border.width: dp(0)\n"
            "       color: \"lightslategrey\"\n"
            "   }\n"
            "\n"
            "   Text {\n"
            "       id: txtPostAuthorSignature\n"
            "       visible: model.authorSignature !== \"\"\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "\n"
            "       color: \"lightslategrey\"\n"
            "       font.italic: true\n"
            "       font.pixelSize: sp(2)\n"
            "\n"
            "       renderType: Text.NativeRendering\n"
            "\n"
            "       text: model.authorSignature\n"
            "       textFormat: Text.RichText\n"
            "       onLinkActivated: Qt.openUrlExternally(link)\n"
            "\n"
            "       clip: false\n"
            "       elide: Text.ElideRight\n"
            "       wrapMode: Text.WordWrap\n"
            "   }\n"
            "\n"
            "   Rectangle {\n"
            "       visible: model.postLikeCount > 0\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "       height: dp(1)\n"
            "       border.width: dp(0)\n"
            "       color: \"lightslategrey\"\n"
            "   }\n"
            "\n"
            "   Text {\n"
            "       id: txtPostLikeCounter\n"
            "       visible: model.postLikeCount > 0\n"
            "       width: rctItem.width - parent.rightPadding - parent.leftPadding\n"
            "       color: \"lightslategrey\"\n"
            "\n"
            "       font.bold: true\n"
            "       font.pixelSize: sp(2)\n"
            "       text: model.postLikeCount + \" like(s)\"\n"
            "   }\n"
            "}\n";
            return qmlStr;
}


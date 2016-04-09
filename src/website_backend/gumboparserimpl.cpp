#include "gumboparserimpl.h"

// FIXME: temp ban support:
// <div class = "forum-ban-info">
//		<div class = "fix-b-card_user" title = "Временный бан">
//			<i class = "b-icon r_card"><i></i></i>
//			<span class = "date-from"></span>
//			<span class = "date-to">до 19.03.2016</span>
//		</div>
// </div>

#define RUBANOK_DEBUG

using namespace BankiRuForum;

namespace {
static const QString g_bankiRuHost = "http://www.banki.ru";

/*static QString gumboElementTypeToString(GumboNodeType gnt)
{
    switch (gnt)
    {
    case GUMBO_NODE_DOCUMENT: return QString("document");
    case GUMBO_NODE_ELEMENT: return QString("element");
    case GUMBO_NODE_TEXT: return QString("text");
    case GUMBO_NODE_CDATA: return QString("cdata");
    case GUMBO_NODE_COMMENT: return QString("comment");
    case GUMBO_NODE_WHITESPACE: return QString("whitespace");
    case GUMBO_NODE_TEMPLATE: return QString("template");
    default: Q_ASSERT(0); return QString();
    }
}*/

static QString gumboElementAttributeValue(GumboNode* node, const char* attrName)
{
    Q_CHECK_PTR(node);
    Q_ASSERT(node->type == GUMBO_NODE_ELEMENT);

    GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, attrName);
    if (!attr) return QString();

    return QString(attr->value);
}

static QString gumboElementClass(GumboNode* node)
{
    return gumboElementAttributeValue(node, "class");
}

static QString gumboElementId(GumboNode* node)
{
    return gumboElementAttributeValue(node, "id");
}

static int gumboChildElementCount(GumboNode* node)
{
    Q_CHECK_PTR(node);
    Q_ASSERT(node->type == GUMBO_NODE_ELEMENT);

    int elementCount = 0;
    GumboVector* nodeChildren = &node->v.element.children;
    for (unsigned int i = 0; i < nodeChildren->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(nodeChildren->data[i]);
        if (childNode->type == GUMBO_NODE_ELEMENT) elementCount++;
    }
    return elementCount;
}

static int gumboChildTextNodeCount(GumboNode* node)
{
    Q_CHECK_PTR(node);
    Q_ASSERT(node->type == GUMBO_NODE_ELEMENT);

    int textNodeCount = 0;
    GumboVector* nodeChildren = &node->v.element.children;
    for (unsigned int i = 0; i < nodeChildren->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(nodeChildren->data[i]);
        if (childNode->type == GUMBO_NODE_TEXT) textNodeCount++;
    }
    return textNodeCount;
}

static QString gumboChildTextNodeValue(GumboNode* node)
{
    Q_CHECK_PTR(node);
    Q_ASSERT(node->type == GUMBO_NODE_ELEMENT);

    QString value;

    GumboVector* nodeChildren = &node->v.element.children;
    for (unsigned int i = 0; i < nodeChildren->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(nodeChildren->data[i]);
        if (childNode->type != GUMBO_NODE_TEXT) continue;

        value += QString::fromUtf8(childNode->v.text.text);
    }

    return value;
}

// Non-recursive(!) search for the first child node with specified tag, from specified position (index of child)
static GumboNode* gumboChildNodeByName(GumboNode* node, GumboTag childTag, unsigned int& startPos)
{
    Q_CHECK_PTR(node);
    Q_ASSERT(node->type == GUMBO_NODE_ELEMENT);

    GumboVector* childNodes = &node->v.element.children;
    Q_CHECK_PTR(childNodes);
    Q_ASSERT(startPos < childNodes->length);

    for (unsigned int i = 0, idxElement = 0; i < childNodes->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(childNodes->data[i]);
        Q_CHECK_PTR(childNode);
        if (childNode->type != GUMBO_NODE_ELEMENT) continue;
//		if (childNode->v.element.tag == childTag) idxElement++;

        if ((childNode->v.element.tag == childTag) && (idxElement >= startPos))
        {
            startPos = idxElement;
            return childNode;
        }
        else idxElement++;
    }

    return nullptr;
}

// Non-recursive(!) search for the first child node with specified class name and tag (div by default)
static GumboNode* gumboChildNodeByClass(GumboNode* node, QString className, GumboTag childTag = GUMBO_TAG_DIV)
{
    Q_CHECK_PTR(node);
    Q_ASSERT(node->type == GUMBO_NODE_ELEMENT);

    GumboVector* childNodes = &node->v.element.children;
    Q_CHECK_PTR(childNodes);

    for (unsigned int i = 0; i < childNodes->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(childNodes->data[i]);
        Q_CHECK_PTR(childNode);
        if (childNode->type != GUMBO_NODE_ELEMENT) continue;

        if ((childNode->v.element.tag == childTag) && (QString::compare(gumboElementClass(childNode), className, Qt::CaseInsensitive) == 0))
        {
            return childNode;
        }
    }

    return nullptr;
}

}

void ForumPageParser::printTagsRecursively(GumboNode *node, int &level)
{
    if (node->type != GUMBO_NODE_ELEMENT) return;

    QString levelStr;
    levelStr.fill('-', level);
    GumboAttribute* idAttr = gumbo_get_attribute(&node->v.element.attributes, "id");
    QString idAttrValue;
    if (idAttr) idAttrValue = QString(", id = ") + idAttr->value;
    GumboAttribute* classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
    QString classAttrValue;
    if (classAttr) classAttrValue = QString(", class = ") + classAttr->value;
    qDebug().noquote() << levelStr << gumbo_normalized_tagname(node->v.element.tag) << idAttrValue << classAttrValue;

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(children->data[i]);
        if (childNode->type != GUMBO_NODE_ELEMENT)continue;

        level += 4;
        printTagsRecursively(childNode, level);
        level -= 4;
    }
}

void ForumPageParser::findMsdivNodesRecursively(GumboNode *node, QVector<GumboNode *> &msdivNodes)
{
    if (node->type != GUMBO_NODE_ELEMENT) return;

    bool isDivTag = (node->v.element.tag == GUMBO_TAG_DIV);
    GumboAttribute* idAttr = gumbo_get_attribute(&node->v.element.attributes, "id");
    if (isDivTag && idAttr)
    {
        QString idAttrValue = QString::fromUtf8(idAttr->value);
        int msdivIndex = idAttrValue.indexOf("msdiv");
        if (msdivIndex != -1)
        {
            // TODO: this number should be stored now, i.e. the id currently extracted twice
            QString msdivNumberStr = idAttrValue.mid(5);
            bool msdivNumberCorrect = false;
            /*int msdivNumber =*/ msdivNumberStr.toInt(&msdivNumberCorrect);
            Q_ASSERT(msdivNumberCorrect);
            msdivNodes.append(node);
        }
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(children->data[i]);
        if (childNode->type != GUMBO_NODE_ELEMENT) continue;

        findMsdivNodesRecursively(childNode, msdivNodes);
    }
}

ForumPageParser::UserBaseInfo ForumPageParser::getUserBaseInfo(GumboNode *userInfoNode)
{
    UserBaseInfo result;

    GumboNode* userNameNode = gumboChildNodeByClass(userInfoNode, "forum-user-name", GUMBO_TAG_DIV);
    Q_CHECK_PTR(userNameNode);
    Q_ASSERT(userNameNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(userNameNode) == 1);
    Q_ASSERT(gumboElementClass(userNameNode) == "forum-user-name");

    unsigned int idxUserProfileNode = 0;
    GumboNode* userProfileNode = gumboChildNodeByName(userNameNode, GUMBO_TAG_A, idxUserProfileNode);
    Q_CHECK_PTR(userProfileNode);
    Q_ASSERT(userProfileNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(userProfileNode) == 1);

    // Read the user profile URL
    QString userProfileUrl = gumboElementAttributeValue(userProfileNode, "href");
    userProfileUrl.prepend(g_bankiRuHost);
    Q_ASSERT(QUrl(userProfileUrl).isValid());

    // Read the user ID
    QString userIdStr = QUrl(userProfileUrl).query();
    QStringList userIdList = userIdStr.split('=', QString::SkipEmptyParts);
    Q_ASSERT(userIdList.size() == 2);
    Q_ASSERT(userIdList[0] == "UID");
    QString userIdValueStr = userIdList[1];
    bool userIdOk = false;
    int userId = userIdValueStr.toInt(&userIdOk);
    Q_ASSERT(userIdOk);

    // Read the user name
    QString userName = gumboElementAttributeValue(userProfileNode, "title");

    result.m_id = userId;
    result.m_name = userName;
    result.m_profileUrl = QUrl(userProfileUrl);
    return result;
}

ForumPageParser::UserAdditionalInfo ForumPageParser::getUserAdditionalInfo(GumboNode *userInfoNode)
{
    UserAdditionalInfo result;

    GumboNode* userAdditionalNode = gumboChildNodeByClass(userInfoNode, "forum-user-additional", GUMBO_TAG_DIV);
    Q_CHECK_PTR(userAdditionalNode);
    Q_ASSERT(userAdditionalNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(userAdditionalNode) >= 3 && gumboChildElementCount(userAdditionalNode) <= 5);
    Q_ASSERT(gumboElementClass(userAdditionalNode) == "forum-user-additional");

    // Read the all message URL and the post count
    // span - span - noindex - a - text
    unsigned int idxSpanNode1 = 0;
    GumboNode* spanNode1 = gumboChildNodeByName(userAdditionalNode, GUMBO_TAG_SPAN, idxSpanNode1);
    Q_CHECK_PTR(spanNode1);
    Q_ASSERT(spanNode1->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(spanNode1) == 1);

    unsigned int idxSpan2 = 0;
    GumboNode* spanNode2 = gumboChildNodeByName(spanNode1, GUMBO_TAG_SPAN, idxSpan2);
    Q_CHECK_PTR(spanNode2);
    Q_ASSERT(spanNode2->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(spanNode2) == 1);

    unsigned int idxNoIndex = 0;
    GumboNode* noindexNode = gumboChildNodeByName(spanNode2, GUMBO_TAG_UNKNOWN, idxNoIndex);
    Q_CHECK_PTR(noindexNode);
    Q_ASSERT(noindexNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(noindexNode) == 1);

    unsigned int idxUserPostLinks = 0;
    GumboNode* userPostLinksNode = gumboChildNodeByName(noindexNode, GUMBO_TAG_A, idxUserPostLinks);
    Q_CHECK_PTR(userPostLinksNode);
    Q_ASSERT(userPostLinksNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(userPostLinksNode) == 0);

    QString userAllPosts = gumboElementAttributeValue(userPostLinksNode, "href");
    userAllPosts.prepend(g_bankiRuHost);
    Q_ASSERT(QUrl(userAllPosts).isValid());

    Q_ASSERT(gumboChildTextNodeCount(userPostLinksNode) == 1);
    QString postCountStr = gumboChildTextNodeValue(userPostLinksNode);
    bool postCountOk = false;
    int postCount = postCountStr.toInt(&postCountOk);
    Q_ASSERT(postCountOk);

    // Read the registration date
    unsigned int idxRegistrationDate = 1;
    GumboNode* registrationDateNode = gumboChildNodeByName(userAdditionalNode, GUMBO_TAG_SPAN, idxRegistrationDate);
    Q_CHECK_PTR(registrationDateNode);
    Q_ASSERT(registrationDateNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(registrationDateNode) == 1);

    idxSpanNode1 = 0;
    spanNode1 = gumboChildNodeByName(registrationDateNode, GUMBO_TAG_SPAN, idxSpanNode1);
    Q_CHECK_PTR(spanNode1);
    Q_ASSERT(spanNode1->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(spanNode1) == 0);
    Q_ASSERT(gumboChildTextNodeCount(spanNode1) == 1);
    QString registrationDateStr = gumboChildTextNodeValue(spanNode1);
    QDate registrationDate = QDate::fromString(registrationDateStr, "dd.MM.yyyy");
    Q_ASSERT(registrationDate.isValid());

    // Read the reputation value
    unsigned int idxReputation = 2;
    GumboNode* reputationNode = gumboChildNodeByName(userAdditionalNode, GUMBO_TAG_SPAN, idxReputation);
    Q_CHECK_PTR(reputationNode);
    Q_ASSERT(reputationNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(reputationNode) == 1);

    idxSpanNode1 = 0;
    spanNode1 = gumboChildNodeByName(reputationNode, GUMBO_TAG_SPAN, idxSpanNode1);
    Q_CHECK_PTR(spanNode1);
    Q_ASSERT(spanNode1->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(spanNode1) == 1);

    unsigned int idxUserReputationHistory = 0;
    GumboNode* userReputationHistoryNode = gumboChildNodeByName(spanNode1, GUMBO_TAG_A, idxUserReputationHistory);
    Q_CHECK_PTR(userReputationHistoryNode);
    Q_ASSERT(userReputationHistoryNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(userReputationHistoryNode) == 0);
    Q_ASSERT(gumboChildTextNodeCount(userReputationHistoryNode) == 1);
    QString reputationStr = gumboChildTextNodeValue(userReputationHistoryNode);
    bool reputationOk = false;
    int reputation = reputationStr.toInt(&reputationOk);
    Q_ASSERT(reputationOk);

    // NOTE: city is optional field, instead the rest of others
    QString cityStr;
    unsigned int idxUserCity = 3;
    GumboNode* userCityNode = gumboChildNodeByName(userAdditionalNode, GUMBO_TAG_SPAN, idxUserCity);
    if (userCityNode)
    {
        Q_ASSERT(userCityNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(userCityNode) == 1);

        idxSpanNode1 = 0;
        spanNode1 = gumboChildNodeByName(userCityNode, GUMBO_TAG_SPAN, idxSpanNode1);

        Q_CHECK_PTR(spanNode1);
        Q_ASSERT(spanNode1->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(spanNode1) == 0);
        Q_ASSERT(gumboChildTextNodeCount(spanNode1) == 1);
        cityStr = gumboChildTextNodeValue(spanNode1);
    }

#ifdef  RUBANOK_DEBUG
    qDebug() << "post count: " + QString::number(postCount) + ", reputation: " + QString::number(reputation) + ", city: " + cityStr
                + ", url: " + userAllPosts + ", date: " + registrationDate.toString(Qt::SystemLocaleShortDate);
#endif // RUBANOK_DEBUG

    result.m_allPostsUrl = QUrl(userAllPosts);
    result.m_postCount = postCount;
    result.m_registrationDate = registrationDate;
    result.m_reputation = reputation;
    result.m_city = cityStr;

    return result;
}

Image ForumPageParser::getUserAvatar(GumboNode *userInfoNode)
{
    Image result;

    GumboNode* userAvatarNode = gumboChildNodeByClass(userInfoNode, "forum-user-avatar", GUMBO_TAG_DIV);
    if(!userAvatarNode) userAvatarNode = gumboChildNodeByClass(userInfoNode, "forum-user-register-avatar", GUMBO_TAG_DIV);
    Q_CHECK_PTR(userAvatarNode);
    Q_ASSERT(userAvatarNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(userAvatarNode) == 1);
    Q_ASSERT(gumboElementClass(userAvatarNode) == "forum-user-avatar" || gumboElementClass(userAvatarNode) == "forum-user-register-avatar");
    if (gumboElementClass(userAvatarNode) == "forum-user-avatar")
    {
        // div - noindex - a - img
        unsigned int idxNoIndex = 0;
        GumboNode* noIndexNode = gumboChildNodeByName(userAvatarNode, GUMBO_TAG_UNKNOWN, idxNoIndex);
        Q_CHECK_PTR(noIndexNode);
        Q_ASSERT(noIndexNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(noIndexNode) == 1);

        unsigned int idxProfileLink = 0;
        GumboNode* profileLinkNode = gumboChildNodeByName(noIndexNode, GUMBO_TAG_A, idxProfileLink);
        Q_CHECK_PTR(profileLinkNode);
        Q_ASSERT(profileLinkNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(profileLinkNode) == 1);

        unsigned int idxImage = 0;
        GumboNode* imageNode = gumboChildNodeByName(profileLinkNode, GUMBO_TAG_IMG, idxImage);
        Q_CHECK_PTR(imageNode);
        Q_ASSERT(imageNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(imageNode) == 0);

        // Get image URL
        QString imageSrcStr = gumboElementAttributeValue(imageNode, "src");
        imageSrcStr.prepend(g_bankiRuHost);
        Q_ASSERT(QUrl(imageSrcStr).isValid());

        // Get image width
        QString imageWidthStr = gumboElementAttributeValue(imageNode, "width");
        bool imageWidthOk = false;
        int imageWidth = imageWidthStr.toInt(&imageWidthOk);
        Q_ASSERT(imageWidthOk);

        // Get image height
        QString imageHeightStr = gumboElementAttributeValue(imageNode, "height");
        bool imageHeightOk = false;
        int imageHeight = imageHeightStr.toInt(&imageHeightOk);
        Q_ASSERT(imageHeightOk);

        result.m_url = QUrl(imageSrcStr);
        result.m_width = imageWidth;
        result.m_height = imageHeight;
    }

    return result;
}

User ForumPageParser::getPostUser(GumboNode *trNode1)
{
    User userInfo;

    GumboNode* userNode = gumboChildNodeByClass(trNode1, "forum-cell-user", GUMBO_TAG_TD);
    Q_CHECK_PTR(userNode);
    Q_ASSERT(userNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(userNode) == 1);
    Q_ASSERT(gumboElementClass(userNode) == "forum-cell-user");

    GumboNode* userInfoNode = gumboChildNodeByClass(userNode, "forum-user-info", GUMBO_TAG_DIV);
    if(!userInfoNode) userInfoNode = gumboChildNodeByClass(userNode, "forum-user-info w-el-dropDown", GUMBO_TAG_DIV);
    Q_CHECK_PTR(userInfoNode);
    Q_ASSERT(userInfoNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(userInfoNode) >= 4);
    Q_ASSERT(gumboElementClass(userInfoNode) == "forum-user-info w-el-dropDown" || gumboElementClass(userInfoNode) == "forum-user-info");

    // Get user base info: id, name, profile URL
    UserBaseInfo ubi = getUserBaseInfo(userInfoNode);

    // Get user additional info: post count, register date, reputation points
    UserAdditionalInfo uai = getUserAdditionalInfo(userInfoNode);

    // Get user avatar image
    Image userAvatar = getUserAvatar(userInfoNode);

#ifdef  RUBANOK_DEBUG
    qDebug() << QString::number(ubi.m_id) + ", " + ubi.m_name + ", " + ubi.m_profileUrl.toDisplayString() + ", "
                + userAvatar.m_url.toString() + ": " + QString::number(userAvatar.m_width) + " x " + QString::number(userAvatar.m_height);
#endif // RUBANOK_DEBUG

    // Base info
    userInfo.m_userId = ubi.m_id;
    userInfo.m_userName = ubi.m_name;
    userInfo.m_userProfileUrl = ubi.m_profileUrl;

    // Avatar image
    userInfo.m_userAvatar = userAvatar;

    // Additional info
    userInfo.m_allPostsUrl = uai.m_allPostsUrl;
    userInfo.m_postCount = uai.m_postCount;
    userInfo.m_registrationDate = uai.m_registrationDate;
    userInfo.m_reputation = uai.m_reputation;
    userInfo.m_city = uai.m_city;

    return userInfo;
}

Post ForumPageParser::getPostValue(GumboNode *trNode1)
{
    Post postInfo;

    GumboNode* postNode = gumboChildNodeByClass(trNode1, "forum-cell-post", GUMBO_TAG_TD);
    Q_CHECK_PTR(postNode);
    Q_ASSERT(postNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(postNode) == 2);
    Q_ASSERT(gumboElementClass(postNode) == "forum-cell-post");

    // 1) <div class="forum-post-date">
    GumboNode* postDateNode = gumboChildNodeByClass(postNode, "forum-post-date", GUMBO_TAG_DIV);
    Q_ASSERT(postDateNode);
    Q_ASSERT(postDateNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(postDateNode) <= 3);
    Q_ASSERT(gumboElementClass(postDateNode) == "forum-post-date");

    GumboNode* spanNode = gumboChildNodeByClass(postDateNode, "", GUMBO_TAG_SPAN /*, idxSpanNode*/);
    Q_ASSERT(spanNode);
    Q_ASSERT(spanNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(spanNode) == 0);
    Q_ASSERT(gumboChildTextNodeCount(spanNode) == 1);
    QString postDateStr = gumboChildTextNodeValue(spanNode);
    QDateTime postDate = QDateTime::fromString(postDateStr, "dd.MM.yyyy hh:mm");
    Q_ASSERT(postDate.isValid());

    // 2) <div class="forum-post-entry" style="font-size: 14px;">
    GumboNode* postEntryNode = gumboChildNodeByClass(postNode, "forum-post-entry", GUMBO_TAG_DIV);
    Q_ASSERT(postEntryNode);
    Q_ASSERT(postEntryNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(postEntryNode) <= 3);
    Q_ASSERT(gumboElementClass(postEntryNode) == "forum-post-entry");

    GumboNode* postTextNode = gumboChildNodeByClass(postEntryNode, "forum-post-text", GUMBO_TAG_DIV);
    Q_ASSERT(postTextNode);
    Q_ASSERT(postTextNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(postTextNode) > 0 || gumboChildTextNodeCount(postTextNode) > 0);
    Q_ASSERT(gumboElementClass(postTextNode) == "forum-post-text");

    // Read message id
    QString messageIdStr = gumboElementId(postTextNode);
    Q_ASSERT(messageIdStr.startsWith("message_text_", Qt::CaseInsensitive));
    QString idStr = messageIdStr.remove("message_text_", Qt::CaseInsensitive);
    bool idOk = false;
    int id = idStr.toInt(&idOk);
    Q_ASSERT(idOk);

    // Read post message text as raw HTML
    unsigned int messageTextBegin = postTextNode->v.element.original_tag.length;
    unsigned int messageTextLength = postTextNode->v.element.end_pos.offset - postTextNode->v.element.start_pos.offset;
    QString fullPostHtml = QString::fromUtf8(postTextNode->v.element.original_tag.data);
    int idxEndDiv = fullPostHtml.indexOf("</div>", 0, Qt::CaseInsensitive);
    messageTextLength -= idxEndDiv;
    QString messageText = fullPostHtml.mid(messageTextBegin, idxEndDiv - messageTextBegin);
    messageText = messageText.trimmed();

    // NOTE: adopt post quote style to poor Qt-supported HTML subset
    messageText = messageText.replace("\r", "");
    messageText = messageText.replace("<br />", "\n");
/*    messageText = messageText.replace("\n", "<br>");
    messageText = messageText.replace("class=\"forum-quote\"", "border='1'");
    // src='/bitrix/images/forum/smile/ag.gif' --> src='http://www.banki.ru/bitrix/images/forum/smile/ag.gif'
    messageText = messageText.replace("/bitrix/", g_bankiRuHost + "/bitrix/");
    messageText = messageText.replace("<th>", "<th bgcolor='darkgrey' align='left' valign='middle' style='white-space: normal'>");*/

    // FIXME: parse quotes
    const QString tableOpenTag = "<table";
    const QString tableCloseTag = "</table>";
    while(true)
    {
        int idxQuoteBegin = messageText.indexOf(tableOpenTag);
        int idxQuoteEnd   = messageText.indexOf(tableCloseTag);
        if( idxQuoteBegin == -1 ) break;

        Q_ASSERT( idxQuoteBegin != -1 && idxQuoteEnd != -1 );

        // Extract quote text
        // FIXME: just remove it to test
        messageText = messageText.remove(idxQuoteBegin, idxQuoteEnd - idxQuoteBegin + tableCloseTag.size());
        qDebug() << messageText;
    }

    // Read user signature
    QString userSignatureStr;
    GumboNode* postSignatureNode = gumboChildNodeByClass(postEntryNode, "forum-user-signature");
    if (postSignatureNode)
    {
        Q_ASSERT(postSignatureNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(postSignatureNode) == 2);
        Q_ASSERT(gumboElementClass(postSignatureNode) == "forum-user-signature");

        unsigned int idxSpanNode = 0;
        spanNode = gumboChildNodeByName(postSignatureNode, GUMBO_TAG_SPAN, idxSpanNode);
        Q_ASSERT(spanNode);
        Q_ASSERT(spanNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(spanNode) <= 2);
        Q_ASSERT(gumboChildTextNodeCount(spanNode) <= 2);
        userSignatureStr = gumboChildTextNodeValue(spanNode);
    }

#ifdef RUBANOK_DEBUG
    qDebug() << "Post:";
    qDebug() << "	ID: " << id;
    qDebug() << "	Text: " << messageText;
    if (!userSignatureStr.isEmpty())
        qDebug() << "   User signature: " << userSignatureStr;
    qDebug() << "	Date: " << postDate;
#endif //  RUBANOK_DEBUG

    postInfo.m_id = id;
//  postInfo.m_postNumber = -1;
    postInfo.m_likeCounter = -1;	// NOTE: will be filled later
    postInfo.m_text = messageText;
//  postInfo.m_style = "";
    postInfo.m_userSignature = userSignatureStr;
    postInfo.m_date = postDate;
//  postInfo.m_permalink = "";

    return postInfo;
}

int ForumPageParser::getLikeCounterValue(GumboNode *trNode2)
{
    // tr2:
    GumboNode* contactsNode = gumboChildNodeByClass(trNode2, "forum-cell-contact", GUMBO_TAG_TD);
    Q_ASSERT(gumboElementClass(contactsNode) == "forum-cell-contact");

    GumboNode* actionsNode = gumboChildNodeByClass(trNode2, "forum-cell-actions", GUMBO_TAG_TD);
    Q_CHECK_PTR(actionsNode);
    Q_ASSERT(actionsNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(actionsNode) == 1);
    Q_ASSERT(gumboElementClass(actionsNode) == "forum-cell-actions");

    // Get the "like" count
    // NOTE: it is type on the site, not my own
    GumboNode* actionLinksNode = gumboChildNodeByClass(actionsNode, "conainer-action-links", GUMBO_TAG_DIV);
    Q_CHECK_PTR(actionLinksNode);
    Q_ASSERT(actionLinksNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(actionLinksNode) == 2);
    Q_ASSERT(gumboElementClass(actionLinksNode) == "conainer-action-links");

    GumboNode* floatLeftNode = gumboChildNodeByClass(actionLinksNode, "float-left", GUMBO_TAG_DIV);
    Q_CHECK_PTR(floatLeftNode);
    Q_ASSERT(floatLeftNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(floatLeftNode) == 1);
    Q_ASSERT(gumboElementClass(floatLeftNode) == "float-left");

    GumboNode* likeNode = gumboChildNodeByClass(floatLeftNode, "like", GUMBO_TAG_DIV);
    Q_CHECK_PTR(likeNode);
    Q_ASSERT(likeNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(likeNode) == 1);
    Q_ASSERT(gumboElementClass(likeNode) == "like");

    GumboNode* likeWidgetNode = gumboChildNodeByClass(likeNode, "like__widget", GUMBO_TAG_DIV);
    Q_CHECK_PTR(likeWidgetNode);
    Q_ASSERT(likeWidgetNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(likeWidgetNode) >= 2 && gumboChildElementCount(likeWidgetNode) <= 5);
    Q_ASSERT(gumboElementClass(likeWidgetNode) == "like__widget");

    GumboNode* likeCounterNode = gumboChildNodeByClass(likeWidgetNode, "like__counter", GUMBO_TAG_SPAN);
    Q_CHECK_PTR(likeCounterNode);
    Q_ASSERT(likeCounterNode->type == GUMBO_NODE_ELEMENT);
    Q_ASSERT(gumboChildElementCount(likeCounterNode) == 0);
    Q_ASSERT(gumboElementClass(likeCounterNode) == "like__counter");
    Q_ASSERT(gumboChildTextNodeCount(likeCounterNode) == 1);

    QString likeCounterStr = gumboChildTextNodeValue(likeCounterNode);
    bool likeNumberOk = false;
    int likeCount = likeCounterStr.toInt(&likeNumberOk);
    Q_ASSERT(likeNumberOk);
    return likeCount;
}

int ForumPageParser::getPostId(GumboNode *msdivNode)
{
    QString messageIdStr = gumboElementId(msdivNode);
    int msdivIndex = messageIdStr.indexOf("msdiv");
    Q_ASSERT(msdivIndex != -1);
    QString msdivNumberStr = messageIdStr.mid(5);
    bool messageIdOk = false;
    int messageId = msdivNumberStr.toInt(&messageIdOk);
    Q_ASSERT(messageIdOk);
    return messageId;
}

void ForumPageParser::fillPostList(GumboNode *node, UserPosts& posts)
{
    // XPath: *[@id="msdiv4453758"]

    // Find div nodes with msdiv id
    QVector<GumboNode*> msdivNodes;
    findMsdivNodesRecursively(node, msdivNodes);

    // table --> tbody --> tr | tr --> td | td
    for (int i = 0; i < msdivNodes.size(); ++i)
    {
        GumboNode* msdivNode = msdivNodes[i];

        Q_CHECK_PTR(msdivNode);
        Q_ASSERT(msdivNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(msdivNode) == 1);

        // one table tag
        unsigned int idxTable = 0;
        GumboNode* tableNode = gumboChildNodeByName(msdivNode, GUMBO_TAG_TABLE, idxTable);
        Q_CHECK_PTR(tableNode);
        Q_ASSERT(tableNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(tableNode) == 1);
        Q_ASSERT(idxTable == 0);

        // one tbody tag
        unsigned int idxTbody = 0;
        GumboNode* tbodyNode = gumboChildNodeByName(tableNode, GUMBO_TAG_TBODY, idxTbody);
        Q_CHECK_PTR(tbodyNode);
        Q_ASSERT(tbodyNode->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(tbodyNode) == 2);
        Q_ASSERT(idxTbody == 0);

        // two tr tags
        unsigned int idxTr1 = 0;
        GumboNode* trNode1 = gumboChildNodeByName(tbodyNode, GUMBO_TAG_TR, idxTr1);
        unsigned int idxTr2 = idxTr1 + 1;
        GumboNode* trNode2 = gumboChildNodeByName(tbodyNode, GUMBO_TAG_TR, idxTr2);
        Q_CHECK_PTR(trNode1);
        Q_CHECK_PTR(trNode2);
        Q_ASSERT(trNode1->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(trNode1) == 2);
        Q_ASSERT(trNode2->type == GUMBO_NODE_ELEMENT);
        Q_ASSERT(gumboChildElementCount(trNode2) == 2);
        Q_ASSERT(idxTr1 == 0);
        Q_ASSERT(idxTr2 == 1);

        // each tr tag has two child td tags
        // tr1:
        User forumUser = getPostUser(trNode1);
        Post forumPost = getPostValue(trNode1);
        forumPost.m_id = getPostId(msdivNode);

        // Read the like counter value from tr2
        forumPost.m_likeCounter = getLikeCounterValue(trNode2);

        // FIXME: fill other post/user info

        posts.append(qMakePair<User, Post>(forumUser, forumPost));
    }
}

int ForumPageParser::getPagePosts(QUrl webPageUrl, UserPosts& userPosts)
{
    QByteArray htmlFileRawContents;

    // Read HTML file contents
    if(webPageUrl.isLocalFile())
    {
        QFile htmlFile( webPageUrl.toLocalFile() );
        if( !htmlFile.open( QFile::ReadOnly ) )
        {
            qDebug() << "I/O: unable to open HTML file ";
            return 1;
        }
        htmlFileRawContents = htmlFile.readAll();
        htmlFile.close();
    }
    else
    {
        // FIXME: implement, use FileDownloader
        Q_ASSERT(0);
    }

    // First determine HTML page encoding
    QTextCodec* htmlCodec = QTextCodec::codecForHtml(htmlFileRawContents);
#ifdef RUBANOK_DEBUG
    qDebug() << "ru.banki.reader: HTML encoding/charset is" << htmlCodec->name();
#endif

    // Convert to UTF-8: Gumbo library understands only this encoding
    QString htmlFileString = htmlCodec->toUnicode(htmlFileRawContents);
    QByteArray htmlFileUtf8Contents = htmlFileString.toUtf8();

    // Parse web page contents
    GumboOutput* output = gumbo_parse(htmlFileUtf8Contents.constData());
    fillPostList(output->root, userPosts);
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // TODO: implement error handling with different return code
    return 0;
}

int ForumPageParser::getPagePosts(QString rawData, UserPosts &userPosts)
{
    // First determine HTML page encoding
    QTextCodec* htmlCodec = QTextCodec::codecForHtml(rawData.toLocal8Bit());
#ifdef RUBANOK_DEBUG
    qDebug() << "ru.banki.reader: HTML encoding/charset is" << htmlCodec->name();
#endif

    // Convert to UTF-8: Gumbo library understands only this encoding
#if defined( Q_OS_WIN )
    QString htmlFileString = htmlCodec->toUnicode(rawData.toLocal8Bit());
    QByteArray htmlFileUtf8Contents = htmlFileString.toUtf8();
#elif defined( Q_OS_ANDROID )
    QByteArray htmlFileUtf8Contents = rawData.toUtf8();
#else
    #error "Unsupported platform, needs testing"
#endif

    // Parse web page contents
    GumboOutput* output = gumbo_parse(htmlFileUtf8Contents.constData());
    fillPostList(output->root, userPosts);
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // TODO: implement error handling with different return code
    return 0;
}

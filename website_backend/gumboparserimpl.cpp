#include "gumboparserimpl.h"

// FIXME: temp ban support:
// <div class = "forum-ban-info">
//		<div class = "fix-b-card_user" title = "Временный бан">
//			<i class = "b-icon r_card"><i></i></i>
//			<span class = "date-from"></span>
//			<span class = "date-to">до 19.03.2016</span>
//		</div>
// </div>

using namespace BankiRuForum;

namespace {
static const QString g_bankiRuHost = "http://www.banki.ru";
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

QString ForumPageParser::extractVideoUrl(QString text) const
{
    static const QString VIDEO_URL_START_STR = "'file':'";
    static const QString VIDEO_URL_END_STR = "',";
    static const QString bbTagBegin("[url]");
    static const QString bbTagEnd("[/url]");

    int videoUrlStartIndex = text.indexOf(VIDEO_URL_START_STR);
    if (videoUrlStartIndex == -1) { Q_ASSERT(0); return QString(); }

    int videoUrlEndIndex = text.indexOf(VIDEO_URL_END_STR, videoUrlStartIndex);
    if (videoUrlEndIndex <= videoUrlStartIndex) { Q_ASSERT(0); return QString(); }

    QString videoUrl = text.mid(videoUrlStartIndex + VIDEO_URL_START_STR.size(),
                                videoUrlEndIndex - videoUrlStartIndex - VIDEO_URL_START_STR.size());

    int bbTagBeginIndex = videoUrl.indexOf(bbTagBegin);
    if (bbTagBeginIndex >= 0)
    {
        int bbTagEndIndex = videoUrl.indexOf(bbTagEnd, bbTagBeginIndex);
        if (bbTagEndIndex == -1)
        {
            Q_ASSERT(0);
            return QString();
        }

        videoUrl = videoUrl.mid(bbTagBeginIndex + bbTagBegin.size(), bbTagEndIndex - bbTagBeginIndex - bbTagEnd.size() + 1);
    }

    return videoUrl;
}

void ForumPageParser::printTagsRecursively(QtGumboNodePtr node, int &level)
{
    Q_UNUSED(level);

    if (!node || !node->isValid()) { Q_ASSERT(0); return; }

    if (!node->isElement())
        return;

#ifdef RBR_PRINT_DEBUG_OUTPUT
    QString levelStr;
    levelStr.fill('-', level);

    QString idAttrValue = "<empty id>";
    if (node->hasAttribute("id"))
        idAttrValue = ", id = " + node->getIdAttribute();

    QString classAttrValue = "<empty class>";
    if (node->hasClassAttribute())
        classAttrValue = ", class = " + node->getClassAttribute();

    qDebug().noquote() << levelStr << node->getTagName() << idAttrValue << classAttrValue;

    QtGumboNodes children = node->getChildren();
    for (auto iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        level += 4;
        printTagsRecursively(*iChild, level);
        level -= 4;
    }
#endif
}

void ForumPageParser::findMsdivNodesRecursively(QtGumboNodePtr node, QVector<QtGumboNodePtr> &msdivNodes)
{
    if (!node || !node->isValid()) { Q_ASSERT(0); return; }

    if (!node->isElement())
        return;

    bool isDivTag = (node->getTag() == HtmlTag::DIV);
    if (isDivTag && node->hasIdAttribute())
    {
        QString idAttrValue = node->getIdAttribute();
        int msdivIndex = idAttrValue.indexOf("msdiv");
        if (msdivIndex != -1)
        {
            // TODO: this number should be stored now, i.e. the id currently extracted twice
            QString msdivNumberStr = idAttrValue.mid(5);
            bool msdivNumberCorrect = false;
            /*int msdivNumber =*/ msdivNumberStr.toInt(&msdivNumberCorrect);
            if (!msdivNumberCorrect) { Q_ASSERT(0); return; }
            msdivNodes.append(node);
        }
    }

    QtGumboNodes children = node->getChildren();
    for (auto iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        findMsdivNodesRecursively(*iChild, msdivNodes);
    }
}

namespace {
int parseUserId(QString userProfileUrl)
{
    // Read the user ID
    QString userIdStr = QUrl(userProfileUrl).query();
    QStringList userIdList = userIdStr.split('=', QString::SkipEmptyParts);
    if (userIdList.size() != 2) { Q_ASSERT(0); return -1; }
    if (userIdList[0] != "UID") { Q_ASSERT(0); return -1; }
    QString userIdValueStr = userIdList[1];
    bool userIdOk = false;
    int userId = userIdValueStr.toInt(&userIdOk);
    if (!userIdOk) { Q_ASSERT(0); return -1; }
    return userId;
}
}

ForumPageParser::UserBaseInfo ForumPageParser::getUserBaseInfo(QtGumboNodePtr userInfoNode)
{
    if (!userInfoNode || !userInfoNode->isValid()) { Q_ASSERT(0); return UserBaseInfo(); }

    UserBaseInfo result;

    QtGumboNodePtr userNameNode = userInfoNode->getElementByClass("forum-user-name", HtmlTag::DIV);
    if (!userNameNode || !userNameNode->isValid()) { Q_ASSERT(0); return UserBaseInfo(); }
//    Q_ASSERT(userNameNode->getChildElementCount() == 1); if (userNameNode->getChildElementCount() != 1) return result;

    QSharedPointer<PostHyperlink> userProfileRef = parseHyperlink(userNameNode->getElementByTag({HtmlTag::A, 0}));
    if(userProfileRef.isNull() || !userProfileRef->isValid()) { Q_ASSERT(0); return UserBaseInfo(); }

    result.m_id = parseUserId(userProfileRef->m_urlStr);
    result.m_name = userProfileRef->m_tip;
    result.m_profileUrl = QUrl(userProfileRef->m_urlStr);

    if (!result.m_profileUrl.isValid()) { Q_ASSERT(0); return UserBaseInfo(); }

    return result;
}

ForumPageParser::UserAdditionalInfo ForumPageParser::getUserAdditionalInfo(QtGumboNodePtr userInfoNode)
{
    UserAdditionalInfo result;

    QtGumboNodePtr userAdditionalNode = userInfoNode->getElementByClass("forum-user-additional", HtmlTag::DIV);
    if (!userAdditionalNode || !userAdditionalNode->isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }
//    Q_ASSERT(userAdditionalNode->getChildElementCount() >= 3 && userAdditionalNode->getChildElementCount() <= 5);

    // Read the all message URL and the post count
    QtGumboNodePtr postLinkNode = userAdditionalNode->getElementByTag(
        {{HtmlTag::SPAN, 1}, {HtmlTag::SPAN, 1}, {HtmlTag::UNKNOWN, 0}, {HtmlTag::A, 0}});
    if (!postLinkNode || !postLinkNode->isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }

    QSharedPointer<PostHyperlink> userPostsLinks = parseHyperlink(postLinkNode);
    if (!userPostsLinks || !userPostsLinks->isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }

    QString userAllPosts = userPostsLinks->m_urlStr;
    if (!QUrl(userAllPosts).isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }

    bool postCountOk = false;
    int postCount = userPostsLinks->m_title.toInt(&postCountOk);
    if (!postCountOk) { Q_ASSERT(0); return UserAdditionalInfo(); }

    // Read the registration date
    QtGumboNodePtr regDateNode = userAdditionalNode->getElementByTag({{HtmlTag::SPAN, 3}, {HtmlTag::SPAN, 1}});
    if (!regDateNode || !regDateNode->isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }
    if (regDateNode->getChildElementCount() != 0) { Q_ASSERT(0); return UserAdditionalInfo(); }
    if (regDateNode->getTextChildrenCount() != 1) { Q_ASSERT(0); return UserAdditionalInfo(); }
    QString registrationDateStr = regDateNode->getChildrenInnerText();
    QDate registrationDate = QDate::fromString(registrationDateStr, "dd.MM.yyyy");
    if (!registrationDate.isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }

    // Read the reputation value
    QtGumboNodePtr userReputationRefNode = userAdditionalNode->getElementByTag({{HtmlTag::SPAN, 5}, {HtmlTag::SPAN, 1}, {HtmlTag::A, 0}});
    if (!userReputationRefNode || !userReputationRefNode->isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }

    QSharedPointer<PostHyperlink> userReputationRef = parseHyperlink(userReputationRefNode);
    if (!userReputationRef || !userReputationRef->isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }

    bool reputationOk = false;
    int reputation = userReputationRef->m_title.toInt(&reputationOk);
    if (!reputationOk) { Q_ASSERT(0); return UserAdditionalInfo(); }

    // NOTE: city is optional field, instead the rest of others
    QString cityStr;
    QtGumboNodePtr userCityNode = userAdditionalNode->getElementByTag({HtmlTag::SPAN, 3});
    if (userCityNode && userCityNode->isValid())
    {
        if (userCityNode->getChildElementCount() != 1) { Q_ASSERT(0); return UserAdditionalInfo(); }

        QtGumboNodePtr spanNode1 = userCityNode->getElementByTag({HtmlTag::SPAN, 0});
        if (!spanNode1 || !spanNode1->isValid()) { Q_ASSERT(0); return UserAdditionalInfo(); }
        if (spanNode1->getChildElementCount() != 0) { Q_ASSERT(0); return UserAdditionalInfo(); }
        if (spanNode1->getTextChildrenCount() != 1) { Q_ASSERT(0); return UserAdditionalInfo(); }

        cityStr = spanNode1->getChildrenInnerText();
    }

#ifdef  RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "post count: " + QString::number(postCount) + ", reputation: " + QString::number(reputation) + ", city: " + cityStr
                + ", url: " + userAllPosts + ", date: " + registrationDate.toString(Qt::SystemLocaleShortDate);
#endif // RBR_PRINT_DEBUG_OUTPUT

    result.m_allPostsUrl = QUrl(userAllPosts);
    result.m_postCount = postCount;
    result.m_registrationDate = registrationDate;
    result.m_reputation = reputation;
    result.m_city = cityStr;
    return result;
}

QSharedPointer<PostImage> ForumPageParser::getUserAvatar(QtGumboNodePtr userInfoNode)
{
    if (!userInfoNode || !userInfoNode->isValid()) { Q_ASSERT(0); return nullptr; }

    QSharedPointer<PostImage> result;

    QtGumboNodePtr userAvatarNode = userInfoNode->getElementByClass("forum-user-avatar", HtmlTag::DIV);
    if (!userAvatarNode || !userAvatarNode->isValid())
        userAvatarNode = userInfoNode->getElementByClass("forum-user-register-avatar", HtmlTag::DIV);

    if (!userAvatarNode || !userAvatarNode->isValid()) { Q_ASSERT(0); return nullptr; }
    if (userAvatarNode->getChildElementCount() != 1) { Q_ASSERT(0); return nullptr; }
    if ((userAvatarNode->getClassAttribute() != "forum-user-avatar") && (userAvatarNode->getClassAttribute() != "forum-user-register-avatar"))
    { Q_ASSERT(0); return nullptr; }

    if (userAvatarNode->getClassAttribute() == "forum-user-avatar")
    {
        QtGumboNodePtr imageNode = userAvatarNode->getElementByTag({{HtmlTag::UNKNOWN, 1}, {HtmlTag::A, 1}, {HtmlTag::IMG, 0}});
        if (!imageNode || !imageNode->isValid()) { Q_ASSERT(0); return nullptr; }

        result = parseImage(imageNode);
    }

    return result;
}

User ForumPageParser::getPostUser(QtGumboNodePtr trNode1)
{
    if (!trNode1 || !trNode1->isValid()) { Q_ASSERT(0); return User(); }

    User userInfo;

    QtGumboNodePtr userNode = trNode1->getElementByClass("forum-cell-user", HtmlTag::TD);
    if (!userNode->isValid()) { Q_ASSERT(0); return User(); }
    if (userNode->getChildElementCount() != 1) { Q_ASSERT(0); return User(); }
    if (userNode->getClassAttribute() != "forum-cell-user") { Q_ASSERT(0); return User(); }

    QtGumboNodePtr userInfoNode = userNode->getElementByClass("forum-user-info", HtmlTag::DIV);
    if (!userInfoNode || !userInfoNode->isValid())
        userInfoNode = userNode->getElementByClass("forum-user-info w-el-dropDown", HtmlTag::DIV);
    if (!userInfoNode->isValid()) { Q_ASSERT(0); return User(); }
    if (userInfoNode->getChildElementCount() < 4) { Q_ASSERT(0); return User(); }
    if ((userInfoNode->getClassAttribute() != "forum-user-info w-el-dropDown") && (userInfoNode->getClassAttribute() != "forum-user-info"))
    { Q_ASSERT(0); return User(); }

    // Get user base info: id, name, profile URL
    UserBaseInfo ubi = getUserBaseInfo(userInfoNode);

    // Get user additional info: post count, register date, reputation points
    UserAdditionalInfo uai = getUserAdditionalInfo(userInfoNode);

    // Get user avatar image
    QSharedPointer<PostImage> userAvatar = getUserAvatar(userInfoNode);
    //if (!userAvatar || !userAvatar->isValid()) { Q_ASSERT(0); return User(); }

#ifdef  RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "User info:" << QString::number(ubi.m_id) + ", " + ubi.m_name + ", " + ubi.m_profileUrl.toDisplayString();
    if (userAvatar)
    qDebug() << "User avatar info:" << userAvatar->m_url + ": " + QString::number(userAvatar->m_width) + " x " + QString::number(userAvatar->m_height);
#endif // RBR_PRINT_DEBUG_OUTPUT

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

Post ForumPageParser::getPostValue(QtGumboNodePtr trNode1)
{
    if (!trNode1 || !trNode1->isValid()) { Q_ASSERT(0); return Post(); }

    Post postInfo;

    QtGumboNodePtr postNode = trNode1->getElementByClass("forum-cell-post", HtmlTag::TD);
    if (!postNode || !postNode->isValid()) { Q_ASSERT(0); return Post(); }
    if (postNode->getChildElementCount() != 2) { Q_ASSERT(0); return Post(); }
    if (postNode->getClassAttribute() != "forum-cell-post") { Q_ASSERT(0); return Post(); }

    // 1) <div class="forum-post-date">
    QtGumboNodePtr postDateNode = postNode->getElementByClass("forum-post-date", HtmlTag::DIV);
    if (!postDateNode || !postDateNode->isValid()) { Q_ASSERT(0); return Post(); }
    if (postDateNode->getChildElementCount() > 3) { Q_ASSERT(0); return Post(); }
    if (postDateNode->getClassAttribute() != "forum-post-date") { Q_ASSERT(0); return Post(); }

    QtGumboNodePtr spanNode = postDateNode->getElementByTag({HtmlTag::SPAN, 0});
    if (!spanNode || !spanNode->isValid()) { Q_ASSERT(0); return Post(); }
    if (spanNode->getChildElementCount() != 0) { Q_ASSERT(0); return Post(); }
    if (spanNode->getTextChildrenCount() != 1) { Q_ASSERT(0); return Post(); }

    QString postDateStr = spanNode->getChildrenInnerText();
    QDateTime postDate = QDateTime::fromString(postDateStr, "dd.MM.yyyy hh:mm");
    if (!postDate.isValid()) { Q_ASSERT(0); return Post(); }

    // 2) <div class="forum-post-entry" style="font-size: 14px;">
    QtGumboNodePtr postEntryNode = postNode->getElementByClass("forum-post-entry", HtmlTag::DIV);
    if (!postEntryNode || !postEntryNode->isValid()) { Q_ASSERT(0); return Post(); }
//    Q_ASSERT(postEntryNode->getChildElementCount() <= 3);
    if (postEntryNode->getClassAttribute() != "forum-post-entry") { Q_ASSERT(0); return Post(); }

    QtGumboNodePtr postTextNode = postEntryNode->getElementByClass("forum-post-text", HtmlTag::DIV);
    if (!postTextNode->isValid()) { Q_ASSERT(0); return Post(); }
    if ((postTextNode->getChildElementCount() == 0) && (postTextNode->getTextChildrenCount() == 0)) { Q_ASSERT(0); return Post(); }
    if (postTextNode->getClassAttribute() != "forum-post-text") { Q_ASSERT(0); return Post(); }

    // Read message id
    QString messageIdStr = postTextNode->getIdAttribute();
    if (!messageIdStr.startsWith("message_text_", Qt::CaseInsensitive)) { Q_ASSERT(0); return Post(); }
    QString idStr = messageIdStr.remove("message_text_", Qt::CaseInsensitive);
    bool idOk = false;
    int id = idStr.toInt(&idOk);
    if (!idOk) { Q_ASSERT(0); return Post(); }

    // Read message contents (HTML)
    QtGumboNodes postTextNodeChildren = postTextNode->getChildren(false);
    parseMessage(postTextNodeChildren, postInfo.m_data);

    // Read user signature
    QString userSignatureStr = getPostUserSignature(postEntryNode);
    userSignatureStr = userSignatureStr.replace("\r", "");
    userSignatureStr = userSignatureStr.replace("\n", "<br>");

    // Read file attachments
    postInfo.m_data << getPostAttachments(postEntryNode);

    // Read post last edit "credentials" (optional)
    QString lastEditStr = getPostLastEdit(postEntryNode);
    lastEditStr = lastEditStr.replace("\r", "");
    lastEditStr = lastEditStr.replace("\n", "<br>");
    lastEditStr = lastEditStr.replace("/profile/", g_bankiRuHost + "/profile/");

#ifdef RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "Post:";
    qDebug() << "	ID: " << id;
    if (!userSignatureStr.isEmpty())
        qDebug() << "   User signature: " << userSignatureStr;
    qDebug() << "	Date: " << postDate;
#endif //  RBR_PRINT_DEBUG_OUTPUT

    postInfo.m_id = id;
//  postInfo.m_postNumber = -1;
    postInfo.m_likeCounter = -1;	// NOTE: will be filled later
    postInfo.m_lastEdit = lastEditStr;
//  postInfo.m_style = "";
    postInfo.m_userSignature = userSignatureStr;
    postInfo.m_date = postDate;
//  postInfo.m_permalink = "";

    return postInfo;
}

void ForumPageParser::parseMessage(QtGumboNodes nodes, IPostObjectList &postObjects) const
{
    for (auto iChild = nodes.begin(); iChild != nodes.end(); ++iChild)
    {
        auto iChildPtr = *iChild;
        if (iChildPtr->isElement())
        {
            switch (iChildPtr->getTag())
            {
            // Rich text
            case HtmlTag::B:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChildPtr->getChildrenInnerText(), true, false, false));
                break;
            }
            case HtmlTag::I:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChildPtr->getChildrenInnerText(), false, true, false));
                break;
            }
            case HtmlTag::U:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChildPtr->getChildrenInnerText(), false, false, true));
                break;
            }
            case HtmlTag::S:
            {
                // FIXME: support striked out text
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChildPtr->getChildrenInnerText(), false, false, true));
                break;
            }
            case HtmlTag::FONT:
            {
                // FIXME: support font tag
                // "<font color="#990033"><b>если не поменяют %</b></font>"
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChildPtr->getChildrenInnerText(), false, false, true));
                break;
            }

            // Quote
            case HtmlTag::TABLE:
            {
                // <table class="forum-quote"> or <table class="forum-code">
                if (iChildPtr->getClassAttribute() == "forum-quote" || iChildPtr->getClassAttribute() == "forum-code")
                {
                    postObjects << parseQuote(*iChild);
                }
                else Q_ASSERT(0);
                break;
            }
            // Line break
            case HtmlTag::BR:
            {
                postObjects << QSharedPointer<PostLineBreak>(new PostLineBreak());
                break;
            }
            // Image (usually smile)
            case HtmlTag::IMG:
            {
                postObjects << parseImage(*iChild);
                break;
            }
            // Hyperlink
            case HtmlTag::A:
            {
                postObjects << parseHyperlink(*iChild);
                break;
            }
            case HtmlTag::DIV:
            {
                // <div id="bx_flv_player_46357291_div" style="width: 400px; height: 300px;">Загрузка плеера</div>
                //     <script>
                Q_ASSERT(iChildPtr->getIdAttribute().startsWith("bx_flv_player_"));
                break;
            }
            case HtmlTag::SCRIPT:
            {
                QString text = iChildPtr->getChildrenInnerText().trimmed();
                if (!text.startsWith("window.bxPlayerOnloadbx_flv_player")) { Q_ASSERT(0); continue; }

                // 'file':'https://www.youtube.com/watch?v=PI9o3v4nttU',
                QString videoUrl = extractVideoUrl(text);
                if (videoUrl.isEmpty())
                    continue;
                postObjects << QSharedPointer<PostVideo>(new PostVideo(videoUrl));
                break;
            }
            case HtmlTag::STYLE:
            case HtmlTag::NOSCRIPT:
            {
                break;
            }
            default: Q_ASSERT(0); break;
            }
        }
        else if (iChildPtr->isText())
        {
            postObjects << QSharedPointer<PostPlainText>(new PostPlainText(iChildPtr->getInnerText().trimmed()));
        }
        else if (iChildPtr->isWhitespace())
        {
            // FIXME: handle
        }
        else Q_ASSERT(0);
    }
}

QString ForumPageParser::getPostLastEdit(QtGumboNodePtr postEntryNode)
{
    // Read post last edit "credentials" (optional)
    QString lastEditStr;
    QtGumboNodePtr postLastEditNode = postEntryNode->getElementByClass("forum-post-lastedit", HtmlTag::DIV);
    if (!postLastEditNode || !postLastEditNode->isValid()) return QString();

    if (postLastEditNode->getChildElementCount() != 1) { Q_ASSERT(0); return QString(); }
    if (postLastEditNode->getClassAttribute() != "forum-post-lastedit") { Q_ASSERT(0); return QString(); }

    QtGumboNodePtr postLastEditSpanNode = postLastEditNode->getElementByClass("forum-post-lastedit", HtmlTag::SPAN);
    if (!postLastEditSpanNode || !postLastEditSpanNode->isValid()) { Q_ASSERT(0); return QString(); }
    if (postLastEditSpanNode->getChildElementCount() < 2) { Q_ASSERT(0); return QString(); }

    QtGumboNodePtr postLastEditUserNode = postLastEditSpanNode->getElementByClass("forum-post-lastedit-user", HtmlTag::SPAN);
    if (!postLastEditUserNode || !postLastEditUserNode->isValid()) { Q_ASSERT(0); return QString(); }
    if (postLastEditUserNode->getChildElementCount() != 1) { Q_ASSERT(0); return QString(); }

    QtGumboNodePtr posLastEditUserLinkNode = postLastEditUserNode->getElementByTag({{HtmlTag::UNKNOWN, 1}, {HtmlTag::A, 0}});
    if (!posLastEditUserLinkNode || !posLastEditUserLinkNode->isValid()) { Q_ASSERT(0); return QString(); }

    QSharedPointer<PostHyperlink> postLastEditUserLink = parseHyperlink(posLastEditUserLinkNode);

    QString userNameRelStr = postLastEditUserLink->m_rel;
    QString userNameHrefStr = postLastEditUserLink->m_urlStr;
    QString userNameStr = postLastEditUserLink->m_title;

    QtGumboNodePtr postLastEditDateNode = postLastEditSpanNode->getElementByClass("forum-post-lastedit-date", HtmlTag::SPAN);
    if (!postLastEditDateNode || !postLastEditDateNode->isValid()) { Q_ASSERT(0); return QString(); }
    if (postLastEditDateNode->getChildElementCount() != 0) { Q_ASSERT(0); return QString(); }
    QString lastEditDateStr = postLastEditDateNode->getChildrenInnerText();

    QString lastEditReasonStr;
    QtGumboNodePtr postLastEditReasonNode = postLastEditSpanNode->getElementByClass("forum-post-lastedit-reason", HtmlTag::SPAN);
    if (postLastEditReasonNode && postLastEditReasonNode->isValid())
    {
        if (postLastEditReasonNode->getChildElementCount() != 1) { Q_ASSERT(0); return QString(); }

        lastEditReasonStr = postLastEditReasonNode->getChildrenInnerText();
        if (lastEditReasonStr != "()") { Q_ASSERT(0); return QString(); }
        lastEditReasonStr.clear();

        QtGumboNodePtr reasonSpanNode = postLastEditReasonNode->getElementByTag({HtmlTag::SPAN, 0});
        if (!reasonSpanNode || !reasonSpanNode->isValid()) { Q_ASSERT(0); return QString(); }
        lastEditReasonStr = "(" + reasonSpanNode->getChildrenInnerText() + ")";
    }

    lastEditStr = "Изменено: <a href=\"" + userNameHrefStr + "\" " + "rel=\"" + userNameRelStr + "\">" + userNameStr + "</a> - " + lastEditDateStr + " " + lastEditReasonStr;

    return lastEditStr;
}

QString ForumPageParser::getPostUserSignature(QtGumboNodePtr postEntryNode)
{
    if (!postEntryNode || !postEntryNode->isValid()) { Q_ASSERT(0); return QString(); }

    // Read user signature
    QString userSignatureStr;
    QtGumboNodePtr postSignatureNode = postEntryNode->getElementByClass("forum-user-signature");
    if (!postSignatureNode || !postSignatureNode->isValid()) return QString();

    if (postSignatureNode->getChildElementCount() != 2) { Q_ASSERT(0); return QString(); }
    if (postSignatureNode->getClassAttribute() != "forum-user-signature") { Q_ASSERT(0); return QString(); }

    QtGumboNodePtr spanNode = postSignatureNode->getElementByTag({HtmlTag::SPAN, 0});
    if (!spanNode || !spanNode->isValid()) { Q_ASSERT(0); return QString(); }

    userSignatureStr = spanNode->getChildrenInnerText();

    // Parse HTML user signatures
    QtGumboNodes children = spanNode->getChildren();
    for (auto iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        auto iChildPtr = *iChild;
        switch (iChildPtr->getTag())
        {
        case HtmlTag::A:
        {
            if (iChildPtr->getAttributeCount() != 3) { Q_ASSERT(0); return QString(); }

            QString hrefAttrValue = iChildPtr->getAttribute("href");
            QString targetAttrValue = iChildPtr->getAttribute("target");
            QString relAttrValue = iChildPtr->getAttribute("rel");
            userSignatureStr +=
                    "<a href=\"" + hrefAttrValue + "\" "
                    + "target=\"" + targetAttrValue + "\" "
                    + "rel=\"" + relAttrValue + "\"" + ">"  + iChildPtr->getChildrenInnerText() + "</a> ";
            userSignatureStr += "<br/>";
            break;
        }
        case HtmlTag::BR:
        {
            break;
        }
        case HtmlTag::IMG:
        {
            QSharedPointer<PostImage> imageObj = parseImage(iChildPtr);
            if (!imageObj || !imageObj->isValid()) { Q_ASSERT(0); return QString(); }

            // FIXME: specify width and height if present
            userSignatureStr += "<img src='" + imageObj->m_url + "'/><br/>";
            break;
        }
        // FIXME: implement other cases
        default:
        {
            Q_ASSERT(0);
            return QString();
        }
        }
    }
    return userSignatureStr;
}

IPostObjectList ForumPageParser::getPostAttachments(QtGumboNodePtr postEntryNode)
{
    if (!postEntryNode || !postEntryNode->isValid()) { Q_ASSERT(0); return IPostObjectList(); }

    // Read post file attachments
    IPostObjectList result;

    QtGumboNodePtr attachmentsNode = postEntryNode->getElementByClass("forum-post-attachments");
    if (!attachmentsNode || !attachmentsNode->isValid())
        return IPostObjectList();

    QtGumboNodePtr labelNode = attachmentsNode->getElementByTag({HtmlTag::LABEL, 0});
    if (!labelNode || !labelNode->isValid()) { Q_ASSERT(0); return IPostObjectList(); }

    QString attachmentsLabelStr = labelNode->getChildrenInnerText();

    result << QSharedPointer<PostLineBreak>(new PostLineBreak());
    result << QSharedPointer<PostRichText>(new PostRichText(attachmentsLabelStr, true, false, false));
    result << QSharedPointer<PostLineBreak>(new PostLineBreak());

    QtGumboNodes children = attachmentsNode->getElementsByClass("forum-post-attachment", HtmlTag::DIV);
    for (auto iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        QtGumboNodePtr attachNode = (*iChild)->getElementByClass("forum-attach",  HtmlTag::DIV);
        if (!attachNode || !attachNode->isValid()) { Q_ASSERT(0); return IPostObjectList(); }

        // FIXME: support other attachment types (if exists)
        QtGumboNodePtr imgNode = attachNode->getElementByClass("popup_image", HtmlTag::IMG);
        if (!imgNode || !imgNode->isValid()) { Q_ASSERT(0); return IPostObjectList(); }

        result << parseImage(imgNode);
    }

    return result;
}

int ForumPageParser::getLikeCounterValue(QtGumboNodePtr trNode2)
{
    if (!trNode2 || !trNode2->isValid()) { Q_ASSERT(0); return -1; }

    // tr2:
    QtGumboNodePtr contactsNode = trNode2->getElementByClass("forum-cell-contact", HtmlTag::TD);
    if (!contactsNode || !contactsNode->isValid()) { Q_ASSERT(0); return -1; }
    if (contactsNode->getClassAttribute() != "forum-cell-contact") { Q_ASSERT(0); return -1; }

    QtGumboNodePtr actionsNode = trNode2->getElementByClass("forum-cell-actions", HtmlTag::TD);
    if (!actionsNode || !actionsNode->isValid()) { Q_ASSERT(0); return -1; }
    if (actionsNode->getChildElementCount() != 1) { Q_ASSERT(0); return -1; }
    if (actionsNode->getClassAttribute() != "forum-cell-actions") { Q_ASSERT(0); return -1; }

    // Get the "like" count
    // NOTE: it is type on the site, not my own
    QtGumboNodePtr actionLinksNode = actionsNode->getElementByClass("conainer-action-links", HtmlTag::DIV);
    if (!actionLinksNode || !actionLinksNode->isValid()) { Q_ASSERT(0); return -1; }
    if (actionLinksNode->getChildElementCount() != 2) { Q_ASSERT(0); return -1; }
    if (actionLinksNode->getClassAttribute() != "conainer-action-links") { Q_ASSERT(0); return -1; }

    QtGumboNodePtr floatLeftNode = actionLinksNode->getElementByClass("float-left", HtmlTag::DIV);
    if (!floatLeftNode || !floatLeftNode->isValid()) { Q_ASSERT(0); return -1; }
    if (floatLeftNode->getChildElementCount() != 1) { Q_ASSERT(0); return -1; }
    if (floatLeftNode->getClassAttribute() != "float-left") { Q_ASSERT(0); return -1; }

    QtGumboNodePtr likeNode = floatLeftNode->getElementByClass("like", HtmlTag::DIV);
    if (!likeNode || !likeNode->isValid()) { Q_ASSERT(0); return -1; }
    if (likeNode->getChildElementCount() != 1) { Q_ASSERT(0); return -1; }
    if (likeNode->getClassAttribute() != "like") { Q_ASSERT(0); return -1; }

    QtGumboNodePtr likeWidgetNode = likeNode->getElementByClass("like__widget", HtmlTag::DIV);
    if (!likeWidgetNode || !likeWidgetNode->isValid()) { Q_ASSERT(0); return -1; }
    if (likeWidgetNode->getChildElementCount() < 2 || likeWidgetNode->getChildElementCount() > 5) { Q_ASSERT(0); return -1; }
    if (likeWidgetNode->getClassAttribute() != "like__widget") { Q_ASSERT(0); return -1; }

    QtGumboNodePtr likeCounterNode = likeWidgetNode->getElementByClass("like__counter", HtmlTag::SPAN);
    if (!likeCounterNode || !likeCounterNode->isValid()) { Q_ASSERT(0); return -1; }
    if (likeCounterNode->getChildElementCount() != 0) { Q_ASSERT(0); return -1; }
    if (likeCounterNode->getClassAttribute() != "like__counter") { Q_ASSERT(0); return -1; }
    if (likeCounterNode->getTextChildrenCount() != 1) { Q_ASSERT(0); return -1; }

    QString likeCounterStr = likeCounterNode->getChildrenInnerText();
    bool likeNumberOk = false;
    int likeCount = likeCounterStr.toInt(&likeNumberOk);
    if (!likeNumberOk) { Q_ASSERT(0); return -1; }
    return likeCount;
}

int ForumPageParser::getPostId(QtGumboNodePtr msdivNode)
{
    if (!msdivNode || !msdivNode->isValid()) { Q_ASSERT(0); return -1; }

    QString messageIdStr = msdivNode->getIdAttribute();
    int msdivIndex = messageIdStr.indexOf("msdiv");
    if (msdivIndex == -1) { Q_ASSERT(0); return -1; }
    QString msdivNumberStr = messageIdStr.mid(5);
    bool messageIdOk = false;
    int messageId = msdivNumberStr.toInt(&messageIdOk);
    if (!messageIdOk) { Q_ASSERT(0); return -1; }
    return messageId;
}

void ForumPageParser::findPageCount(QtGumboNodePtr node, int &pageCount)
{
    if (!node || !node->isValid()) { Q_ASSERT(0); return; }

    pageCount = 0;

    QtGumboNodes paginationNodes = node->getElementsByClassRecursive("ui-pagination__item", HtmlTag::LI);
    // 12 for last page, 16 for others
    if ((paginationNodes.size() != 16) && (paginationNodes.size() != 12)) { Q_ASSERT(0); return; }

    QtGumboNodePtr lastPageNode = paginationNodes[paginationNodes.size() == 16 ? 7 : 11];
    if (!lastPageNode || !lastPageNode->isValid()) { Q_ASSERT(0); return; }

    QSharedPointer<PostHyperlink> lastPageHref = parseHyperlink(lastPageNode->getElementByTag({HtmlTag::A, 0}));
    if (!lastPageHref || !lastPageHref->isValid()) { Q_ASSERT(0); return; }

    QString pageCountStr = lastPageHref->m_title;

    bool pageCountOk = false;
    pageCount = pageCountStr.toInt(&pageCountOk);
    if (!pageCountOk) { pageCount = 0; Q_ASSERT(0); return; }

    if (paginationNodes.size() != 16)
        pageCount++;
}

void ForumPageParser::fillPostList(QtGumboNodePtr node, UserPosts& posts)
{
    if (!node || !node->isValid()) { Q_ASSERT(0); return; }

    // XPath: *[@id="msdiv4453758"]

    // Find div nodes with msdiv id
    QtGumboNodes msdivNodes;
    findMsdivNodesRecursively(node, msdivNodes);

    // table --> tbody --> tr | tr --> td | td
    for (int i = 0; i < msdivNodes.size(); ++i)
    {
        QtGumboNodePtr msdivNode = msdivNodes[i];
        if (!msdivNode || !msdivNode->isValid()) { Q_ASSERT(0); return; }
        if (msdivNode->getChildElementCount() != 1) { Q_ASSERT(0); return; }

        QtGumboNodePtr tbodyNode = msdivNode->getElementByTag({{HtmlTag::TABLE, 1}, {HtmlTag::TBODY, 1}});
        if (!tbodyNode || !tbodyNode->isValid()) { Q_ASSERT(0); return; }

        // two tr tags
        int idxTr1 = 0;
        QtGumboNodePtr trNode1 = tbodyNode->getElementByTag({HtmlTag::TR, idxTr1}, &idxTr1);
        if (!trNode1 || !trNode1->isValid()) { Q_ASSERT(0); return; }

        int idxTr2 = idxTr1 + 1;
        QtGumboNodePtr trNode2 = tbodyNode->getElementByTag({HtmlTag::TR, idxTr2}, &idxTr2);
        if (!trNode2 || !trNode1->isValid()) { Q_ASSERT(0); return; }

        if (trNode1->getChildElementCount() != 2) { Q_ASSERT(0); return; }
        if (trNode2->getChildElementCount() != 2) { Q_ASSERT(0); return; }
        if (idxTr1 != 0) { Q_ASSERT(0); return; }
        if (idxTr2 != 1) { Q_ASSERT(0); return; }

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

QSharedPointer<PostHyperlink> ForumPageParser::parseHyperlink(QtGumboNodePtr aNode) const
{
    if (!aNode || !aNode->isValid() || !aNode->isElement()) { Q_ASSERT(0); return nullptr; }

    // Read hyperlink target
    QString urlStr = aNode->getAttribute("href");
    if (!urlStr.startsWith(g_bankiRuHost) && !urlStr.startsWith("http")) urlStr.prepend(g_bankiRuHost);
    if (!QUrl(urlStr).isValid()) { Q_ASSERT(0); return nullptr; }

    // Read hyperlink balloon tip
    QString tipStr = aNode->getAttribute("title");

    QString relStr = aNode->getAttribute("rel");

    // Read hyperlink display name
    QString titleStr = aNode->getChildrenInnerText();

    return QSharedPointer<PostHyperlink>(new PostHyperlink(urlStr, titleStr, tipStr, relStr));
}

QSharedPointer<PostImage> ForumPageParser::parseImage(QtGumboNodePtr imgNode) const
{
    if (!imgNode || !imgNode->isValid() || !imgNode->isElement()) { Q_ASSERT(0); return nullptr; }

    QSharedPointer<PostImage> result(new PostImage);

    // Get image URL
    QString imageSrcStr = imgNode->getAttribute("src");
    if (imageSrcStr.isEmpty()) { Q_ASSERT(0); return nullptr; }
    if (imageSrcStr.startsWith("//")) imageSrcStr.prepend("http:");
    else if (!imageSrcStr.startsWith(g_bankiRuHost) && !imageSrcStr.startsWith("http:") && !imageSrcStr.startsWith("https:"))
    {
        imageSrcStr.prepend(g_bankiRuHost);
    }
    if (!QUrl(imageSrcStr).isValid()) { Q_ASSERT(0); return nullptr; }
    result->m_url = imageSrcStr;

    // Get image width
    QString imageWidthStr = imgNode->getAttribute("width");
    if (!imageWidthStr.isEmpty())
    {
        bool imageWidthOk = false;
        result->m_width = imageWidthStr.toInt(&imageWidthOk);
        if (!imageWidthOk) { Q_ASSERT(0); return nullptr; }
    }

    // Get image height
    QString imageHeightStr = imgNode->getAttribute("height");
    if (!imageHeightStr.isEmpty())
    {
        bool imageHeightOk = false;
        result->m_height = imageHeightStr.toInt(&imageHeightOk);
        if (!imageHeightOk) { Q_ASSERT(0); return nullptr; }
    }

    // Get image border size
    QString imageBorderStr = imgNode->getAttribute("border");
    if (!imageBorderStr.isEmpty())
    {
        bool imageBorderOk = false;
        result->m_border = imageBorderStr.toInt(&imageBorderOk);
        if (!imageBorderOk) { Q_ASSERT(0); return nullptr; }
    }

    // Get image alternative name
    result->m_altName = imgNode->getAttribute("alt");

    // Get image identifier
    result->m_id = imgNode->getAttribute("id");

    // Get image class name
    result->m_className = imgNode->getAttribute("class");

    return result;
}

QSharedPointer<PostQuote> ForumPageParser::parseQuote(QtGumboNodePtr tableNode) const
{
    if (!tableNode || !tableNode->isValid()) { Q_ASSERT(0); return nullptr; }

    QSharedPointer<PostQuote> result(new PostQuote);

    // Read the quote title
    QtGumboNodePtr theadTrThNode = tableNode->getElementByTag({{HtmlTag::THEAD, 0}, {HtmlTag::TR, 0}, {HtmlTag::TH, 0}});
    if (!theadTrThNode || !theadTrThNode->isValid()) { Q_ASSERT(0); return nullptr; }
    result->m_title = theadTrThNode->getChildrenInnerText();

    QtGumboNodePtr tbodyTrTdNode = tableNode->getElementByTag({{HtmlTag::TBODY, 1}, {HtmlTag::TR, 0}, {HtmlTag::TD, 0}});
    if (!tbodyTrTdNode || !tbodyTrTdNode->isValid()) { Q_ASSERT(0); return nullptr; }

    // Read the quote author and source: e.g.
    // <b>QWASQ</b> <a href="/forum/?PAGE_NAME=message&FID=22&TID=74420&MID=4453640#message4453640" target="_blank" rel="nofollow">пишет</a>:<br />
    // <b>
    // NOTE: optional
    int tbodyTrTdNodeChildIndex = 0;
    QtGumboNodePtr tbodyTrTdANode = tbodyTrTdNode->getElementByTag({HtmlTag::A, 0});
    bool tbodyTrTdANodeValid = tbodyTrTdANode && tbodyTrTdANode->isValid();
    QString tbodyTrTdANodeText = tbodyTrTdANodeValid ? tbodyTrTdANode->getChildrenInnerText().trimmed() : QString();
    if (tbodyTrTdANodeValid && (tbodyTrTdANodeText.compare(QUOTE_WRITE_VERB, Qt::CaseInsensitive) == 0))
    {
        tbodyTrTdNodeChildIndex++;

        QtGumboNodePtr tbodyTrTdBNode = tbodyTrTdNode->getElementByTag({HtmlTag::B, 0});
        if (!tbodyTrTdBNode || !tbodyTrTdBNode->isValid()) { Q_ASSERT(0); return nullptr; }
        if (tbodyTrTdBNode && tbodyTrTdBNode->isValid())
        {
            result->m_userName = tbodyTrTdBNode->getChildrenInnerText();
        }

        // <a>
        tbodyTrTdNodeChildIndex++;
        if (!tbodyTrTdANode || !tbodyTrTdANode->isValid()) { Q_ASSERT(0); return nullptr; }
        QString quoteSourceUrl = tbodyTrTdANode->getAttribute("href");
        if (!quoteSourceUrl.startsWith(g_bankiRuHost))
        {
            quoteSourceUrl.replace("/forum/?", g_bankiRuHost + "/forum/?");
        }
        result->m_url = QUrl(quoteSourceUrl);
        if (!result->m_url.isValid()) { Q_ASSERT(0); return nullptr; }

        // ":"
        tbodyTrTdNodeChildIndex++;

        // <br>
        // FIXME: <br> is optional here
        QtGumboNodePtr tbodyTrTdBrNode = tbodyTrTdNode->getElementByTag({HtmlTag::BR, 0});
        //Q_ASSERT(tbodyTrTdBrNode->isValid());
        if (tbodyTrTdBrNode && tbodyTrTdBrNode->isValid())
            tbodyTrTdNodeChildIndex++;
    }

    // Read the quote body
    // NOTE: quote text is HTML
    if (tbodyTrTdNodeChildIndex >= tbodyTrTdNode->getChildElementCount(false)) { Q_ASSERT(0); return nullptr; }
    QtGumboNodes tbodyTrTdChildren = tbodyTrTdNode->getChildren(false);
    parseMessage(tbodyTrTdChildren.mid(tbodyTrTdNodeChildIndex), result->m_data);
    return result;
}

int ForumPageParser::getPagePosts(QString rawData, UserPosts &userPosts, int &pageCount)
{
    m_htmlDocument.reset(new QtGumboDocument(rawData));

    // Parse web page contents
    findPageCount(m_htmlDocument->rootNode(), pageCount);
    fillPostList(m_htmlDocument->rootNode(), userPosts);

    // TODO: implement error handling with different return code
    return 0;
}

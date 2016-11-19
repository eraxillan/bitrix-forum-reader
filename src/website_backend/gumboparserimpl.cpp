#include "gumboparserimpl.h"

// FIXME: temp ban support:
// <div class = "forum-ban-info">
//		<div class = "fix-b-card_user" title = "Временный бан">
//			<i class = "b-icon r_card"><i></i></i>
//			<span class = "date-from"></span>
//			<span class = "date-to">до 19.03.2016</span>
//		</div>
// </div>

//#define RUBANOK_DEBUG

using namespace BankiRuForum;

namespace {
static const QString g_bankiRuHost = "http://www.banki.ru";
}

void ForumPageParser::printTagsRecursively(QtGumboNode node, int &level)
{
    Q_UNUSED(level);
    if (!node.isElement()) return;

#ifdef RUBANOK_DEBUG
    QString levelStr;
    levelStr.fill('-', level);

    QString idAttrValue = "<empty id>";
    if (node.hasAttribute("id")) idAttrValue = ", id = " + node.getIdAttribute();

    QString classAttrValue = "<empty class>";
    if (node.hasClassAttribute()) classAttrValue = ", class = " + node.getClassAttribute();

    qDebug().noquote() << levelStr << node.getTagName() << idAttrValue << classAttrValue;

    QtGumboNodes children = node.getChildren();
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        level += 4;
        printTagsRecursively(*iChild, level);
        level -= 4;
    }
#endif
}

void ForumPageParser::findMsdivNodesRecursively(QtGumboNode node, QVector<QtGumboNode> &msdivNodes)
{
    if (!node.isElement()) return;

    bool isDivTag = node.getTag() == HtmlTag::DIV;
    if (isDivTag && node.hasIdAttribute())
    {
        QString idAttrValue = node.getIdAttribute();
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

    QtGumboNodes children = node.getChildren();
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
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
    Q_ASSERT(userIdList.size() == 2);
    Q_ASSERT(userIdList[0] == "UID");
    QString userIdValueStr = userIdList[1];
    bool userIdOk = false;
    int userId = userIdValueStr.toInt(&userIdOk);
    Q_ASSERT(userIdOk);
    return userId;
}
}

ForumPageParser::UserBaseInfo ForumPageParser::getUserBaseInfo(QtGumboNode userInfoNode)
{
    UserBaseInfo result;

    QtGumboNode userNameNode = userInfoNode.gumboChildNodeByClass("forum-user-name", HtmlTag::DIV);
    Q_ASSERT(userNameNode.isValid()); if (!userNameNode.isValid()) return result;
    Q_ASSERT(userNameNode.gumboChildElementCount() == 1); if (userNameNode.gumboChildElementCount() != 1) return result;

    QSharedPointer<PostHyperlink> userProfileRef = parseHyperlink(userNameNode.gumboChildNodeByName(HtmlTag::A));
    Q_ASSERT(!userProfileRef.isNull() && userProfileRef->isValid());
    result.m_id = parseUserId(userProfileRef->m_urlStr);
    result.m_name = userProfileRef->m_tip;
    result.m_profileUrl = QUrl(userProfileRef->m_urlStr);
    Q_ASSERT(result.m_profileUrl.isValid());

    return result;
}

ForumPageParser::UserAdditionalInfo ForumPageParser::getUserAdditionalInfo(QtGumboNode userInfoNode)
{
    UserAdditionalInfo result;

    QtGumboNode userAdditionalNode = userInfoNode.gumboChildNodeByClass("forum-user-additional", HtmlTag::DIV);
    Q_ASSERT(userAdditionalNode.gumboChildElementCount() >= 3 && userAdditionalNode.gumboChildElementCount() <= 5);

    // Read the all message URL and the post count
    QtGumboNode postLinkNode = userAdditionalNode.childNodeByTag({{HtmlTag::SPAN, 0}, {HtmlTag::SPAN, 0}, {HtmlTag::UNKNOWN, 0}, {HtmlTag::A, 0}});
    QSharedPointer<PostHyperlink> userPostsLinks = parseHyperlink(postLinkNode);

    QString userAllPosts = userPostsLinks->m_urlStr;
    bool postCountOk = false;
    int postCount = userPostsLinks->m_title.toInt(&postCountOk);
    Q_ASSERT(postCountOk);

    // Read the registration date
    QtGumboNode regDateNode = userAdditionalNode.childNodeByTag({{HtmlTag::SPAN, 1}, {HtmlTag::SPAN, 0}});
    Q_ASSERT(regDateNode.gumboChildElementCount() == 0);
    Q_ASSERT(regDateNode.gumboChildTextNodeCount() == 1);
    QString registrationDateStr = regDateNode.gumboChildTextNodeValue();
    QDate registrationDate = QDate::fromString(registrationDateStr, "dd.MM.yyyy");
    Q_ASSERT(registrationDate.isValid());

    // Read the reputation value
    QtGumboNode userReputationRefNode = userAdditionalNode.childNodeByTag({{HtmlTag::SPAN, 2}, {HtmlTag::SPAN, 0}, {HtmlTag::A, 0}});
    QSharedPointer<PostHyperlink> userReputationRef = parseHyperlink(userReputationRefNode);
    bool reputationOk = false;
    int reputation = userReputationRef->m_title.toInt(&reputationOk);
    Q_ASSERT(reputationOk);

    // NOTE: city is optional field, instead the rest of others
    QString cityStr;
    QtGumboNode userCityNode = userAdditionalNode.childNodeByTag({{HtmlTag::SPAN, 3}});
    if (userCityNode.isValid())
    {
        Q_ASSERT(userCityNode.gumboChildElementCount() == 1);

        QtGumboNode spanNode1 = userCityNode.gumboChildNodeByName(HtmlTag::SPAN);

        Q_ASSERT(spanNode1.gumboChildElementCount() == 0);
        Q_ASSERT(spanNode1.gumboChildTextNodeCount() == 1);
        cityStr = spanNode1.gumboChildTextNodeValue();
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

QSharedPointer<PostImage> ForumPageParser::getUserAvatar(QtGumboNode userInfoNode)
{
    QSharedPointer<PostImage> result;

    QtGumboNode userAvatarNode = userInfoNode.gumboChildNodeByClass("forum-user-avatar", HtmlTag::DIV);
    if(!userAvatarNode.isValid()) userAvatarNode = userInfoNode.gumboChildNodeByClass("forum-user-register-avatar", HtmlTag::DIV);
    Q_ASSERT(userAvatarNode.isValid());
    Q_ASSERT(userAvatarNode.gumboChildElementCount() == 1);
    Q_ASSERT(userAvatarNode.getClassAttribute() == "forum-user-avatar" || userAvatarNode.getClassAttribute() == "forum-user-register-avatar");
    if (userAvatarNode.getClassAttribute() == "forum-user-avatar")
    {
        QtGumboNode imageNode = userAvatarNode.childNodeByTag({{HtmlTag::UNKNOWN, 0}, {HtmlTag::A, 0}, {HtmlTag::IMG, 0}});
        result = parseImage(imageNode);
    }

    return result;
}

User ForumPageParser::getPostUser(QtGumboNode trNode1)
{
    User userInfo;

    QtGumboNode userNode = trNode1.gumboChildNodeByClass("forum-cell-user", HtmlTag::TD);
    Q_ASSERT(userNode.isValid());
    Q_ASSERT(userNode.gumboChildElementCount() == 1);
    Q_ASSERT(userNode.getClassAttribute() == "forum-cell-user");

    QtGumboNode userInfoNode = userNode.gumboChildNodeByClass("forum-user-info", HtmlTag::DIV);
    if(!userInfoNode.isValid()) userInfoNode = userNode.gumboChildNodeByClass("forum-user-info w-el-dropDown", HtmlTag::DIV);
    Q_ASSERT(userInfoNode.isValid());
    Q_ASSERT(userInfoNode.gumboChildElementCount() >= 4);
    Q_ASSERT(userInfoNode.getClassAttribute() == "forum-user-info w-el-dropDown" || userInfoNode.getClassAttribute() == "forum-user-info");

    // Get user base info: id, name, profile URL
    UserBaseInfo ubi = getUserBaseInfo(userInfoNode);

    // Get user additional info: post count, register date, reputation points
    UserAdditionalInfo uai = getUserAdditionalInfo(userInfoNode);

    // Get user avatar image
    QSharedPointer<PostImage> userAvatar = getUserAvatar(userInfoNode);

#ifdef  RUBANOK_DEBUG
    qDebug() << "User info:" << QString::number(ubi.m_id) + ", " + ubi.m_name + ", " + ubi.m_profileUrl.toDisplayString();
    if (userAvatar)
    qDebug() << "User avatar info:" << userAvatar->m_url + ": " + QString::number(userAvatar->m_width) + " x " + QString::number(userAvatar->m_height);
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

Post ForumPageParser::getPostValue(QtGumboNode trNode1)
{
    Post postInfo;

    QtGumboNode postNode = trNode1.gumboChildNodeByClass("forum-cell-post", HtmlTag::TD);
    Q_ASSERT(postNode.isValid());
    Q_ASSERT(postNode.gumboChildElementCount() == 2);
    Q_ASSERT(postNode.getClassAttribute() == "forum-cell-post");

    // 1) <div class="forum-post-date">
    QtGumboNode postDateNode = postNode.gumboChildNodeByClass("forum-post-date", HtmlTag::DIV);
    Q_ASSERT(postDateNode.isValid());
    Q_ASSERT(postDateNode.gumboChildElementCount() <= 3);
    Q_ASSERT(postDateNode.getClassAttribute() == "forum-post-date");

    QtGumboNode spanNode = postDateNode.gumboChildNodeByClass("", HtmlTag::SPAN);
    Q_ASSERT(spanNode.isValid());
    Q_ASSERT(spanNode.gumboChildElementCount() == 0);
    Q_ASSERT(spanNode.gumboChildTextNodeCount() == 1);
    QString postDateStr = spanNode.gumboChildTextNodeValue();
    QDateTime postDate = QDateTime::fromString(postDateStr, "dd.MM.yyyy hh:mm");
    Q_ASSERT(postDate.isValid());

    // 2) <div class="forum-post-entry" style="font-size: 14px;">
    QtGumboNode postEntryNode = postNode.gumboChildNodeByClass("forum-post-entry", HtmlTag::DIV);
    Q_ASSERT(postEntryNode.isValid());
    Q_ASSERT(postEntryNode.gumboChildElementCount() <= 3);
    Q_ASSERT(postEntryNode.getClassAttribute() == "forum-post-entry");

    QtGumboNode postTextNode = postEntryNode.gumboChildNodeByClass("forum-post-text", HtmlTag::DIV);
    Q_ASSERT(postTextNode.isValid());
    Q_ASSERT(postTextNode.gumboChildElementCount() > 0 || postTextNode.gumboChildTextNodeCount() > 0);
    Q_ASSERT(postTextNode.getClassAttribute() == "forum-post-text");

    // Read message id
    QString messageIdStr = postTextNode.getIdAttribute();
    Q_ASSERT(messageIdStr.startsWith("message_text_", Qt::CaseInsensitive));
    QString idStr = messageIdStr.remove("message_text_", Qt::CaseInsensitive);
    bool idOk = false;
    int id = idStr.toInt(&idOk);
    Q_ASSERT(idOk);

    // Read message contents (HTML)
    QtGumboNodes postTextNodeChildren = postTextNode.getChildren(false);
    parseMessage(postTextNodeChildren, postInfo.m_data);

    // Read user signature
    QString userSignatureStr = getPostUserSignature(postEntryNode);
    userSignatureStr = userSignatureStr.replace("\r", "");
    userSignatureStr = userSignatureStr.replace("\n", "<br>");

    // Read post last edit "credentials" (optional)
    QString lastEditStr = getPostLastEdit(postEntryNode);
    lastEditStr = lastEditStr.replace("\r", "");
    lastEditStr = lastEditStr.replace("\n", "<br>");
    lastEditStr = lastEditStr.replace("/profile/", g_bankiRuHost + "/profile/");

#ifdef RUBANOK_DEBUG
    qDebug() << "Post:";
    qDebug() << "	ID: " << id;
    if (!userSignatureStr.isEmpty())
        qDebug() << "   User signature: " << userSignatureStr;
    qDebug() << "	Date: " << postDate;
#endif //  RUBANOK_DEBUG

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
    for (QtGumboNodes::iterator iChild = nodes.begin(); iChild != nodes.end(); ++iChild)
    {
        if (iChild->isElement())
        {
            switch (iChild->getTag())
            {
            // Rich text
            case HtmlTag::B:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChild->gumboChildTextNodeValue(), true, false, false));
                break;
            }
            case HtmlTag::I:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChild->gumboChildTextNodeValue(), false, true, false));
                break;
            }
            case HtmlTag::U:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChild->gumboChildTextNodeValue(), false, false, true));
                break;
            }

            // Quote
            case HtmlTag::TABLE:
            {
                // <table class="forum-quote"> or <table class="forum-code">
                if (iChild->getClassAttribute() == "forum-quote" || iChild->getClassAttribute() == "forum-code")
                {
                    postObjects << parseQuote(*iChild);
                }
                else Q_ASSERT(0);
                break;
            }
            // Line break
            case HtmlTag::BR:
            {
                // FIXME: filter extra line breaks
 //               postObjects << QSharedPointer<PostPlainText>(new PostPlainText("\n"));
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
            // FIXME: WTF? video?
            case HtmlTag::STYLE:
            {
                break;
            }
            case HtmlTag::DIV:
            {
                // <div id="bx_flv_player_46357291_div" style="width: 400px; height: 300px;">Загрузка плеера</div>
                //     <script>
                Q_ASSERT(iChild->getIdAttribute().startsWith("bx_flv_player_"));
                break;
            }
            case HtmlTag::SCRIPT:
            {
                QString text = iChild->gumboChildTextNodeValue().trimmed();
                Q_ASSERT(text.startsWith("window.bxPlayerOnloadbx_flv_player"));

                // 'file':'https://www.youtube.com/watch?v=PI9o3v4nttU',
                const QString VIDEO_URL_START_STR = "'file':'";
                const QString VIDEO_URL_END_STR = "',";
                int videoUrlStartIndex = text.indexOf(VIDEO_URL_START_STR);
                Q_ASSERT(videoUrlStartIndex >= 0);

                int videoUrlEndIndex = text.indexOf(VIDEO_URL_END_STR, videoUrlStartIndex);
                Q_ASSERT(videoUrlEndIndex > videoUrlStartIndex);

                QString videoUrl = text.mid(videoUrlStartIndex + VIDEO_URL_START_STR.size(),
                                            videoUrlEndIndex - videoUrlStartIndex - VIDEO_URL_START_STR.size());
                postObjects << QSharedPointer<PostVideo>(new PostVideo(videoUrl));
                break;
            }
            case HtmlTag::NOSCRIPT:
            {
                break;
            }
            default: Q_ASSERT(0); break;
            }
        }
        else if (iChild->isText())
        {
            postObjects << QSharedPointer<PostPlainText>(new PostPlainText(iChild->getText().trimmed()));
        }
        // FIXME: unknown item type
        //else Q_ASSERT(0);
    }
}

QString ForumPageParser::getPostLastEdit(QtGumboNode postEntryNode)
{
    // Read post last edit "credentials" (optional)
    QString lastEditStr;
    QtGumboNode postLastEditNode = postEntryNode.gumboChildNodeByClass("forum-post-lastedit", HtmlTag::DIV);
    if (!postLastEditNode.isValid()) return QString();

    Q_ASSERT(postLastEditNode.gumboChildElementCount() == 1);
    Q_ASSERT(postLastEditNode.getClassAttribute() == "forum-post-lastedit");

    QtGumboNode postLastEditSpanNode = postLastEditNode.gumboChildNodeByClass("forum-post-lastedit", HtmlTag::SPAN);
    Q_ASSERT(postLastEditSpanNode.isValid());
    Q_ASSERT(postLastEditSpanNode.gumboChildElementCount() >= 2);

    QtGumboNode postLastEditUserNode = postLastEditSpanNode.gumboChildNodeByClass("forum-post-lastedit-user", HtmlTag::SPAN);
    Q_ASSERT(postLastEditUserNode.isValid());
    Q_ASSERT(postLastEditUserNode.gumboChildElementCount() == 1);
    QtGumboNode posLastEditUserLinkNode = postLastEditUserNode.childNodeByTag({{HtmlTag::UNKNOWN, 0}, {HtmlTag::A, 0}});
    QSharedPointer<PostHyperlink> postLastEditUserLink = parseHyperlink(posLastEditUserLinkNode);

    QString userNameRelStr = postLastEditUserLink->m_rel;
    QString userNameHrefStr = postLastEditUserLink->m_urlStr;
    QString userNameStr = postLastEditUserLink->m_title;

    QtGumboNode postLastEditDateNode = postLastEditSpanNode.gumboChildNodeByClass("forum-post-lastedit-date", HtmlTag::SPAN);
    Q_ASSERT(postLastEditDateNode.isValid());
    Q_ASSERT(postLastEditDateNode.gumboChildElementCount() == 0);
    QString lastEditDateStr = postLastEditDateNode.gumboChildTextNodeValue();

    QString lastEditReasonStr;
    QtGumboNode postLastEditReasonNode = postLastEditSpanNode.gumboChildNodeByClass("forum-post-lastedit-reason", HtmlTag::SPAN);
    if (postLastEditReasonNode.isValid())
    {
        Q_ASSERT(postLastEditReasonNode.gumboChildElementCount() == 1);
        lastEditReasonStr = postLastEditReasonNode.gumboChildTextNodeValue();
        Q_ASSERT(lastEditReasonStr == "()");
        lastEditReasonStr.clear();

        QtGumboNode reasonSpanNode = postLastEditReasonNode.gumboChildNodeByName(HtmlTag::SPAN);
        Q_ASSERT(reasonSpanNode.isValid());
        lastEditReasonStr = "(" + reasonSpanNode.gumboChildTextNodeValue() + ")";
    }

    lastEditStr = "Изменено: <a href=\"" + userNameHrefStr + "\" " + "rel=\"" + userNameRelStr + "\">" + userNameStr + "</a> - " + lastEditDateStr + " " + lastEditReasonStr;

    return lastEditStr;
}

QString ForumPageParser::getPostUserSignature(QtGumboNode postEntryNode)
{
    // Read user signature
    QString userSignatureStr;
    QtGumboNode postSignatureNode = postEntryNode.gumboChildNodeByClass("forum-user-signature");
    if (!postSignatureNode.isValid()) return QString();

    Q_ASSERT(postSignatureNode.gumboChildElementCount() == 2);
    Q_ASSERT(postSignatureNode.getClassAttribute() == "forum-user-signature");

    QtGumboNode spanNode = postSignatureNode.gumboChildNodeByName(HtmlTag::SPAN);
    Q_ASSERT(spanNode.isValid());
    Q_ASSERT(spanNode.gumboChildElementCount() <= 2);
    Q_ASSERT(spanNode.gumboChildTextNodeCount() <= 2);
    userSignatureStr = spanNode.gumboChildTextNodeValue();

    // Parse HTML user signatures
    QtGumboNodes children = spanNode.getChildren();
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        switch (iChild->getTag())
        {
        case HtmlTag::A:
        {
            Q_ASSERT(iChild->getAttributeCount() == 3);

            QString hrefAttrValue = iChild->getAttribute("href");
            QString targetAttrValue = iChild->getAttribute("target");
            QString relAttrValue = iChild->getAttribute("rel");
            userSignatureStr +=
                    "<a href=\"" + hrefAttrValue + "\" "
                    + "target=\"" + targetAttrValue + "\" "
                    + "rel=\"" + relAttrValue + "\"" + ">"  + iChild->gumboChildTextNodeValue() + "</a>";
            break;
        }
        case HtmlTag::BR:
        {
            break;
        }
        // FIXME: implement other cases
        default: Q_ASSERT(0);
        }
    }
    return userSignatureStr;
}

int ForumPageParser::getLikeCounterValue(QtGumboNode trNode2)
{
    // tr2:
    QtGumboNode contactsNode = trNode2.gumboChildNodeByClass("forum-cell-contact", HtmlTag::TD);
    Q_ASSERT(contactsNode.isValid());
    Q_ASSERT(contactsNode.getClassAttribute() == "forum-cell-contact");

    QtGumboNode actionsNode = trNode2.gumboChildNodeByClass("forum-cell-actions", HtmlTag::TD);
    Q_ASSERT(actionsNode.isValid());
    Q_ASSERT(actionsNode.gumboChildElementCount() == 1);
    Q_ASSERT(actionsNode.getClassAttribute() == "forum-cell-actions");

    // Get the "like" count
    // NOTE: it is type on the site, not my own
    QtGumboNode actionLinksNode = actionsNode.gumboChildNodeByClass("conainer-action-links", HtmlTag::DIV);
    Q_ASSERT(actionLinksNode.isValid());
    Q_ASSERT(actionLinksNode.gumboChildElementCount() == 2);
    Q_ASSERT(actionLinksNode.getClassAttribute() == "conainer-action-links");

    QtGumboNode floatLeftNode = actionLinksNode.gumboChildNodeByClass("float-left", HtmlTag::DIV);
    Q_ASSERT(floatLeftNode.isValid());
    Q_ASSERT(floatLeftNode.gumboChildElementCount() == 1);
    Q_ASSERT(floatLeftNode.getClassAttribute() == "float-left");

    QtGumboNode likeNode = floatLeftNode.gumboChildNodeByClass("like", HtmlTag::DIV);
    Q_ASSERT(likeNode.isValid());
    Q_ASSERT(likeNode.gumboChildElementCount() == 1);
    Q_ASSERT(likeNode.getClassAttribute() == "like");

    QtGumboNode likeWidgetNode = likeNode.gumboChildNodeByClass("like__widget", HtmlTag::DIV);
    Q_ASSERT(likeWidgetNode.isValid());
    Q_ASSERT(likeWidgetNode.gumboChildElementCount() >= 2 && likeWidgetNode.gumboChildElementCount() <= 5);
    Q_ASSERT(likeWidgetNode.getClassAttribute() == "like__widget");

    QtGumboNode likeCounterNode = likeWidgetNode.gumboChildNodeByClass("like__counter", HtmlTag::SPAN);
    Q_ASSERT(likeCounterNode.isValid());
    Q_ASSERT(likeCounterNode.gumboChildElementCount() == 0);
    Q_ASSERT(likeCounterNode.getClassAttribute() == "like__counter");
    Q_ASSERT(likeCounterNode.gumboChildTextNodeCount() == 1);

    QString likeCounterStr = likeCounterNode.gumboChildTextNodeValue();
    bool likeNumberOk = false;
    int likeCount = likeCounterStr.toInt(&likeNumberOk);
    Q_ASSERT(likeNumberOk);
    return likeCount;
}

int ForumPageParser::getPostId(QtGumboNode msdivNode)
{
    QString messageIdStr = msdivNode.getIdAttribute();
    int msdivIndex = messageIdStr.indexOf("msdiv");
    Q_ASSERT(msdivIndex != -1);
    QString msdivNumberStr = messageIdStr.mid(5);
    bool messageIdOk = false;
    int messageId = msdivNumberStr.toInt(&messageIdOk);
    Q_ASSERT(messageIdOk);
    return messageId;
}

void ForumPageParser::findPageCount(QtGumboNode node, int &pageCount)
{
    pageCount = 0;

    QtGumboNodes paginationNodes = node.gumboChildNodesByClassRecursive("ui-pagination__item", HtmlTag::LI);
    Q_ASSERT(paginationNodes.size() == 16);
    if (paginationNodes.size() != 16) return;

    QtGumboNode lastPageNode = paginationNodes[7];
    Q_ASSERT(lastPageNode.isValid()); if (!lastPageNode.isValid()) return;

    QSharedPointer<PostHyperlink> lastPageHref = parseHyperlink(lastPageNode.gumboChildNodeByName(HtmlTag::A));
    QString pageCountStr = lastPageHref->m_title;

    bool pageCountOk = false;
    pageCount = pageCountStr.toInt(&pageCountOk);
    Q_ASSERT(pageCount);
    if(!pageCountOk) { pageCount = 0; return; }
}

void ForumPageParser::fillPostList(QtGumboNode node, UserPosts& posts)
{
    // XPath: *[@id="msdiv4453758"]

    // Find div nodes with msdiv id
    QVector<QtGumboNode> msdivNodes;
    findMsdivNodesRecursively(node, msdivNodes);

    // table --> tbody --> tr | tr --> td | td
    for (int i = 0; i < msdivNodes.size(); ++i)
    {
        QtGumboNode msdivNode = msdivNodes[i];

        Q_ASSERT(msdivNode.isValid());
        Q_ASSERT(msdivNode.gumboChildElementCount() == 1);
        QtGumboNode tbodyNode = msdivNode.childNodeByTag({{HtmlTag::TABLE, 0}, {HtmlTag::TBODY, 0}});

        // two tr tags
        int idxTr1 = 0;
        QtGumboNode trNode1 = tbodyNode.gumboChildNodeByName(HtmlTag::TR, idxTr1, &idxTr1);
        int idxTr2 = idxTr1 + 1;
        QtGumboNode trNode2 = tbodyNode.gumboChildNodeByName(HtmlTag::TR, idxTr2, &idxTr2);
        Q_ASSERT(trNode1.isValid());
        Q_ASSERT(trNode2.isValid());
        Q_ASSERT(trNode1.gumboChildElementCount() == 2);
        Q_ASSERT(trNode2.gumboChildElementCount() == 2);
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

QSharedPointer<PostHyperlink> ForumPageParser::parseHyperlink(QtGumboNode aNode) const
{
    QSharedPointer<PostHyperlink> result;
    Q_ASSERT(aNode.isValid() && aNode.isElement()); if (!aNode.isValid() || !aNode.isElement()) return result;

    // Read hyperlink target
    QString urlStr = aNode.getAttribute("href");
    if (!urlStr.startsWith(g_bankiRuHost) && !urlStr.startsWith("http")) urlStr.prepend(g_bankiRuHost);
    Q_ASSERT(QUrl(urlStr).isValid()); if (!QUrl(urlStr).isValid()) return result;

    // Read hyperlink balloon tip
    QString tipStr = aNode.getAttribute("title");

    QString relStr = aNode.getAttribute("rel");

    // Read hyperlink display name
    QString titleStr = aNode.gumboChildTextNodeValue();

    result.reset(new PostHyperlink(urlStr, titleStr, tipStr, relStr));
    return result;
}

QSharedPointer<PostImage> ForumPageParser::parseImage(QtGumboNode imgNode) const
{
    QSharedPointer<PostImage> result(new PostImage);
    Q_ASSERT(imgNode.isValid() && imgNode.isElement()); if (!imgNode.isValid() || !imgNode.isElement()) return result;

    // Get image URL
    QString imageSrcStr = imgNode.getAttribute("src");
    Q_ASSERT(!imageSrcStr.isEmpty());
    if (!imageSrcStr.startsWith(g_bankiRuHost)) imageSrcStr.prepend(g_bankiRuHost);
    Q_ASSERT(QUrl(imageSrcStr).isValid());
    result->m_url = imageSrcStr;

    // Get image width
    QString imageWidthStr = imgNode.getAttribute("width");
    if (!imageWidthStr.isEmpty())
    {
        bool imageWidthOk = false;
        result->m_width = imageWidthStr.toInt(&imageWidthOk);
        Q_ASSERT(imageWidthOk);
    }

    // Get image height
    QString imageHeightStr = imgNode.getAttribute("height");
    if (!imageHeightStr.isEmpty())
    {
        bool imageHeightOk = false;
        result->m_height = imageHeightStr.toInt(&imageHeightOk);
        Q_ASSERT(imageHeightOk);
    }

    // Get image border size
    QString imageBorderStr = imgNode.getAttribute("border");
    if (!imageBorderStr.isEmpty())
    {
        bool imageBorderOk = false;
        result->m_border = imageBorderStr.toInt(&imageBorderOk);
        Q_ASSERT(imageBorderOk);
    }

    // Get image alternative name
    result->m_altName = imgNode.getAttribute("alt");

    // Get image identifier
    result->m_id = imgNode.getAttribute("id");

    // Get image class name
    result->m_className = imgNode.getAttribute("class");

    return result;
}

QSharedPointer<PostQuote> ForumPageParser::parseQuote(QtGumboNode tableNode) const
{
    QSharedPointer<PostQuote> result(new PostQuote);

    // Read the quote title
    QtGumboNode theadTrThNode = tableNode.childNodeByTag({{HtmlTag::THEAD, 0}, {HtmlTag::TR, 0}, {HtmlTag::TH, 0}});
    Q_ASSERT(theadTrThNode.isValid());
    result->m_title = theadTrThNode.gumboChildTextNodeValue();

    int tbodyTrTdNodeChildIndex = 0;
    QtGumboNode tbodyTrTdNode = tableNode.childNodeByTag({{HtmlTag::TBODY, 0}, {HtmlTag::TR, 0}, {HtmlTag::TD, 0}});
    Q_ASSERT(tbodyTrTdNode.isValid());

    // Read the quote author and source: e.g.
    // <b>QWASQ</b> <a href="/forum/?PAGE_NAME=message&FID=22&TID=74420&MID=4453640#message4453640" target="_blank" rel="nofollow">пишет</a>:<br />
    // <b>
    // NOTE: optional
    QtGumboNode tbodyTrTdBNode = tbodyTrTdNode.gumboChildNodeByName(HtmlTag::B);
    if (tbodyTrTdBNode.isValid())
    {
        tbodyTrTdNodeChildIndex++;
        result->m_userName = tbodyTrTdBNode.gumboChildTextNodeValue();

        // <a>
        QtGumboNode tbodyTrTdANode = tbodyTrTdNode.gumboChildNodeByName(HtmlTag::A);
        tbodyTrTdNodeChildIndex++;
        Q_ASSERT(tbodyTrTdANode.isValid());
        QString quoteSourceUrl = tbodyTrTdANode.getAttribute("href");
        quoteSourceUrl.replace("/forum/?", g_bankiRuHost + "/forum/?");
        result->m_url = QUrl(quoteSourceUrl);
        Q_ASSERT(result->m_url.isValid());

        // ":"
        tbodyTrTdNodeChildIndex++;

        // <br>
        QtGumboNode tbodyTrTdBrNode = tbodyTrTdNode.gumboChildNodeByName(HtmlTag::BR);
        tbodyTrTdNodeChildIndex++;
        Q_ASSERT(tbodyTrTdBrNode.isValid());
    }

    // Read the quote body
    // NOTE: quote text is HTML
    Q_ASSERT(tbodyTrTdNodeChildIndex < tbodyTrTdNode.gumboChildElementCount(false));
    QtGumboNodes tbodyTrTdChildren = tbodyTrTdNode.getChildren(false);
    parseMessage(tbodyTrTdChildren.mid(tbodyTrTdNodeChildIndex), result->m_data);
    return result;
}

int ForumPageParser::getPagePosts(QString rawData, UserPosts &userPosts, int &pageCount)
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
    findPageCount(output->root, pageCount);
    fillPostList(output->root, userPosts);
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // TODO: implement error handling with different return code
    return 0;
}

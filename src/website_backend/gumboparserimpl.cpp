#include "gumboparserimpl.h"
#include "common/logger.h"

// FIXME: temp ban support:
// <div class = "forum-ban-info">
//		<div class = "fix-b-card_user" title = "Временный бан">
//			<i class = "b-icon r_card"><i></i></i>
//			<span class = "date-from"></span>
//			<span class = "date-to">до 19.03.2016</span>
//		</div>
// </div>

using namespace bfr;

namespace {
static const QString g_bankiRuHost = "http://www.banki.ru";
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

void ForumPageParser::printTagsRecursively(QtGumboNodePtr node, int &level)
{
    BFR_RETURN_VOID_IF(!node || !node->isValid(), "invalid node");

    if (!node->isElement())
        return;

#ifdef BFR_PRINT_DEBUG_OUTPUT
    QString levelStr;
    levelStr.fill('-', level);

    QString idAttrValue = "<empty id>";
    if (node->hasAttribute("id"))
        idAttrValue = ", id = " + node->getIdAttribute();

    QString classAttrValue = "<empty class>";
    if (node->hasClassAttribute())
        classAttrValue = ", class = " + node->getClassAttribute();

    ConsoleLogger->info("{} {} {} {}", levelStr, node->getTagName(), idAttrValue, classAttrValue);

    QtGumboNodes children = node->getChildren();
    for (auto iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        level += 4;
        printTagsRecursively(*iChild, level);
        level -= 4;
    }
#else
    Q_UNUSED(level);
#endif
}

void ForumPageParser::findMsdivNodesRecursively(QtGumboNodePtr node, QVector<QtGumboNodePtr> &msdivNodes)
{
    BFR_RETURN_VOID_IF(!node || !node->isValid(), "Invalid input parameters");

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
            BFR_RETURN_VOID_IF(!msdivNumberCorrect, "Invalid ID string format: not a number");
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
    BFR_DECLARE_DEFAULT_RETURN_TYPE_N_VALUE(int, -1);

    // Read the user ID
    QString userIdStr = QUrl(userProfileUrl).query();
    QStringList userIdList = userIdStr.split('=', QString::SkipEmptyParts);
    BFR_RETURN_DEFAULT_IF(userIdList.size() != 2, "invalid user id string format");
    BFR_RETURN_DEFAULT_IF(userIdList[0] != "UID", "no 'UID' string found");
    QString userIdValueStr = userIdList[1];
    bool userIdOk = false;
    int userId = userIdValueStr.toInt(&userIdOk);
    BFR_RETURN_DEFAULT_IF(!userIdOk, "Invalid userId format (not a number)");
    return userId;
}
}

ForumPageParser::UserBaseInfo ForumPageParser::getUserBaseInfo(QtGumboNodePtr userInfoNode)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(UserBaseInfo);

    BFR_RETURN_DEFAULT_IF(!userInfoNode || !userInfoNode->isValid(), "Invalid input parameters");

    QtGumboNodePtr userNameNode = userInfoNode->getElementByClass("forum-user-name", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!userNameNode || !userNameNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF((userNameNode->getChildElementCount() != 1) && (userNameNode->getChildElementCount() != 2), "Invalid node");

    PostHyperlinkPtr userProfileRef = parseHyperlink(userNameNode->getElementByTag({HtmlTag::A, 0}));
    BFR_RETURN_DEFAULT_IF(userProfileRef.isNull() || !userProfileRef->isValid(), "Invalid node");

    UserBaseInfo result;
    result.m_id = parseUserId(userProfileRef->m_urlStr);
    result.m_name = userProfileRef->m_tip;
    result.m_profileUrl = QUrl(userProfileRef->m_urlStr);
    BFR_RETURN_DEFAULT_IF(result.m_id <= 0, "User ID must be strictly greater than zero");
    BFR_RETURN_DEFAULT_IF(result.m_name.isEmpty(), "User name cannot be empty");
    BFR_RETURN_DEFAULT_IF(!result.m_profileUrl.isValid(), "User profile URL is invalid");

    return result;
}

ForumPageParser::UserAdditionalInfo ForumPageParser::getUserAdditionalInfo(QtGumboNodePtr userInfoNode)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(UserAdditionalInfo);

    BFR_RETURN_DEFAULT_IF(!userInfoNode || !userInfoNode->isValid(), "Invalid input parameters");

    QtGumboNodePtr userAdditionalNode = userInfoNode->getElementByClass("forum-user-additional", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!userAdditionalNode || !userAdditionalNode->isValid(), "Invalid node");

    // Read the all message URL and the post count
    QtGumboNodePtr postLinkNode = userAdditionalNode->getElementByTag(
        {{HtmlTag::SPAN, 1}, {HtmlTag::SPAN, 1}, {HtmlTag::UNKNOWN, 0}, {HtmlTag::A, 0}});
    BFR_RETURN_DEFAULT_IF(!postLinkNode || !postLinkNode->isValid(), "Invalid node");

    PostHyperlinkPtr userPostsLinks = parseHyperlink(postLinkNode);
    BFR_RETURN_DEFAULT_IF(!userPostsLinks || !userPostsLinks->isValid(), "Invalid node");

    QString userAllPosts = userPostsLinks->m_urlStr;
    BFR_RETURN_DEFAULT_IF(!QUrl(userAllPosts).isValid(), "Invalid URL");

    bool postCountOk = false;
    int postCount = userPostsLinks->m_title.toInt(&postCountOk);
    BFR_RETURN_DEFAULT_IF(!postCountOk, "Invalid post count string format: not a number");

    // Read the registration date
    QtGumboNodePtr regDateNode = userAdditionalNode->getElementByTag({{HtmlTag::SPAN, 3}, {HtmlTag::SPAN, 1}});
    BFR_RETURN_DEFAULT_IF(!regDateNode || !regDateNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(regDateNode->getChildElementCount() != 0, "Invalid node child element count");
    BFR_RETURN_DEFAULT_IF(regDateNode->getTextChildrenCount() != 1, "Invalid node child text element count");

    QString registrationDateStr = regDateNode->getChildrenInnerText();
    QDate registrationDate = QDate::fromString(registrationDateStr, "dd.MM.yyyy");
    BFR_RETURN_DEFAULT_IF(!registrationDate.isValid(), "Invalid registration date string format: not a date");

    // Read the reputation value
    QtGumboNodePtr userReputationRefNode = userAdditionalNode->getElementByTag({{HtmlTag::SPAN, 5}, {HtmlTag::SPAN, 1}, {HtmlTag::A, 0}});
    BFR_RETURN_DEFAULT_IF(!userReputationRefNode || !userReputationRefNode->isValid(), "Invalid node");

    PostHyperlinkPtr userReputationRef = parseHyperlink(userReputationRefNode);
    BFR_RETURN_DEFAULT_IF(!userReputationRef || !userReputationRef->isValid(), "Invalid node");

    bool reputationOk = false;
    int reputation = userReputationRef->m_title.toInt(&reputationOk);
    BFR_RETURN_DEFAULT_IF(!reputationOk, "Invalid reputation string format: not a number");

    // NOTE: city is optional field, instead the rest of others
    QString cityStr;
    QtGumboNodePtr userCityNode = userAdditionalNode->getElementByTag({HtmlTag::SPAN, 3});
    if (userCityNode && userCityNode->isValid())
    {
        BFR_RETURN_DEFAULT_IF(userCityNode->getChildElementCount() != 1, "Invalid node");

        QtGumboNodePtr spanNode1 = userCityNode->getElementByTag({HtmlTag::SPAN, 0});
        BFR_RETURN_DEFAULT_IF(!spanNode1 || !spanNode1->isValid(), "Invalid node");
        BFR_RETURN_DEFAULT_IF(spanNode1->getChildElementCount() != 0, "Invalid node child element count");
        BFR_RETURN_DEFAULT_IF(spanNode1->getTextChildrenCount() != 1, "Invalid node text child element count");

        cityStr = spanNode1->getChildrenInnerText();
    }

#ifdef BFR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("post count: {}, reputation: {}, city: {}, all posts url: {}, registration date: {}",
                        postCount, reputation, cityStr, userAllPosts, registrationDate.toString(Qt::SystemLocaleShortDate));
#endif

    UserAdditionalInfo result;
    result.m_allPostsUrl = QUrl(userAllPosts);
    result.m_postCount = postCount;
    result.m_registrationDate = registrationDate;
    result.m_reputation = reputation;
    result.m_city = cityStr;
    return result;
}

PostImagePtr ForumPageParser::getUserAvatar(QtGumboNodePtr userInfoNode)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(PostImagePtr);

    BFR_RETURN_DEFAULT_IF(!userInfoNode || !userInfoNode->isValid(), "Invalid input parameters");

    QtGumboNodePtr userAvatarNode = userInfoNode->getElementByClass("forum-user-avatar", HtmlTag::DIV);
    if (!userAvatarNode || !userAvatarNode->isValid())
        userAvatarNode = userInfoNode->getElementByClass("forum-user-register-avatar", HtmlTag::DIV);

    BFR_RETURN_DEFAULT_IF(!userAvatarNode || !userAvatarNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(userAvatarNode->getChildElementCount() != 1, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF((userAvatarNode->getClassAttribute() != "forum-user-avatar") && (userAvatarNode->getClassAttribute() != "forum-user-register-avatar"), "Invalid node class");

    PostImagePtr result;
    if (userAvatarNode->getClassAttribute() == "forum-user-avatar")
    {
        QtGumboNodePtr imageNode = userAvatarNode->getElementByTag({{HtmlTag::UNKNOWN, 1}, {HtmlTag::A, 1}, {HtmlTag::IMG, 0}});
        BFR_RETURN_DEFAULT_IF(!imageNode || !imageNode->isValid(), "Invalid node");

        result = parseImage(imageNode);
    }

    return result;
}

UserPtr ForumPageParser::getPostUser(QtGumboNodePtr trNode1)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(UserPtr);
    BFR_RETURN_DEFAULT_IF(!trNode1 || !trNode1->isValid(), "Invalid input parameters");

    QtGumboNodePtr userNode = trNode1->getElementByClass("forum-cell-user", HtmlTag::TD);
    BFR_RETURN_DEFAULT_IF(!userNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(userNode->getChildElementCount() != 1, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(userNode->getClassAttribute() != "forum-cell-user", "Invalid node class");

    QtGumboNodePtr userInfoNode = userNode->getElementByClass("forum-user-info", HtmlTag::DIV);
    if (!userInfoNode || !userInfoNode->isValid())
        userInfoNode = userNode->getElementByClass("forum-user-info w-el-dropDown", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!userInfoNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(userInfoNode->getChildElementCount() < 4, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF((userInfoNode->getClassAttribute() != "forum-user-info w-el-dropDown") && (userInfoNode->getClassAttribute() != "forum-user-info"), "Invalid node class");

    // Get user base info: id, name, profile URL
    UserBaseInfo ubi = getUserBaseInfo(userInfoNode);

    // Get user additional info: post count, register date, reputation points
    UserAdditionalInfo uai = getUserAdditionalInfo(userInfoNode);

    // Get user avatar image
    // NOTE: it is optional
    PostImagePtr userAvatar = getUserAvatar(userInfoNode);

#ifdef BFR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("User info: id = {}, name = {}, profile url: {}", ubi.m_id, ubi.m_name, ubi.m_profileUrl.toDisplayString());
    if (userAvatar)
        ConsoleLogger->info("User avatar info: url = {}, size = {} x {}", userAvatar->m_url, userAvatar->m_width, userAvatar->m_height);
#endif

    // Base info
    UserPtr userInfo(new User);
    userInfo->m_userId = ubi.m_id;
    userInfo->m_userName = ubi.m_name;
    userInfo->m_userProfileUrl = ubi.m_profileUrl;

    // Avatar image
    userInfo->m_userAvatar = userAvatar;

    // Additional info
    userInfo->m_allPostsUrl = uai.m_allPostsUrl;
    userInfo->m_postCount = uai.m_postCount;
    userInfo->m_registrationDate = uai.m_registrationDate;
    userInfo->m_reputation = uai.m_reputation;
    userInfo->m_city = uai.m_city;

    return userInfo;
}

PostPtr ForumPageParser::getPostValue(QtGumboNodePtr trNode1)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(PostPtr);

    BFR_RETURN_DEFAULT_IF(!trNode1 || !trNode1->isValid(), "Invalid input parameters");

    QtGumboNodePtr postNode = trNode1->getElementByClass("forum-cell-post", HtmlTag::TD);
    BFR_RETURN_DEFAULT_IF(!postNode || !postNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(postNode->getChildElementCount() != 2, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(postNode->getClassAttribute() != "forum-cell-post", "Invalid node class");

    // 1) <div class="forum-post-date">
    QtGumboNodePtr postDateNode = postNode->getElementByClass("forum-post-date", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!postDateNode || !postDateNode->isValid(), "Invalid post date string format: not a date");
    BFR_RETURN_DEFAULT_IF(postDateNode->getChildElementCount() > 3, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(postDateNode->getClassAttribute() != "forum-post-date", "Invalid node class");

    QtGumboNodePtr spanNode = postDateNode->getElementByTag({HtmlTag::SPAN, 0});
    BFR_RETURN_DEFAULT_IF(!spanNode || !spanNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(spanNode->getChildElementCount() != 0, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(spanNode->getTextChildrenCount() != 1, "Invalid text child element count");

    QString postDateStr = spanNode->getChildrenInnerText();
    QDateTime postDate = QDateTime::fromString(postDateStr, "dd.MM.yyyy hh:mm");
    BFR_RETURN_DEFAULT_IF(!postDate.isValid(), "Invalid post date string format: not a date");

    // 2) <div class="forum-post-entry" style="font-size: 14px;">
    QtGumboNodePtr postEntryNode = postNode->getElementByClass("forum-post-entry", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!postEntryNode || !postEntryNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(postEntryNode->getClassAttribute() != "forum-post-entry", "Invalid node class");

    QtGumboNodePtr postTextNode = postEntryNode->getElementByClass("forum-post-text", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!postTextNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF((postTextNode->getChildElementCount() == 0) && (postTextNode->getTextChildrenCount() == 0), "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(postTextNode->getClassAttribute() != "forum-post-text", "Invalid node class");

    // Read message id
    QString messageIdStr = postTextNode->getIdAttribute();
    BFR_RETURN_DEFAULT_IF(!messageIdStr.startsWith("message_text_", Qt::CaseInsensitive), "Invalid message ID string format");
    QString idStr = messageIdStr.remove("message_text_", Qt::CaseInsensitive);
    bool idOk = false;
    int id = idStr.toInt(&idOk);
    BFR_RETURN_DEFAULT_IF(!idOk, "Invalid message ID string format: not a number");

    // Read message contents (HTML)
    QtGumboNodes postTextNodeChildren = postTextNode->getChildren(false);
    PostPtr postInfo(new Post);
    parseMessage(postTextNodeChildren, postInfo->m_data);

    // Read user signature
    QString userSignatureStr = getPostUserSignature(postEntryNode);
    userSignatureStr = userSignatureStr.replace("\r", "");
    userSignatureStr = userSignatureStr.replace("\n", "<br>");

    // Read file attachments
    postInfo->m_data << getPostAttachments(postEntryNode);

    // Read post last edit "credentials" (optional)
    QString lastEditStr = getPostLastEdit(postEntryNode);
    lastEditStr = lastEditStr.replace("\r", "");
    lastEditStr = lastEditStr.replace("\n", "<br>");
    lastEditStr = lastEditStr.replace("/profile/", g_bankiRuHost + "/profile/");

#ifdef BFR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("Post:");
    ConsoleLogger->info("	ID: {}", id);
    if (!userSignatureStr.isEmpty())
        ConsoleLogger->info("   User signature: {}", userSignatureStr);
    ConsoleLogger->info("	Date: {}", postDate);
#endif

    postInfo->m_id = id;
//  postInfo->m_postNumber = -1;
    postInfo->m_likeCounter = -1;	// NOTE: will be filled later
    postInfo->m_lastEdit = lastEditStr;
//  postInfo->m_style = "";
    postInfo->m_userSignature = userSignatureStr;
    postInfo->m_date = postDate;
//  postInfo->m_permalink = "";

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
                postObjects << PostRichTextPtr(new PostRichText(iChildPtr->getChildrenInnerText(), "black", true, false, false, false));
                break;
            }
            case HtmlTag::I:
            {
                postObjects << PostRichTextPtr(new PostRichText(iChildPtr->getChildrenInnerText(), "black", false, true, false, false));
                break;
            }
            case HtmlTag::U:
            {
                postObjects << PostRichTextPtr(new PostRichText(iChildPtr->getChildrenInnerText(), "black", false, false, true, false));
                break;
            }
            case HtmlTag::S:
            {
                postObjects << PostRichTextPtr(new PostRichText(iChildPtr->getChildrenInnerText(), "black", false, false, false, true));
                break;
            }
            case HtmlTag::FONT:
            {
                BFR_RETURN_VOID_IF(iChildPtr->getAttributeCount() != 1, "Invalid attribute count");

                QString textColor = "black";
                if (iChildPtr->hasAttribute("color"))
                    textColor = iChildPtr->getAttribute("color");
                QtGumboNodes fontTagChildren = iChildPtr->getChildren(false);
                for (QtGumboNodePtr node : fontTagChildren)
                {
                    if (node->isElement())
                    {
                        switch (node->getTag())
                        {
                        case HtmlTag::B:
                        {
                            postObjects << PostRichTextPtr(new PostRichText(" " + node->getChildrenInnerText() + " ", textColor, true, false, false, false));
                            break;
                        }
                        // Line break
                        case HtmlTag::BR:
                        {
                            postObjects << PostLineBreakPtr(new PostLineBreak());
                            break;
                        }
                        // FIXME: implement other text formatting tags as above
                        default:
                        {
                            BFR_RETURN_VOID_IF(true, "Unsupported HTML tag detected");
                            break;
                        }
                        }
                    }
                    else if (node->isText())
                    {
                        //postObjects << PostPlainTextPtr(new PostPlainText(node.getInnerText().trimmed()));
                        postObjects << PostRichTextPtr(new PostRichText(" " + node->getInnerText().trimmed() + " ", textColor, false, false, false, false));
                    }
                }
                break;
            }
            // Quote
            case HtmlTag::TABLE:
            {
                // <table class="forum-quote">
                // <table class="forum-code">
                // <table class="forum-spoiler">
                if (iChildPtr->getClassAttribute() == "forum-quote" || iChildPtr->getClassAttribute() == "forum-code")
                {
                    postObjects << parseQuote(*iChild);
                }
                else if (iChildPtr->getClassAttribute() == "forum-spoiler")
                {
                    postObjects << parseSpoiler(*iChild);
                }
                else
                {
                    BFR_RETURN_VOID_IF(true, "Invalid quote node class");
                }
                break;
            }
            // Line break
            case HtmlTag::WBR:  // FIXME: implement this correctly as browsers do
            case HtmlTag::BR:
            {
                postObjects << PostLineBreakPtr(new PostLineBreak());
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
                BFR_RETURN_VOID_IF(!iChildPtr->getIdAttribute().startsWith("bx_flv_player_"), "Invalid video player node ID");
                break;
            }
            case HtmlTag::SCRIPT:
            {
                QString text = iChildPtr->getChildrenInnerText().trimmed();
                BFR_RETURN_VOID_IF(!text.startsWith("window.bxPlayerOnloadbx_flv_player"), "Invalid video player JS text");

                // 'file':'https://www.youtube.com/watch?v=PI9o3v4nttU',
                const QString VIDEO_URL_START_STR = "'file':'";
                const QString VIDEO_URL_END_STR = "',";
                const QString VIDEO_URL_TAG_START_STR = "[url]";
                const QString VIDEO_URL_TAG_END_STR = "[/url]";
                int videoUrlStartIndex = text.indexOf(VIDEO_URL_START_STR);
                BFR_RETURN_VOID_IF(videoUrlStartIndex < 0, "No video URL begin found");

                int videoUrlEndIndex = text.indexOf(VIDEO_URL_END_STR, videoUrlStartIndex);
                BFR_RETURN_VOID_IF(videoUrlEndIndex <= videoUrlStartIndex, "No video URL end found");

                QString videoUrl = text.mid(videoUrlStartIndex + VIDEO_URL_START_STR.size(),
                                            videoUrlEndIndex - videoUrlStartIndex - VIDEO_URL_START_STR.size());
                videoUrl = videoUrl.trimmed();

                int videoUrlTagStartIndex = videoUrl.indexOf(VIDEO_URL_TAG_START_STR);
                if (videoUrlTagStartIndex >= 0)
                {
                    int videoUrlTagEndIndex = videoUrl.indexOf(VIDEO_URL_TAG_END_STR);
                    BFR_RETURN_VOID_IF(videoUrlTagEndIndex <= videoUrlTagStartIndex, "No video URL end tag found");

                    videoUrl = videoUrl.mid(videoUrlTagStartIndex + VIDEO_URL_TAG_START_STR.size(),
                                            videoUrlTagEndIndex - videoUrlTagStartIndex - VIDEO_URL_TAG_START_STR.size());
                    videoUrl = videoUrl.trimmed();
                }
                postObjects << PostVideoPtr(new PostVideo(videoUrl));
                break;
            }
            case HtmlTag::STYLE:
            case HtmlTag::NOSCRIPT:
            {
                break;
            }
            default:
            {
                BFR_RETURN_VOID_IF(true, "Unsupported HTML tag");
                break;
            }
            }
        }
        else if (iChildPtr->isText())
        {
            // FIXME: ugly hack to remove ':' from the quote body beginning
            QString text = iChildPtr->getInnerText().trimmed();
            if (m_textQuoteFlag)
            {
                text = text.remove(0, 1);
                text = text.trimmed();
                m_textQuoteFlag = false;
            }

            postObjects << PostPlainTextPtr(new PostPlainText(text));
        }
        else
        {
            if (iChildPtr->isWhitespace()) continue;
            if (iChildPtr->isComment()) continue;

            BFR_RETURN_VOID_IF(true, "Unsupported HTML entity");
        }
    }
}

QString ForumPageParser::getPostLastEdit(QtGumboNodePtr postEntryNode)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(QString);

    BFR_RETURN_DEFAULT_IF(!postEntryNode || !postEntryNode->isValid(), "Invalid input parameters");

    // Read post last edit info (optional)
    QString lastEditStr;
    QtGumboNodePtr postLastEditNode = postEntryNode->getElementByClass("forum-post-lastedit", HtmlTag::DIV);
    if (!postLastEditNode || !postLastEditNode->isValid())
        return QString();

    BFR_RETURN_DEFAULT_IF(postLastEditNode->getChildElementCount() != 1, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(postLastEditNode->getClassAttribute() != "forum-post-lastedit", "Invalid node class");

    QtGumboNodePtr postLastEditSpanNode = postLastEditNode->getElementByClass("forum-post-lastedit", HtmlTag::SPAN);
    BFR_RETURN_DEFAULT_IF(!postLastEditSpanNode || !postLastEditSpanNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(postLastEditSpanNode->getChildElementCount() < 2, "Invalid child element count");

    QtGumboNodePtr postLastEditUserNode = postLastEditSpanNode->getElementByClass("forum-post-lastedit-user", HtmlTag::SPAN);
    BFR_RETURN_DEFAULT_IF(!postLastEditUserNode || !postLastEditUserNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(postLastEditUserNode->getChildElementCount() != 1, "Invalid child element count");

    QtGumboNodePtr posLastEditUserLinkNode = postLastEditUserNode->getElementByTag({{HtmlTag::UNKNOWN, 1}, {HtmlTag::A, 0}});
    BFR_RETURN_DEFAULT_IF(!posLastEditUserLinkNode || !posLastEditUserLinkNode->isValid(), "Invalid node");

    PostHyperlinkPtr postLastEditUserLink = parseHyperlink(posLastEditUserLinkNode);

    QString userNameRelStr = postLastEditUserLink->m_rel;
    QString userNameHrefStr = postLastEditUserLink->m_urlStr;
    QString userNameStr = postLastEditUserLink->m_title;

    QtGumboNodePtr postLastEditDateNode = postLastEditSpanNode->getElementByClass("forum-post-lastedit-date", HtmlTag::SPAN);
    BFR_RETURN_DEFAULT_IF(!postLastEditDateNode || !postLastEditDateNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(postLastEditDateNode->getChildElementCount() != 0, "Invalid child element count");
    QString lastEditDateStr = postLastEditDateNode->getChildrenInnerText();

    QString lastEditReasonStr;
    QtGumboNodePtr postLastEditReasonNode = postLastEditSpanNode->getElementByClass("forum-post-lastedit-reason", HtmlTag::SPAN);
    if (postLastEditReasonNode && postLastEditReasonNode->isValid())
    {
        BFR_RETURN_DEFAULT_IF(postLastEditReasonNode->getChildElementCount() != 1, "Invalid child element count");

        lastEditReasonStr = postLastEditReasonNode->getChildrenInnerText();
        BFR_RETURN_DEFAULT_IF(lastEditReasonStr != "()", "Invalid last edit reason string format");
        lastEditReasonStr.clear();

        QtGumboNodePtr reasonSpanNode = postLastEditReasonNode->getElementByTag({HtmlTag::SPAN, 0});
        BFR_RETURN_DEFAULT_IF(!reasonSpanNode || !reasonSpanNode->isValid(), "Invalid node");
        lastEditReasonStr = "(" + reasonSpanNode->getChildrenInnerText() + ")";
    }

    // FIXME: replace HTML with pure QML code
    lastEditStr = "Изменено: <a href=\"" + userNameHrefStr + "\" " + "rel=\"" + userNameRelStr + "\">" + userNameStr + "</a> - " + lastEditDateStr + " " + lastEditReasonStr;

    return lastEditStr;
}

QString ForumPageParser::getPostUserSignature(QtGumboNodePtr postEntryNode)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(QString);

    BFR_RETURN_DEFAULT_IF(!postEntryNode || !postEntryNode->isValid(), "Invalid input parameters");

    // Read user signature
    QString userSignatureStr;
    QtGumboNodePtr postSignatureNode = postEntryNode->getElementByClass("forum-user-signature");
    if (!postSignatureNode || !postSignatureNode->isValid())
        return QString();

    BFR_RETURN_DEFAULT_IF(postSignatureNode->getChildElementCount() != 2, "Invalid child node count");
    BFR_RETURN_DEFAULT_IF(postSignatureNode->getClassAttribute() != "forum-user-signature", "Invalid node class");

    QtGumboNodePtr spanNode = postSignatureNode->getElementByTag({HtmlTag::SPAN, 0});
    BFR_RETURN_DEFAULT_IF(!spanNode || !spanNode->isValid(), "Invalid node");

    userSignatureStr = spanNode->getChildrenInnerText();

    // Parse HTML user signatures
    QtGumboNodes children = spanNode->getChildren();
    for (auto iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        auto iChildPtr = *iChild;
        switch (iChildPtr->getTag())
        {
        case HtmlTag::B:
        {
            userSignatureStr += "<b>" + iChildPtr->getChildrenInnerText() + "</b><br/>";
            break;
        }
        case HtmlTag::A:
        {
            BFR_RETURN_DEFAULT_IF(iChildPtr->getAttributeCount() != 3, "Invalid attribute count");

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
            PostImagePtr imageObj = parseImage(iChildPtr);
            BFR_RETURN_DEFAULT_IF(!imageObj || !imageObj->isValid(), "Invalid node");

            // FIXME: specify width and height if present
            userSignatureStr += "<img src='" + imageObj->m_url + "'/><br/>";
            break;
        }
        // TODO: implement other cases if discovered
        default:
        {
            BFR_RETURN_DEFAULT_IF(true, "Not implemented");
        }
        }
    }
    return userSignatureStr;
}

IPostObjectList ForumPageParser::getPostAttachments(QtGumboNodePtr postEntryNode)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(IPostObjectList);

    BFR_RETURN_DEFAULT_IF(!postEntryNode || !postEntryNode->isValid(), "Invalid input parameters");

    // Read post file attachments
    IPostObjectList result;

    QtGumboNodePtr attachmentsNode = postEntryNode->getElementByClass("forum-post-attachments");
    if (!attachmentsNode || !attachmentsNode->isValid())
        return IPostObjectList();

    QtGumboNodePtr labelNode = attachmentsNode->getElementByTag({HtmlTag::LABEL, 0});
    BFR_RETURN_DEFAULT_IF(!labelNode || !labelNode->isValid(), "Invalid node");

    QString attachmentsLabelStr = labelNode->getChildrenInnerText();

    result << PostLineBreakPtr(new PostLineBreak());
    result << PostRichTextPtr(new PostRichText(attachmentsLabelStr, "black", true, false, false, false));
    result << PostLineBreakPtr(new PostLineBreak());

    QtGumboNodes children = attachmentsNode->getElementsByClass("forum-post-attachment", HtmlTag::DIV);
    for (auto iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        QtGumboNodePtr attachNode = (*iChild)->getElementByClass("forum-attach",  HtmlTag::DIV);
        BFR_RETURN_DEFAULT_IF(!attachNode || !attachNode->isValid(), "Invalid node");

        // FIXME: support other attachment types (if exists)
        QtGumboNodePtr imgNode = attachNode->getElementByClass("popup_image", HtmlTag::IMG);
        BFR_RETURN_DEFAULT_IF(!imgNode || !imgNode->isValid(), "Invalid node");

        result << parseImage(imgNode);
    }

    return result;
}

int ForumPageParser::getLikeCounterValue(QtGumboNodePtr trNode2)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE_N_VALUE(int, -1);

    BFR_RETURN_DEFAULT_IF(!trNode2 || !trNode2->isValid(), "Invalid node");

    // tr2:
    QtGumboNodePtr contactsNode = trNode2->getElementByClass("forum-cell-contact", HtmlTag::TD);
    BFR_RETURN_DEFAULT_IF(!contactsNode || !contactsNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(contactsNode->getClassAttribute() != "forum-cell-contact", "Invalid node class");

    QtGumboNodePtr actionsNode = trNode2->getElementByClass("forum-cell-actions", HtmlTag::TD);
    BFR_RETURN_DEFAULT_IF(!actionsNode || !actionsNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(actionsNode->getChildElementCount() != 1, "Invalid child element node");
    BFR_RETURN_DEFAULT_IF(actionsNode->getClassAttribute() != "forum-cell-actions", "Invalid node class");

    // Get the "like" count
    // NOTE: it is type on the site, not my own
    QtGumboNodePtr actionLinksNode = actionsNode->getElementByClass("conainer-action-links", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!actionLinksNode || !actionLinksNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(actionLinksNode->getChildElementCount() != 2, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(actionLinksNode->getClassAttribute() != "conainer-action-links", "Invalid node class");

    QtGumboNodePtr floatLeftNode = actionLinksNode->getElementByClass("float-left", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!floatLeftNode || !floatLeftNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(floatLeftNode->getChildElementCount() != 1, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(floatLeftNode->getClassAttribute() != "float-left", "Invalid node class");

    QtGumboNodePtr likeNode = floatLeftNode->getElementByClass("like", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!likeNode || !likeNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(likeNode->getChildElementCount() != 1, "Invalid node child element count");
    BFR_RETURN_DEFAULT_IF(likeNode->getClassAttribute() != "like", "Invalid node class");

    QtGumboNodePtr likeWidgetNode = likeNode->getElementByClass("like__widget", HtmlTag::DIV);
    BFR_RETURN_DEFAULT_IF(!likeWidgetNode || !likeWidgetNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(likeWidgetNode->getChildElementCount() < 2 || likeWidgetNode->getChildElementCount() > 5, "Invalid child element count");
    BFR_RETURN_DEFAULT_IF(likeWidgetNode->getClassAttribute() != "like__widget", "Invalid node class");

    QtGumboNodePtr likeCounterNode = likeWidgetNode->getElementByClass("like__counter", HtmlTag::SPAN);
    BFR_RETURN_DEFAULT_IF(!likeCounterNode || !likeCounterNode->isValid(), "Invalid node");
    BFR_RETURN_DEFAULT_IF(likeCounterNode->getChildElementCount() != 0, "Invalid child element class");
    BFR_RETURN_DEFAULT_IF(likeCounterNode->getClassAttribute() != "like__counter", "Invalid node class");
    BFR_RETURN_DEFAULT_IF(likeCounterNode->getTextChildrenCount() != 1, "Invalid text child element count");

    QString likeCounterStr = likeCounterNode->getChildrenInnerText();
    bool likeNumberOk = false;
    int likeCount = likeCounterStr.toInt(&likeNumberOk);
    BFR_RETURN_DEFAULT_IF(!likeNumberOk, "Invalid like counter string format: not a number");
    return likeCount;
}

int ForumPageParser::getPostId(QtGumboNodePtr msdivNode)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE_N_VALUE(int, -1);

    BFR_RETURN_DEFAULT_IF(!msdivNode || !msdivNode->isValid(), "Invalid input parameters");

    QString messageIdStr = msdivNode->getIdAttribute();
    int msdivIndex = messageIdStr.indexOf("msdiv");
    BFR_RETURN_DEFAULT_IF(msdivIndex == -1, "Invalid message ID string format");
    QString msdivNumberStr = messageIdStr.mid(5);
    bool messageIdOk = false;
    int messageId = msdivNumberStr.toInt(&messageIdOk);
    BFR_RETURN_DEFAULT_IF(!messageIdOk, "Invalid message ID string format: not a number");
    return messageId;
}

void ForumPageParser::findPageCount(QString rawData, int &pageCount)
{
    pageCount = 0;

    // NOTE: alternative method
    const QString PAGES_STR = "pages: ";
    int pagesIdxBegin = rawData.indexOf(PAGES_STR);
    int pagesIdxEnd   = rawData.indexOf(",", pagesIdxBegin);
    BFR_RETURN_VOID_IF(pagesIdxBegin < 0, "No page count expession found");
    BFR_RETURN_VOID_IF(pagesIdxEnd <= pagesIdxBegin, "Invalid page count expression");
    int pageCountStrSize = pagesIdxEnd - pagesIdxBegin - PAGES_STR.size();
    BFR_RETURN_VOID_IF(pageCountStrSize <= 0, "Negative page count");
    QString pageCountStr = rawData.mid(pagesIdxBegin + PAGES_STR.size(), pageCountStrSize);
    bool pageCountOk = false;
    pageCount = pageCountStr.toInt(&pageCountOk);
    if (!pageCountOk) pageCount = 0;
    BFR_RETURN_VOID_IF(!pageCountOk, "Invalid page count string format: not a number");
}

void ForumPageParser::fillPostList(QtGumboNodePtr node, PostList &posts)
{
    BFR_RETURN_VOID_IF(!node || !node->isValid(), "Invalid input parameters");

    // XPath: *[@id="msdiv4453758"]

    // Find div nodes with msdiv id
    QtGumboNodes msdivNodes;
    findMsdivNodesRecursively(node, msdivNodes);

    // table --> tbody --> tr | tr --> td | td
    for (int i = 0; i < msdivNodes.size(); ++i)
    {
        QtGumboNodePtr msdivNode = msdivNodes[i];
        BFR_RETURN_VOID_IF(!msdivNode || !msdivNode->isValid(), "Invalid node");
        BFR_RETURN_VOID_IF(msdivNode->getChildElementCount() != 1, "Invalid child element count");

        QtGumboNodePtr tbodyNode = msdivNode->getElementByTag({{HtmlTag::TABLE, 1}, {HtmlTag::TBODY, 1}});
        BFR_RETURN_VOID_IF(!tbodyNode || !tbodyNode->isValid(), "Invalid node");

        // two tr tags
        int idxTr1 = 0;
        QtGumboNodePtr trNode1 = tbodyNode->getElementByTag({HtmlTag::TR, idxTr1}, &idxTr1);
        BFR_RETURN_VOID_IF(!trNode1 || !trNode1->isValid(), "Invalid node");

        int idxTr2 = idxTr1 + 1;
        QtGumboNodePtr trNode2 = tbodyNode->getElementByTag({HtmlTag::TR, idxTr2}, &idxTr2);
        BFR_RETURN_VOID_IF(!trNode2 || !trNode1->isValid(), "Invalid node");

        BFR_RETURN_VOID_IF(trNode1->getChildElementCount() != 2, "Invalid child element count");
        BFR_RETURN_VOID_IF(trNode2->getChildElementCount() != 2, "Invalid child element count");
        BFR_RETURN_VOID_IF(idxTr1 != 0, "Invalid node index");
        BFR_RETURN_VOID_IF(idxTr2 != 1, "Invalid node index");

        // each tr tag has two child td tags
        // tr1:
        UserPtr forumUser = getPostUser(trNode1);
        PostPtr forumPost = getPostValue(trNode1);
        forumPost->m_id = getPostId(msdivNode);

        // Read the like counter value from tr2
        forumPost->m_likeCounter = getLikeCounterValue(trNode2);

        forumPost->m_author = forumUser;

        // FIXME: fill other post/user info

        posts << forumPost;
    }
}

PostHyperlinkPtr ForumPageParser::parseHyperlink(QtGumboNodePtr aNode) const
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(PostHyperlinkPtr);
    BFR_RETURN_DEFAULT_IF(!aNode || !aNode->isValid() || !aNode->isElement(), "Invalid input parameters");

    // Read hyperlink target
    QString urlStr = aNode->getAttribute("href");
    if (!urlStr.startsWith(g_bankiRuHost) && !urlStr.startsWith("http"))
        urlStr.prepend(g_bankiRuHost);
    BFR_RETURN_DEFAULT_IF(!QUrl(urlStr).isValid(), "Invalid hyperlink URL");

    // Read hyperlink balloon tip
    QString tipStr = aNode->getAttribute("title");

    QString relStr = aNode->getAttribute("rel");

    // Read hyperlink display name
    QString titleStr = aNode->getChildrenInnerText();

    return PostHyperlinkPtr(new PostHyperlink(urlStr, titleStr, tipStr, relStr));
}

PostImagePtr ForumPageParser::parseImage(QtGumboNodePtr imgNode) const
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(PostImagePtr);
    BFR_RETURN_DEFAULT_IF(!imgNode || !imgNode->isValid() || !imgNode->isElement(), "Invalid input parameters");

    PostImagePtr result(new PostImage);

    // Get image URL
    QString imageSrcStr = imgNode->getAttribute("src");
    BFR_RETURN_DEFAULT_IF(imageSrcStr.isEmpty(), "Image source attribute is empty or absent");
    if (imageSrcStr.startsWith("//"))
        imageSrcStr.prepend("http:");
    else if (!imageSrcStr.startsWith(g_bankiRuHost) && !imageSrcStr.startsWith("http:") && !imageSrcStr.startsWith("https:"))
    {
        imageSrcStr.prepend(g_bankiRuHost);
    }
    BFR_RETURN_DEFAULT_IF(!QUrl(imageSrcStr).isValid(), "Invalid image URL");
    result->m_url = imageSrcStr;

    // Get image width
    QString imageWidthStr = imgNode->getAttribute("width");
    if (!imageWidthStr.isEmpty())
    {
        bool imageWidthOk = false;
        result->m_width = imageWidthStr.toInt(&imageWidthOk);
        BFR_RETURN_DEFAULT_IF(!imageWidthOk, "Invalid image width string format: not a number");
    }

    // Get image height
    QString imageHeightStr = imgNode->getAttribute("height");
    if (!imageHeightStr.isEmpty())
    {
        bool imageHeightOk = false;
        result->m_height = imageHeightStr.toInt(&imageHeightOk);
        BFR_RETURN_DEFAULT_IF(!imageHeightOk, "Invalid image height string format: not a number");
    }

    // Get image border size
    QString imageBorderStr = imgNode->getAttribute("border");
    if (!imageBorderStr.isEmpty())
    {
        bool imageBorderOk = false;
        result->m_border = imageBorderStr.toInt(&imageBorderOk);
        BFR_RETURN_DEFAULT_IF(!imageBorderOk, "Invalid image border string format: not a number");
    }

    // Get image alternative name
    result->m_altName = imgNode->getAttribute("alt");

    // Get image identifier
    result->m_id = imgNode->getAttribute("id");

    // Get image class name
    result->m_className = imgNode->getAttribute("class");

    return result;
}

PostQuotePtr ForumPageParser::parseQuote(QtGumboNodePtr tableNode) const
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(PostQuotePtr);

    BFR_RETURN_DEFAULT_IF(!tableNode || !tableNode->isValid(), "Invalid input parameters");

    PostQuotePtr result(new PostQuote);

    // Read the quote title
    QtGumboNodePtr theadTrThNode = tableNode->getElementByTag({{HtmlTag::THEAD, 0}, {HtmlTag::TR, 0}, {HtmlTag::TH, 0}});
    BFR_RETURN_DEFAULT_IF(!theadTrThNode || !theadTrThNode->isValid(), "Invalid node");
    result->m_title = theadTrThNode->getChildrenInnerText();

    QtGumboNodePtr tbodyTrTdNode = tableNode->getElementByTag({{HtmlTag::TBODY, 1}, {HtmlTag::TR, 0}, {HtmlTag::TD, 0}});
    BFR_RETURN_DEFAULT_IF(!tbodyTrTdNode || !tbodyTrTdNode->isValid(), "Invalid node");

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
        QtGumboNodePtr tbodyTrTdBNode = tbodyTrTdNode->getElementByTag({HtmlTag::B, 0});
        if (tbodyTrTdBNode && tbodyTrTdBNode->isValid())
        {
            result->m_userName = tbodyTrTdBNode->getChildrenInnerText();
            tbodyTrTdNodeChildIndex++;
        }

        // <a>
        tbodyTrTdNodeChildIndex++;
        BFR_RETURN_DEFAULT_IF(!tbodyTrTdANode || !tbodyTrTdANode->isValid(), "Invalid node");
        QString quoteSourceUrl = tbodyTrTdANode->getAttribute("href");
        if (!quoteSourceUrl.startsWith(g_bankiRuHost))
        {
            quoteSourceUrl.replace("/forum/?", g_bankiRuHost + "/forum/?");
        }
        result->m_url = QUrl(quoteSourceUrl);
        BFR_RETURN_DEFAULT_IF(!result->m_url.isValid(), "Invalid quote source URL");

        // Find the quote body start
        BFR_RETURN_DEFAULT_IF(tbodyTrTdNodeChildIndex >= tbodyTrTdNode->getChildElementCount(false), "Invalid node index");
        QtGumboNodes tbodyTrTdChildren = tbodyTrTdNode->getChildren(false);
        for (int i = tbodyTrTdNodeChildIndex; i < tbodyTrTdChildren.size(); ++i)
        {
            QtGumboNodePtr temp = tbodyTrTdChildren[i];
            if (temp->isText())
            {
                QString tempText = temp->getInnerText().trimmed();
                if (tempText == ":")
                {
                    tbodyTrTdNodeChildIndex += 2;
                    break;
                }
                if (tempText.startsWith(":"))
                {
                    m_textQuoteFlag = true;
                    break;
                }
            }
        }
    }

    // Read the quote body
    // NOTE: quote text is HTML too
    // FIXME: temponary workaround! i will investigate the problem in future 
//    BFR_RETURN_DEFAULT_IF(tbodyTrTdNodeChildIndex >= tbodyTrTdNode->getChildElementCount(false), "Invalid node index");
    QtGumboNodes tbodyTrTdChildren = tbodyTrTdNode->getChildren(false);

 #ifdef BFR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("-------------------------------------");
    ConsoleLogger->info("Start index: {}", tbodyTrTdNodeChildIndex);
    for (int i = 0; i < tbodyTrTdChildren.size(); ++i)
    {
        const QString idxString = i == tbodyTrTdNodeChildIndex ? QString("[ *") + QString::number(i) + QString("* ]")
                                                               : QString("[ ")  + QString::number(i) + QString(" ]");

        if (tbodyTrTdChildren[i]->isElement())
            ConsoleLogger->info("{} Element: {}", idxString, tbodyTrTdChildren[i]->getTagName());
        else if (tbodyTrTdChildren[i]->isText())
            ConsoleLogger->info("{} Text: {}", idxString, tbodyTrTdChildren[i]->getInnerText());
        else if (!tbodyTrTdChildren[i]->isComment() && !tbodyTrTdChildren[i]->isWhitespace())
            ConsoleLogger->info("{} Unknown item", idxString);
    }
    ConsoleLogger->info("-------------------------------------");
#endif

    parseMessage(tbodyTrTdChildren.mid(tbodyTrTdNodeChildIndex), result->m_data);
    return result;
}

PostSpoilerPtr ForumPageParser::parseSpoiler(QtGumboNodePtr tableNode) const
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(PostSpoilerPtr);

    BFR_RETURN_DEFAULT_IF(!tableNode || !tableNode->isValid(), "Invalid input parameters");

    PostSpoilerPtr result(new PostSpoiler);

    // Read the quote title
    QtGumboNodePtr theadTrThNode = tableNode->getElementByTag({{HtmlTag::THEAD, 0}, {HtmlTag::TR, 0}, {HtmlTag::TH, 0}, {HtmlTag::DIV, 0}});
    BFR_RETURN_DEFAULT_IF(!theadTrThNode || !theadTrThNode->isValid(), "Invalid node");
    result->m_title = theadTrThNode->getChildrenInnerText();
    BFR_RETURN_DEFAULT_IF((result->m_title.back() != QChar(9650).unicode()) && (result->m_title.back() != QChar(9660).unicode()), "Invalid spoiler title char");

    result->m_title = result->m_title.remove(result->m_title.size()-1, 1);
    result->m_title = result->m_title.trimmed();

    QtGumboNodePtr tbodyTrTdNode = tableNode->getElementByTag({{HtmlTag::TBODY, 1}, {HtmlTag::TR, 0}, {HtmlTag::TD, 0}});
    BFR_RETURN_DEFAULT_IF(!tbodyTrTdNode || !tbodyTrTdNode->isValid(), "Invalid node");

    // Read the spoiler body
    // NOTE: spoiler text is HTML too
    QtGumboNodes tbodyTrTdChildren = tbodyTrTdNode->getChildren(false);

 #ifdef BFR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("-------------------------------------");
    for (int i = 0; i < tbodyTrTdChildren.size(); ++i)
    {
        const QString idxString = QString("[ ")  + QString::number(i) + QString(" ]");

        if (tbodyTrTdChildren[i]->isElement())
            ConsoleLogger->info("{} Element: {}", idxString, tbodyTrTdChildren[i]->getTagName());
        else if (tbodyTrTdChildren[i]->isText())
            ConsoleLogger->info("{} Text: {}", idxString, tbodyTrTdChildren[i]->getInnerText());
        else if (!tbodyTrTdChildren[i]->isComment() && !tbodyTrTdChildren[i]->isWhitespace())
            ConsoleLogger->info("{} Unknown item", idxString);
    }
    ConsoleLogger->info("-------------------------------------");
#endif

    parseMessage(tbodyTrTdChildren, result->m_data);
    return result;
}

// IForumPageReader implementation ////////////////////////////////////////////

namespace
{
QByteArray convertHtmlToUft8(QByteArray rawHtmlData)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE(QByteArray);

    QByteArray result;

    QTextCodec *htmlCodec = QTextCodec::codecForHtml(rawHtmlData);
    BFR_RETURN_DEFAULT_IF(!htmlCodec, "No HTML codec found");
#ifdef BFR_PRINT_DEBUG_OUTPUT
    ConsoleLogger->info("HTML encoding/charset is '{}'", htmlCodec->name().toStdString());
#endif
    QString resultStr = htmlCodec->toUnicode(rawHtmlData);
    result = resultStr.toUtf8();
    return result;
}
}

result_code::Type ForumPageParser::getPageCount(QByteArray rawData, int &pageCount)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE_N_VALUE(result_code::Type, result_code::Type::Fail);

    QByteArray utfData = convertHtmlToUft8(rawData);
    BFR_RETURN_DEFAULT_IF(utfData.isEmpty(), "Unable to convert HTML page contents to UTF-8");

    findPageCount(rawData, pageCount);

    // TODO: implement error handling with different return code
    return result_code::Type::Ok;
}

result_code::Type ForumPageParser::getPagePosts(QByteArray rawData, PostList &userPosts)
{
    BFR_DECLARE_DEFAULT_RETURN_TYPE_N_VALUE(result_code::Type, result_code::Type::Fail);

    QByteArray utfData = convertHtmlToUft8(rawData);
    BFR_RETURN_DEFAULT_IF(utfData.isEmpty(), "Unable to convert HTML page contents to UTF-8");

    m_htmlDocument.reset(new QtGumboDocument(utfData));

    // Parse web page contents
    fillPostList(m_htmlDocument->rootNode(), userPosts);

    // TODO: implement error handling with different return code
    return result_code::Type::Ok;
}
///////////////////////////////////////////////////////////////////////////////

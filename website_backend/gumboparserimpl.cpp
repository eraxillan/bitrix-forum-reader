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

void ForumPageParser::printTagsRecursively(QtGumboNode node, int &level)
{
    Q_UNUSED(level);
    if (!node.isElement()) return;

#ifdef RBR_PRINT_DEBUG_OUTPUT
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

    QtGumboNode userNameNode = userInfoNode.getElementByClass("forum-user-name", HtmlTag::DIV);
    Q_ASSERT(userNameNode.isValid()); if (!userNameNode.isValid()) return result;
    Q_ASSERT(userNameNode.getChildElementCount() == 1 || userNameNode.getChildElementCount() == 2);
    if (userNameNode.getChildElementCount() != 1 && userNameNode.getChildElementCount() != 2) return result;
    Q_ASSERT(userNameNode.getChildren()[0].getTag() == HtmlTag::A || userNameNode.getChildren()[0].getTag() == HtmlTag::SCRIPT);
    if (userNameNode.getChildElementCount() == 2)
    {
        Q_ASSERT(userNameNode.getChildren()[1].getTag() == HtmlTag::A || userNameNode.getChildren()[1].getTag() == HtmlTag::SCRIPT);
    }

    QSharedPointer<PostHyperlink> userProfileRef = parseHyperlink(userNameNode.getElementByTag({HtmlTag::A, 0}));
    Q_ASSERT(!userProfileRef.isNull() && userProfileRef->isValid());
    result.m_id = parseUserId(userProfileRef->m_urlStr);
    result.m_name = userProfileRef->m_tip;
    result.m_profileUrl = QUrl(userProfileRef->m_urlStr);
    Q_ASSERT(result.m_id > 0);
    Q_ASSERT(!result.m_name.isEmpty());
    Q_ASSERT(result.m_profileUrl.isValid());

    return result;
}

ForumPageParser::UserAdditionalInfo ForumPageParser::getUserAdditionalInfo(QtGumboNode userInfoNode)
{
    UserAdditionalInfo result;

    QtGumboNode userAdditionalNode = userInfoNode.getElementByClass("forum-user-additional", HtmlTag::DIV);
    Q_ASSERT(userAdditionalNode.getChildElementCount() >= 3 && userAdditionalNode.getChildElementCount() <= 6);

    // Read the all message URL and the post count
    QtGumboNode postLinkNode = userAdditionalNode.getElementByTag({{HtmlTag::SPAN, 0}, {HtmlTag::SPAN, 0}, {HtmlTag::UNKNOWN, 0}, {HtmlTag::A, 0}});
    QSharedPointer<PostHyperlink> userPostsLinks = parseHyperlink(postLinkNode);

    QString userAllPosts = userPostsLinks->m_urlStr;
    bool postCountOk = false;
    int postCount = userPostsLinks->m_title.toInt(&postCountOk);
    Q_ASSERT(postCountOk);

    // Read the registration date
    QtGumboNode regDateNode = userAdditionalNode.getElementByTag({{HtmlTag::SPAN, 1}, {HtmlTag::SPAN, 0}});
    Q_ASSERT(regDateNode.getChildElementCount() == 0);
    Q_ASSERT(regDateNode.getTextChildrenCount() == 1);
    QString registrationDateStr = regDateNode.getChildrenInnerText();
    QDate registrationDate = QDate::fromString(registrationDateStr, "dd.MM.yyyy");
    Q_ASSERT(registrationDate.isValid());

    // Read the reputation value
    QtGumboNode userReputationRefNode = userAdditionalNode.getElementByTag({{HtmlTag::SPAN, 2}, {HtmlTag::SPAN, 0}, {HtmlTag::A, 0}});
    QSharedPointer<PostHyperlink> userReputationRef = parseHyperlink(userReputationRefNode);
    bool reputationOk = false;
    int reputation = userReputationRef->m_title.toInt(&reputationOk);
    Q_ASSERT(reputationOk);

    // NOTE: city is optional field, instead the rest of others
    QString cityStr;
    QtGumboNode userCityNode = userAdditionalNode.getElementByTag({HtmlTag::SPAN, 3});
    if (userCityNode.isValid())
    {
        Q_ASSERT(userCityNode.getChildElementCount() == 1);

        QtGumboNode spanNode1 = userCityNode.getElementByTag({HtmlTag::SPAN, 0});

        Q_ASSERT(spanNode1.getChildElementCount() == 0);
        Q_ASSERT(spanNode1.getTextChildrenCount() == 1);
        cityStr = spanNode1.getChildrenInnerText();
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

QSharedPointer<PostImage> ForumPageParser::getUserAvatar(QtGumboNode userInfoNode)
{
    QSharedPointer<PostImage> result;

    QtGumboNode userAvatarNode = userInfoNode.getElementByClass("forum-user-avatar", HtmlTag::DIV);
    if(!userAvatarNode.isValid()) userAvatarNode = userInfoNode.getElementByClass("forum-user-register-avatar", HtmlTag::DIV);
    Q_ASSERT(userAvatarNode.isValid());
    Q_ASSERT(userAvatarNode.getChildElementCount() == 1);
    Q_ASSERT(userAvatarNode.getClassAttribute() == "forum-user-avatar" || userAvatarNode.getClassAttribute() == "forum-user-register-avatar");
    if (userAvatarNode.getClassAttribute() == "forum-user-avatar")
    {
        QtGumboNode imageNode = userAvatarNode.getElementByTag({{HtmlTag::UNKNOWN, 0}, {HtmlTag::A, 0}, {HtmlTag::IMG, 0}});
        result = parseImage(imageNode);
    }

    return result;
}

User ForumPageParser::getPostUser(QtGumboNode trNode1)
{
    User userInfo;

    QtGumboNode userNode = trNode1.getElementByClass("forum-cell-user", HtmlTag::TD);
    Q_ASSERT(userNode.isValid());
    Q_ASSERT(userNode.getChildElementCount() == 1);
    Q_ASSERT(userNode.getClassAttribute() == "forum-cell-user");

    QtGumboNode userInfoNode = userNode.getElementByClass("forum-user-info", HtmlTag::DIV);
    if(!userInfoNode.isValid()) userInfoNode = userNode.getElementByClass("forum-user-info w-el-dropDown", HtmlTag::DIV);
    Q_ASSERT(userInfoNode.isValid());
    Q_ASSERT(userInfoNode.getChildElementCount() >= 4);
    Q_ASSERT(userInfoNode.getClassAttribute() == "forum-user-info w-el-dropDown" || userInfoNode.getClassAttribute() == "forum-user-info");

    // Get user base info: id, name, profile URL
    UserBaseInfo ubi = getUserBaseInfo(userInfoNode);

    // Get user additional info: post count, register date, reputation points
    UserAdditionalInfo uai = getUserAdditionalInfo(userInfoNode);

    // Get user avatar image
    QSharedPointer<PostImage> userAvatar = getUserAvatar(userInfoNode);

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

Post ForumPageParser::getPostValue(QtGumboNode trNode1)
{
    Post postInfo;

    QtGumboNode postNode = trNode1.getElementByClass("forum-cell-post", HtmlTag::TD);
    Q_ASSERT(postNode.isValid());
    Q_ASSERT(postNode.getChildElementCount() == 2);
    Q_ASSERT(postNode.getClassAttribute() == "forum-cell-post");

    // 1) <div class="forum-post-date">
    QtGumboNode postDateNode = postNode.getElementByClass("forum-post-date", HtmlTag::DIV);
    Q_ASSERT(postDateNode.isValid());
    Q_ASSERT(postDateNode.getChildElementCount() <= 3);
    Q_ASSERT(postDateNode.getClassAttribute() == "forum-post-date");

    QtGumboNode spanNode = postDateNode.getElementByTag({HtmlTag::SPAN, 0});
    Q_ASSERT(spanNode.isValid());
    Q_ASSERT(spanNode.getChildElementCount() == 0);
    Q_ASSERT(spanNode.getTextChildrenCount() == 1);
    QString postDateStr = spanNode.getChildrenInnerText();
    QDateTime postDate = QDateTime::fromString(postDateStr, "dd.MM.yyyy hh:mm");
    Q_ASSERT(postDate.isValid());

    // 2) <div class="forum-post-entry" style="font-size: 14px;">
    QtGumboNode postEntryNode = postNode.getElementByClass("forum-post-entry", HtmlTag::DIV);
    Q_ASSERT(postEntryNode.isValid());
    Q_ASSERT(postEntryNode.getChildElementCount() <= 4);
    Q_ASSERT(postEntryNode.getClassAttribute() == "forum-post-entry");

    QtGumboNode postTextNode = postEntryNode.getElementByClass("forum-post-text", HtmlTag::DIV);
    Q_ASSERT(postTextNode.isValid());
    Q_ASSERT(postTextNode.getChildElementCount() > 0 || postTextNode.getTextChildrenCount() > 0);
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
    for (QtGumboNodes::iterator iChild = nodes.begin(); iChild != nodes.end(); ++iChild)
    {
        if (iChild->isElement())
        {
            switch (iChild->getTag())
            {
            // Rich text
            case HtmlTag::B:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChild->getChildrenInnerText(), "black", true, false, false, false));
                break;
            }
            case HtmlTag::I:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChild->getChildrenInnerText(), "black", false, true, false, false));
                break;
            }
            case HtmlTag::U:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChild->getChildrenInnerText(), "black", false, false, true, false));
                break;
            }
            case HtmlTag::S:
            {
                postObjects << QSharedPointer<PostRichText>(new PostRichText(iChild->getChildrenInnerText(), "black", false, false, false, true));
                break;
            }
            case HtmlTag::FONT:
            {
                Q_ASSERT(iChild->getAttributeCount() == 1);

                QString textColor = "black";
                if (iChild->hasAttribute("color")) textColor = iChild->getAttribute("color");
                QtGumboNodes fontTagChildren = iChild->getChildren(false);
                for (QtGumboNode node: fontTagChildren)
                {
                    if (node.isElement())
                    {
                        switch (node.getTag())
                        {
                        case HtmlTag::B:
                        {
                            postObjects << QSharedPointer<PostRichText>(new PostRichText(" " + node.getChildrenInnerText() + " ", textColor, true, false, false, false));
                            break;
                        }
                        // Line break
                        case HtmlTag::BR:
                        {
                            postObjects << QSharedPointer<PostLineBreak>(new PostLineBreak());
                            break;
                        }
                        // FIXME: implement other text formatting tags as above
                        default:
                        {
                            Q_ASSERT_X(0, Q_FUNC_INFO, "unknown HTML tag");
                            break;
                        }
                        }
                    }
                    else if (node.isText())
                    {
                        //postObjects << QSharedPointer<PostPlainText>(new PostPlainText(node.getInnerText().trimmed()));
                        postObjects << QSharedPointer<PostRichText>(new PostRichText(" " + node.getInnerText().trimmed() + " ", textColor, false, false, false, false));
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
                if (iChild->getClassAttribute() == "forum-quote" || iChild->getClassAttribute() == "forum-code")
                {
                    postObjects << parseQuote(*iChild);
                }
                else if (iChild->getClassAttribute() == "forum-spoiler")
                {
                    postObjects << parseSpoiler(*iChild);
                }
                else
                {
                    Q_ASSERT_X(0, Q_FUNC_INFO, "invalid quote class");
                }
                break;
            }
            // Line break
            case HtmlTag::WBR:  // FIXME: implement this correctly as browsers do
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
                Q_ASSERT(iChild->getIdAttribute().startsWith("bx_flv_player_"));
                break;
            }
            case HtmlTag::SCRIPT:
            {
                QString text = iChild->getChildrenInnerText().trimmed();
                Q_ASSERT(text.startsWith("window.bxPlayerOnloadbx_flv_player"));

                // 'file':'https://www.youtube.com/watch?v=PI9o3v4nttU',
                const QString VIDEO_URL_START_STR = "'file':'";
                const QString VIDEO_URL_END_STR = "',";
                const QString VIDEO_URL_TAG_START_STR = "[url]";
                const QString VIDEO_URL_TAG_END_STR = "[/url]";
                int videoUrlStartIndex = text.indexOf(VIDEO_URL_START_STR);
                Q_ASSERT(videoUrlStartIndex >= 0);

                int videoUrlEndIndex = text.indexOf(VIDEO_URL_END_STR, videoUrlStartIndex);
                Q_ASSERT(videoUrlEndIndex > videoUrlStartIndex);

                QString videoUrl = text.mid(videoUrlStartIndex + VIDEO_URL_START_STR.size(),
                                            videoUrlEndIndex - videoUrlStartIndex - VIDEO_URL_START_STR.size());
                videoUrl = videoUrl.trimmed();

                int videoUrlTagStartIndex = videoUrl.indexOf(VIDEO_URL_TAG_START_STR);
                if (videoUrlTagStartIndex >= 0)
                {
                    int videoUrlTagEndIndex = videoUrl.indexOf(VIDEO_URL_TAG_END_STR);
                    Q_ASSERT(videoUrlTagEndIndex > videoUrlTagStartIndex);

                    videoUrl = videoUrl.mid(videoUrlTagStartIndex + VIDEO_URL_TAG_START_STR.size(),
                                            videoUrlTagEndIndex - videoUrlTagStartIndex - VIDEO_URL_TAG_START_STR.size());
                    videoUrl = videoUrl.trimmed();
                }
                postObjects << QSharedPointer<PostVideo>(new PostVideo(videoUrl));
                break;
            }
            case HtmlTag::STYLE:
            case HtmlTag::NOSCRIPT:
            {
                break;
            }
            default:
            {
                Q_ASSERT_X(0, Q_FUNC_INFO, "unknown HTML tag");
                break;
            }
            }
        }
        else if (iChild->isText())
        {
            // FIXME: ugly hack to remove ':' from the quote body beginning
            QString text = iChild->getInnerText().trimmed();
            if (m_textQuoteFlag)
            {
                text = text.remove(0, 1);
                text = text.trimmed();
                m_textQuoteFlag = false;
            }

            postObjects << QSharedPointer<PostPlainText>(new PostPlainText(text));
        }
        else
        {
            if (iChild->isWhitespace()) continue;
            if (iChild->isComment()) continue;

            Q_ASSERT_X(0, Q_FUNC_INFO, "unknown HTML item");
        }
    }
}

QString ForumPageParser::getPostLastEdit(QtGumboNode postEntryNode)
{
    // Read post last edit "credentials" (optional)
    QString lastEditStr;
    QtGumboNode postLastEditNode = postEntryNode.getElementByClass("forum-post-lastedit", HtmlTag::DIV);
    if (!postLastEditNode.isValid()) return QString();

    Q_ASSERT(postLastEditNode.getChildElementCount() == 1);
    Q_ASSERT(postLastEditNode.getClassAttribute() == "forum-post-lastedit");

    QtGumboNode postLastEditSpanNode = postLastEditNode.getElementByClass("forum-post-lastedit", HtmlTag::SPAN);
    Q_ASSERT(postLastEditSpanNode.isValid());
    Q_ASSERT(postLastEditSpanNode.getChildElementCount() >= 2);

    QtGumboNode postLastEditUserNode = postLastEditSpanNode.getElementByClass("forum-post-lastedit-user", HtmlTag::SPAN);
    Q_ASSERT(postLastEditUserNode.isValid());
    Q_ASSERT(postLastEditUserNode.getChildElementCount() == 1);
    QtGumboNode posLastEditUserLinkNode = postLastEditUserNode.getElementByTag({{HtmlTag::UNKNOWN, 0}, {HtmlTag::A, 0}});
    QSharedPointer<PostHyperlink> postLastEditUserLink = parseHyperlink(posLastEditUserLinkNode);

    QString userNameRelStr = postLastEditUserLink->m_rel;
    QString userNameHrefStr = postLastEditUserLink->m_urlStr;
    QString userNameStr = postLastEditUserLink->m_title;

    QtGumboNode postLastEditDateNode = postLastEditSpanNode.getElementByClass("forum-post-lastedit-date", HtmlTag::SPAN);
    Q_ASSERT(postLastEditDateNode.isValid());
    Q_ASSERT(postLastEditDateNode.getChildElementCount() == 0);
    QString lastEditDateStr = postLastEditDateNode.getChildrenInnerText();

    QString lastEditReasonStr;
    QtGumboNode postLastEditReasonNode = postLastEditSpanNode.getElementByClass("forum-post-lastedit-reason", HtmlTag::SPAN);
    if (postLastEditReasonNode.isValid())
    {
        Q_ASSERT(postLastEditReasonNode.getChildElementCount() == 1);
        lastEditReasonStr = postLastEditReasonNode.getChildrenInnerText();
        Q_ASSERT(lastEditReasonStr == "()");
        lastEditReasonStr.clear();

        QtGumboNode reasonSpanNode = postLastEditReasonNode.getElementByTag({HtmlTag::SPAN, 0});
        Q_ASSERT(reasonSpanNode.isValid());
        lastEditReasonStr = "(" + reasonSpanNode.getChildrenInnerText() + ")";
    }

    lastEditStr = "Изменено: <a href=\"" + userNameHrefStr + "\" " + "rel=\"" + userNameRelStr + "\">" + userNameStr + "</a> - " + lastEditDateStr + " " + lastEditReasonStr;

    return lastEditStr;
}

QString ForumPageParser::getPostUserSignature(QtGumboNode postEntryNode)
{
    // Read user signature
    QString userSignatureStr;
    QtGumboNode postSignatureNode = postEntryNode.getElementByClass("forum-user-signature");
    if (!postSignatureNode.isValid()) return QString();

    Q_ASSERT(postSignatureNode.getChildElementCount() == 2);
    Q_ASSERT(postSignatureNode.getClassAttribute() == "forum-user-signature");

    QtGumboNode spanNode = postSignatureNode.getElementByTag({HtmlTag::SPAN, 0});
    Q_ASSERT(spanNode.isValid());
    userSignatureStr = spanNode.getChildrenInnerText();

    // Parse HTML user signatures
    QtGumboNodes children = spanNode.getChildren();
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        switch (iChild->getTag())
        {
        case HtmlTag::B:
        {
            userSignatureStr += "<b>" + iChild->getChildrenInnerText() + "</b><br/>";
            break;
        }
        case HtmlTag::A:
        {
            Q_ASSERT(iChild->getAttributeCount() == 3);

            QString hrefAttrValue = iChild->getAttribute("href");
            QString targetAttrValue = iChild->getAttribute("target");
            QString relAttrValue = iChild->getAttribute("rel");
            userSignatureStr +=
                    "<a href=\"" + hrefAttrValue + "\" "
                    + "target=\"" + targetAttrValue + "\" "
                    + "rel=\"" + relAttrValue + "\"" + ">"  + iChild->getChildrenInnerText() + "</a> ";
            userSignatureStr += "<br/>";
            break;
        }
        case HtmlTag::BR:
        {
            break;
        }
        case HtmlTag::IMG:
        {
            QSharedPointer<PostImage> imageObj = parseImage(*iChild);
            Q_ASSERT(imageObj && imageObj->isValid());

            // FIXME: specify width and height if present
            userSignatureStr += "<img src='" + imageObj->m_url + "'/><br/>";
            break;
        }
        // FIXME: implement other cases
        default: Q_ASSERT(0);
        }
    }
    return userSignatureStr;
}

IPostObjectList ForumPageParser::getPostAttachments(QtGumboNode postEntryNode)
{
    // Read post file attachments
    IPostObjectList result;
    QtGumboNode attachmentsNode = postEntryNode.getElementByClass("forum-post-attachments");
    if (!attachmentsNode.isValid()) return result;

    QtGumboNode labelNode = attachmentsNode.getElementByTag({HtmlTag::LABEL, 0});
    Q_ASSERT(labelNode.isValid()); if (!labelNode.isValid()) return result;
    QString attachmentsLabelStr = labelNode.getChildrenInnerText();

    result << QSharedPointer<PostLineBreak>(new PostLineBreak());
    result << QSharedPointer<PostRichText>(new PostRichText(attachmentsLabelStr, "black", true, false, false, false));
    result << QSharedPointer<PostLineBreak>(new PostLineBreak());

    QtGumboNodes children = attachmentsNode.getElementsByClass("forum-post-attachment", HtmlTag::DIV);
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        QtGumboNode attachNode = iChild->getElementByClass("forum-attach",  HtmlTag::DIV);

        // FIXME: support other attachment types (if exists)
        QtGumboNode imgNode = attachNode.getElementByClass("popup_image", HtmlTag::IMG);
        Q_ASSERT(imgNode.isValid()); if(!imgNode.isValid()) continue;

        result << parseImage(imgNode);
    }

    return result;
}

int ForumPageParser::getLikeCounterValue(QtGumboNode trNode2)
{
    // tr2:
    QtGumboNode contactsNode = trNode2.getElementByClass("forum-cell-contact", HtmlTag::TD);
    Q_ASSERT(contactsNode.isValid());
    Q_ASSERT(contactsNode.getClassAttribute() == "forum-cell-contact");

    QtGumboNode actionsNode = trNode2.getElementByClass("forum-cell-actions", HtmlTag::TD);
    Q_ASSERT(actionsNode.isValid());
    Q_ASSERT(actionsNode.getChildElementCount() == 1);
    Q_ASSERT(actionsNode.getClassAttribute() == "forum-cell-actions");

    // Get the "like" count
    // NOTE: it is type on the site, not my own
    QtGumboNode actionLinksNode = actionsNode.getElementByClass("conainer-action-links", HtmlTag::DIV);
    Q_ASSERT(actionLinksNode.isValid());
    Q_ASSERT(actionLinksNode.getChildElementCount() == 2);
    Q_ASSERT(actionLinksNode.getClassAttribute() == "conainer-action-links");

    QtGumboNode floatLeftNode = actionLinksNode.getElementByClass("float-left", HtmlTag::DIV);
    Q_ASSERT(floatLeftNode.isValid());
    Q_ASSERT(floatLeftNode.getChildElementCount() == 1);
    Q_ASSERT(floatLeftNode.getClassAttribute() == "float-left");

    QtGumboNode likeNode = floatLeftNode.getElementByClass("like", HtmlTag::DIV);
    Q_ASSERT(likeNode.isValid());
    Q_ASSERT(likeNode.getChildElementCount() == 1);
    Q_ASSERT(likeNode.getClassAttribute() == "like");

    QtGumboNode likeWidgetNode = likeNode.getElementByClass("like__widget", HtmlTag::DIV);
    Q_ASSERT(likeWidgetNode.isValid());
    Q_ASSERT(likeWidgetNode.getChildElementCount() >= 2 && likeWidgetNode.getChildElementCount() <= 5);
    Q_ASSERT(likeWidgetNode.getClassAttribute() == "like__widget");

    QtGumboNode likeCounterNode = likeWidgetNode.getElementByClass("like__counter", HtmlTag::SPAN);
    Q_ASSERT(likeCounterNode.isValid());
    Q_ASSERT(likeCounterNode.getChildElementCount() == 0);
    Q_ASSERT(likeCounterNode.getClassAttribute() == "like__counter");
    Q_ASSERT(likeCounterNode.getTextChildrenCount() == 1);

    QString likeCounterStr = likeCounterNode.getChildrenInnerText();
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

void ForumPageParser::findPageCount(QString rawData, int &pageCount)
{
    pageCount = 0;

    const QString PAGES_STR = "pages: ";
    int pagesIdxBegin = rawData.indexOf(PAGES_STR);
    int pagesIdxEnd   = rawData.indexOf(",", pagesIdxBegin);
    Q_ASSERT(pagesIdxBegin >= 0); if (pagesIdxBegin < 0) return;
    Q_ASSERT(pagesIdxEnd > pagesIdxBegin); if (pagesIdxEnd <= pagesIdxBegin) return;
    int pageCountStrSize = pagesIdxEnd - pagesIdxBegin - PAGES_STR.size();
    Q_ASSERT(pageCountStrSize > 0); if (pageCountStrSize <= 0) return;
    QString pageCountStr = rawData.mid(pagesIdxBegin + PAGES_STR.size(), pageCountStrSize);
    bool pageCountOk = false;
    pageCount = pageCountStr.toInt(&pageCountOk);
    Q_ASSERT(pageCountOk); if (!pageCountOk) pageCount = 0;
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
        Q_ASSERT(msdivNode.getChildElementCount() == 1);
        QtGumboNode tbodyNode = msdivNode.getElementByTag({{HtmlTag::TABLE, 0}, {HtmlTag::TBODY, 0}});

        // two tr tags
        int idxTr1 = 0;
        QtGumboNode trNode1 = tbodyNode.getElementByTag({HtmlTag::TR, idxTr1}, &idxTr1);
        int idxTr2 = idxTr1 + 1;
        QtGumboNode trNode2 = tbodyNode.getElementByTag({HtmlTag::TR, idxTr2}, &idxTr2);
        Q_ASSERT(trNode1.isValid());
        Q_ASSERT(trNode2.isValid());
        Q_ASSERT(trNode1.getChildElementCount() == 2);
        Q_ASSERT(trNode2.getChildElementCount() == 2);
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
    QString titleStr = aNode.getChildrenInnerText();

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
    if (imageSrcStr.startsWith("//")) imageSrcStr.prepend("http:");
    else if (!imageSrcStr.startsWith(g_bankiRuHost) && !imageSrcStr.startsWith("http:") && !imageSrcStr.startsWith("https:"))
    {
        imageSrcStr.prepend(g_bankiRuHost);
    }
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
    QtGumboNode theadTrThNode = tableNode.getElementByTag({{HtmlTag::THEAD, 0}, {HtmlTag::TR, 0}, {HtmlTag::TH, 0}});
    Q_ASSERT(theadTrThNode.isValid());
    result->m_title = theadTrThNode.getChildrenInnerText();

    int tbodyTrTdNodeChildIndex = 0;
    QtGumboNode tbodyTrTdNode = tableNode.getElementByTag({{HtmlTag::TBODY, 0}, {HtmlTag::TR, 0}, {HtmlTag::TD, 0}});
    Q_ASSERT(tbodyTrTdNode.isValid());

    // Read the quote author and source: e.g.
    // <b>QWASQ</b> <a href="/forum/?PAGE_NAME=message&FID=22&TID=74420&MID=4453640#message4453640" target="_blank" rel="nofollow">пишет</a>:<br />
    // <b>
    // NOTE: optional
    QtGumboNode tbodyTrTdANode = tbodyTrTdNode.getElementByTag({HtmlTag::A, 0});
    QString tbodyTrTdANodeText = tbodyTrTdANode.isValid() ? tbodyTrTdANode.getChildrenInnerText().trimmed() : QString();
    if (tbodyTrTdANode.isValid() && (tbodyTrTdANodeText.compare(QUOTE_WRITE_VERB, Qt::CaseInsensitive) == 0))
    {
        QtGumboNode tbodyTrTdBNode = tbodyTrTdNode.getElementByTag({HtmlTag::B, 0});
        if (tbodyTrTdBNode.isValid())
        {
            result->m_userName = tbodyTrTdBNode.getChildrenInnerText();
            tbodyTrTdNodeChildIndex++;
        }

        // <a>
        tbodyTrTdNodeChildIndex++;
        Q_ASSERT(tbodyTrTdANode.isValid());
        QString quoteSourceUrl = tbodyTrTdANode.getAttribute("href");
        if (!quoteSourceUrl.startsWith(g_bankiRuHost))
        {
            quoteSourceUrl.replace("/forum/?", g_bankiRuHost + "/forum/?");
        }
        result->m_url = QUrl(quoteSourceUrl);
        Q_ASSERT(result->m_url.isValid());

        // Find the quote body start
        Q_ASSERT(tbodyTrTdNodeChildIndex < tbodyTrTdNode.getChildElementCount(false));
        QtGumboNodes tbodyTrTdChildren = tbodyTrTdNode.getChildren(false);
        for (int i = tbodyTrTdNodeChildIndex; i < tbodyTrTdChildren.size(); ++i)
        {
            QtGumboNode temp = tbodyTrTdChildren[i];
            if (temp.isText())
            {
                QString tempText = temp.getInnerText().trimmed();
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
    Q_ASSERT(tbodyTrTdNodeChildIndex < tbodyTrTdNode.getChildElementCount(false));
    QtGumboNodes tbodyTrTdChildren = tbodyTrTdNode.getChildren(false);

 #ifdef RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "-------------------------------------";
    qDebug() << "Start index:" << tbodyTrTdNodeChildIndex;
    for (int i = 0; i < tbodyTrTdChildren.size(); ++i)
    {
        const QString idxString = i == tbodyTrTdNodeChildIndex ? QString("[ *") + QString::number(i) + QString("* ]")
                                                               : QString("[ ")  + QString::number(i) + QString(" ]");

        if (tbodyTrTdChildren[i].isElement())
            qDebug() << idxString << "Element:" << tbodyTrTdChildren[i].getTagName();
        else if (tbodyTrTdChildren[i].isText())
            qDebug() << idxString << "Text" << tbodyTrTdChildren[i].getInnerText();
        else if (!tbodyTrTdChildren[i].isComment() && !tbodyTrTdChildren[i].isWhitespace())
            qDebug() << idxString << "Unknown item:";
    }
    qDebug() << "-------------------------------------";
#endif

    parseMessage(tbodyTrTdChildren.mid(tbodyTrTdNodeChildIndex), result->m_data);
    return result;
}

QSharedPointer<PostSpoiler> ForumPageParser::parseSpoiler(QtGumboNode tableNode) const
{
    QSharedPointer<PostSpoiler> result(new PostSpoiler);

    // Read the quote title
    QtGumboNode theadTrThNode = tableNode.getElementByTag({{HtmlTag::THEAD, 0}, {HtmlTag::TR, 0}, {HtmlTag::TH, 0}, {HtmlTag::DIV, 0}});
    Q_ASSERT(theadTrThNode.isValid()); if (!theadTrThNode.isValid()) return QSharedPointer<PostSpoiler>();
    result->m_title = theadTrThNode.getChildrenInnerText();
    Q_ASSERT(result->m_title[result->m_title.size()-1] == QChar(9650).unicode()
          || result->m_title[result->m_title.size()-1] == QChar(9660).unicode());
    if ((result->m_title[result->m_title.size()-1] != QChar(9650).unicode())
     && (result->m_title[result->m_title.size()-1] != QChar(9660).unicode())) return QSharedPointer<PostSpoiler>();
    result->m_title = result->m_title.remove(result->m_title.size()-1, 1);
    result->m_title = result->m_title.trimmed();

    QtGumboNode tbodyTrTdNode = tableNode.getElementByTag({{HtmlTag::TBODY, 0}, {HtmlTag::TR, 0}, {HtmlTag::TD, 0}});
    Q_ASSERT(tbodyTrTdNode.isValid());

    // Read the spoiler body
    // NOTE: spoiler text is HTML too
    QtGumboNodes tbodyTrTdChildren = tbodyTrTdNode.getChildren(false);

 #ifdef RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "-------------------------------------";
    for (int i = 0; i < tbodyTrTdChildren.size(); ++i)
    {
        const QString idxString = QString("[ ")  + QString::number(i) + QString(" ]");

        if (tbodyTrTdChildren[i].isElement())
            qDebug() << idxString << "Element:" << tbodyTrTdChildren[i].getTagName();
        else if (tbodyTrTdChildren[i].isText())
            qDebug() << idxString << "Text" << tbodyTrTdChildren[i].getInnerText();
        else if (!tbodyTrTdChildren[i].isComment() && !tbodyTrTdChildren[i].isWhitespace())
            qDebug() << idxString << "Unknown item:";
    }
    qDebug() << "-------------------------------------";
#endif

    parseMessage(tbodyTrTdChildren, result->m_data);
    return result;
}

// IForumPageReader implementation ////////////////////////////////////////////

namespace
{
QByteArray convertHtmlToUft8(QByteArray rawHtmlData)
{
    QByteArray result;

    QTextCodec* htmlCodec = QTextCodec::codecForHtml(rawHtmlData);
    Q_ASSERT(htmlCodec); if (!htmlCodec) return QByteArray();
#ifdef RBR_PRINT_DEBUG_OUTPUT
    qDebug() << "HTML encoding/charset is:" << htmlCodec->name();
#endif
    QString resultStr = htmlCodec->toUnicode(rawHtmlData);
    result = resultStr.toUtf8();
    return result;
}
}

int ForumPageParser::getPageCount(QByteArray rawData, int &pageCount)
{
    QByteArray utfData = convertHtmlToUft8(rawData);
    if (utfData.isEmpty()) { Q_ASSERT(0); return 0; }

    // Parse page count
    findPageCount(utfData, pageCount);

    // TODO: implement error handling with different return code
    return 0;
}

int ForumPageParser::getPagePosts(QByteArray rawData, UserPosts &userPosts)
{
    QByteArray utfData = convertHtmlToUft8(rawData);

    // Parse web page contents
    GumboOutput* output = gumbo_parse(utfData.constData());
    fillPostList(output->root, userPosts);
    gumbo_destroy_output(&kGumboDefaultOptions, output);

    // TODO: implement error handling with different return code
    return 0;
}
///////////////////////////////////////////////////////////////////////////////

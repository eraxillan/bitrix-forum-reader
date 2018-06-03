#ifndef __BFR_WEBSITEINTERFACE_FWD_H__
#define __BFR_WEBSITEINTERFACE_FWD_H__

#include <QtCore/QList>
#include <QtCore/QSharedPointer>

namespace bfr
{

struct IPostObject;
using IPostObjectPtr = QSharedPointer<IPostObject>;
using IPostObjectList = QList<IPostObjectPtr>;

struct PostSpoiler;
using PostSpoilerPtr = QSharedPointer<PostSpoiler>;

struct PostQuote;
using PostQuotePtr = QSharedPointer<PostQuote>;

struct PostImage;
using PostImagePtr = QSharedPointer<PostImage>;

struct PostLineBreak;
using PostLineBreakPtr = QSharedPointer<PostLineBreak>;

struct PostPlainText;
using PostPlainTextPtr = QSharedPointer<PostPlainText>;

struct PostRichText;
using PostRichTextPtr = QSharedPointer<PostRichText>;

struct PostVideo;
using PostVideoPtr = QSharedPointer<PostVideo>;

struct PostHyperlink;
using PostHyperlinkPtr = QSharedPointer<PostHyperlink>;

// ----------------------------------------------------------------------------------------------------------------

struct Post;
using PostPtr = QSharedPointer<Post>;

struct User;
using UserPtr = QSharedPointer<User>;

using UserPost = QPair<UserPtr, PostPtr>;
using UserPosts = QVector<UserPost>;
using PageUserPosts = QVector<UserPosts>;

}  // namespace bfr

#endif // __BFR_WEBSITEINTERFACE_FWD_H__

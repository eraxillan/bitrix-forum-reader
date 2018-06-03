#include "catch.hpp"

#include "common/filedownloader.h"
#include "website_backend/gumboparserimpl.h"

static const QString g_forumFirstPageUrl("http://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=74420&PAGEN_1=1#forum-message-list");

//---------------------------------------------------------------------------------------------------------------------------------------

TEST_CASE("Get forum page count", "[FileDownloader][ForumPageParser]") {

    SECTION("Validating forum first page URL") {
        REQUIRE(!g_forumFirstPageUrl.isEmpty());
        REQUIRE(QUrl(g_forumFirstPageUrl).isValid());

        SECTION("Downloading forum first page contents") {
            QByteArray htmlRawData;
            REQUIRE(FileDownloader::downloadUrl(g_forumFirstPageUrl, htmlRawData));
            REQUIRE(!htmlRawData.isEmpty());

            SECTION("Parsing forum first page contents to get page count") {
                int pageCount = -1;
                bfr::ForumPageParser fpp;
                REQUIRE(fpp.getPageCount(htmlRawData, pageCount) == result_code::Type::Ok);
                REQUIRE(pageCount > 0);
                INFO("Forum page count: " << pageCount);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------

TEST_CASE("Get forum page posts", "[FileDownloader][ForumPageParser]") {
    REQUIRE(!g_forumFirstPageUrl.isEmpty());
    REQUIRE(QUrl(g_forumFirstPageUrl).isValid());

    SECTION("Downloading forum first page contents") {
        QByteArray htmlRawData;
        REQUIRE(FileDownloader::downloadUrl(g_forumFirstPageUrl, htmlRawData));
        REQUIRE(!htmlRawData.isEmpty());

        SECTION("Parsing forum first page contents to get page count") {
            int pageCount = -1;
            bfr::ForumPageParser fpp;
            REQUIRE(fpp.getPageCount(htmlRawData, pageCount) == result_code::Type::Ok);
            REQUIRE(pageCount > 0);
            INFO("Forum page count: " << pageCount);

            SECTION("Parsing forum page posts") {
                for (int i = 1; i <= pageCount; ++i) {
                    INFO("Forum page number: " << i);
                    bfr::UserPosts userPosts;
                    REQUIRE(fpp.getPagePosts(htmlRawData, userPosts) == result_code::Type::Ok);
                    REQUIRE(!userPosts.isEmpty());

                    // FIXME: investigate whether section is required here
                    /*SECTION("Validating forum page posts") */{
                        for (int j = 1; j <= userPosts.size(); ++j) {
                            INFO("Forum post number: " << j);
                            REQUIRE(userPosts[j-1].first->isValid());
                            REQUIRE(userPosts[j-1].second->isValid());
                        }
                    }
                }
            }
        }
    }
}

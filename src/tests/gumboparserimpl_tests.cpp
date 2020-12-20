/*
 * This file is part of Bitrix Forum Reader.
 *
 * Copyright (C) 2016-2020 Alexander Kamyshnikov <axill777@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#ifdef Q_OS_WINDOWS
#define CATCH_CONFIG_COLOUR_WINDOWS
#elif defined(Q_OS_UNIX)
#define CATCH_CONFIG_COLOUR_ANSI
#else
#define CATCH_CONFIG_COLOUR_NONE
#endif
#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <common/filedownloader.h>
#include <website_backend/gumboparserimpl.h>

static const QString g_forumFirstPageUrl("https://www.banki.ru/forum/?PAGE_NAME=read&FID=22&TID=358149");

//---------------------------------------------------------------------------------------------------------------------------------------

TEST_CASE("Serialization of forum page posts to file", "[ForumReader]") {
	SECTION("Equality operators check") {
		bfr::PostPtr p1(new bfr::Post);
		bfr::PostPtr p2(new bfr::Post);
		bool b1 = (p1 == p2);
		bool b2 = (*p1 == *p2);
		REQUIRE(!b1);
		REQUIRE(b2);
	}

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
					bfr::PostList userPosts;
					REQUIRE(fpp.getPagePosts(htmlRawData, userPosts) == result_code::Type::Ok);
					REQUIRE(!userPosts.isEmpty());

#ifdef BFR_SERIALIZATION_ENABLED
					SECTION("Serialize and deserialize data") {
						REQUIRE(result_code::succeeded(bfr::serializePosts(userPosts)));

						bfr::PostList userPosts_2;
						REQUIRE(result_code::succeeded(bfr::deserializePosts(userPosts_2)));

						REQUIRE(userPosts != userPosts_2);
						REQUIRE(comparePostLists(userPosts, userPosts_2));
					}
#endif
				}
			}
		}
	}
}

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
					bfr::PostList userPosts;
					REQUIRE(fpp.getPagePosts(htmlRawData, userPosts) == result_code::Type::Ok);
					REQUIRE(!userPosts.isEmpty());

					// FIXME: investigate whether section is required here
					/*SECTION("Validating forum page posts") */ {
						for (int j = 1; j <= userPosts.size(); ++j) {
							INFO("Forum post number: " << j);
							REQUIRE(userPosts[j - 1]->m_author->isValid());
							REQUIRE(userPosts[j - 1]->isValid());
						}
					}
				}
			}
		}
	}
}

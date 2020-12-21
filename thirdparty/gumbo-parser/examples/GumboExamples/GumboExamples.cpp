// GumboExamples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#if defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS
#endif // defined(_WIN32) || defined(_WIN64)


#include "C:/Projects/gumbo-parser/src/gumbo.h"
#ifdef _WIN64
#pragma comment( lib, "C:/Projects/gumbo-parser/visualc/x64/Debug/gumbo.lib" )
#elif _WIN32
#pragma comment( lib, "C:/Projects/gumbo-parser/visualc/Debug/gumbo.lib" )
#endif

static std::string gumboElementTypeToString(GumboNodeType gnt)
{
	switch (gnt)
	{
	case GUMBO_NODE_DOCUMENT: return std::string("document");
	case GUMBO_NODE_ELEMENT: return std::string("element");
	case GUMBO_NODE_TEXT: return std::string("text");
	case GUMBO_NODE_CDATA: return std::string("cdata");
	case GUMBO_NODE_COMMENT: return std::string("comment");
	case GUMBO_NODE_WHITESPACE: return std::string("whitespace");
	case GUMBO_NODE_TEMPLATE: return std::string("template");
	default: assert(0); return std::string();
	}
}

//----------------------------------------------------------------------------------------------------------

// FIXME: save full post history to the LOCAL database
// FIXME: add sorting by user/post reputation option

namespace BankiRuForum
{
	struct Post
	{
		int m_id;
		int m_postNumber;
		int m_likeCounter;

		bool m_isQuote;
		// Quote properties
		std::string m_author;	// QString, needed for quotes
		// Normal post properties
		std::string m_text;	// QString
		std::string m_style;	// QString
		std::string m_userSignature;	// QString
		std::string m_date; // QDate
		std::string m_permalink;	// QUrl

		std::vector<Post> m_embedQuotes;	// QVector
	};

	struct User
	{
		std::string m_userName;
		std::string m_userAvatarUrl;
		int m_postCount;
		std::string m_registerDate; // QDate
		int m_reputation;
	};
}

//----------------------------------------------------------------------------------------------------------

class ForumPageParser
{
public:
	// forum-post-entry stuff -------------------------

	void parseForumPostEntry(GumboNode* node)
	{
		assert(node->type == GUMBO_NODE_ELEMENT);
		assert(node->v.element.tag == GUMBO_TAG_DIV);

		GumboAttribute* classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
		assert(classAttr != NULL);
		assert(strCiCmp(classAttr->value, "forum-post-entry"));

		GumboVector* nodeChildren = &node->v.element.children;
		int fpeElementCount = 0;
		for (std::size_t i = 0; i < nodeChildren->length;i++)
		{
			GumboNode* divChildNode = static_cast<GumboNode*>(nodeChildren->data[i]);
			assert(divChildNode != NULL);
			if (divChildNode->type != GUMBO_NODE_ELEMENT) continue;

			fpeElementCount++;
			assert(divChildNode->v.element.tag == GUMBO_TAG_DIV);

			classAttr = gumbo_get_attribute(&divChildNode->v.element.attributes, "class");
			if (strCiCmp(classAttr->value, "forum-post-text"))
			{
				parseForumPostText(divChildNode);
			}
			else if (strCiCmp(classAttr->value, "forum-post-attachments"))
			{
				parseForumPostAttachments(divChildNode);
			}
			else if (strCiCmp(classAttr->value, "forum-post-lastedit"))
			{
				parseForumPostLastEdit(divChildNode);
			}
			else if (strCiCmp(classAttr->value, "forum-user-signature"))
			{
				parseForumUserSignature(divChildNode);
			}
			else
			{
				assert(0);
			}
		}
		assert(fpeElementCount <= 3);
	}

	void parseForumPostText(GumboNode* node)
	{
		assert(node->type == GUMBO_NODE_ELEMENT);
		assert(node->v.element.tag == GUMBO_TAG_DIV);

		GumboAttribute* classAttr = gumbo_get_attribute(&node->v.element.attributes, "class");
		assert(classAttr != NULL);
		assert(strCiCmp(classAttr->value, "forum-post-text"));

		GumboAttribute* idAttr = gumbo_get_attribute(&node->v.element.attributes, "id");
		assert(idAttr != NULL);
		std::string idStr(idAttr->value);
		std::size_t messageTextStrIndex = idStr.find("message_text_");
		assert(messageTextStrIndex != std::string::npos);

		std::string messageIdStr = idStr.substr(13);
		std::cout << "Forum post found, id: " << messageIdStr << std::endl;

		// FIXME: remove after debug completion
		if (!strCiCmp(messageIdStr, "4453758"))return;

		std::string forumPostText = "";
		GumboVector* nodeChildren = &node->v.element.children;
		for (std::size_t i = 0; i < nodeChildren->length; i++)
		{
			GumboNode* childNode = static_cast<GumboNode*>(nodeChildren->data[i]);
			assert(childNode != NULL);
			if (childNode->type == GUMBO_NODE_ELEMENT)
			{
				// FIXME: parse message quotes (<table> tags)

				std::cout << "Tag found: " << gumbo_normalized_tagname(childNode->v.element.tag);
				std::cout << ", children: " << childNode->v.element.children.length << std::endl;
				//
			}
			else if (childNode->type == GUMBO_NODE_TEXT)
			{
				std::string elementTextUtf8(childNode->v.text.text);
				std::string elementTextCp1251 = "";
				utf8ToCp1251(elementTextCp1251, elementTextUtf8);
				forumPostText += elementTextCp1251;
//				std::cout << "Forum message text: " << elementTextCp1251 << std::endl;
			}
			else std::cout << "Ignoring \"" << gumboElementTypeToString(childNode->type) << "\" node..." << std::endl;

//			classAttr = gumbo_get_attribute(&childNode->v.element.attributes, "class");
		}

		std::cout << "Forum post: " << std::endl << forumPostText << std::endl;
	}

	void parseForumPostAttachments(GumboNode* node)
	{
		//
	}

	void parseForumPostLastEdit(GumboNode* node)
	{
		//
	}

	void parseForumUserSignature(GumboNode* node)
	{
		//
	}

	// ------------------------------------------------
};

// TODO: case-insensitive comparison
static void searchForDivBlocks(GumboNode* node)
{
	if (node->type != GUMBO_NODE_ELEMENT) return;

	// <div class="forum-post-entry">
	GumboAttribute* classAttr = NULL;
	if ((node->v.element.tag == GUMBO_TAG_DIV) && (classAttr = gumbo_get_attribute(&node->v.element.attributes, "class")))
	{
		std::string classAttrStr(classAttr->value);
		if (classAttrStr == "forum-post-entry" )
		{
			// <div class="forum-post-text" id="message_text_4453758">
			GumboNode* fpeNode = node;

			ForumPageParser fpp;
			fpp.parseForumPostEntry(fpeNode);

			for (unsigned int i = 0; i < fpeNode->v.element.children.length; ++i)
			{
#if 0
				GumboNode* fpeChildNode = static_cast< GumboNode* >(fpeNode->v.element.children.data[i]);
				if (fpeChildNode->type != GUMBO_NODE_ELEMENT) continue;
				assert(fpeChildNode->v.element.tag == GUMBO_TAG_DIV);
				if (fpeChildNode->v.element.tag != GUMBO_TAG_DIV) continue;

				std::cout << "    FPE children tags:" << gumbo_normalized_tagname(fpeChildNode->v.element.tag) << std::endl;
				for (int iAttr = 0; iAttr < fpeChildNode->v.element.attributes.length;++iAttr)
				{
					GumboAttribute* attr = static_cast< GumboAttribute* >( fpeChildNode->v.element.attributes.data[iAttr] );
					assert(attr != NULL);
					std::cout << "ATTR: " << attr->name << " = " << attr->value << std::endl;
				}

				if (fpeChildNode->v.element.tag == GUMBO_TAG_DIV)
				{
					GumboAttribute* fpeChildNodeClassAttr = gumbo_get_attribute(&fpeChildNode->v.element.attributes, "class");
					assert(fpeChildNodeClassAttr != NULL);
					std::string fpeChildNodeClassAttrStr(fpeChildNodeClassAttr->value);
					if (fpeChildNodeClassAttrStr == "forum-post-text")
					{
						GumboNode* fptNode = fpeChildNode;
						assert(fptNode->type == GUMBO_NODE_ELEMENT);
//                       qDebug() << "  forum-post-text children count: " << fptNode->v.element.children.length;

						GumboAttribute* fptNodeIdAttr = gumbo_get_attribute(&fptNode->v.element.attributes, "id");
						assert(fptNodeIdAttr != NULL);

						//std::cout << "    Forum post found: " << fptNodeIdAttr->value << std::endl;

						// FIXME: remove after debugging
						if (std::strcmp(fptNodeIdAttr->value, "message_text_4453758") != 0) continue;

						GumboVector* fptNodeChildren = &fptNode->v.element.children;
						for (unsigned int j = 0; j < fptNodeChildren->length; ++j)
						{
							GumboNode* fptNodeChild = static_cast< GumboNode* >(fptNodeChildren->data[j]);
							std::cout << "Element type: " << gumboElementTypeToString( fptNodeChild->type ) << std::endl;

							if (fptNodeChild->type == GUMBO_NODE_TEXT)
							{
								/*std::locale loc(std::locale(), new std::codecvt_utf8<char>);
								std::cout.imbue(loc);*/
								std::string TEST(fptNodeChild->v.text.text);

								WCHAR wBuf[1024] = { 0 };
								int res2 = MultiByteToWideChar(CP_UTF8, 0, fptNodeChild->v.text.text, -1, wBuf, 1024);
								char mbcsBuf[1024] = { 0 };
								int res3 = WideCharToMultiByte(1251, 0, wBuf, 1024, mbcsBuf, 1024, NULL, NULL);
								std::string TEST_2(mbcsBuf);
								std::cout << "Element text: " << TEST_2 << std::endl;

								break;
							}
							else if (fptNodeChild->type == GUMBO_NODE_ELEMENT)
							{
								std::cout << "    " << gumbo_normalized_tagname(fptNodeChild->v.element.tag) << std::endl;
								//qDebug() << "     " << gumbo_tag_from_original_text( &fptNodeChild->v.element.original_tag );
							}
						}
					}
				}
#endif
			}

			std::cout << "----------------------------" << std::endl;
		}
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i)
	{
		searchForDivBlocks(static_cast< GumboNode* >(children->data[i]));
	}
}

int main(int argc, char *argv[])
{
//	system("chcp 65001");
	system("chcp 1251");

	// Read HTML file contents
	std::string htmlFileName = "C:\\Projects\\__DATA\\test_page_30.html";
	std::ifstream htmlFileStream(htmlFileName, std::ios::in | std::ios::binary);
	if (!htmlFileStream)
	{
		std::cout << "File " << htmlFileName << " not found!";
		return( EXIT_FAILURE );
	}

	std::string htmlFileContents;
	htmlFileStream.seekg(0, std::ios::end);
	htmlFileContents.resize(htmlFileStream.tellg());
	htmlFileStream.seekg(0, std::ios::beg);
	htmlFileStream.read(&htmlFileContents[0], htmlFileContents.size());
	htmlFileStream.close();

	// Convert it to UTF-8
	// NOTE: Gumbo works ONLY with UTF-8 documents
	char* bufUtf8 = new char[htmlFileContents.length() * 2];
	memset(bufUtf8, 0, htmlFileContents.length() * 2);
	cp1251ToUtf8(bufUtf8, htmlFileContents.c_str());
//	delete[] bufUtf8;
//	return 0;

	// Parse web page contents
	GumboOutput* output = gumbo_parse(/*htmlFileContents.c_str()*/ bufUtf8 );
//    search_for_links(output->root);
	searchForDivBlocks(output->root);

	gumbo_destroy_output(&kGumboDefaultOptions, output);

	delete[] bufUtf8;
	_getchar_nolock();
	return 0;
}

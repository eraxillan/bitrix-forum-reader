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
#ifndef __BFR_QTGUMBONODE_H__
#define __BFR_QTGUMBONODE_H__

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QTextCodec>

#include <memory>

#include <gumbo-parser/src/gumbo.h>

#include "html_tag.h"

class QtGumboNode;
using QtGumboNodePtr = std::shared_ptr<QtGumboNode>;
using QtGumboNodes = QVector<QtGumboNodePtr>;
using QtGumboNodePathItem = QPair<QString, size_t>;
using QtGumboNodePath = QList<QtGumboNodePathItem>;

enum class QtGumboNodeType { Invalid = -1, Document = 0, Element = 1, Text, CDATA, Comment, Whitespace, Template, Count };

#ifdef QT_GUMBO_METADATA
using QtStringMap = QMap<QString, QString>;

struct QtGumboNodeProps {
	bool m_isValid = false;

	QtGumboNodeType m_type;
	QtGumboNodePath m_path;

	QtGumboNodePtr m_parent;
	size_t m_parentIndex;

	HtmlTag m_tag = HtmlTag::UNKNOWN;
	QString m_tagName;
	QString m_html;

	QtStringMap m_attributes;
	QString m_id;
	QString m_class;

	// Text node only
	QString m_text;

	// Element only
	QString m_childrenText;

	QtGumboNodes m_children;
	QtGumboNodes m_elementChildren;
	QtGumboNodes m_textChildren;
};

using QtGumboNodePropsPtr = std::shared_ptr<QtGumboNodeProps>;
#endif

class QtGumboNode {
	GumboNode *m_node;

#ifdef QT_GUMBO_METADATA
	QtGumboNodePropsPtr m_props;
#endif

public:
	QtGumboNode();
	QtGumboNode(GumboNode *node);

#ifdef QT_GUMBO_METADATA
	void fillMetadata(QtGumboNodePropsPtr props);
	void setMetadata(QtGumboNodePropsPtr props) { m_props = props; }
#endif

	// Delete copy and move constructors and assign operators
	QtGumboNode(QtGumboNode const &) = delete; // Copy construct
	QtGumboNode(QtGumboNode &&) = delete; // Move construct
	QtGumboNode &operator=(QtGumboNode const &) = delete; // Copy assign
	QtGumboNode &operator=(QtGumboNode &&) = delete; // Move assign

	bool isValid() const;
	bool isWhitespace() const;
	bool isComment() const;
	bool isElement() const;
	bool isText() const;
	bool isDocument() const;

	QtGumboNodeType getType() const;

	QtGumboNodePtr getParent() const;
	size_t getParentIndex() const;

	QtGumboNodePath getPath() const;

	HtmlTag getTag() const;
	QString getTagName() const;

	bool hasAttribute(QString name) const;
	bool hasIdAttribute() const;
	bool hasClassAttribute() const;
	size_t getAttributeCount() const;
	QString getAttribute(QString name) const;
	QString getIdAttribute() const;
	QString getClassAttribute() const;

	QtGumboNodes getChildren(bool elementsOnly = true) const;
	QtGumboNodes getTextChildren() const;
	int getChildElementCount(bool elementsOnly = true) const;
	int getTextChildrenCount() const;

	QString getInnerText() const;
	QString getChildrenInnerText();

	// Non-recursive(!) search for the first child node with specified tag, from specified position (index of child)
	QtGumboNodePtr getElementByTag(std::pair<HtmlTag, int> tagDesc, int *foundPos = nullptr);
	QtGumboNodePtr getElementByTag(std::initializer_list<std::pair<HtmlTag, int>> tagDescs, int *foundPos = nullptr);

	// Non-recursive(!) search for the first child node with specified class name and tag (div by default)
	QtGumboNodePtr getElementByClass(QString className, HtmlTag childTag = HtmlTag::DIV) const;
	QtGumboNodes getElementsByClass(QString className, HtmlTag childTag = HtmlTag::DIV) const;

	// Recursive search for the first child node with specified class name and tag (div by default)
	QtGumboNodes getElementsByClassRecursive(QString className, HtmlTag childTag = HtmlTag::DIV) const;

	// Tag text and position in raw HTML text
	size_t getTagLength() const;
	size_t getStartPos() const;
	size_t getEndPos() const;
	QString getHtml() const;
};

class QtGumboNodePool
{
protected:
	using NodeMap = QMap<GumboNode *, QtGumboNodePtr>;
	NodeMap m_nodes;

#ifdef QT_GUMBO_METADATA
	using MetainfoMap = QMap<GumboNode *, QtGumboNodePropsPtr>;
	MetainfoMap m_nodeProps;
#endif

	QtGumboNodePool() { }
	~QtGumboNodePool() { }

public:
	static QtGumboNodePool &globalInstance();

	// Delete copy and move constructors and assign operators
	QtGumboNodePool(QtGumboNodePool const &) = delete; // Copy construct
	QtGumboNodePool(QtGumboNodePool &&) = delete; // Move construct
	QtGumboNodePool &operator=(QtGumboNodePool const &) = delete; // Copy assign
	QtGumboNodePool &operator=(QtGumboNodePool &&) = delete; // Move assign

	QtGumboNodePtr getNode(GumboNode *node);
};

#endif // __BFR_QTGUMBONODE_H__

#ifndef QTGUMBONODE_H
#define QTGUMBONODE_H

#include <QtCore/QtCore>

#include "gumbo-parser/src/gumbo.h"
#include "html_tag.h"

//#define QT_GUMBO_METADATA

class QtGumboNode;
typedef QVector<QtGumboNode> QtGumboNodes;

#ifdef QT_GUMBO_METADATA
typedef QMap<QString, QString> QtStringMap;

struct QtGumboNodeProps
{
    bool m_isValid = false;
    bool m_isElement = false;
    bool m_isText = false;

    HtmlTag m_tag = HtmlTag::UNKNOWN;
    QString m_tagName;
    QString m_html;

    QtStringMap m_attributes;
    QString m_id;
    QString m_class;

    // m_isText == true
    QString m_text;
    QString m_childrenText;

    QtGumboNodes m_children;
    QtGumboNodes m_elementChildren;
    QtGumboNodes m_textChildren;
};
#endif

class QtGumboNode
{
    GumboNode* m_node = nullptr;

#ifdef QT_GUMBO_METADATA
    QtGumboNodeProps m_props;
#endif

public:
    QtGumboNode();
    QtGumboNode(GumboNode *node);

    bool isValid() const;
    bool isElement() const;
    bool isText() const;
    bool isWhitespace() const;
    bool isComment() const;

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
    QtGumboNode getElementByTag(std::pair< HtmlTag, int > tagDesc, int* foundPos = nullptr);
    QtGumboNode getElementByTag(std::initializer_list< std::pair< HtmlTag, int > > tagDescs, int *foundPos = nullptr);

    // Non-recursive(!) search for the first child node with specified class name and tag (div by default)
    QtGumboNode getElementByClass(QString className, HtmlTag childTag = HtmlTag::DIV) const;
    QtGumboNodes getElementsByClass(QString className, HtmlTag childTag = HtmlTag::DIV) const;

    // Recursive search for the first child node with specified class name and tag (div by default)
    QtGumboNodes getElementsByClassRecursive(QString className, HtmlTag childTag = HtmlTag::DIV) const;

    // Tag text and position in raw HTML text
    size_t getTagLength() const;
    size_t getStartPos() const;
    size_t getEndPos() const;
    QString getHtml() const;
};

#endif // QTGUMBONODE_H

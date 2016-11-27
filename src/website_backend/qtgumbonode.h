#ifndef QTGUMBONODE_H
#define QTGUMBONODE_H

#include <QtCore/QtCore>

#include "gumbo.h"
#include "html_tag.h"

class QtGumboNode;
typedef QVector<QtGumboNode> QtGumboNodes;

class QtGumboNode
{
    GumboNode* m_node;

public:
    QtGumboNode();
    QtGumboNode(GumboNode *node);

    bool isValid() const;
    bool isElement() const;
    bool isText() const;

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

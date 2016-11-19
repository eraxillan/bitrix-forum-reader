#ifndef QTGUMBONODE_H
#define QTGUMBONODE_H

#include <QtCore/QtCore>

#include "gumbo.h"
#include "html_tag.h"

class QtGumboNode
{
    GumboNode* m_node;

public:
    typedef QVector<QtGumboNode> QtGumboNodes;

    QtGumboNode();

    QtGumboNode(GumboNode* node);

    bool isValid() const;
    bool isElement() const;
    bool isText() const;

    HtmlTag getTag() const;
    QString getTagName() const;

    // NOTE: attribute name cannot contain non-latin charecters
    bool hasAttribute(QString name) const;

    QString getAttribute(QString name) const;

    bool hasIdAttribute() const;
    bool hasClassAttribute() const;
    QString getIdAttribute() const;
    QString getClassAttribute() const;
    size_t getAttributeCount() const;

    QtGumboNodes getChildren(bool elementsOnly = true) const;
    int gumboChildElementCount(bool elementsOnly = true) const;
    int gumboChildTextNodeCount();

    QString getText() const;

    QString gumboChildTextNodeValue();

    // Non-recursive(!) search for the first child node with specified tag, from specified position (index of child)
    QtGumboNode childNodeByTag(std::pair< HtmlTag, int > tagDesc, int* foundPos = nullptr);
    QtGumboNode childNodeByTag(std::initializer_list< std::pair< HtmlTag, int > > tagDescs, int *foundPos = nullptr);

    // Non-recursive(!) search for the first child node with specified class name and tag (div by default)
    QtGumboNode gumboChildNodeByClass(QString className, HtmlTag childTag = HtmlTag::DIV) const;

    // Recursive search for the first child node with specified class name and tag (div by default)
    QtGumboNodes gumboChildNodesByClassRecursive(QString className, HtmlTag childTag = HtmlTag::DIV);

    // FIXME: deprecated! will be unneeded after full HTML parsing implemented (post body too)
    /*size_t getTagLength() const;
    size_t getStartPos() const;
    size_t getEndPos() const;
    QString getHtml() const;*/
};

typedef QtGumboNode::QtGumboNodes QtGumboNodes;

#endif // QTGUMBONODE_H

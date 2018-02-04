#ifndef QTGUMBONODE_H
#define QTGUMBONODE_H

#include <QtCore/QtCore>

#include "gumbo-parser/src/gumbo.h"
#include "html_tag.h"

class QtGumboNode;
using QtGumboNodePtr = std::shared_ptr<QtGumboNode>;
using QtGumboNodes = QVector<QtGumboNodePtr>;

//#define QT_GUMBO_DEBUG

class QtGumboNode
{
public:
    enum class Type { Invalid = -1, Document = 0, Element = 1, Text, CDATA, Comment, Whitespace, Template, Count };

    using NodePathItem = QPair<QString, size_t>;
    using NodePath = QList<NodePathItem>;

private:
    GumboNode *m_node;

#ifdef QT_GUMBO_DEBUG
    Type m_type;
    NodePath m_path;

    QtGumboNodePtr m_parent;
    size_t m_parentIndex;

    // Element only
    //QtGumboNodes m_children;
    QString m_tagName;
    QString m_html;
    QString m_idAttr;
    QString m_classAttr;
#endif

public:
    QtGumboNode();
    QtGumboNode(GumboNode *node);

    // FIXME: implement copy ctor and operator, assignment operator

    bool isValid() const;
    bool isWhitespace() const;
    bool isElement() const;
    bool isText() const;
    bool isDocument() const;

    Type getType() const;

    QtGumboNodePtr getParent() const;
    size_t getParentIndex() const;

    NodePath getPath() const;

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
    QtGumboNodePtr getElementByTag(std::pair< HtmlTag, int > tagDesc, int* foundPos = nullptr);
    QtGumboNodePtr getElementByTag(std::initializer_list< std::pair< HtmlTag, int > > tagDescs, int *foundPos = nullptr);

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

#endif // QTGUMBONODE_H

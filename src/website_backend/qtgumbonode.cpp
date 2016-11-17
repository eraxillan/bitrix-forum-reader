#include "qtgumbonode.h"


QtGumboNode::QtGumboNode() : m_node(nullptr)
{
}

QtGumboNode::QtGumboNode(GumboNode *node) : m_node(node)
{
}

bool QtGumboNode::isValid() const { return m_node != nullptr; }

bool QtGumboNode::isElement() const { return m_node->type == GUMBO_NODE_ELEMENT; }

bool QtGumboNode::isText() const { return m_node->type == GUMBO_NODE_TEXT; }

HtmlTag QtGumboNode::getTag() const { return HtmlTag(m_node->v.element.tag); }

QString QtGumboNode::getTagName() const { return QString(gumbo_normalized_tagname(m_node->v.element.tag)); }

bool QtGumboNode::hasAttribute(QString name) const
{
    GumboAttribute* attr = gumbo_get_attribute(&m_node->v.element.attributes, name.toLatin1().constData());
    return (attr != nullptr);
}

QString QtGumboNode::getAttribute(QString name) const
{
    QString result;
    GumboAttribute* attr = gumbo_get_attribute(&m_node->v.element.attributes, name.toLatin1().constData());
    if (attr) result = QString::fromUtf8(attr->value);
    return result;
}

bool QtGumboNode::hasIdAttribute() const { return hasAttribute("id"); }

bool QtGumboNode::hasClassAttribute() const { return hasAttribute("class"); }

QString QtGumboNode::getIdAttribute() const { return getAttribute("id"); }

QString QtGumboNode::getClassAttribute() const { return getAttribute("class"); }

size_t QtGumboNode::getAttributeCount() const { return m_node->v.element.attributes.length; }

QtGumboNode::QtGumboNodes QtGumboNode::getChildren(bool elementsOnly) const
{
    QtGumboNodes result;

    GumboVector* children = &m_node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(children->data[i]);
        if (elementsOnly && (childNode->type != GUMBO_NODE_ELEMENT)) continue;

        result << QtGumboNode(childNode);
    }

    return result;
}

int QtGumboNode::gumboChildElementCount(bool elementsOnly) const
{
    return getChildren(elementsOnly).size();
}

int QtGumboNode::gumboChildTextNodeCount()
{
    int textNodeCount = 0;
    GumboVector* nodeChildren = &m_node->v.element.children;
    for (unsigned int i = 0; i < nodeChildren->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(nodeChildren->data[i]);
        if (childNode->type == GUMBO_NODE_TEXT) textNodeCount++;
    }
    return textNodeCount;
}

QString QtGumboNode::getText() const
{
    Q_ASSERT(isText()); if (!isText()) return QString();

    return QString::fromUtf8(m_node->v.text.text);
}

QString QtGumboNode::gumboChildTextNodeValue()
{
    QString value;

    GumboVector* nodeChildren = &m_node->v.element.children;
    for (unsigned int i = 0; i < nodeChildren->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(nodeChildren->data[i]);
        if (childNode->type != GUMBO_NODE_TEXT) continue;

        value += QString::fromUtf8(childNode->v.text.text);
    }

    return value;
}

QtGumboNode QtGumboNode::gumboChildNodeByName(HtmlTag childTag, int startPos, int *foundPos)
{
    QtGumboNodes children = getChildren();
    // NOTE: not all chilren are elements
    //       Q_ASSERT(startPos < children.size()); if (startPos >= children.size()) return QtGumboNode();

    int elementIndex = 0;
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        if ((iChild->getTag() == childTag) && (elementIndex >= startPos))
        {
            if (foundPos) *foundPos = elementIndex;
            return *iChild;
        }
        else elementIndex++;
    }

    return QtGumboNode();
}

QtGumboNode QtGumboNode::gumboChildNodeByClass(QString className, HtmlTag childTag) const
{
    QtGumboNodes children = getChildren();
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        if ((iChild->getTag() == childTag) && (QString::compare(iChild->getClassAttribute(), className, Qt::CaseInsensitive) == 0))
        {
            return *iChild;
        }
    }

    return QtGumboNode();
}

QtGumboNode::QtGumboNodes QtGumboNode::gumboChildNodesByClassRecursive(QString className, HtmlTag childTag)
{
    Q_ASSERT(isValid()); if (!isValid()) return QtGumboNodes();

    QtGumboNodes result;

    QtGumboNodes children = getChildren();
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        if ((iChild->getTag() == childTag) && (QString::compare(iChild->getClassAttribute(), className, Qt::CaseInsensitive) == 0))
        {
            result << *iChild;
        }

        QtGumboNodes childResult = iChild->gumboChildNodesByClassRecursive(className, childTag);
        if (!childResult.empty()) result << childResult;
    }

    return result;
}

/*size_t QtGumboNode::getTagLength() const
{
    return m_node->v.element.original_tag.length;
}

size_t QtGumboNode::getStartPos() const
{
    return m_node->v.element.start_pos.offset;
}

size_t QtGumboNode::getEndPos() const
{
    return m_node->v.element.end_pos.offset;
}

QString QtGumboNode::getHtml() const
{
    return QString::fromUtf8(m_node->v.element.original_tag.data);
}
*/

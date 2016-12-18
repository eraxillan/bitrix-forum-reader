#include "qtgumbonode.h"

namespace {
const char* const ID_ATTRIBUTE 		= u8"id";
const char* const CLASS_ATTRIBUTE 	= u8"class";
}

QtGumboNode::QtGumboNode() : m_node(nullptr)
{
}

QtGumboNode::QtGumboNode(GumboNode *node) : m_node(node)
{
}

bool QtGumboNode::isValid() const
{
    return (m_node != nullptr);
}

bool QtGumboNode::isElement() const
{
    Q_ASSERT(isValid()); if(!isValid()) return false;

    return (m_node->type == GUMBO_NODE_ELEMENT);
}

bool QtGumboNode::isText() const
{
    Q_ASSERT(isValid()); if(!isValid()) return false;

    return (m_node->type == GUMBO_NODE_TEXT);
}

HtmlTag QtGumboNode::getTag() const
{
    if (isElement()) return HtmlTag(m_node->v.element.tag);
    return HtmlTag::UNKNOWN;
}

QString QtGumboNode::getTagName() const
{
    Q_ASSERT(isValid()); if (!isValid()) return QString();

    return QString(gumbo_normalized_tagname(m_node->v.element.tag));
}

bool QtGumboNode::hasAttribute(QString name) const
{
    Q_ASSERT(isValid()); if(!isValid()) return false;
    Q_ASSERT(!name.isEmpty()); if (name.isEmpty()) return false;

    GumboAttribute* attr = gumbo_get_attribute(&m_node->v.element.attributes, name.toUtf8().constData());
    return (attr != nullptr);
}

QString QtGumboNode::getAttribute(QString name) const
{
    Q_ASSERT(isValid()); if(!isValid()) return QString();
    Q_ASSERT(!name.isEmpty()); if (name.isEmpty()) return QString();

    QString result;
    GumboAttribute* attr = gumbo_get_attribute(&m_node->v.element.attributes, name.toUtf8().constData());
    if (attr) result = QString::fromUtf8(attr->value);
    return result;
}

bool QtGumboNode::hasIdAttribute() const
{
    return hasAttribute(ID_ATTRIBUTE);
}

bool QtGumboNode::hasClassAttribute() const
{
    return hasAttribute(CLASS_ATTRIBUTE);
}

QString QtGumboNode::getIdAttribute() const
{
    return getAttribute(ID_ATTRIBUTE);
}

QString QtGumboNode::getClassAttribute() const
{
    return getAttribute(CLASS_ATTRIBUTE);
}

size_t QtGumboNode::getAttributeCount() const
{
    Q_ASSERT(isValid()); if(!isValid()) return 0;

    return m_node->v.element.attributes.length;
}

QtGumboNodes QtGumboNode::getChildren(bool elementsOnly) const
{
    Q_ASSERT(isValid()); if(!isValid()) return QtGumboNodes();

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

int QtGumboNode::getChildElementCount(bool elementsOnly) const
{
    return getChildren(elementsOnly).size();
}

int QtGumboNode::getTextChildrenCount() const
{
    Q_ASSERT(isValid()); if(!isValid()) return 0;

    int textNodeCount = 0;
    GumboVector* nodeChildren = &m_node->v.element.children;
    for (unsigned int i = 0; i < nodeChildren->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(nodeChildren->data[i]);
        if (childNode->type == GUMBO_NODE_TEXT) textNodeCount++;
    }
    return textNodeCount;
}

QString QtGumboNode::getInnerText() const
{
    Q_ASSERT(isText()); if (!isText()) return QString();

    return QString::fromUtf8(m_node->v.text.text);
}

QString QtGumboNode::getChildrenInnerText()
{
    Q_ASSERT(isValid()); if(!isValid()) return QString();

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

QtGumboNode QtGumboNode::getElementByTag(std::pair< HtmlTag, int > tagDesc, int *foundPos)
{
    Q_ASSERT(isValid()); if(!isValid()) return QtGumboNode();
    Q_ASSERT(tagDesc.second >= 0); if (tagDesc.second < 0) return QtGumboNode();

    QtGumboNodes children = getChildren();
    // NOTE: not all chilren are elements, so the assert below can fail:
//  Q_ASSERT(startPos < children.size()); if (startPos >= children.size()) return QtGumboNode();
    int elementIndex = 0;
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        if ((iChild->getTag() == tagDesc.first) && (elementIndex >= tagDesc.second))
        {
            if (foundPos) *foundPos = elementIndex;
            return *iChild;
        }
        else elementIndex++;
    }
    return QtGumboNode();
}

QtGumboNode QtGumboNode::getElementByTag(std::initializer_list< std::pair< HtmlTag, int> > tagDescsInitList, int *foundPos)
{
    Q_ASSERT(isValid()); if(!isValid()) return QtGumboNode();
    Q_ASSERT(tagDescsInitList.size() > 0); if (!tagDescsInitList.size()) return QtGumboNode();

    typedef std::pair<HtmlTag, int> TagDesc;
    typedef std::initializer_list<TagDesc> InitList;

    QVector<TagDesc> tagDescs;
    for (InitList::iterator iItem = tagDescsInitList.begin(); iItem != tagDescsInitList.end(); ++iItem)
    {
        Q_ASSERT(iItem->second >= 0);

        tagDescs << std::make_pair(iItem->first, iItem->second);
    }

    QtGumboNode result = *this;
    while (!tagDescs.isEmpty())
    {
        auto tagDesc = tagDescs.first();
        tagDescs.removeFirst();

        result = result.getElementByTag({tagDesc.first, tagDesc.second}, foundPos);
        if (!result.isValid())
        {
            result = QtGumboNode();
            break;
        }
    }
    return result;
}

QtGumboNode QtGumboNode::getElementByClass(QString className, HtmlTag childTag) const
{
    Q_ASSERT(isValid()); if(!isValid()) return QtGumboNode();
    Q_ASSERT(!className.isEmpty()); if (className.isEmpty()) return QtGumboNode();

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

QtGumboNodes QtGumboNode::getElementsByClass(QString className, HtmlTag childTag) const
{
    Q_ASSERT(isValid()); if(!isValid()) return QtGumboNodes();
    Q_ASSERT(!className.isEmpty()); if (className.isEmpty()) return QtGumboNodes();

    QtGumboNodes result;

    QtGumboNodes children = getChildren();
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        if ((iChild->getTag() == childTag) && (QString::compare(iChild->getClassAttribute(), className, Qt::CaseInsensitive) == 0))
        {
            result << (*iChild);
        }
    }
    return result;
}

QtGumboNodes QtGumboNode::getElementsByClassRecursive(QString className, HtmlTag childTag) const
{
    Q_ASSERT(isValid()); if (!isValid()) return QtGumboNodes();
    Q_ASSERT(!className.isEmpty()); if (className.isEmpty()) return QtGumboNodes();

    QtGumboNodes result;

    QtGumboNodes children = getChildren();
    for (QtGumboNodes::iterator iChild = children.begin(); iChild != children.end(); ++iChild)
    {
        if ((iChild->getTag() == childTag) && (QString::compare(iChild->getClassAttribute(), className, Qt::CaseInsensitive) == 0))
        {
            result << *iChild;
        }

        QtGumboNodes childResult = iChild->getElementsByClassRecursive(className, childTag);
        if (!childResult.empty()) result << childResult;
    }

    return result;
}

size_t QtGumboNode::getTagLength() const
{
    Q_ASSERT(isValid()); if (!isValid()) return 0;

    return m_node->v.element.original_tag.length;
}

size_t QtGumboNode::getStartPos() const
{
    Q_ASSERT(isValid()); if (!isValid()) return 0;

    return m_node->v.element.start_pos.offset;
}

size_t QtGumboNode::getEndPos() const
{
    Q_ASSERT(isValid()); if (!isValid()) return 0;

    return m_node->v.element.end_pos.offset;
}

QString QtGumboNode::getHtml() const
{
    Q_ASSERT(isValid()); if (!isValid()) return QString();

    return QString::fromUtf8(m_node->v.element.original_tag.data);
}

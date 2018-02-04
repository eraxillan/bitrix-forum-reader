#ifndef QTGUMBODOCUMENT_H
#define QTGUMBODOCUMENT_H

#include "qtgumbonode.h"

class QtGumboDocument;
using QtGumboDocumentPtr = std::shared_ptr<QtGumboDocument>;

class QtGumboDocument
{
    QByteArray m_rawHtmlData;

    GumboOutput *m_output;

    QtGumboNodePtr m_documentNode;
    QtGumboNodePtr m_rootNode;

    bool Parse();

public:
    QtGumboDocument();
    QtGumboDocument(QString rawData);
    ~QtGumboDocument();

    QtGumboNodePtr documentNode() const;
    QtGumboNodePtr rootNode() const;
    // FIXME: implement errors list getter

    void prettify();

    // User-defined copy assignment, copy-and-swap form
    QtGumboDocument& operator=(QtGumboDocument other)
    {
        std::swap(m_rawHtmlData, other.m_rawHtmlData);
        std::swap(m_output, other.m_output);
        std::swap(m_documentNode, other.m_documentNode);
        std::swap(m_rootNode, other.m_rootNode);
        return *this;
    }
};

#endif // QTGUMBODOCUMENT_H

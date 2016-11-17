#ifndef QTGUMBODOCUMENT_H
#define QTGUMBODOCUMENT_H

#include "qtgumbonode.h"

class QtGumboDocument
{
    GumboOutput* m_output = nullptr;
    QtGumboNode m_rootNode;

public:
    QtGumboDocument(QString rawData)
    {
        QTextCodec* htmlCodec = QTextCodec::codecForHtml(rawData.toLocal8Bit());
    #ifdef RUBANOK_DEBUG
        qDebug() << "ru.banki.reader: HTML encoding/charset is" << htmlCodec->name();
    #endif

        // Convert to UTF-8: Gumbo library understands only this encoding
    #if defined( Q_OS_WIN )
        QString htmlFileString = htmlCodec->toUnicode(rawData.toLocal8Bit());
        QByteArray htmlFileUtf8Contents = htmlFileString.toUtf8();
    #elif defined( Q_OS_ANDROID )
        QByteArray htmlFileUtf8Contents = rawData.toUtf8();
    #else
        #error "Unsupported platform, needs testing"
    #endif

        // Parse web page contents
        m_output = gumbo_parse(htmlFileUtf8Contents.constData());
        m_rootNode = QtGumboNode(m_output->root);
    }

    ~QtGumboDocument()
    {
        gumbo_destroy_output(&kGumboDefaultOptions, m_output);
    }

    QtGumboNode rootNode() const { return m_rootNode; }
};

#endif // QTGUMBODOCUMENT_H

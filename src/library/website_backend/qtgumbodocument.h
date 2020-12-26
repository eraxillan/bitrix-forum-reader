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
#ifndef __BFR_QTGUMBODOCUMENT_H__
#define __BFR_QTGUMBODOCUMENT_H__

#include "qtgumbonode.h"

class QtGumboDocument;
using QtGumboDocumentPtr = std::shared_ptr<QtGumboDocument>;

class QtGumboDocument {
	QByteArray m_rawHtmlData;

	GumboOutput *m_output;

	QtGumboNodePtr m_documentNode;
	QtGumboNodePtr m_rootNode;

	bool parse();

public:
	QtGumboDocument();
	QtGumboDocument(const QString &rawData);
	~QtGumboDocument();

	QtGumboNodePtr documentNode() const;
	QtGumboNodePtr rootNode() const;
	// FIXME: implement errors list getter

	void prettify() const;

	// User-defined copy assignment, copy-and-swap form
	QtGumboDocument &operator=(QtGumboDocument other);
};

#endif // __BFR_QTGUMBODOCUMENT_H__

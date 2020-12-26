#include "translator.h"

#include <QtCore/QCoreApplication>

#include <spdlog/spdlog.h>

Translator::Translator(QQmlEngine *engine) {

	m_translator = new QTranslator(this);
	m_engine = engine;
}

void Translator::selectLanguage(const QString &language) {

	qApp->removeTranslator(m_translator);

	if (!m_translator->load(QString(":/i18n/qml_%1").arg(language))) {
		SPDLOG_ERROR("Failed to load QML translation file for language {}, falling back to English", language.toUtf8().data());
		return;
	}

	qApp->installTranslator(m_translator);
	m_engine->retranslate();

	emit languageChanged();
}

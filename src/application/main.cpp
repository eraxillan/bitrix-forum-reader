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
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlFileSelector>

#include <iostream>

#include <common/logger.h>
#include <common/forumthreadurl.h>
#include <forumreader.h>

// FIXME: minimize use of Qt containers, smart pointers and primitive types (use stdlib ones instead)
// FIXME: move raw strings (error messages etc.) to separate file and get them thru new LocalizationManager class
// FIXME: enforce constness
// FIXME: enforce strict C++ compiler warnings
//
// FIXME: add user whitelist
// FIXME: add sorting by user/post reputation option
// FIXME: add full error stack storage code like PCode do
// FIXME: save full post history to the LOCAL SQLite database
// FIXME: add abitity to assign a note string to each forum user (e.g. "useless one")

namespace {
bool initLogLibrary() {
	try {
		std::cout << "initializing spdlog..." << std::endl;

		// Console multi threaded logger with color
#if defined(Q_OS_ANDROID)
		/*auto android_logger =*/spdlog::android_logger_mt("console", "Bitrix Forum Reader"); // "android"
#elif defined(Q_OS_UNIX)
		/*auto console =*/ spdlog::stdout_color_mt("console");
#else
		auto sink = std::make_shared<spdlog::sinks::windebug_sink_st>();
		auto logger = std::make_shared<spdlog::logger>("console", sink);
		spdlog::register_logger(logger);
#endif

		// Customize msg format for all messages
		spdlog::set_pattern("[%^%L%$][%D %H:%M:%S.%e][%P:%t] %v");
		spdlog::set_level(spdlog::level::trace);

		ConsoleLogger->info("spdlog was successfully initialized");
	}
	// Exceptions will only be thrown upon failed logger or sink construction (not during logging)
	catch (const spdlog::spdlog_ex &ex) {
		std::cout << "spdlog init failed: " << ex.what() << std::endl;
		return false;
	}

	return true;
}

void deinitLogLibrary() {
	ConsoleLogger->info("deinitializing spdlog...");

	// Release and close all loggers
	spdlog::drop_all();

	std::cout << "spdlog deinit succeeded" << std::endl;
}

#ifdef BFR_DUMP_GENERATED_QML_IN_FILES
bool setupQmlDumpDirectory(const QString &dumpDirPath) {
	QDir dumpDir(dumpDirPath);
	BFR_RETURN_VALUE_IF(!dumpDir.isReadable(), false, "unable to get application local data directory path!");

	// Clean the generated QML output directory
	if (dumpDir.exists(BFR_QML_OUTPUT_DIR)) {
		BFR_RETURN_VALUE_IF(!dumpDir.cd(BFR_QML_OUTPUT_DIR), false, "unable to change current directory!");
		BFR_RETURN_VALUE_IF(!dumpDir.removeRecursively(), false, "unable to remove entire directory!");
		BFR_RETURN_VALUE_IF(!dumpDir.cdUp(), false, "unable to change current directory!");
	}
	// Now recreate it
	BFR_RETURN_VALUE_IF(!dumpDir.mkdir(BFR_QML_OUTPUT_DIR), false, "unable to create subdirectory!");
	BFR_RETURN_VALUE_IF(!dumpDir.cd(BFR_QML_OUTPUT_DIR), false, "unable to change current directory!");

	return true;
}
#endif
} // namespace

int main(int argc, char *argv[]) {
	if (!initLogLibrary())
		return 1;

#ifdef Q_OS_ANDROID
	qputenv("QT_QUICK_CONTROLS_STYLE", "Material");
#endif

	// FIXME: test correctness due to error messages:
	// "Warning: QML import could not be resolved in any of the import paths: name.eraxillan.bfr"
	qmlRegisterType<ForumThreadUrl>("name.eraxillan.bfr", 1, 0, "ForumThreadUrl");
	qmlRegisterType<ForumReader>("name.eraxillan.bfr", 1, 0, "ForumReader");

	QGuiApplication::setApplicationName("Bitrix Forum Reader");
	QGuiApplication::setOrganizationName("Alexander Kamyshnikov");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication app(argc, argv);

#ifdef BFR_DUMP_GENERATED_QML_IN_FILES
	// Application data on mobile platform must be stored
	// in the specific directory only
#if defined(Q_OS_ANDROID)
	const QString appDataDirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
	if (!setupQmlDumpDirectory(appDataDirPath))
		return 1;
#elif defined(Q_OS_IOS)
	// FIXME: implement if possible
#else
	// Desktop platforms allow to use application directory
	const QString appDataDirPath(qApp->applicationDirPath());
	if (!setupQmlDumpDirectory(appDataDirPath))
		return 1;
#endif
#endif

	int exitCode = -1;
	try {
		QQmlApplicationEngine engine;

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
		QDir appDir(qApp->applicationDirPath());
		appDir.cdUp();
		appDir.cdUp();
		appDir.cd("thirdparty/fluid/qml");
		engine.addImportPath(appDir.path());
#endif

		QStringList selectors;
#ifdef QT_EXTRA_FILE_SELECTOR
		selectors += QT_EXTRA_FILE_SELECTOR;
#else
		if (app.arguments().contains("--android")) {
			qputenv("QT_QUICK_CONTROLS_CONF", "://qml/+android/qtquickcontrols2.conf");
			selectors += "android";
		} else if (app.arguments().contains("--ios")) {
			qputenv("QT_QUICK_CONTROLS_CONF", "://qml/+ios/qtquickcontrols2.conf");
			selectors += "ios";
		}
#endif

		QQmlFileSelector::get(&engine)->setExtraSelectors(selectors);

		engine.load("://qml/main.qml");
		if (engine.rootObjects().isEmpty())
			throw 1;

		exitCode = app.exec();
	} catch (...) {
		ConsoleLogger->critical("ERROR: unhandled exception caught!");

		deinitLogLibrary();
		return 1;
	}

	deinitLogLibrary();
	return exitCode;
}

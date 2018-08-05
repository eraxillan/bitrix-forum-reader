#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QQuickStyle>
#include <QQmlFileSelector>
#include <QScreen>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

#ifdef Q_OS_WINDOWS
#define CATCH_CONFIG_COLOUR_WINDOWS
#elif defined(Q_OS_UNIX)
#define CATCH_CONFIG_COLOUR_ANSI
#else
#define CATCH_CONFIG_COLOUR_NONE
#endif
#define CATCH_CONFIG_RUNNER     // no generated main()
#include "catch.hpp"

#include <common/logger.h>
#include <common/forumthreadurl.h>

#include "website_backend/gumboparserimpl.h"
#include "qml_frontend/forumreader.h"

// FIXME: add copyright and license info in source files
// FIXME: move raw strings (error messages etc.) to separate file and get them thru new LocalizationManager class
//
// FIXME: add user whitelist
// FIXME: add sorting by user/post reputation option
// FIXME: add full error stack storage code like PCode do
// FIXME: save full post history to the LOCAL SQLite database
// FIXME: add abitity to assign a note string to each forum user (e.g. "useless one")

/*
static float getDpi(float& textScaleFactor)
{
    QScreen* screen = qApp->primaryScreen();

    float dpi = 0;
#if defined(Q_OS_WIN)
    textScaleFactor = 15.0f;
    dpi = static_cast<float>(screen->logicalDotsPerInch() * qApp->devicePixelRatio());
#elif defined(Q_OS_ANDROID)
    Q_UNUSED(screen);

    QAndroidJniObject qtActivity = QAndroidJniObject::callStaticObjectMethod(
                "org/qtproject/qt5/android/QtNative",
                "activity",
                "()Landroid/app/Activity;");
    QAndroidJniObject resources = qtActivity.callObjectMethod(
                "getResources",
                "()Landroid/content/res/Resources;");
    QAndroidJniObject displayMetrics = resources.callObjectMethod(
                "getDisplayMetrics",
                "()Landroid/util/DisplayMetrics;");
    int density = displayMetrics.getField<int>("densityDpi");
    dpi = density;

    float scaledDensity = displayMetrics.getField<float>("scaledDensity");
    textScaleFactor = scaledDensity;
#else
    textScaleFactor = 15.0f;
    dpi = screen->physicalDotsPerInch() * qApp->devicePixelRatio();
#endif
    return dpi;
}
*/

bool initLogLibrary()
{
    try
    {
        std::cout << "initializing spdlog..." << std::endl;

        // Console logger with color
#ifndef Q_OS_WIN
        spdlog::stdout_color_mt("console");
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
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "spdlog init failed: " << ex.what() << std::endl;
        return false;
    }

    return true;
}

void deinitLogLibrary()
{
    ConsoleLogger->info("deinitializing spdlog...");

    // Release and close all loggers
    spdlog::drop_all();

    std::cout << "spdlog deinit succeeded" << std::endl;
}

int main(int argc, char *argv[])
{
    if (!initLogLibrary())
        return 1;

    qsrand(1);
    qmlRegisterType<ForumThreadUrl>("ru.banki.reader", 1, 0, "ForumThreadUrl");
    qmlRegisterType<ForumReader>("ru.banki.reader", 1, 0, "ForumReader");

    QGuiApplication::setApplicationName("Bitrix Forum Reader");
    QGuiApplication::setOrganizationName("Alexander Kamyshnikov");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    Catch::Session session;
    int returnCode = session.applyCommandLine(argc, argv, Catch::Session::OnUnusedOptions::Ignore);
    if (returnCode != 0) return returnCode;
    for (auto unusedOpt : session.unusedTokens())
    {
        if (unusedOpt.type == Catch::Clara::Parser::Token::LongOpt)
        {
            if (unusedOpt.data == "test")
            {
                int numFailed = session.run();
                // NOTE: on Unices only the lower 8 bits are usually used, clamping
                // the return value to 255 prevents false negative when some multiple
                // of 256 tests has failed
                return (numFailed < 0xff ? numFailed : 0xff);
            }
        }
    }

#ifdef BFR_DUMP_GENERATED_QML_IN_FILES
    QDir appRootDir(qApp->applicationDirPath());
    Q_ASSERT(appRootDir.isReadable());
    // Clean the generated QML output directory
    if (appRootDir.exists(BFR_QML_OUTPUT_DIR))
    {
        Q_ASSERT(appRootDir.cd(BFR_QML_OUTPUT_DIR));
        Q_ASSERT(appRootDir.removeRecursively());
        Q_ASSERT(appRootDir.cdUp());
    }
    Q_ASSERT(appRootDir.mkdir(BFR_QML_OUTPUT_DIR));
    Q_ASSERT(appRootDir.cd(BFR_QML_OUTPUT_DIR));
#endif

    int exitCode = -1;
    try
    {
        QQmlApplicationEngine engine;

        QString projectRootDir = QDir(QCoreApplication::applicationDirPath()
                                      + QDir::separator() + QLatin1String("..")
                                      + QDir::separator() + QLatin1String("..")
                                      + QDir::separator() + QLatin1String("..")
                                      + QDir::separator() + QLatin1String("..")
                                      ).canonicalPath() + QDir::separator();
        engine.addImportPath(projectRootDir + QLatin1String("fluid") + QDir::separator() + QLatin1String("qml"));
        engine.addImportPath(projectRootDir + QLatin1String("src") + QDir::separator() + QLatin1String("qml_frontend") + QDir::separator() + QLatin1String("qml"));
        engine.addImportPath(projectRootDir + QLatin1String("src") + QDir::separator() + QLatin1String("qml_frontend") + QDir::separator() + QLatin1String("qml") + QDir::separator() + QLatin1String("+android"));
        engine.addImportPath(projectRootDir + QLatin1String("src") + QDir::separator() + QLatin1String("qml_frontend") + QDir::separator() + QLatin1String("qml") + QDir::separator() + QLatin1String("+ios"));

        /*
        float textScaleFactor = 0.0f;
        float displayDpi = getDpi(textScaleFactor);
        engine.rootContext()->setContextProperty("displayDpi", displayDpi);
        engine.rootContext()->setContextProperty("textScaleFactor", textScaleFactor);
        */

        QStringList selectors;
    #ifdef QT_EXTRA_FILE_SELECTOR
        selectors += QT_EXTRA_FILE_SELECTOR;
    #else
        if (app.arguments().contains("--android"))
        {
            qputenv("QT_QUICK_CONTROLS_CONF", "://qml/+android/qtquickcontrols2.conf");
            selectors += "android";
        }
        else if (app.arguments().contains("--ios"))
        {
            qputenv("QT_QUICK_CONTROLS_CONF", "://qml/+ios/qtquickcontrols2.conf");
            selectors += "ios";
        }
    #endif

        QQmlFileSelector::get(&engine)->setExtraSelectors(selectors);

        engine.load("://qml/main.qml");
        if (engine.rootObjects().isEmpty())
            throw 1;

        exitCode = app.exec();
    }
    catch (...)
    {
        ConsoleLogger->critical("ERROR: unhandled exception caught!");

        deinitLogLibrary();
        return 1;
    }

    deinitLogLibrary();
    return exitCode;
}

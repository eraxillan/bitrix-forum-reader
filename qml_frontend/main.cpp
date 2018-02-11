#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QQuickStyle>
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

#include "website_backend/gumboparserimpl.h"
#include "qml_frontend/forumreader.h"

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

int main(int argc, char *argv[])
{
    qsrand(1);
    qmlRegisterType<ForumReader>("ru.banki.reader", 1, 0, "ForumReader");

    QGuiApplication::setApplicationName("Banki.ru Reader");
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

#ifdef RBR_DUMP_GENERATED_QML_IN_FILES
    QDir appRootDir(qApp->applicationDirPath());
    Q_ASSERT(appRootDir.isReadable());
    // Clean the generated QML output directory
    if (appRootDir.exists(RBR_QML_OUTPUT_DIR))
    {
        Q_ASSERT(appRootDir.cd(RBR_QML_OUTPUT_DIR));
        Q_ASSERT(appRootDir.removeRecursively());
        Q_ASSERT(appRootDir.cdUp());
    }
    Q_ASSERT(appRootDir.mkdir(RBR_QML_OUTPUT_DIR));
    Q_ASSERT(appRootDir.cd(RBR_QML_OUTPUT_DIR));
#endif

    try
    {
        QQmlApplicationEngine engine;

        /*
        float textScaleFactor = 0.0f;
        float displayDpi = getDpi(textScaleFactor);
        engine.rootContext()->setContextProperty("displayDpi", displayDpi);
        engine.rootContext()->setContextProperty("textScaleFactor", textScaleFactor);
        */

        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
        if (engine.rootObjects().isEmpty())
            return 1;

        return app.exec();
    }
    catch (...)
    {
        qDebug() << "ERROR: exception caught!";
        return 2;
    }
}

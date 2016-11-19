
#include <QtCore>
#include <QtQml>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickImageProvider>
#include <QScreen>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#endif

#include "src/website_backend/gumboparserimpl.h"
#include "src/qml_backend/forumreader.h"

// FIXME: add full error stack storage code like PCode do
// FIXME: save full post history to the LOCAL database
// FIXME: add sorting by user/post reputation option
// FIXME: add abitity to assign a note string to each forum user (e.g. "useless one")
int getDpi(float& textScaleFactor)
{
    QScreen* screen = qApp->primaryScreen();

    qreal dpi = 0;
#if defined(Q_OS_WIN)
    textScaleFactor = 15.0f;
    dpi = /*3 **/ screen->logicalDotsPerInch() * qApp->devicePixelRatio();
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
    dpi = screen->physicalDotsPerInch() * qApp->devicePixelRatio();
#endif
    return dpi;
}

int main(int argc, char *argv[])
{
    qsrand(1);

    QApplication app(argc, argv);

    qmlRegisterType<ForumReader>("ru.banki.reader", 1, 0, "ForumReader");

    QQmlApplicationEngine engine;

    float textScaleFactor = 0.0f;
    int displayDpi = getDpi(textScaleFactor);
    engine.rootContext()->setContextProperty("displayDpi", displayDpi);
    engine.rootContext()->setContextProperty("textScaleFactor", textScaleFactor);

	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}


#include <QtCore>
#include <QtQml>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickImageProvider>

#include "src/website_backend/gumboparserimpl.h"
#include "src/qml_backend/useravatarimageprovider.h"
#include "src/qml_backend/forumreader.h"

// FIXME: save full post history to the LOCAL database
// FIXME: add sorting by user/post reputation option

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<ForumReader>("ru.banki.reader", 1, 0, "ForumReader");
    qmlRegisterType<ForumModel>( "ru.banki.reader", 1, 0, "ForumModel");

    QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

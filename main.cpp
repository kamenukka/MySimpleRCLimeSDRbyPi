//#include <QApplication>
#include <QtWidgets/QApplication>

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "appcore.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    //app.setWindowIcon(QIcon("src/settings.png"));

    QQmlApplicationEngine engine;

    AppCore appCore;    // Создаём ядро приложения
    QQmlContext *context = engine.rootContext();    // Создаём корневой контекст
    /* Загружаем объект в контекст для установки соединения,
     * а также определяем имя, по которому будет происходить соединение
     * */
    context->setContextProperty("appCore", &appCore);
    engine.rootContext()->setContextProperty("CurDirPath",QString(QDir::currentPath()));

    // И загружаем в него исходники qml
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    appCore.init();

    return app.exec();
}

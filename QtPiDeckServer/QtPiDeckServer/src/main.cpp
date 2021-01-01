#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "ServerApplication.hpp"

auto main(int argc, char *argv[]) -> int
{
    QtPiDeck::ServerApplication app;
    return app.start(argc, argv);
}

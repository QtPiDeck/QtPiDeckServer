#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "ServerApplication.hpp"

auto main(int argc, char *argv[]) -> int
{
    QtPiDeck::Server::ServerApplication app;
    return app.start(argc, argv);
}

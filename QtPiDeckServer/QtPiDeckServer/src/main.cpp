#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "ServerApplication.hpp"

int main(int argc, char *argv[])
{
    QtPiDeck::Server::ServerApplication app;
    return app.start(argc, argv);
}

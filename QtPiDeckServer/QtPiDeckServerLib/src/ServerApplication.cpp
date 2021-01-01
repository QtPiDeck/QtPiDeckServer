#include "ServerApplication.hpp"

#include <QCoreApplication>

#include "Services/MessageBus.hpp"

namespace QtPiDeck {
ServerApplication::ServerApplication() = default;

auto ServerApplication::mainPage() -> QUrl {
    return QUrl{QStringLiteral("qrc:/qml/pages/main.qml")};
}

void ServerApplication::initialPreparations() {
    Application::initialPreparations();

    ioc().registerSingleton(std::make_shared<Services::MessageBus>(nullptr));

#if QT_VERSION_MAJOR < 6
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
}

void ServerApplication::appCreated() {

}

void ServerApplication::engineCreated(QQmlApplicationEngine & engine) {
    // during early testing(server and client in app) communication was broken
    // starting server this way fixes a problem
    // might need additional testing when client side app is ready to connect
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &m_deckServer, &Network::DeckServer::start);
}
}

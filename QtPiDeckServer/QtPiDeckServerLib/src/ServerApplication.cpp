#include "ServerApplication.hpp"

#include "Services/MessageBus.hpp"

namespace QtPiDeck::Server {
ServerApplication::ServerApplication() = default;

auto ServerApplication::mainPage() -> QUrl {
    return QUrl{QStringLiteral("qrc:/qml/pages/main.qml")};
}

void ServerApplication::appStartupPreparations() {
    Application::appStartupPreparations();

    ioc().registerSingleton(std::make_shared<Services::MessageBus>(nullptr));
}

void ServerApplication::setupEngine(QQmlApplicationEngine & engine) {
    // during early testing(server and client in app) communication was broken
    // starting server this way fixes a problem
    // might need additional testing when client side app is ready to connect
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &m_deckServer, &Network::DeckServer::start);
}
}

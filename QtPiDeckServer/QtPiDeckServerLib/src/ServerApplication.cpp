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
}

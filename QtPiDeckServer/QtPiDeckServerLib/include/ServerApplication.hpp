#pragma once

#include "Application.hpp"
#include "Network/DeckServer.hpp"

namespace QtPiDeck::Server {
class ServerApplication final : public QtPiDeck::Application {
public:
    ServerApplication();

protected:
    auto mainPage() -> QUrl final;
    void appStartupPreparations() final;
    void setupEngine(QQmlApplicationEngine & engine) final;

private:
    Network::DeckServer m_deckServer;
};
}

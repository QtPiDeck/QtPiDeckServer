#pragma once

#include "Application.hpp"
#include "Network/DeckServer.hpp"

namespace QtPiDeck::Server {
class ServerApplication final : public QtPiDeck::Application {
public:
    ServerApplication();

protected:
    auto mainPage() -> QUrl final;
    void initialPreparations() final;
    void appCreated() final;
    void engineCreated(QQmlApplicationEngine & engine) final;

private:
    Network::DeckServer m_deckServer;
};
}

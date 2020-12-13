#pragma once

#include "Application.hpp"

namespace QtPiDeck::Server {
class ServerApplication final : public QtPiDeck::Application {
public:
    ServerApplication();

protected:
    auto mainPage() -> QUrl final;
    void appStartupPreparations() final;
};
}

#pragma once

#include "Services/ISettingsStorage.hpp"

namespace QtPiDeck::Services {
class IServerSettingsStorage : public ISettingsStorage {
public:
    [[nodiscard]] virtual auto deckServerAddress() const noexcept -> QString = 0;
    virtual void setDeckServerAddress(const QString& deckServerAddress) noexcept = 0;
    [[nodiscard]] virtual auto deckServerPort() const noexcept -> QString = 0;
    virtual void setDeckServerPort(const QString& deckServerPort) noexcept = 0;
};
}

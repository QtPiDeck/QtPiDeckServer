#pragma once

#include <memory>

#include <QSettings>

#include "Services/IServerSettingsStorage.hpp"

namespace QtPiDeck::Services {
class SettingsStorage final : public IServerSettingsStorage {
public:
    SettingsStorage();
    SettingsStorage(const SettingsStorage&) = delete;
    SettingsStorage(SettingsStorage&&) = default;
    ~SettingsStorage() final = default;

    auto operator=(const SettingsStorage&) -> SettingsStorage& = delete;
    auto operator=(SettingsStorage&&) -> SettingsStorage& = default;

    [[nodiscard]] auto read(const QString& key, const QVariant& defaultValue = QVariant{}) const noexcept -> QVariant final;
    void store(const QString& key, const QVariant& value) noexcept final;

    [[nodiscard]] auto deckServerAddress() const noexcept -> QString final;
    void setDeckServerAddress(const QString& deckServerAddress) noexcept final;
    [[nodiscard]] auto deckServerPort() const noexcept -> QString final;
    void setDeckServerPort(const QString& deckServerPort) noexcept final;
    [[nodiscard]] auto obsWebsocketAddress() const noexcept -> QString final;
    void setObsWebsocketAddress(const QString& obsWebsocketAddress) noexcept final;
    [[nodiscard]] auto obsWebsocketPort() const noexcept -> QString final;
    void setObsWebsocketPort(const QString& obsWebsocketPort) noexcept final;
private:
    std::unique_ptr<QSettings> m_settings;

    inline static const QString DeckServerAddressKey{"DeckServerAddress"};
    inline static const QString DeckServerAddressDefaultValue{"127.0.0.1"};
    inline static const QString DeckServerPortKey{"DeckServerPort"};
    inline static const QString DeckServerPortDefaultValue{"13000"};
    inline static const QString ObsWebsocketAddressKey{"ObsWebsocketAddress"};
    inline static const QString ObsWebsocketAddressDefaultValue{"127.0.0.1"};
    inline static const QString ObsWebsocketPortKey{"ObsWebsocketPort"};
    inline static const QString ObsWebsocketPortDefaultValue{"4444"};
};
}

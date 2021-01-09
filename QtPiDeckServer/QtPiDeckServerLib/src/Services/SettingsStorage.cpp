#include "Services/SettingsStorage.hpp"

#include <QCoreApplication>

namespace QtPiDeck::Services {
SettingsStorage::SettingsStorage() {
    QCoreApplication::setOrganizationName("QtPiDeck");
    QCoreApplication::setOrganizationDomain("qtpideck.com");
    QCoreApplication::setApplicationName("QtPiDeckServer");

    m_settings = std::make_unique<QSettings>(nullptr);
}

auto SettingsStorage::read(const QString& key, const QVariant& defaultValue) const noexcept -> QVariant
{
    return m_settings->value(key, defaultValue);
}

void SettingsStorage::store(const QString& key, const QVariant& value) noexcept
{
    m_settings->setValue(key, value);
}

auto SettingsStorage::deckServerAddress() const noexcept -> QString {
    return read(DeckServerAddressKey, DeckServerAddressDefaultValue).toString();
}

void SettingsStorage::setDeckServerAddress(const QString& deckServerAddress) noexcept {
    store(DeckServerAddressKey, deckServerAddress);
}

auto SettingsStorage::deckServerPort() const noexcept -> QString {
    return read(DeckServerPortKey, DeckServerPortDefaultValue).toString();
}

void SettingsStorage::setDeckServerPort(const QString& deckServerPort) noexcept {
    store(DeckServerPortKey, deckServerPort);
}

auto SettingsStorage::obsWebsocketAddress() const noexcept -> QString {
    return read(ObsWebsocketAddressKey, ObsWebsocketAddressDefaultValue).toString();
}

void SettingsStorage::setObsWebsocketAddress(const QString& obsWebsocketAddress) noexcept {
    store(ObsWebsocketAddressKey, obsWebsocketAddress);
}

auto SettingsStorage::obsWebsocketPort() const noexcept -> QString {
    return read(ObsWebsocketPortKey, ObsWebsocketPortDefaultValue).toString();
}

void SettingsStorage::setObsWebsocketPort(const QString& obsWebsocketPort) noexcept {
    store(ObsWebsocketPortKey, obsWebsocketPort);
}
}

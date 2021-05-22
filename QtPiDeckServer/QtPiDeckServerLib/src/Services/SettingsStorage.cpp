#include "Services/SettingsStorage.hpp"

#include <QCoreApplication>

namespace QtPiDeck::Services {
SettingsStorage::SettingsStorage() {
  QCoreApplication::setOrganizationName("QtPiDeck");
  QCoreApplication::setOrganizationDomain("qtpideck.com");
  QCoreApplication::setApplicationName("QtPiDeckServer");

  m_settings = std::make_unique<QSettings>(nullptr);
}

auto SettingsStorage::read(const QString& key, const QVariant& defaultValue) const noexcept -> QVariant {
  return m_settings->value(key, defaultValue);
}

void SettingsStorage::store(const QString& key, const QVariant& value) noexcept { m_settings->setValue(key, value); }

auto SettingsStorage::deckServerAddress() const noexcept -> QString {
  return read(DeckServerAddress.key, DeckServerAddress.defaultValue).toString();
}

void SettingsStorage::setDeckServerAddress(const QString& deckServerAddress) noexcept {
  store(DeckServerAddress.key, deckServerAddress);
}

auto SettingsStorage::deckServerPort() const noexcept -> QString {
  return read(DeckServerPort.key, DeckServerPort.defaultValue).toString();
}

void SettingsStorage::setDeckServerPort(const QString& deckServerPort) noexcept {
  store(DeckServerPort.key, deckServerPort);
}

auto SettingsStorage::obsWebsocketAddress() const noexcept -> QString {
  return read(ObsWebsocketAddress.key, ObsWebsocketAddress.defaultValue).toString();
}

void SettingsStorage::setObsWebsocketAddress(const QString& obsWebsocketAddress) noexcept {
  store(ObsWebsocketAddress.key, obsWebsocketAddress);
}

auto SettingsStorage::obsWebsocketPort() const noexcept -> QString {
  return read(ObsWebsocketPort.key, ObsWebsocketPort.defaultValue).toString();
}

void SettingsStorage::setObsWebsocketPort(const QString& obsWebsocketPort) noexcept {
  store(ObsWebsocketPort.key, obsWebsocketPort);
}
}

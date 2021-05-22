#pragma once

#include <memory>

#include <QSettings>

#include "QtPiDeckServerDefines.hpp"
#include "Services/IServerSettingsStorage.hpp"

namespace QtPiDeck::Services {
template<class TVal>
struct FieldData {
  QString key;
  TVal defaultValue;
};

template<class TVal>
FieldData(QString b, TVal e) -> FieldData<TVal>;

class SettingsStorage final : public IServerSettingsStorage {
public:
  SettingsStorage();
  SettingsStorage(const SettingsStorage&) = delete;
  SettingsStorage(SettingsStorage&&) = default;
  ~SettingsStorage() final = default;

  auto operator=(const SettingsStorage&) -> SettingsStorage& = delete;
  auto operator=(SettingsStorage&&) -> SettingsStorage& = default;

  [[nodiscard]] auto read(const QString& key, const QVariant& defaultValue = QVariant{}) const noexcept
      -> QVariant final;
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

  inline static const FieldData DeckServerAddress{"DeckServerAddress"_qs, "127.0.0.1"_qs};
  inline static const FieldData DeckServerPort{"DeckServerPort"_qs, "13000"_qs};
  inline static const FieldData ObsWebsocketAddress{"ObsWebsocketAddress"_qs, "127.0.0.1"_qs};
  inline static const FieldData ObsWebsocketPort{"ObsWebsocketPort"_qs, "4444"_qs};
};
}

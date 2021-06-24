#pragma once

#include <QDebug>
#include <QObject>

#include <boost/log/attributes/constant.hpp>
#include <boost/log/trivial.hpp>

#include "Services/IServerSettingsStorage.hpp"
#include "Services/UseServices.hpp"

namespace QtPiDeck::ViewModels {
class SettingsViewModel : public QObject, public Services::UseServices<Services::IServerSettingsStorage> {
  Q_OBJECT // NOLINT
#if QT_VERSION_MAJOR == 6
  Q_PROPERTY(QString deckServerAddress READ deckServerAddress WRITE setDeckServerAddress NOTIFY deckServerAddressChanged
                 BINDABLE bindableDeckServerAddress)
  Q_PROPERTY(QString deckServerPort READ deckServerPort WRITE setDeckServerPort NOTIFY deckServerPortChanged BINDABLE
                 bindableDeckServerPort)
  Q_PROPERTY(QString obsWebsocketAddress READ obsWebsocketAddress WRITE setObsWebsocketAddress NOTIFY
                 obsWebsocketAddressChanged BINDABLE bindableObsWebsocketAddress)
  Q_PROPERTY(QString obsWebsocketPort READ obsWebsocketPort WRITE setObsWebsocketPort NOTIFY obsWebsocketPortChanged
                 BINDABLE bindableObsWebsocketPort)
#else
  Q_PROPERTY(
      QString deckServerAddress READ deckServerAddress WRITE setDeckServerAddress NOTIFY deckServerAddressChanged)
  Q_PROPERTY(QString deckServerPort READ deckServerPort WRITE setDeckServerPort NOTIFY deckServerPortChanged)
  Q_PROPERTY(QString obsWebsocketAddress READ obsWebsocketAddress WRITE setObsWebsocketAddress NOTIFY
                 obsWebsocketAddressChanged)
  Q_PROPERTY(QString obsWebsocketPort READ obsWebsocketPort WRITE setObsWebsocketPort NOTIFY obsWebsocketPortChanged)
#endif
public:
  explicit SettingsViewModel(QObject* parent = nullptr) noexcept;
  SettingsViewModel(QObject* parent, const std::shared_ptr<Services::IServerSettingsStorage>& settingsStorage) noexcept;

  [[nodiscard]] auto deckServerAddress() const noexcept -> QString { return m_deckServerAddress; }
  void setDeckServerAddress(const QString& deckServerAddress) noexcept;
  [[nodiscard]] auto deckServerPort() const noexcept -> QString { return m_deckServerPort; }
  void setDeckServerPort(const QString& deckServerPort) noexcept;
  [[nodiscard]] auto obsWebsocketAddress() const noexcept -> QString { return m_obsWebsocketAddress; }
  void setObsWebsocketAddress(const QString& obsWebsocketAddress) noexcept;
  [[nodiscard]] auto obsWebsocketPort() const noexcept -> QString { return m_obsWebsocketPort; }
  void setObsWebsocketPort(const QString& obsWebsocketPort) noexcept;

  Q_INVOKABLE void saveSettings() noexcept;

#if QT_VERSION_MAJOR == 6
  auto bindableDeckServerAddress() -> QBindable<QString> { return &m_deckServerAddress; }
  auto bindableDeckServerPort() -> QBindable<QString> { return &m_deckServerPort; }
  auto bindableObsWebsocketAddress() -> QBindable<QString> { return &m_obsWebsocketAddress; }
  auto bindableObsWebsocketPort() -> QBindable<QString> { return &m_obsWebsocketPort; }
#endif

  static void registerType();

signals:
  void deckServerAddressChanged();
  void deckServerPortChanged();
  void obsWebsocketAddressChanged();
  void obsWebsocketPortChanged();

private:
#if QT_VERSION_MAJOR == 6
  Q_OBJECT_BINDABLE_PROPERTY(SettingsViewModel, QString, m_deckServerAddress, // NOLINT
                             &SettingsViewModel::deckServerAddressChanged)
  Q_OBJECT_BINDABLE_PROPERTY(SettingsViewModel, QString, m_deckServerPort, &SettingsViewModel::deckServerPortChanged) // NOLINT
  Q_OBJECT_BINDABLE_PROPERTY(SettingsViewModel, QString, m_obsWebsocketAddress, // NOLINT
                             &SettingsViewModel::obsWebsocketAddressChanged)
  Q_OBJECT_BINDABLE_PROPERTY(SettingsViewModel, QString, m_obsWebsocketPort, // NOLINT
                             &SettingsViewModel::obsWebsocketPortChanged)
#else
  QString m_deckServerAddress;
  QString m_deckServerPort;
  QString m_obsWebsocketAddress;
  QString m_obsWebsocketPort;
#endif

  mutable boost::log::sources::severity_logger<boost::log::trivial::severity_level> m_slg;
};
}

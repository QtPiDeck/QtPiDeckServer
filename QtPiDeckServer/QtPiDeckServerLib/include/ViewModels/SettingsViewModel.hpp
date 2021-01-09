#pragma once

#include <QObject>
#include <QDebug>

#include "QtBinding.hpp"

#include "Services/UseServices.hpp"
#include "Services/IServerSettingsStorage.hpp"

namespace QtPiDeck::ViewModels {
class SettingsViewModel : public QObject, public Services::UseServices<Services::IServerSettingsStorage> {
    Q_OBJECT // NOLINT
    Q_PROPERTY(QString deckServerAddress READ deckServerAddress WRITE setDeckServerAddress
               NOTIFY deckServerAddressChanged
#if QT_VERSION_MAJOR == 6
               BINDABLE bindableDeckServerAddress
#endif
               )
    Q_PROPERTY(QString deckServerPort READ deckServerPort WRITE setDeckServerPort NOTIFY deckServerPortChanged
#if QT_VERSION_MAJOR == 6
               BINDABLE bindableDeckServerPort
#endif
               )
    Q_PROPERTY(QString obsWebsocketAddress READ obsWebsocketAddress WRITE setObsWebsocketAddress
               NOTIFY obsWebsocketAddressChanged
#if QT_VERSION_MAJOR == 6
               BINDABLE bindableObsWebsocketAddress
#endif
               )
    Q_PROPERTY(QString obsWebsocketPort READ obsWebsocketPort WRITE setObsWebsocketPort NOTIFY obsWebsocketPortChanged
#if QT_VERSION_MAJOR == 6
               BINDABLE bindableObsWebsocketPort
#endif
               )
    Q_OBJECT_BINDABLE_PROPERTY(SettingsViewModel, QString, m_deckServerPort)
    Q_OBJECT_BINDABLE_PROPERTY(SettingsViewModel, QString, m_deckServerAddress)
    Q_OBJECT_BINDABLE_PROPERTY(SettingsViewModel, QString, m_obsWebsocketAddress)
    Q_OBJECT_BINDABLE_PROPERTY(SettingsViewModel, QString, m_obsWebsocketPort)
public:
    explicit SettingsViewModel(QObject* parent = nullptr) noexcept;
    SettingsViewModel(QObject* parent, std::shared_ptr<Services::IServerSettingsStorage> settingsStorage) noexcept;

    [[nodiscard]] auto deckServerAddress() const noexcept -> QString { return m_deckServerAddress; }
    void setDeckServerAddress(const QString& deckServerAddress) noexcept;
    [[nodiscard]] auto deckServerPort() const noexcept -> QString { return m_deckServerPort; }
    void setDeckServerPort(const QString& deckServerPort) noexcept;
    [[nodiscard]] auto obsWebsocketAddress() const noexcept -> QString { return m_obsWebsocketAddress; }
    void setObsWebsocketAddress(const QString& obsWebsocketAddress) noexcept;
    [[nodiscard]] auto obsWebsocketPort() const noexcept -> QString { return m_obsWebsocketPort; }
    void setObsWebsocketPort(const QString& obsWebsocketPort) noexcept;

    Q_INVOKABLE void saveSettings() noexcept;

    BINDABLE(QString, bindableDeckServerAddress, m_deckServerAddress)
    BINDABLE(QString, bindableDeckServerPort, m_deckServerPort)
    BINDABLE(QString, bindableObsWebsocketAddress, m_obsWebsocketAddress)
    BINDABLE(QString, bindableObsWebsocketPort, m_obsWebsocketPort)

    static void registerType();

signals:
    void deckServerAddressChanged();
    void deckServerPortChanged();
    void obsWebsocketAddressChanged();
    void obsWebsocketPortChanged();
};
}

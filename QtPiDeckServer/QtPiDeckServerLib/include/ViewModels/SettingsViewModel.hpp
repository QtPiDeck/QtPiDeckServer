#pragma once

#include <QObject>
#include <QDebug>

#include "Services/UseServices.hpp"
#include "Services/IServerSettingsStorage.hpp"

namespace QtPiDeck::ViewModels {
class SettingsViewModel : public QObject, public Services::UseServices<Services::IServerSettingsStorage> {
    Q_OBJECT // NOLINT
    Q_PROPERTY(QString deckServerAddress READ deckServerAddress WRITE setDeckServerAddress NOTIFY deckServerAddressChanged)
    Q_PROPERTY(QString deckServerPort READ deckServerPort WRITE setDeckServerPort NOTIFY deckServerPortChanged)
public:
    explicit SettingsViewModel(QObject* parent = nullptr) noexcept;
    SettingsViewModel(QObject* parent, std::shared_ptr<Services::IServerSettingsStorage>&& storageService) noexcept;

    [[nodiscard]] auto deckServerAddress() const noexcept -> QString { return m_deckServerAddress; }
    void setDeckServerAddress(const QString& deckServerAddress) noexcept;
    [[nodiscard]] auto deckServerPort() const noexcept -> QString { return m_deckServerPort; }
    void setDeckServerPort(const QString& deckServerPort) noexcept;

    Q_INVOKABLE void saveSettings() noexcept;

    static void registerType();

signals:
    void deckServerAddressChanged();
    void deckServerPortChanged();

private:
    QString m_deckServerAddress;
    QString m_deckServerPort;
};
}

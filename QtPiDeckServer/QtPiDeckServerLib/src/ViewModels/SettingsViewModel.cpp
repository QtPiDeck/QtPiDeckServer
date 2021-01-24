#include "ViewModels/SettingsViewModel.hpp"

#include "Application.hpp"

namespace QtPiDeck::ViewModels {
SettingsViewModel::SettingsViewModel(QObject* parent) noexcept : SettingsViewModel(parent, nullptr) {

}

SettingsViewModel::SettingsViewModel(QObject* parent,
                                     const std::shared_ptr<Services::IServerSettingsStorage> & settingsStorage) noexcept
    : QObject(parent) {
    setService(settingsStorage);
    if (!settingsStorage) {
        qDebug() << "No IClientSettingsStorage provided";
        return;
    }

    m_deckServerAddress = settingsStorage->deckServerAddress();
    m_deckServerPort = settingsStorage->deckServerPort();
    m_obsWebsocketAddress = settingsStorage->obsWebsocketAddress();
    m_obsWebsocketPort = settingsStorage->obsWebsocketPort();
}

void SettingsViewModel::setDeckServerAddress(const QString& deckServerAddress) noexcept {
    if (deckServerAddress == m_deckServerAddress) {
        return;
    }

    m_deckServerAddress = deckServerAddress;
    emit deckServerAddressChanged();
}

void SettingsViewModel::setDeckServerPort(const QString& deckServerPort) noexcept {
    if (deckServerPort == m_deckServerPort) {
        return;
    }

    m_deckServerPort = deckServerPort;
    emit deckServerPortChanged();
}

void SettingsViewModel::setObsWebsocketAddress(const QString& obsWebsocketAddress) noexcept {
    if (obsWebsocketAddress == m_obsWebsocketAddress) {
        return;
    }

    m_obsWebsocketAddress = obsWebsocketAddress;
    emit obsWebsocketAddressChanged();
}

void SettingsViewModel::setObsWebsocketPort(const QString& obsWebsocketPort) noexcept {
    if (obsWebsocketPort == m_obsWebsocketPort) {
        return;
    }

    m_obsWebsocketPort = obsWebsocketPort;
    emit obsWebsocketPortChanged();
}

void SettingsViewModel::saveSettings() noexcept {
    auto& settingsStorage = service<Services::IServerSettingsStorage>();
    if (!settingsStorage) {
        qDebug() << "No IClientSettingsStorage provided";
        return;
    }

    settingsStorage->setDeckServerAddress(m_deckServerAddress);
    settingsStorage->setDeckServerPort(m_deckServerPort);
    settingsStorage->setObsWebsocketAddress(m_obsWebsocketAddress);
    settingsStorage->setObsWebsocketPort(m_obsWebsocketPort);

    qDebug() << "Settings were saved";
}

void SettingsViewModel::registerType() {
    qmlRegisterType<SettingsViewModel>("QtPiDeck", 1, 0, "SettingsViewModel");
}
}

// not so private anymore, huh
// overload object creation in qml for a specific type
namespace QQmlPrivate
{
using namespace QtPiDeck;
template<>
class QQmlElement<ViewModels::SettingsViewModel> final : public ViewModels::SettingsViewModel // NOLINT
{
public:
    using ViewModels::SettingsViewModel::SettingsViewModel;
    ~QQmlElement() override {
        QQmlPrivate::qdeclarativeelement_destructor(this);
    }
    static void operator delete(void *ptr) { // NOLINT
        ::operator delete (ptr);
    }
    static void operator delete(void *, void *) { // NOLINT
    }
};

template<>
#if QT_VERSION_MAJOR == 6
void createInto<ViewModels::SettingsViewModel>(void *memory, void *) { // NOLINT
#else
void createInto<ViewModels::SettingsViewModel>(void *memory) {
#endif
    const auto& ioc = Application::current()->ioc();
    using viewModelType = QQmlElement<ViewModels::SettingsViewModel>;
    [[maybe_unused]] auto* _ = ioc.make<viewModelType, Services::CreationType::RawInMemory>(memory);
}
}

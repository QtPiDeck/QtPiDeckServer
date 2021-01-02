#include "ViewModels/SettingsViewModel.hpp"

#include "Application.hpp"

namespace QtPiDeck::ViewModels {
SettingsViewModel::SettingsViewModel(QObject* parent) noexcept : SettingsViewModel(parent, nullptr) {

}

SettingsViewModel::SettingsViewModel(QObject* parent,
                                     std::shared_ptr<Services::IServerSettingsStorage>&& storageService) noexcept
    : QObject(parent) {
    setService(storageService);
    const auto& settingsStorage = service<Services::IServerSettingsStorage>();
    if (!settingsStorage) {
        qDebug() << "No IClientSettingsStorage provided";
        return;
    }

    m_deckServerAddress = settingsStorage->deckServerAddress();
    m_deckServerPort = settingsStorage->deckServerPort();
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

void SettingsViewModel::saveSettings() noexcept {
    auto& settingsStorage = service<Services::IServerSettingsStorage>();
    if (!settingsStorage) {
        qDebug() << "No IClientSettingsStorage provided";
        return;
    }

    settingsStorage->setDeckServerAddress(m_deckServerAddress);
    settingsStorage->setDeckServerPort(m_deckServerPort);
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
    auto settingsStorage = Application::current()->ioc().resolveService<Services::IServerSettingsStorage>();
    new (memory) QQmlElement<ViewModels::SettingsViewModel>{nullptr, std::move(settingsStorage)};
}
}

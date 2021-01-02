#include "ServerApplication.hpp"

#include <QCoreApplication>

#include "Services/MessageBus.hpp"
#include "Services/SettingsStorage.hpp"
#include "ViewModels/SettingsViewModel.hpp"

static void initStaticResouces() {
    Q_INIT_RESOURCE(qml); // NOLINT
}

namespace QtPiDeck {
ServerApplication::ServerApplication() = default;

auto ServerApplication::mainPage() -> QUrl {
    return QUrl{QStringLiteral("qrc:/qml/pages/main.qml")};
}

void ServerApplication::initialPreparations() {
    Application::initialPreparations();

    ioc().registerSingleton(std::make_shared<Services::MessageBus>(nullptr));
    ioc().registerService<Services::IServerSettingsStorage, Services::SettingsStorage>();

#if QT_VERSION_MAJOR < 6
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
}

void ServerApplication::appCreated() {
    initStaticResouces();
}

void ServerApplication::engineCreated(QQmlApplicationEngine & engine) {
    // during early testing(server and client in app) communication was broken
    // starting server this way fixes a problem
    // might need additional testing when client side app is ready to connect
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &m_deckServer, &Network::DeckServer::start); // clazy:exclude=connect-non-signal

    engine.addImportPath("qrc:/qml/components");

    ViewModels::SettingsViewModel::registerType();
}
}

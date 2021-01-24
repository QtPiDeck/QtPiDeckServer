#include "ServerApplication.hpp"

#include <QCoreApplication>

#include "Services/MessageBus.hpp"
#include "Services/ObsMessageIdGenerator.hpp"
#include "Services/SettingsStorage.hpp"
#if QT_VERSION_MAJOR == 6
#include "Services/WebSocketpp.hpp"
#else
#include "Services/WebSocketQt.hpp"
#endif
#include "ViewModels/SettingsViewModel.hpp"

static void initStaticResouces() {
  Q_INIT_RESOURCE(qml); // NOLINT
}

namespace QtPiDeck {
ServerApplication::ServerApplication() = default;

auto ServerApplication::mainPage() -> QUrl { return QUrl{QStringLiteral("qrc:/qml/pages/main.qml")}; }

void ServerApplication::initialPreparations() {
  Application::initialPreparations();

  ioc().registerSingleton<Services::IMessageBus>(std::make_shared<Services::MessageBus>(nullptr));
  ioc().registerService<Services::IServerSettingsStorage, Services::SettingsStorage>();
  ioc().registerService<Services::IObsMessageIdGenerator, Services::ObsMessageIdGenerator>();
#if QT_VERSION_MAJOR == 6
  ioc().registerService<Services::IWebSocket, Services::WebSocketpp>();
#else
  ioc().registerService<Services::IWebSocket, Services::WebSocketQt>();
#endif

#if QT_VERSION_MAJOR < 6
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
}

void ServerApplication::appCreated() { initStaticResouces(); }

void ServerApplication::engineCreated(QQmlApplicationEngine& engine) {
  // during early testing(server and client in app) communication was broken
  // starting server this way fixes a problem
  // might need additional testing when client side app is ready to connect
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &m_deckServer, // clazy:exclude=connect-non-signal
                   &Network::DeckServer::start);

  engine.addImportPath("qrc:/qml/components");

  ViewModels::SettingsViewModel::registerType();

  m_obsWebsocketClient = ioc().make<Network::Obs::ObsWebsocketClient>();
  m_obsWebsocketClient->connectToObs();
}
}

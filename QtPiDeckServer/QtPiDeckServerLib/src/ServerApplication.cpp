#include "ServerApplication.hpp"

#include <QCoreApplication>
#include <QQuickStyle>

#include "Network/Obs/ObsRequests.hpp"
#include "Services/MessageBus.hpp"
#include "Services/ObsMessageIdGenerator.hpp"
#include "Services/SettingsStorage.hpp"
#include "Services/WebSocketQt.hpp"
#include "Utilities/Literals.hpp"
#include "ViewModels/SettingsViewModel.hpp"

#include "QtDefinitions.hpp"

static void initStaticResouces() {
  Q_INIT_RESOURCE(qml); // NOLINT
}

namespace QtPiDeck {
auto ServerApplication::mainPage() -> QUrl {
  using namespace QtPiDeck::Utilities::literals;
  return "qrc:/qml/pages/main.qml"_qurl;
}

void ServerApplication::initialPreparations() {
  Application::initialPreparations();

  // native windows style doesn't look pretty
  // this will stay until custom style is created
#if QT_VERSION_MAJOR == 6
  if (QSysInfo::productType() == QtPlatform::windows) {
    QQuickStyle::setStyle(QtStyle::Basic);
  }
#endif

  ioc().registerSingleton<Services::IMessageBus>(std::make_shared<Services::MessageBus>(nullptr));
  ioc().registerService<Services::IServerSettingsStorage, Services::SettingsStorage>();
  ioc().registerService<Services::IObsMessageIdGenerator, Services::ObsMessageIdGenerator>();
  ioc().registerService<Services::IWebSocket, Services::WebSocketQt>();

#if QT_VERSION_MAJOR < 6
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
}

void ServerApplication::appCreated() { initStaticResouces(); }

void ServerApplication::engineCreated(QQmlApplicationEngine& engine) {
  m_deckServer = ioc().make<Network::DeckServer>();

  // during early testing(server and client in app) communication was broken
  // starting server this way fixes a problem
  // might need additional testing when client side app is ready to connect
  QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, // clazy:exclude=connect-non-signal
                   m_deckServer.get(), &Network::DeckServer::start);

  using namespace QtPiDeck::Utilities::literals;
  engine.addImportPath("qrc:/qml/components"_qs);

  ViewModels::SettingsViewModel::registerType();

  m_obsWebsocketClient = ioc().make<Network::Obs::ObsWebsocketClient>();
  m_obsWebsocketClient->connectToObs();
}
}

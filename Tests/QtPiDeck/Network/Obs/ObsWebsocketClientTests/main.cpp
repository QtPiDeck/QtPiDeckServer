#include <QtTest>

#include "Network/Obs/ObsWebsocketClient.hpp"

namespace QtPiDeck::Network::Obs::Tests {
class ObsWebsocketClientTests : public QObject {
  Q_OBJECT // NOLINT

private slots:
  void init();
  //void

private: // NOLINT(readability-redundant-access-specifiers)
  std::unique_ptr<ObsWebsocketClient> client;
};

void ObsWebsocketClientTests::init() {}



}

QTEST_APPLESS_MAIN(QtPiDeck::Network::Obs::Tests::ObsWebsocketClientTests) // NOLINT

#include "main.moc"

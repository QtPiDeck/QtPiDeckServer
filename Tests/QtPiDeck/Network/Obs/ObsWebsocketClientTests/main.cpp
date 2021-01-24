#include <QtTest>

#include "Network/Obs/ObsWebsocketClient.hpp"

namespace QtPiDeck::Services::Tests {
class ObsWebsocketClientTests : public QObject {
    Q_OBJECT // NOLINT

private slots:
    void init();

private: // NOLINT(readability-redundant-access-specifiers)
};

void ObsWebsocketClientTests::init() {
}

}

QTEST_APPLESS_MAIN(QtPiDeck::Services::Tests::ObsWebsocketClientTests) // NOLINT

#include "main.moc"

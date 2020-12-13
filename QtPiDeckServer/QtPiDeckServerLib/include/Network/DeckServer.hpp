#pragma once

#include <QObject>
#include <QTcpServer>

#include "Application.hpp"

namespace QtPiDeck::Server::Network {

enum DeckMessages : uint64_t {
    PingReceived = 0x100
};

class DeckServer : public QObject {
    Q_OBJECT // NOLINT
public:
    explicit DeckServer(QObject *parent = nullptr);

public slots: // NOLINT(readability-redundant-access-specifiers)
    void handleConnection();
    void readData();

private:
    void connectToServerSignal();
    void sendPong();

    QTcpServer *m_server{};
    QTcpSocket *m_socket{};
    QMetaObject::Connection m_serverConnection;
};
}

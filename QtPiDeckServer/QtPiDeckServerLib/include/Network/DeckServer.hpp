#pragma once

#include <optional>

#include <QObject>
#include <QTcpServer>

#include "Application.hpp"
#include "Network/MessageHeader.hpp"
#include "Services/IMessageBus.hpp"

namespace QtPiDeck::Network {

enum DeckMessages : uint64_t { PingReceived = 0x100 };

class DeckServer : public QObject, public Services::UseServices<Services::IMessageBus> {
  Q_OBJECT // NOLINT
public:
  explicit DeckServer(QObject* parent, std::shared_ptr<Services::IMessageBus> messageBus);

  void start();

public slots: // NOLINT(readability-redundant-access-specifiers)
  void handleConnection();
  void readData();

private:
  void connectToServerSignal();
  void sendPong(const Bus::Message&);
  void processMessage(const QtPiDeck::Network::MessageHeader& header) noexcept;

  QTcpServer m_server;
  QTcpSocket* m_socket{};
  std::optional<QMetaObject::Connection> m_serverConnection;

  Utilities::Connection m_sub;
};
}

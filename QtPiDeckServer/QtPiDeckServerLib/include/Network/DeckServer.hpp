#pragma once

#include <optional>

#include <QObject>
#include <QTcpServer>

#include <boost/log/attributes/constant.hpp>
#include <boost/log/trivial.hpp>

#include "Services/UseServices.hpp"
#include "Network/MessageHeader.hpp"
#include "Services/IMessageBus.hpp"
#include "Utilities/Logging.hpp"

namespace QtPiDeck::Network {

enum DeckMessages : uint64_t { PingReceived = 0x100 };

namespace detail {
template<class Derived, class TcpServer = QTcpServer, class TcpSocket = QTcpSocket>
class DeckServerPrivate : public QObject, public Services::UseServices<Services::IMessageBus> {
public:
  explicit DeckServerPrivate(QObject* parent, std::shared_ptr<Services::IMessageBus> messageBus);
  void start();

  [[nodiscard]] auto currentConnection() -> TcpSocket* { return m_socket; }
  [[nodiscard]] auto currentConnection() const -> const TcpSocket* { return m_socket; }

protected:
  [[nodiscard]] auto server() -> TcpServer& { return m_server; }
  [[nodiscard]] auto server() const -> const TcpServer& { return m_server; }

private:
  void connectToNewConnectionServerSignal();
  void handleConnection();
  void subscribeToUtilityMessages();
  void sendPong(const Bus::Message& message);

  Utilities::Connection m_pingConnection{};
  std::optional<QMetaObject::Connection> m_serverConnection{};
  TcpServer m_server{};
  TcpSocket* m_socket{};

  mutable boost::log::sources::severity_logger<boost::log::trivial::severity_level> m_slg;
};
}

class DeckServer : public detail::DeckServerPrivate<DeckServer> {
  Q_OBJECT // NOLINT
  friend detail::DeckServerPrivate<DeckServer>;
  using DeckServerPrivate<DeckServer>::DeckServerPrivate;
};
}

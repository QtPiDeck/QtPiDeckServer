#include "DeckServer.hpp"

#include "Network/DeckDataStream.hpp"

namespace QtPiDeck::Network::detail {

template<class Derived, class TcpServer, class TcpSocket>
DeckServerPrivate<Derived, TcpServer, TcpSocket>::DeckServerPrivate(
    QObject* parent, std::shared_ptr<Services::IMessageBus> messageBus) noexcept
    : QObject(parent) {
  setService(std::move(messageBus));
  Utilities::initLogger(m_slg, "DeckServer");
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::start() noexcept {
  connectToNewConnectionServerSignal();

  subscribeToUtilityMessages();

  // default port and address will be in global config
  constexpr qint16 defaultPort = 13000;
  if (!m_server.listen(QHostAddress::LocalHost, defaultPort)) {
    BOOST_LOG_SEV(m_slg, Utilities::severity::warning) << "Failed to start TcpServer";
  }

  BOOST_LOG_SEV(m_slg, Utilities::severity::debug) << "Server setup completed";
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::connectToNewConnectionServerSignal() {
  // until multiple connections feature is supported
  assert(!m_serverConnection.has_value()); // LCOV_EXCL_LINE NOLINT

  m_serverConnection = connect(&m_server, &TcpServer::newConnection, this, &DeckServerPrivate::handleConnection);
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::subscribeToUtilityMessages() {
  using namespace Services;
  m_pingConnection = subscribe(*service<IMessageBus>(), this, &DeckServerPrivate::sendPong, DeckMessages::PingReceived);
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::handleConnection() noexcept {
  BOOST_LOG_SEV(m_slg, Utilities::severity::debug) << "New client";
  // only one connection at the same time
  disconnect(*m_serverConnection);
  m_serverConnection.reset();
  m_socket = m_server.nextPendingConnection();
  connect(m_socket, &TcpSocket::disconnected, this, &DeckServerPrivate::connectToNewConnectionServerSignal);
  connect(m_socket, &TcpSocket::readyRead, this, &DeckServerPrivate::readData);
}

template<class T, class Socket>
void writeObject(const T& object, Socket* socket) {
  QByteArray tmp;
  tmp.reserve(sizeof(T));
  DeckDataStream outStream{&tmp, QIODevice::WriteOnly};
  outStream << object;
  socket->write(tmp);
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::sendPong(const Bus::Message& message) {
  const auto requestId = [&message]() noexcept {
    QString value;
    QDataStream qds{message.payload};
    qds >> value;
    return value;
  }();
  BOOST_LOG_SEV(m_slg, Utilities::severity::debug) << "Sending pong" << requestId.toStdString();
  writeObject(getEmptyMessageHeader(MessageType::Pong, requestId), m_socket);
  m_socket->flush();
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::processMessage(
    const QtPiDeck::Network::MessageHeader& header) noexcept {
  auto sendMessage = [this](DeckMessages messageType, const QString& requestId) {
    QByteArray payload;
    QDataStream qds{&payload, QIODevice::WriteOnly};
    qds << requestId;
    service<Services::IMessageBus>()->sendMessage(Bus::Message{messageType, payload});
  };
  switch (header.messageType) {
  case QtPiDeck::Network::MessageType::Ping:
    BOOST_LOG_SEV(m_slg, Utilities::severity::debug) << "Got ping" << header.requestId.toStdString();
    sendMessage(DeckMessages::PingReceived, header.requestId);
    break;
  default:
    BOOST_LOG_SEV(m_slg, Utilities::severity::warning) << "Unhandled message";
    break;
  }
}
template<class T, class TSocket>
auto readObject(TSocket* socket) -> std::optional<T> {
  DeckDataStream inStream{socket};
  T object{};

  inStream.startTransaction();
  inStream >> object;
  if (inStream.commitTransaction()) {
    return object;
  }

  return std::nullopt;
}
template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::readData() {
  const auto header = readObject<QtPiDeck::Network::MessageHeader>(m_socket);

  if (header) {
    processMessage(*header);
  }
}
}
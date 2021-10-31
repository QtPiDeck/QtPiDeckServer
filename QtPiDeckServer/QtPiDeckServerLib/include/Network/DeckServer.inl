#include "DeckServer.hpp"

namespace QtPiDeck::Network::detail {

template<class Derived, class TcpServer, class TcpSocket>
DeckServerPrivate<Derived, TcpServer, TcpSocket>::DeckServerPrivate(QObject* parent,
                                                         std::shared_ptr<Services::IMessageBus> messageBus)
    : QObject(parent) {
  setService(std::move(messageBus));
  Utilities::initLogger(m_slg, "DeckServer");
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::start() {
  connectToNewConnectionServerSignal();

  subscribeToUtilityMessages();

  // default port and address will be in global config
  /*constexpr qint16 defaultPort = 13000;
  if (!m_server.listen(QHostAddress::LocalHost, defaultPort)) {
    BOOST_LOG_SEV(m_slg, Utilities::severity::warning) << "Failed to start TcpServer";
  }

  BOOST_LOG_SEV(m_slg, Utilities::severity::debug) << "Server setup completed";*/
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::connectToNewConnectionServerSignal() {
  if (m_serverConnection) {
    disconnect(*m_serverConnection);
  }

  m_serverConnection = connect(&m_server, &TcpServer::newConnection, this, &DeckServerPrivate::handleConnection);
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::subscribeToUtilityMessages() {
  using namespace Services;
  m_pingConnection = subscribe(*service<IMessageBus>(), this, &DeckServerPrivate::sendPong, DeckMessages::PingReceived);
}

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::handleConnection() {
  BOOST_LOG_SEV(m_slg, Utilities::severity::debug) << "New client";
  // only one connection at the same time
  disconnect(*m_serverConnection);
  m_serverConnection.reset();
  m_socket = m_server.nextPendingConnection();
  /*connect(m_socket, &QTcpSocket::disconnected, this, &DeckServer::connectToNewConnectionServerSignal);
  connect(m_socket, &QTcpSocket::readyRead, this, &DeckServer::readData);*/
}

/*namespace {
template<class T, class Socket>
void writeObject(const T& object, Socket* socket) {
  QByteArray tmp;
  tmp.reserve(sizeof(T));
  DeckDataStream outStream{&tmp, QIODevice::WriteOnly};
  outStream << object;
  socket->write(tmp);
}
}*/

template<class Derived, class TcpServer, class TcpSocket>
void DeckServerPrivate<Derived, TcpServer, TcpSocket>::sendPong(const Bus::Message& message) {
  /*const auto requestId = [&message]() noexcept {
    QString value;
    QDataStream qds{message.payload};
    qds >> value;
    return value;
  }();
  qDebug() << "Sending pong" << requestId;
  writeObject(getEmptyMessageHeader(MessageType::Pong, requestId), m_socket);
  m_socket->flush();*/
}
}
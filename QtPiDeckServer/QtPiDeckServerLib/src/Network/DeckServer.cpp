#include "Network/DeckServer.hpp"

#include <QTcpSocket>

#include "Application.hpp"
#include "Network/DeckDataStream.hpp"
#include "Network/MessageHeader.hpp"
#include "Services/IMessageBus.hpp"

namespace QtPiDeck::Network {
DeckServer::DeckServer(QObject* parent, std::shared_ptr<Services::IMessageBus> messageBus) : QObject(parent) {
  setService(std::move(messageBus));
}

void DeckServer::start() {
  connectToServerSignal();
  m_sub =
      Services::subscribe(*service<Services::IMessageBus>(), this, &DeckServer::sendPong, DeckMessages::PingReceived);
  // default port and address will be in global config
  constexpr qint16 defaultPort = 13000;
  if (!m_server.listen(QHostAddress::LocalHost, defaultPort)) {
    qWarning() << "Failed to start DeckServer";
  }

  qDebug() << "Server started";
}

namespace {
template<class T>
void writeObject(const T& object, QTcpSocket* socket) {
  QByteArray tmp;
  tmp.reserve(sizeof(T));
  DeckDataStream outStream{&tmp, QIODevice::WriteOnly};
  outStream << object;
  socket->write(tmp);
}
}

void DeckServer::sendPong(const Bus::Message& message) {
  const auto requestId = [&message]() noexcept {
    QString value;
    QDataStream qds{message.payload};
    qds >> value;
    return value;
  }();
  qDebug() << "Sending pong" << requestId;
  writeObject(getEmptyMessageHeader(MessageType::Pong, requestId), m_socket);
  m_socket->flush();
}

void DeckServer::handleConnection() {
  qDebug() << "New client";
  // only one connection at the same time
  disconnect(*m_serverConnection);
  m_serverConnection.reset();
  m_socket = m_server.nextPendingConnection();
  connect(m_socket, &QTcpSocket::disconnected, this, &DeckServer::connectToServerSignal);
  connect(m_socket, &QTcpSocket::readyRead, this, &DeckServer::readData);
}

void DeckServer::processMessage(const QtPiDeck::Network::MessageHeader& header) noexcept {
  auto sendMessage = [this](DeckMessages messageType, QString requestId) {
    QByteArray payload;
    QDataStream qds{&payload, QIODevice::WriteOnly};
    qds << requestId;
    // see CMakeLists.txt
#if QT_VERSION_CHECK(6, 1, 0) == QT_VERSION && _MSC_VER
    ServiceUser<Services::IMessageBus>::service()
#else
    service<Services::IMessageBus>()
#endif
        ->sendMessage(Bus::Message{messageType, payload});
  };
  switch (header.messageType) {
  case QtPiDeck::Network::MessageType::Ping:
    qDebug() << "Got ping" << header.requestId;
    sendMessage(DeckMessages::PingReceived, header.requestId);
    break;
  default:
    qWarning() << "Unhandled message";
    break;
  }
}

namespace {
template<class T>
auto readObject(QTcpSocket* socket) -> std::optional<T> {
  DeckDataStream inStream{socket};
  T object{};

  inStream.startTransaction();
  inStream >> object;
  if (inStream.commitTransaction()) {
    return object;
  }

  return std::nullopt;
}
}

void DeckServer::readData() {
  const auto header = readObject<QtPiDeck::Network::MessageHeader>(m_socket);

  if (header) {
    processMessage(*header);
  }
}

void DeckServer::connectToServerSignal() {
  if (m_serverConnection) {
    disconnect(*m_serverConnection);
  }

  m_serverConnection = connect(&m_server, &QTcpServer::newConnection, this, &DeckServer::handleConnection);
}
}

#include "Network/DeckServer.inl"

#include <QTcpSocket>

#include "Application.hpp"
#include "Network/DeckDataStream.hpp"
#include "Network/MessageHeader.hpp"
#include "Services/IMessageBus.hpp"


namespace QtPiDeck::Network {
/*void DeckServer::start() {
  connectToNewConnectionServerSignal();
  m_sub =
      Services::subscribe(*service<Services::IMessageBus>(), this, &DeckServer::sendPong, DeckMessages::PingReceived);
  // default port and address will be in global config
  constexpr qint16 defaultPort = 13000;
  if (!m_server.listen(QHostAddress::LocalHost, defaultPort)) {
    qWarning() << "Failed to start DeckServer";
  }

  qDebug() << "Server started";
}*/

//void DeckServer::processMessage(const QtPiDeck::Network::MessageHeader& header) noexcept {
  /*auto sendMessage = [this](DeckMessages messageType, QString requestId) {
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
  }*/
//}

/*namespace {
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

void DeckServer::readData() {*/
  /*const auto header = readObject<QtPiDeck::Network::MessageHeader>(m_socket);

  if (header) {
    processMessage(*header);
  }*/
//}
}

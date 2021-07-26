#include "Services/WebSocketQt.hpp"

namespace QtPiDeck::Services {
void WebSocketQt::connect(QStringView address) noexcept { m_webSocket.open(QUrl{address.toString()}); }

void WebSocketQt::disconnect() noexcept { m_webSocket.close(); }

auto WebSocketQt::send(QByteArray message) noexcept -> std::optional<SendingError> {
  if (!m_webSocket.isValid()) {
    return SendingError::NotConnected;
  }

  // makes copy of payload
  m_webSocket.sendBinaryMessage(message);

  return std::nullopt;
}

void WebSocketQt::setMessageReceivedHandler(MessageReceivedHandler handler) noexcept {
  if (m_messageReceivedConnection) {
    QObject::disconnect(*m_messageReceivedConnection);
  }

  m_messageReceivedConnection = QObject::connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this,
                                                 [handler](const QByteArray& msg) { handler(msg); });
}

void WebSocketQt::setFailHandler(FailHandler handler) noexcept {
  if (m_failConnection) {
    QObject::disconnect(*m_failConnection);
  }

  m_failConnection = QObject::connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                                      this, [handler](QAbstractSocket::SocketError error) {
                                        if (error == QAbstractSocket::SocketError::ConnectionRefusedError) {
                                          handler(ConnectionError::RefusedConnection);
                                        } else {
                                          handler(ConnectionError::Unspecified);
                                        }
                                      });
}

void WebSocketQt::setConnectedHandler(ConnectedHandler handler) noexcept {
  if (m_connectedConnection) {
    QObject::disconnect(*m_connectedConnection);
  }

  m_connectedConnection = QObject::connect(&m_webSocket, &QWebSocket::connected, this, [handler] { handler(); });
}
}

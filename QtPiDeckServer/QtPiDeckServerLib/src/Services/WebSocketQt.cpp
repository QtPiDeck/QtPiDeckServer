#include "Services/WebSocketQt.hpp"

namespace QtPiDeck::Services {
void WebSocketQt::connect(QStringView address) noexcept { m_webSocket.open(QUrl{address.toString()}); }

void WebSocketQt::disconnect() noexcept { m_webSocket.close(); }

auto WebSocketQt::send(QLatin1String message) noexcept -> std::optional<SendingError> {
  if (!m_webSocket.isValid()) {
    return SendingError::NotConnected;
  }

  m_webSocket.sendTextMessage(message);

  return std::nullopt;
}

void WebSocketQt::setTextReceivedHandler(TextReceivedHandler handler) noexcept {
  if (m_textReceivedConnection) {
    QObject::disconnect(*m_textReceivedConnection);
  }

  m_textReceivedConnection = QObject::connect(&m_webSocket, &QWebSocket::textMessageReceived, this,
                                              [handler](const QString& msg) { handler(msg); });
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

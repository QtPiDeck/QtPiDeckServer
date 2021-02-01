#pragma once

#include <QThread>
#include <QtWebSockets/QtWebSockets>

#include "IWebSocket.hpp"

namespace QtPiDeck::Services {
class WebSocketQt final : public QObject, public IWebSocket {
  Q_OBJECT // NOLINT
public:
  void connect(QStringView address) noexcept final;
  void disconnect() noexcept final;
  [[nodiscard]] auto send(QLatin1String message) noexcept -> std::optional<SendingError> final;
  void setTextReceivedHandler(TextReceivedHandler handler) noexcept final;
  void setFailHandler(FailHandler handler) noexcept final;
  void setConnectedHandler(ConnectedHandler handler) noexcept final;

private:
  QWebSocket m_webSocket;
  std::optional<QMetaObject::Connection> m_textReceivedConnection;
  std::optional<QMetaObject::Connection> m_failConnection;
  std::optional<QMetaObject::Connection> m_connectedConnection;
};
}

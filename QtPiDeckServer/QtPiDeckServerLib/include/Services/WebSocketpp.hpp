#pragma once

#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include "IWebSocket.hpp"

namespace QtPiDeck::Services {
class WebSocketpp final : public IWebSocket {
public:
  WebSocketpp();
  WebSocketpp(const WebSocketpp& /*other*/) = delete;
  WebSocketpp(WebSocketpp&& /*other*/) = delete;
  ~WebSocketpp() noexcept final;
  auto operator=(const WebSocketpp& /*other*/) -> WebSocketpp& = delete;
  auto operator=(WebSocketpp&& /*other*/) -> WebSocketpp& = delete;
  void connect(QStringView address) noexcept final;
  void disconnect() noexcept final;
  [[nodiscard]] auto send(QByteArray message) noexcept -> std::optional<SendingError> final;
  void setMessageReceivedHandler(MessageReceivedHandler handler) noexcept final { m_messageReceivedHandler = handler; }
  void setFailHandler(FailHandler handler) noexcept final { m_failHandler = handler; }
  void setConnectedHandler(ConnectedHandler handler) noexcept final { m_connectedHandler = handler; }

private:
  auto connected() noexcept -> bool;

  websocketpp::client<websocketpp::config::asio_client> m_webSocket{};
  websocketpp::connection_hdl m_connectionHandle{};
  MessageReceivedHandler m_messageReceivedHandler{};
  FailHandler m_failHandler{};
  ConnectedHandler m_connectedHandler{};
  std::thread m_iothread{};
};
}

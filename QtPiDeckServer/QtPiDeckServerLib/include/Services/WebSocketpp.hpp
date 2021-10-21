#pragma once

#include <thread>

#if defined(__clang__)
static_assert(__cplusplus >= 201103);
#if __has_include(<__config>)
#include <__config>
#if defined(_LIBCPP_VERSION)
/* #ifndef BOOST_ASIO_HAS_CLANG_LIBCXX
#error "No BOOST_ASIO_HAS_CLANG_LIBCXX"
#endif*/
#else
#error "No _LIBCPP_VERSION"
#endif
#else
#error "No __config header"
#endif
#endif

#include <boost/asio/detail/config.hpp>

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

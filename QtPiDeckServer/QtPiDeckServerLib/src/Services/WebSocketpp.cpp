#include "Services/WebSocketpp.hpp"

#include <QDebug>

namespace QtPiDeck::Services {
WebSocketpp::WebSocketpp() {
  m_webSocket.set_access_channels(websocketpp::log::elevel::none);
  m_webSocket.set_error_channels(websocketpp::log::elevel::all);

  m_webSocket.init_asio();
  m_webSocket.start_perpetual();
  m_webSocket.set_message_handler(
      [this](const websocketpp::connection_hdl& /*hdl*/, const websocketpp::config::asio_client::message_type::ptr& msg) {
        if (m_messageReceivedHandler) {
          m_messageReceivedHandler(QByteArray::fromStdString(msg->get_payload()));
        }
      });
  m_webSocket.set_fail_handler([this](const websocketpp::connection_hdl& /*hdl*/) {
    if (m_failHandler) {
      m_failHandler(ConnectionError::RefusedConnection);
    }
  });
  m_webSocket.set_open_handler([this](websocketpp::connection_hdl hdl) {
    m_connectionHandle = std::move(hdl);
    if (m_connectedHandler) {
      m_connectedHandler();
    }
  });
  m_iothread = std::thread{[this] { m_webSocket.run(); }};
}

WebSocketpp::~WebSocketpp() noexcept {
  m_webSocket.stop_perpetual();
  if (connected()) {
    disconnect();
  }

  if (m_iothread.joinable()) {
    m_iothread.join();
  }
}

void WebSocketpp::disconnect() noexcept {
  websocketpp::lib::error_code silent;
  m_webSocket.close(m_connectionHandle, websocketpp::close::status::normal, "", silent);
}

void WebSocketpp::connect(QStringView address) noexcept {
  if (connected()) {
    disconnect();
  }

  websocketpp::lib::error_code ec;
  const auto connection = m_webSocket.get_connection(address.toString().toStdString(), ec);
  if (ec.value() != 0) {
    if (m_failHandler) {
      m_failHandler(ConnectionError::Unspecified);
    }

    return;
  }

  m_webSocket.connect(connection);
}

auto WebSocketpp::connected() noexcept -> bool {
  websocketpp::lib::error_code silent;
  const auto& con = m_webSocket.get_con_from_hdl(m_connectionHandle, silent);
  return con && con->get_state() == websocketpp::session::state::open;
}

auto WebSocketpp::send(QByteArray message) noexcept -> std::optional<SendingError> {
  if (!connected()) {
    return SendingError::NotConnected;
  }

  websocketpp::lib::error_code ec;
  // makes a copy of data
  m_webSocket.send(m_connectionHandle, message.data(), message.size(), websocketpp::frame::opcode::text, ec);
  if (ec.value() != 0) {
    // return some error?
  }

  return std::nullopt;
}
}

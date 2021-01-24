#include "Services/WebSocketpp.hpp"

#include <QDebug>

namespace QtPiDeck::Services {
WebSocketpp::WebSocketpp() noexcept {
  m_webSocket.set_access_channels(websocketpp::log::elevel::none);
  m_webSocket.set_error_channels(websocketpp::log::elevel::all);

  m_webSocket.init_asio();
  m_webSocket.start_perpetual();
  m_webSocket.set_message_handler(
      [this](websocketpp::connection_hdl /*hdl*/, websocketpp::config::asio_client::message_type::ptr msg) {
        if (m_textReceivedHandler) {
          m_textReceivedHandler(QString::fromStdString(msg->get_payload()));
        }
      });
  m_webSocket.set_fail_handler([this](websocketpp::connection_hdl /*hdl*/) {
    if (m_failHandler) {
      m_failHandler(ConnectionError::RefusedConnection);
    }
  });
  m_webSocket.set_open_handler([this](websocketpp::connection_hdl hdl) {
    m_connectionHandle = hdl;
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
  if (ec.value()) {
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

auto WebSocketpp::send(QLatin1String message) noexcept -> std::optional<SendingError> {
  if (!connected()) {
    return SendingError::NotConnected;
  }

  websocketpp::lib::error_code ec;
  m_webSocket.send(m_connectionHandle, message.data(), message.size(), websocketpp::frame::opcode::text, ec);
  if (ec.value()) {
    // return some error?
  }

  return std::nullopt;
}
}

#pragma once

#include <array>
#include <optional>
#include <unordered_map>

#include <QAbstractSocket>
#include <QLoggingCategory>

#include "Bus/ObsMessages.hpp"
#include "ObsRequests.hpp"
#include "Services/IMessageBus.hpp"
#include "Services/IObsMessageIdGenerator.hpp"
#include "Services/IServerSettingsStorage.hpp"
#include "Services/IWebSocket.hpp"
#include "Services/UseServices.hpp"

Q_DECLARE_LOGGING_CATEGORY(ObsWebsocket) // NOLINT

namespace QtPiDeck::Network::Obs {

class ObsWebsocketClient : public QObject,
                           public Services::UseServices<Services::IMessageBus, Services::IServerSettingsStorage,
                                                        Services::IObsMessageIdGenerator, Services::IWebSocket> {
  Q_OBJECT // NOLINT

public:
  template <class... TServices>
  ObsWebsocketClient(const std::shared_ptr<TServices>&... services) noexcept {
    (setService<TServices>(std::move(services)), ...);
    auto& webSocket = service<Services::IWebSocket>();
    webSocket->setTextReceivedHandler([this](QString message) { receivedTestMessage(std::move(message)); });
    webSocket->setConnectedHandler([this] { checkAuthRequirement(); });
    webSocket->setFailHandler([this](Services::IWebSocket::ConnectionError error) {
      constexpr std::array errors = {QAbstractSocket::SocketError::ConnectionRefusedError,
                                     QAbstractSocket::SocketError::UnknownSocketError};
      webSocketError(errors.at(static_cast<uint32_t>(error)));
    });
  }

  void connectToObs() noexcept;

  template<ObsRequest TRequest>
  void sendRequest(TRequest requestId, Bus::ObsMessages callbackMessageId) noexcept {
    std::variant<General, MediaControl> d;
    [[maybe_unused]] auto k = std::get<General>(d);
    sendRequest(static_cast<uint16_t>(requestId), callbackMessageId);
  }

  void sendRequest(uint16_t requestId, Bus::ObsMessages callbackMessageId) noexcept;

public slots: // NOLINT(readability-redundant-access-specifiers)
  void webSocketError(QAbstractSocket::SocketError error);
  void receivedTestMessage(QString message);
  void checkAuthRequirement();

private:
  void send(uint16_t requestId, const QString& messageId, Bus::ObsMessages callbackMessageId) noexcept;

  std::unordered_map<QString, Bus::ObsMessages> m_pendingResponses;

  std::optional<bool> m_authorized;

  inline static const QString WebSocketProtocol{"ws://"};
  inline static const QString SecuredWebSocketProtocol{"wss://"};

  inline static const QString DefaultAddress{"localhost:4444"};
};
}

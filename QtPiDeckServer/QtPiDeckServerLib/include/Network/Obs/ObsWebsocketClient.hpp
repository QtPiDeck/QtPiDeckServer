#pragma once

#include <array>
#include <optional>
#include <unordered_map>

#include <QAbstractSocket>
#include <QDataStream>
#include <QLoggingCategory>

#include "Bus/ObsMessages.hpp"
#include "Network/Obs/GetAuthRequiredResponse.hpp"
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
  template<class... TServices>
  ObsWebsocketClient(const std::shared_ptr<TServices>&... services) noexcept {
    (setService<TServices>(services), ...);
    constexpr size_t numberOfServices = 4;
    static_assert(sizeof...(TServices) == numberOfServices);
    auto& webSocket = service<Services::IWebSocket>();
    webSocket->setTextReceivedHandler([this](QString message) { receivedTestMessage(std::move(message)); });
    webSocket->setConnectedHandler([this] { checkAuthRequirement(); });
    webSocket->setFailHandler([this](Services::IWebSocket::ConnectionError error) {
      constexpr std::array errors = {QAbstractSocket::SocketError::ConnectionRefusedError,
                                     QAbstractSocket::SocketError::UnknownSocketError};
      webSocketError(errors.at(static_cast<uint32_t>(error)));
    });

    m_authResponseReceived = service<Services::IMessageBus>()->subscribe(
        this,
        [this](const Bus::Message& message) noexcept {
          const auto obj = GetAuthRequiredResponse::fromJson([&message]() noexcept {
            QDataStream qbs{message.payload};
            QString jsonText;
            qbs >> jsonText;
            return jsonText;
          }());

          if (!isRequestSuccessful(obj)) {
            qWarning() << "Failed to check authorization(%1)"_qls.arg(*obj.error);
            m_authorized = false;
            return;
          }

          m_authorized = !obj.authRequired;
        },
        Bus::ObsMessages::GetAuthRequiredResponseReceived);
  }

  void connectToObs() noexcept;

#if __cpp_concepts
  template<ObsRequest TRequest>
#else
  template<class TRequest>
#endif
  void sendRequest(TRequest requestId, Bus::ObsMessages callbackMessageId) noexcept {
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

  Services::Subscription m_authResponseReceived;

  inline static const QString WebSocketProtocol{"ws://"};
  inline static const QString SecuredWebSocketProtocol{"wss://"};

  inline static const QString DefaultAddress{"localhost:4444"};
};
}

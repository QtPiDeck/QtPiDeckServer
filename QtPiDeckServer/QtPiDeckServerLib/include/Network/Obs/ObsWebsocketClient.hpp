#pragma once

#include <array>
#include <optional>
#include <unordered_map>

#include <QAbstractSocket>
#include <QDataStream>

#include <boost/log/attributes/constant.hpp>
#include <boost/log/trivial.hpp>

#include "Bus/ObsMessages.hpp"
#include "Network/Obs/Models/GetAuthRequiredResponse.hpp"
#include "ObsRequests.hpp"
#include "Services/IMessageBus.hpp"
#include "Services/IObsMessageIdGenerator.hpp"
#include "Services/IServerSettingsStorage.hpp"
#include "Services/IWebSocket.hpp"
#include "Services/UseServices.hpp"
#include "Utilities/Logging.hpp"

namespace QtPiDeck::Network::Obs {
class ObsWebsocketClient : public QObject,
                           public Services::UseServices<Services::IMessageBus, Services::IServerSettingsStorage,
                                                        Services::IObsMessageIdGenerator, Services::IWebSocket> {
  Q_OBJECT // NOLINT

public:
  ObsWebsocketClient(const std::shared_ptr<Services::IMessageBus>& messageBusService,
                     const std::shared_ptr<Services::IServerSettingsStorage>& serviceSettingsStorageService,
                     const std::shared_ptr<Services::IObsMessageIdGenerator>& obsMessageIdGeneratorService,
                     const std::shared_ptr<Services::IWebSocket>& webSocketService) noexcept;

  void connectToObs() noexcept;
  void sendRequest(ObsRequest requestId, Bus::ObsMessages callbackMessageId) noexcept {
    sendRequest(std::visit([](auto&& arg) -> uint16_t { return static_cast<uint16_t>(arg); }, requestId),
                callbackMessageId);
  }
  void sendRequest(uint16_t requestId, Bus::ObsMessages callbackMessageId) noexcept;

public slots: // NOLINT(readability-redundant-access-specifiers)
  void webSocketError(QAbstractSocket::SocketError error);
  void receivedMessage(QByteArray message);
  void checkAuthRequirement();

private:
  void send(uint16_t requestId, const QString& messageId, Bus::ObsMessages callbackMessageId) noexcept;

  std::unordered_map<QString, Bus::ObsMessages> m_pendingResponses{};

  std::optional<bool> m_authorized;

  Utilities::Connection m_authResponseReceived;
  mutable boost::log::sources::severity_logger<boost::log::trivial::severity_level> m_slg;

  inline static const QString WebSocketProtocol{"ws://"};
  inline static const QString SecuredWebSocketProtocol{"wss://"};

  inline static const QString DefaultAddress{"localhost:4444"};
};
}

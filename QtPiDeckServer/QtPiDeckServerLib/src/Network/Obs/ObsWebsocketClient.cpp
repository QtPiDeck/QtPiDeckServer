#include "Network/Obs/ObsWebsocketClient.hpp"

#include "boost/algorithm/cxx11/any_of.hpp"
#include <algorithm>

#if QT_VERSION_MAJOR == 6
#include <QIODevice>
#endif

#include <QMetaEnum>

#include "Utilities/Literals.hpp"

namespace QtPiDeck::Network::Obs {
ObsWebsocketClient::ObsWebsocketClient(
    const std::shared_ptr<Services::IMessageBus>& messageBusService,
    const std::shared_ptr<Services::IServerSettingsStorage>& serviceSettingsStorageService,
    const std::shared_ptr<Services::IObsMessageIdGenerator>& obsMessageIdGeneratorService,
    const std::shared_ptr<Services::IWebSocket>& webSocketService) noexcept {
  Utilities::initLogger(m_slg, "ObsWebsocketClient");
  setService(messageBusService);
  setService(serviceSettingsStorageService);
  setService(obsMessageIdGeneratorService);
  setService(webSocketService);
  auto& webSocket = service<Services::IWebSocket>();
#if defined(__cpp_lib_bind_front)
  webSocket->setMessageReceivedHandler(std::bind_front(&ObsWebsocketClient::receivedMessage, this));
  webSocket->setConnectedHandler(std::bind_front(&ObsWebsocketClient::checkAuthRequirement, this));
#else
  webSocket->setMessageReceivedHandler([this](QByteArray message) { receivedMessage(std::move(message)); });
  webSocket->setConnectedHandler([this] { checkAuthRequirement(); });
#endif
  webSocket->setFailHandler([this](Services::IWebSocket::ConnectionError error) {
    constexpr std::array errors = {QAbstractSocket::SocketError::ConnectionRefusedError,
                                   QAbstractSocket::SocketError::UnknownSocketError};
    webSocketError(errors.at(static_cast<uint32_t>(error)));
  });

  m_authResponseReceived = service<Services::IMessageBus>()->subscribe(
      this,
      [this](const Bus::Message& message) noexcept {
        const auto obj = Models::GetAuthRequiredResponse::fromJson(QString{message.payload});

        if (!isRequestSuccessful(obj)) {
          using namespace Utilities::literals;
          BOOST_LOG_SEV(m_slg, Utilities::severity::warning)
              << "Failed to check authorization(" << obj.error.value().toStdString() << ")";
          m_authorizationStatus = AuthorizationStatus::NonAuthorized;
          return;
        }

        m_authorizationStatus = obj.authRequired ? AuthorizationStatus::NonAuthorized : AuthorizationStatus::Authorized;
      },
      Bus::ObsMessages::GetAuthRequiredResponseReceived);
}

void ObsWebsocketClient::connectToObs() noexcept {
  using namespace Utilities::literals;
  m_authorizationStatus = AuthorizationStatus::Unchecked;
  const auto obsAddress = [settings = service<Services::IServerSettingsStorage>(), protocol = WebSocketProtocol]() noexcept {
    return settings ? "%1%2:%3"_qls.arg(protocol, settings->obsWebsocketAddress(), settings->obsWebsocketPort())
                    : "%1%2"_qls.arg(protocol, DefaultAddress);
  }();

  service<Services::IWebSocket>()->connect(obsAddress);
}

void ObsWebsocketClient::webSocketError(QAbstractSocket::SocketError error) noexcept {
  BOOST_LOG_SEV(m_slg, Utilities::severity::warning)
      << "Connection error: " << QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(error);
}

void ObsWebsocketClient::receivedMessage(QByteArray message) {
  using namespace Utilities::literals;

  if (const auto obj = QJsonDocument::fromJson(message).object(); obj.contains("update-type"_qls)) {
    throw std::logic_error("Not implemented");
  } else {
    const auto& id = obj["message-id"_qls].toString();
    service<Services::IMessageBus>()->sendMessage({m_pendingResponses.at(id), message});
    m_pendingResponses.erase(id);
  }
}

void ObsWebsocketClient::checkAuthRequirement() {
  sendRequest(General::GetAuthRequired, Bus::ObsMessages::GetAuthRequiredResponseReceived);
}

void ObsWebsocketClient::sendRequest(uint16_t requestId, Bus::ObsMessages callbackMessageId) noexcept {
  send(requestId, service<Services::IObsMessageIdGenerator>()->generateId(RequestTypes.at(requestId)),
       callbackMessageId);
}

void ObsWebsocketClient::send(uint16_t requestId, const QString& messageId,
                              Bus::ObsMessages callbackMessageId) noexcept {
  auto isNotAllowedWithoutAuthorization = [](uint16_t id) noexcept {
    constexpr std::array allowedRequests = {static_cast<uint16_t>(General::GetAuthRequired)};
    return boost::algorithm::any_of_equal(allowedRequests, id);
  };

  if (m_authorizationStatus == AuthorizationStatus::NonAuthorized && isNotAllowedWithoutAuthorization(requestId)) {
    BOOST_LOG_SEV(m_slg, Utilities::severity::warning)
        << "Refused to send request " << RequestTypesRaw.at(requestId) << " without authorization";
    return;
  }

  m_pendingResponses.emplace(messageId, callbackMessageId);

  const QJsonDocument doc = [&requestId, &messageId]() noexcept {
    using namespace Utilities::literals;
    QJsonObject obj;
    obj["request-type"_qls] = RequestTypes.at(requestId);
    obj["message-id"_qls] = messageId;
    return QJsonDocument{obj};
  }();

  [[maybe_unused]] auto sendResult = service<Services::IWebSocket>()->send(doc.toJson(QJsonDocument::Compact));
}
}

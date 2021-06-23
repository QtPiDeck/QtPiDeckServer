#include "Network/Obs/ObsWebsocketClient.hpp"

#include "boost/algorithm/cxx11/any_of.hpp"
#include <algorithm>

#if QT_VERSION_MAJOR == 6
#include <QIODevice>
#endif

#include <QMetaEnum>

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
  webSocket->setTextReceivedHandler(std::bind_front(&ObsWebsocketClient::receivedTestMessage, this));
  webSocket->setConnectedHandler(std::bind_front(&ObsWebsocketClient::checkAuthRequirement, this));
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

void ObsWebsocketClient::connectToObs() noexcept {
  m_authorized.reset();
  const auto obsAddress = [settings = service<Services::IServerSettingsStorage>(), protocol = WebSocketProtocol] {
    return settings ? "%1%2:%3"_qls.arg(protocol, settings->obsWebsocketAddress(), settings->obsWebsocketPort())
                    : "%1%2"_qls.arg(protocol, DefaultAddress);
  }();

  service<Services::IWebSocket>()->connect(obsAddress);
}

void ObsWebsocketClient::webSocketError(QAbstractSocket::SocketError error) {
  BOOST_LOG_SEV(m_slg, Utilities::severity::warning)
      << "Connection error: " << QMetaEnum::fromType<QAbstractSocket::SocketError>().valueToKey(error);
}

void ObsWebsocketClient::receivedTestMessage(QString message) {
  auto convertMessage = [&message] {
    QByteArray qba;
    QDataStream qds{&qba, QIODevice::WriteOnly};
    qds << message;
    return qba;
  };

  if (const auto obj = QJsonDocument::fromJson(message.toUtf8()).object(); obj.contains("update-type"_qls)) {
    // to be implemented
  } else {
    const auto& id = obj["message-id"_qls].toString();
    service<Services::IMessageBus>()->sendMessage({m_pendingResponses.at(id), convertMessage()});
    m_pendingResponses.erase(id);
  }
}

void ObsWebsocketClient::checkAuthRequirement() {
  sendRequest(General::GetAuthReqired, Bus::ObsMessages::GetAuthRequiredResponseReceived);
}

void ObsWebsocketClient::sendRequest(uint16_t requestId, Bus::ObsMessages callbackMessageId) noexcept {
  send(requestId, service<Services::IObsMessageIdGenerator>()->generateId(RequestTypes.at(requestId)),
       callbackMessageId);
}

void ObsWebsocketClient::send(uint16_t requestId, const QString& messageId,
                              Bus::ObsMessages callbackMessageId) noexcept {
  auto isNotAllowedWithoutAuthorization = [](uint16_t id) noexcept {
    constexpr std::array allowedRequests = {static_cast<uint16_t>(General::GetAuthReqired)};
    return boost::algorithm::any_of_equal(allowedRequests, id);
  };

  if (m_authorized && !*m_authorized && isNotAllowedWithoutAuthorization(requestId)) {
    BOOST_LOG_SEV(m_slg, Utilities::severity::warning)
        << "Refused to send request " << RequestTypesRaw.at(requestId) << " without authorization";
    return;
  }

  m_pendingResponses.emplace(messageId, callbackMessageId);

  const QJsonDocument doc = [&requestId, &messageId]() noexcept {
    QJsonObject obj;
    obj["request-type"_qls] = RequestTypes.at(requestId);
    obj["message-id"_qls] = messageId;
    return QJsonDocument{obj};
  }();

  const auto byteArray = doc.toJson(QJsonDocument::Compact);
  const auto jsonString = QLatin1String{byteArray};

  [[maybe_unused]] auto sendResult = service<Services::IWebSocket>()->send(jsonString);
}
}

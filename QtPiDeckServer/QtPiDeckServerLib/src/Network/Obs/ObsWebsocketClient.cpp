#include "Network/Obs/ObsWebsocketClient.hpp"

#include <algorithm>

#if QT_VERSION_MAJOR == 6
#include <QIODevice>
#endif

#include <QDataStream>

#include "Network/Obs/GetAuthRequiredResponse.hpp"
#include "Network/Obs/ObsRequests.hpp"

Q_LOGGING_CATEGORY(ObsWebsocket, "ObsWebsocket") // NOLINT

namespace QtPiDeck::Network::Obs {
void ObsWebsocketClient::connectToObs() noexcept {
  m_authorized.reset();
  const auto obsAddress = [settings = service<Services::IServerSettingsStorage>(), protocol = WebSocketProtocol] {
    if (settings) {
      return "%1%2:%3"_qls.arg(protocol, settings->obsWebsocketAddress(), settings->obsWebsocketPort());
    }

    return "%1%2"_qls.arg(protocol, DefaultAddress);
  }();

  service<Services::IWebSocket>()->connect(obsAddress);

  service<Services::IMessageBus>()->subscribe(
      this,
      [this](const Bus::Message& message) noexcept {
        const auto obj = ParseObsResponse<General::GetAuthReqired>([&message]() noexcept {
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

void ObsWebsocketClient::webSocketError(QAbstractSocket::SocketError error) { qCWarning(ObsWebsocket) << error; }

void ObsWebsocketClient::receivedTestMessage(QString message) {
  auto convertMessage = [&message] {
    QByteArray qba;
    QDataStream qds{&qba, QIODevice::WriteOnly};
    qds << message;
    return qba;
  };

  qDebug().noquote() << message;

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
    constexpr std::array allowedRequests = {General::GetAuthReqired};
    return std::find(std::begin(allowedRequests), std::end(allowedRequests), id) == std::cend(allowedRequests);
  };

  if (m_authorized && !*m_authorized && isNotAllowedWithoutAuthorization(requestId)) {
    qCWarning(ObsWebsocket) << "Refused to send request %1 without authorization"_qs.arg(RequestTypes.at(requestId));
    return;
  }

  m_pendingResponses.emplace(messageId, callbackMessageId);

  const QJsonDocument doc = [&requestId, &messageId]() noexcept {
    QJsonObject obj;
    obj["request-type"_qls] = RequestTypes.at(requestId);
    obj["message-id"_qls] = messageId;
    return QJsonDocument{obj};
  }();

  const auto jsonText = doc.toJson(QJsonDocument::Compact);
  const auto jsonString = QLatin1String{jsonText};

  [[maybe_unused]] auto sendResult = service<Services::IWebSocket>()->send(jsonString);
}
}

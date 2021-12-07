#include "BoostUnitTest.hpp"

#include "Network/Obs/ObsWebsocketClient.hpp"
#include "Utilities/Literals.hpp"

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>

CT_BOOST_AUTO_TEST_SUITE(ObsWebsocketClient)

class NoopWebSocket : public QtPiDeck::Services::IWebSocket {
public:
  void connect(QStringView /*address*/) noexcept override {}
  void disconnect() noexcept override {}
  auto send(QByteArray /*message*/) noexcept -> std::optional<SendingError> override { return {}; }
  void setMessageReceivedHandler(MessageReceivedHandler /*handler*/) noexcept override {}
  void setConnectedHandler(ConnectedHandler /*handler*/) noexcept override {}
  void setFailHandler(FailHandler /*handler*/) noexcept override {}
};

class ConnectableWebSocket final : public NoopWebSocket {
public:
  void connect(QStringView address) noexcept final { m_address = address.toString(); }

  auto address() -> QString { return m_address; }

private:
  QString m_address;
};

class NoopMessageBus : public QtPiDeck::Services::IMessageBus {
public:
  [[nodiscard]] auto subscribe(QObject* /*context*/,
                               const std::function<void(const QtPiDeck::Bus::Message&)>& /*method*/) noexcept
      -> QtPiDeck::Utilities::Connection override {
    return {};
  }
  [[nodiscard]] auto subscribe(QObject* /*context*/,
                               const std::function<void(const QtPiDeck::Bus::Message&)>& /*method*/,
                               uint64_t /*messageType*/) noexcept -> QtPiDeck::Utilities::Connection override {
    return {};
  }
  void unsubscribe(QtPiDeck::Utilities::Connection& /*subscription*/) noexcept override {}
  void sendMessage(QtPiDeck::Bus::Message /*message*/) noexcept override {}
};

CT_BOOST_AUTO_TEST_CASE(connectWithoutStorage) {
  auto webSocket = std::shared_ptr<QtPiDeck::Services::IWebSocket>(new ConnectableWebSocket());
  auto client = std::make_unique<QtPiDeck::Network::Obs::ObsWebsocketClient>(
      std::shared_ptr<QtPiDeck::Services::IMessageBus>(new NoopMessageBus), nullptr, nullptr, webSocket);

  client->connectToObs();
  using namespace QtPiDeck::Network::Obs;
  using namespace QtPiDeck::Utilities::literals;
  CT_BOOST_TEST(std::dynamic_pointer_cast<ConnectableWebSocket>(webSocket)->address() ==
                CT_QStringLiteral("ws://localhost:4444"));
}

class NoopServerSettingsStorage : public QtPiDeck::Services::IServerSettingsStorage {
  [[nodiscard]] auto read(const QString& /*key*/, const QVariant& /*defaultValue*/) const noexcept
      -> QVariant override {
    return {};
  }
  void store(const QString& /*key*/, const QVariant& /*value*/) noexcept override {}
  [[nodiscard]] auto deckServerAddress() const noexcept -> QString override { return {}; }
  void setDeckServerAddress(const QString& /*deckServerAddress*/) noexcept override {}
  [[nodiscard]] auto deckServerPort() const noexcept -> QString override { return {}; }
  void setDeckServerPort(const QString& /*deckServerPort*/) noexcept override {}
  [[nodiscard]] auto obsWebsocketAddress() const noexcept -> QString override { return {}; }
  void setObsWebsocketAddress(const QString& /*obsWebsocketAddress*/) noexcept override {}
  [[nodiscard]] auto obsWebsocketPort() const noexcept -> QString override { return {}; }
  void setObsWebsocketPort(const QString& /*obsWebsocketPort*/) noexcept override {}
};

class ServerSettingsStorage final : public NoopServerSettingsStorage {
public:
  [[nodiscard]] auto obsWebsocketAddress() const noexcept -> QString final { return Address; }
  [[nodiscard]] auto obsWebsocketPort() const noexcept -> QString final { return Port; }

  inline static const QString Address = "thishost";
  inline static const QString Port = "12345";
};

CT_BOOST_AUTO_TEST_CASE(connectWithStorage) {
  auto webSocket = std::shared_ptr<QtPiDeck::Services::IWebSocket>(new ConnectableWebSocket());
  auto client = std::make_unique<QtPiDeck::Network::Obs::ObsWebsocketClient>(
      std::shared_ptr<QtPiDeck::Services::IMessageBus>(new NoopMessageBus),
      std::shared_ptr<QtPiDeck::Services::IServerSettingsStorage>(new ServerSettingsStorage()),
      std::shared_ptr<QtPiDeck::Services::IObsMessageIdGenerator>(), webSocket);

  client->connectToObs();
  using namespace QtPiDeck::Network::Obs;
  using namespace QtPiDeck::Utilities::literals;
  CT_BOOST_TEST(std::dynamic_pointer_cast<ConnectableWebSocket>(webSocket)->address() ==
                CT_QStringLiteral("ws://%1:%2").arg(ServerSettingsStorage::Address).arg(ServerSettingsStorage::Port));
}

class SimpleMessageBus final : public NoopMessageBus {
public:
  [[nodiscard]] auto subscribe(QObject* /*context*/, const std::function<void(const QtPiDeck::Bus::Message&)>& method,
                               uint64_t /*messageType*/) noexcept -> QtPiDeck::Utilities::Connection final {
    m_call = method;
    return {};
  }

  void sendMessage(QtPiDeck::Bus::Message mess) noexcept final { m_call(mess); }

private:
  std::function<void(const QtPiDeck::Bus::Message&)> m_call;
};

class ConnectedWebSocket final : public NoopWebSocket {
public:
  void setConnectedHandler(ConnectedHandler handler) noexcept final { m_handler = handler; }
  void setMessageReceivedHandler(MessageReceivedHandler handler) noexcept final { m_receivedHandler = handler; }
  auto send(QByteArray message) noexcept -> std::optional<SendingError> final {
    m_data = message;
    m_message = QLatin1String{m_data};
    return {};
  }
  void received(QByteArray data) { m_receivedHandler(std::move(data)); }
  void connected() { m_handler(); }
  [[nodiscard]] auto message() const -> QLatin1String { return m_message; }

private:
  ConnectedHandler m_handler;
  MessageReceivedHandler m_receivedHandler;
  QLatin1String m_message;
  QByteArray m_data;
};

class NoopObsMessageIdGenerator final : public QtPiDeck::Services::IObsMessageIdGenerator {
public:
  [[nodiscard]] auto generateId() const noexcept -> QString final { return {}; }

  [[nodiscard]] auto generateId(const QString& /*requestName*/) const noexcept -> QString final { return {}; }
};

BOOST_DATA_TEST_CASE(askAuthAfterConnect, // NOLINT
                     boost::unit_test::data::make(true, false) ^ boost::unit_test::data::make(false, true), required,
                     result) {
  auto webSocket = std::shared_ptr<QtPiDeck::Services::IWebSocket>(new ConnectedWebSocket());
  auto messageBus = std::shared_ptr<QtPiDeck::Services::IMessageBus>(new SimpleMessageBus());
  auto client = std::make_unique<QtPiDeck::Network::Obs::ObsWebsocketClient>(
      messageBus, std::shared_ptr<QtPiDeck::Services::IServerSettingsStorage>(),
      std::shared_ptr<QtPiDeck::Services::IObsMessageIdGenerator>(new NoopObsMessageIdGenerator()), webSocket);

  client->connectToObs();
  auto casted = std::dynamic_pointer_cast<ConnectedWebSocket>(webSocket);
  casted->connected();
  CT_BOOST_TEST(casted->message().contains(QLatin1String{QtPiDeck::Network::Obs::RequestTypesRaw[static_cast<uint16_t>(
      QtPiDeck::Network::Obs::General::GetAuthRequired)]}));

  const auto json = [&]() {
    using namespace QtPiDeck::Network::Obs::Models;
    QJsonObject obj;
    obj[GetAuthRequiredResponse::statusField] = GetAuthRequiredResponse::successStatus;
    obj[GetAuthRequiredResponse::authRequiredField] = required;
    return QJsonDocument{obj}.toJson();
  }();

  casted->received(json);
  CT_BOOST_TEST(client->isAuthorized() == result);
}

CT_BOOST_AUTO_TEST_CASE(askAuthAfterConnectUnsuccessfulResponse) {
  auto webSocket = std::shared_ptr<QtPiDeck::Services::IWebSocket>(new ConnectedWebSocket());
  auto messageBus = std::shared_ptr<QtPiDeck::Services::IMessageBus>(new SimpleMessageBus());
  auto client = std::make_unique<QtPiDeck::Network::Obs::ObsWebsocketClient>(
      messageBus, std::shared_ptr<QtPiDeck::Services::IServerSettingsStorage>(),
      std::shared_ptr<QtPiDeck::Services::IObsMessageIdGenerator>(new NoopObsMessageIdGenerator()), webSocket);

  client->connectToObs();
  auto casted = std::dynamic_pointer_cast<ConnectedWebSocket>(webSocket);
  casted->connected();
  CT_BOOST_TEST(casted->message().contains(QLatin1String{QtPiDeck::Network::Obs::RequestTypesRaw[static_cast<uint16_t>(
      QtPiDeck::Network::Obs::General::GetAuthRequired)]}));

  const auto json = [&]() {
    using namespace QtPiDeck::Network::Obs::Models;
    QJsonObject obj;
    obj[GetAuthRequiredResponse::statusField] = CT_QStringLiteral("error");
    obj[GetAuthRequiredResponse::errorField] = CT_QStringLiteral("Some error");
    return QJsonDocument{obj}.toJson();
  }();

  casted->received(json);
  CT_BOOST_TEST(!client->isAuthorized());
}

CT_BOOST_AUTO_TEST_CASE(receiveUpdateMessage) {
  auto webSocket = std::shared_ptr<QtPiDeck::Services::IWebSocket>(new ConnectedWebSocket());
  auto messageBus = std::shared_ptr<QtPiDeck::Services::IMessageBus>(new SimpleMessageBus());
  auto client = std::make_unique<QtPiDeck::Network::Obs::ObsWebsocketClient>(
      messageBus, std::shared_ptr<QtPiDeck::Services::IServerSettingsStorage>(),
      std::shared_ptr<QtPiDeck::Services::IObsMessageIdGenerator>(new NoopObsMessageIdGenerator()), webSocket);

  client->connectToObs();
  auto casted = std::dynamic_pointer_cast<ConnectedWebSocket>(webSocket);
  casted->connected();
  CT_BOOST_TEST(casted->message().contains(QLatin1String{QtPiDeck::Network::Obs::RequestTypesRaw[static_cast<uint16_t>(
      QtPiDeck::Network::Obs::General::GetAuthRequired)]}));

  const auto json = [&]() {
    using namespace QtPiDeck::Network::Obs::Models;
    QJsonObject obj;
    obj[GetAuthRequiredResponse::statusField] = GetAuthRequiredResponse::successStatus;
    obj["update-type"_qls] = CT_QStringLiteral("Random");
    return QJsonDocument{obj}.toJson();
  }();

  CT_BOOST_CHECK_THROW(casted->received(json), std::logic_error);
}

class FailedWebSocket final : public NoopWebSocket {
public:
  void setFailHandler(FailHandler failHandler) noexcept final { m_failHandler = failHandler; }
  void emitError(ConnectionError error) { m_failHandler(error); }

private:
  FailHandler m_failHandler;
};

CT_BOOST_AUTO_TEST_CASE(handleFailedWebsocket) {
  auto webSocket = std::shared_ptr<QtPiDeck::Services::IWebSocket>(new FailedWebSocket());
  auto client = std::make_unique<QtPiDeck::Network::Obs::ObsWebsocketClient>(
      std::shared_ptr<QtPiDeck::Services::IMessageBus>(new NoopMessageBus), nullptr, nullptr, webSocket);

  std::static_pointer_cast<FailedWebSocket>(webSocket)->emitError(
      QtPiDeck::Services::IWebSocket::ConnectionError::RefusedConnection);
}

CT_BOOST_AUTO_TEST_CASE(crashIfUnhandledError) {
  auto webSocket = std::shared_ptr<QtPiDeck::Services::IWebSocket>(new FailedWebSocket());
  auto client = std::make_unique<QtPiDeck::Network::Obs::ObsWebsocketClient>(
      std::shared_ptr<QtPiDeck::Services::IMessageBus>(new NoopMessageBus), nullptr, nullptr, webSocket);

  constexpr auto wrongError = static_cast<QtPiDeck::Services::IWebSocket::ConnectionError>(
      static_cast<uint32_t>(QtPiDeck::Services::IWebSocket::ConnectionError::Unspecified) + 1);

  BOOST_CHECK_THROW(std::static_pointer_cast<FailedWebSocket>(webSocket)->emitError(wrongError),
                    std::out_of_range); // NOLINT
}

CT_BOOST_AUTO_TEST_SUITE_END()

#define BOOST_TEST_MODULE ObsWebsocketClientTests // NOLINT
#include "BoostUnitTest.hpp"

#include "Network/Obs/ObsWebsocketClient.hpp"
#include "Utilities/Literals.hpp"
#include "Utilities/Logging.hpp"

auto main(int argc, char* argv[]) -> int {
  QtPiDeck::Utilities::initLogging("ObsWebsocketClientTests");
  return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}

CT_BOOST_AUTO_TEST_SUITE(ObsWebsocketClient)

class NoopWebSocket : public QtPiDeck::Services::IWebSocket {
public:
  void connect(QStringView /*address*/) noexcept override {}
  void disconnect() noexcept override {}
  auto send(QLatin1String /*message*/) noexcept -> std::optional<SendingError> override { return {}; }
  void setTextReceivedHandler(TextReceivedHandler /*handler*/) noexcept override {}
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
      std::shared_ptr<QtPiDeck::Services::IMessageBus>(new NoopMessageBus),
      std::shared_ptr<QtPiDeck::Services::IServerSettingsStorage>(),
      std::shared_ptr<QtPiDeck::Services::IObsMessageIdGenerator>(), webSocket);

  client->connectToObs();
  using namespace QtPiDeck::Network::Obs;
  using namespace QtPiDeck::Utilities::literals;
  CT_BOOST_TEST(std::dynamic_pointer_cast<ConnectableWebSocket>(webSocket)->address() == "ws://localhost:4444"_qs);
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
                "ws://%1:%2"_qs.arg(ServerSettingsStorage::Address).arg(ServerSettingsStorage::Port));
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
  auto send(QLatin1String message) noexcept -> std::optional<SendingError> final {
    m_data.resize(std::size(message));
    std::memcpy(std::data(m_data), std::data(message), std::size(message));
    m_message = QLatin1String{m_data};
    return {};
  }
  void connected() { m_handler(); }
  [[nodiscard]] auto message() const -> QLatin1String { return m_message; }

private:
  ConnectedHandler m_handler;
  QLatin1String m_message;
  QByteArray m_data;
};

class NoopObsMessageIdGenerator final : public QtPiDeck::Services::IObsMessageIdGenerator {
public:
  [[nodiscard]] auto generateId() const noexcept -> QString final { return {}; }

  [[nodiscard]] auto generateId(const QString& /*requestName*/) const noexcept -> QString final { return {}; }
};

CT_BOOST_AUTO_TEST_CASE(askAuthAfterConnect) {
  auto webSocket = std::shared_ptr<QtPiDeck::Services::IWebSocket>(new ConnectedWebSocket());
  auto messageBus = std::shared_ptr<QtPiDeck::Services::IMessageBus>(new SimpleMessageBus());
  auto client = std::make_unique<QtPiDeck::Network::Obs::ObsWebsocketClient>(
      messageBus, std::shared_ptr<QtPiDeck::Services::IServerSettingsStorage>(),
      std::shared_ptr<QtPiDeck::Services::IObsMessageIdGenerator>(new NoopObsMessageIdGenerator()), webSocket);

  client->connectToObs();
  auto casted = std::dynamic_pointer_cast<ConnectedWebSocket>(webSocket);
  casted->connected();
  CT_BOOST_TEST(casted->message().contains(QLatin1String{QtPiDeck::Network::Obs::RequestTypesRaw[static_cast<uint16_t>(
      QtPiDeck::Network::Obs::General::GetAuthReqired)]}));
}

CT_BOOST_AUTO_TEST_SUITE_END()
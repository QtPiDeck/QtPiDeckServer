#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "Network/Obs/ObsWebsocketClient.hpp"
using namespace QtPiDeck;
using namespace QtPiDeck::Network::Obs;

class NoopMessageBus : public Services::IMessageBus {
public:
  [[nodiscard]] virtual auto subscribe(QObject* /*context*/, const std::function<void(const Bus::Message&)>& /*method*/)
      -> Services::Subscription {
    return {};
  }
  [[nodiscard]] virtual auto subscribe(QObject* /*context*/, const std::function<void(const Bus::Message&)>& /*method*/,
                                       uint64_t /*messageType*/) -> Services::Subscription {
    return {};
  }
  virtual void unsubscribe(Services::Subscription&) {}
  virtual void sendMessage(Bus::Message) {}
};

class NoopServerSettingsStorage : public Services::IServerSettingsStorage {
  virtual auto read(const QString& /*key*/, const QVariant& /*defaultValue*/) const noexcept -> QVariant { return {}; }
  virtual void store(const QString& /*key*/, const QVariant& /*value*/) noexcept {}
  virtual auto deckServerAddress() const noexcept -> QString { return {}; }
  virtual void setDeckServerAddress(const QString& /*deckServerAddress*/) noexcept {}
  virtual auto deckServerPort() const noexcept -> QString { return {}; }
  virtual void setDeckServerPort(const QString& /*deckServerPort*/) noexcept {}
  virtual auto obsWebsocketAddress() const noexcept -> QString { return {}; }
  virtual void setObsWebsocketAddress(const QString& /*obsWebsocketAddress*/) noexcept {}
  virtual auto obsWebsocketPort() const noexcept -> QString { return {}; }
  virtual void setObsWebsocketPort(const QString& /*obsWebsocketPort*/) noexcept {}
};

class NoopWebSocket : public Services::IWebSocket {
public:
  virtual void connect(QStringView /*address*/) noexcept {}
  virtual void disconnect() noexcept final {}
  virtual auto send(QLatin1String /*message*/) noexcept -> std::optional<SendingError> { return {}; }
  virtual void setTextReceivedHandler(TextReceivedHandler /*handler*/) noexcept {}
  virtual void setConnectedHandler(ConnectedHandler /*handler*/) noexcept {}
  virtual void setFailHandler(FailHandler /*handler*/) noexcept {}
};

class NoopObsMessageIdGenerator final : public Services::IObsMessageIdGenerator {
public:
  auto generateId() const noexcept -> QString final { return {}; }

  auto generateId(const QString& /*requestName*/) const noexcept -> QString final { return {}; }
};

class ConnectableWebSocket final : public NoopWebSocket {
public:
  void connect(QStringView address) noexcept final { m_address = address.toString(); }

  auto address() -> QString { return m_address; }

private:
  QString m_address;
};

class ServerSettingsStorage final : public NoopServerSettingsStorage {
public:
  auto obsWebsocketAddress() const noexcept -> QString final { return Address; }
  auto obsWebsocketPort() const noexcept -> QString final { return Port; }

  inline static const QString Address = "thishost";
  inline static const QString Port = "12345";
};

TEST_CASE("ObsWebsocketClient connects", "[ObsWebsocketClient]") {
  auto webSocket = std::shared_ptr<Services::IWebSocket>(new ConnectableWebSocket());
  SECTION("Without SettingsStorage") {
    auto client = std::make_unique<ObsWebsocketClient>(std::shared_ptr<Services::IMessageBus>(new NoopMessageBus),
                                                       std::shared_ptr<Services::IServerSettingsStorage>(),
                                                       std::shared_ptr<Services::IObsMessageIdGenerator>(), webSocket);

    client->connectToObs();
    REQUIRE(std::dynamic_pointer_cast<ConnectableWebSocket>(webSocket)->address() == "ws://localhost:4444"_qs);
  }

  SECTION("With SettingsStorage") {
    auto client = std::make_unique<ObsWebsocketClient>(
        std::shared_ptr<Services::IMessageBus>(new NoopMessageBus),
        std::shared_ptr<Services::IServerSettingsStorage>(new ServerSettingsStorage()),
        std::shared_ptr<Services::IObsMessageIdGenerator>(), webSocket);

    client->connectToObs();
    REQUIRE(std::dynamic_pointer_cast<ConnectableWebSocket>(webSocket)->address() ==
            "ws://%1:%2"_qs.arg(ServerSettingsStorage::Address).arg(ServerSettingsStorage::Port));
  }
}

class SimpleMessageBus final : public NoopMessageBus {
public:
  [[nodiscard]] auto subscribe(QObject* /*context*/, const std::function<void(const Bus::Message&)>& method,
                               uint64_t /*messageType*/) -> Services::Subscription final {
    m_call = method;
    return {};
  }

  void sendMessage(Bus::Message mess) final { m_call(mess); }

private:
  std::function<void(const Bus::Message&)> m_call;
};

class ConnectedWebSocket final : public NoopWebSocket {
public:
  void setConnectedHandler(ConnectedHandler handler) noexcept final { m_handler = handler; }
  auto send(QLatin1String message) noexcept -> std::optional<SendingError> final {
    m_data.resize(message.size());
    std::memcpy(m_data.data(), message.data(), message.size());
    m_message = QLatin1String{m_data};
    return {};
  }
  void connected() { m_handler(); }
  QLatin1String message() const { return m_message; }

private:
  ConnectedHandler m_handler;
  QLatin1String m_message;
  QByteArray m_data;
};

TEST_CASE("ObsWebsocketClient asks authentification after connecting", "[ObsWebsocketClient]") {
  auto webSocket = std::shared_ptr<Services::IWebSocket>(new ConnectedWebSocket());
  auto messageBus = std::shared_ptr<Services::IMessageBus>(new SimpleMessageBus());
  auto client = std::make_unique<ObsWebsocketClient>(
      messageBus, std::shared_ptr<Services::IServerSettingsStorage>(),
      std::shared_ptr<Services::IObsMessageIdGenerator>(new NoopObsMessageIdGenerator()), webSocket);

  client->connectToObs();
  auto casted = std::dynamic_pointer_cast<ConnectedWebSocket>(webSocket);
  casted->connected();
  auto msg = casted->message();
  REQUIRE(casted->message().contains(QLatin1String{RequestTypesRaw[static_cast<uint16_t>(General::GetAuthReqired)]}));
}

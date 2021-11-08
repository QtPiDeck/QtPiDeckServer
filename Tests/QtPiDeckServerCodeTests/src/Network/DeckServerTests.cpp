#define BOOST_TEST_MODULE DeckServerTests // NOLINT
#include "BoostUnitTest.hpp"

#include "Network/DeckServer.inl"
#include "Utilities/Literals.hpp"
#include "Utilities/Logging.hpp"

auto main(int argc, char* argv[]) -> int {
  QtPiDeck::Utilities::initLogging("DeckServerTests");
  return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}

using QtPiDeck::Bus::Message;
using QtPiDeck::Network::DeckMessages;
using QtPiDeck::Network::detail::DeckServerPrivate;
using QtPiDeck::Services::IMessageBus;
using QtPiDeck::Utilities::Connection;

namespace {
class EmptyIMessageBusImpl : public IMessageBus {
public:
  auto subscribe(QObject* /*context*/, const std::function<void(const Message&)>& /*method*/) noexcept
      -> Connection override {
    return {};
  }
  auto subscribe(QObject* /*context*/, const std::function<void(const Message&)>& /*method*/,
                 uint64_t /*messageType*/) noexcept -> Connection override {
    return {};
  }
  void unsubscribe(Connection& /*connection*/) noexcept override {}
  void sendMessage(Message /*message*/) noexcept override {}
};

class TrackableMessageBus : public EmptyIMessageBusImpl {
public:
  auto subscribe(QObject* /*context*/, const std::function<void(const Message&)>& /*method*/,
                 uint64_t messageType) noexcept -> Connection override {
    m_subscribedTypes.push_back(messageType);
    return {};
  }

  [[nodiscard]] auto subscribedTypes() const -> const std::vector<uint64_t>& { return m_subscribedTypes; }

  static auto create() -> std::shared_ptr<TrackableMessageBus> { return std::make_shared<TrackableMessageBus>(); }

private:
  std::vector<uint64_t> m_subscribedTypes{};
};

class DeckServerImpl : public DeckServerPrivate<DeckServerImpl> {
  friend DeckServerPrivate<DeckServerImpl>;
  using DeckServerPrivate<DeckServerImpl>::DeckServerPrivate;

public:
  auto getService() noexcept -> std::shared_ptr<IMessageBus> { return service<IMessageBus>(); }
};

class TcpSocket final : public QIODevice {
  Q_OBJECT // NOLINT
public:
  void emitDisconnected() { emit disconnected(); }
  void emitReadyRead() { emit readyRead(); }
  void flush() { ++m_flushCalls; }
  auto writeData(const char* /*src*/, qint64 /*size*/) -> qint64 final { return {}; }
  auto readData(char* /*dst*/, qint64 /*size*/) -> qint64 final { return {}; }
signals:
  void disconnected();

private:
  uint32_t m_flushCalls{0};
  std::vector<uint64_t> m_readDataCalls{};
  std::vector<uint64_t> m_writeDataCalls{};
};

class TcpServer : public QObject {
  Q_OBJECT // NOLINT
public:
  [[nodiscard]] auto nextPendingConnection() -> TcpSocket* { return &m_socket; }
  void emitNewConnection() { emit newConnection(); }

signals:
  void newConnection();

private:
  TcpSocket m_socket;
};

class TcpServerFailure : public TcpServer {
public:
  [[nodiscard]] auto listen(QHostAddress::SpecialAddress /*address*/, qint16 /*port*/) -> bool { // NOLINT
    return false;
  }
};
class DeckServerImplFailure : public DeckServerPrivate<DeckServerImpl, TcpServerFailure, TcpSocket> {
  friend DeckServerPrivate<DeckServerImpl, TcpServerFailure, TcpSocket>;
  using DeckServerPrivate<DeckServerImpl, TcpServerFailure, TcpSocket>::DeckServerPrivate;

public:
  [[nodiscard]] auto getServer() -> TcpServer& { return server(); }
};

class TcpServerSuccess : public TcpServer {
public:
  [[nodiscard]] auto listen(QHostAddress::SpecialAddress /*address*/, qint16 /*port*/) -> bool { // NOLINT
    return true;
  }
};
class DeckServerImplSuccess : public DeckServerPrivate<DeckServerImpl, TcpServerSuccess, TcpSocket> {
  friend DeckServerPrivate<DeckServerImpl, TcpServerSuccess, TcpSocket>;
  using DeckServerPrivate<DeckServerImpl, TcpServerSuccess, TcpSocket>::DeckServerPrivate;

public:
  [[nodiscard]] auto getServer() -> TcpServerSuccess& { return server(); }
};

struct Fixture {
  Fixture() : messageBus(TrackableMessageBus ::create()) {}
  std::shared_ptr<TrackableMessageBus> messageBus;
};

struct FixtureSuccess : Fixture {
  FixtureSuccess() : Fixture(), server(nullptr, messageBus) { server.start(); }
  DeckServerImplSuccess server;
};
}

CT_BOOST_AUTO_TEST_SUITE(DeckServer)

CT_BOOST_AUTO_TEST_CASE(constructorShouldSetServices) {
  auto messageBus = std::make_shared<EmptyIMessageBusImpl>();
  auto server = DeckServerImpl(nullptr, messageBus);
  auto serviceInServer = server.getService();
  CT_BOOST_TEST(messageBus.get() == serviceInServer.get());
}

CT_BOOST_AUTO_TEST_SUITE_END()

CT_BOOST_FIXTURE_TEST_SUITE(DeckServer, Fixture)

CT_BOOST_AUTO_TEST_CASE(startShouldInitServerFailure) {
  auto server = DeckServerImplFailure(nullptr, messageBus);
  server.start();
}

CT_BOOST_AUTO_TEST_SUITE_END()

CT_BOOST_FIXTURE_TEST_SUITE(DeckServer, FixtureSuccess)

CT_BOOST_AUTO_TEST_CASE(startShouldInitServerSuccess) {
  auto& tcpServer = server.getServer();
  tcpServer.emitNewConnection();
  CT_BOOST_TEST(server.currentConnection() != nullptr);
  CT_BOOST_TEST(std::size(messageBus->subscribedTypes()) == 1);
  CT_BOOST_TEST(messageBus->subscribedTypes().at(0) == DeckMessages::PingReceived);
}

CT_BOOST_AUTO_TEST_CASE(shouldWorkWithOnlyOneConnection) {
  auto& tcpServer = server.getServer();
  CT_BOOST_TEST(server.currentConnection() == nullptr);
  tcpServer.emitNewConnection();
  auto* connection = server.currentConnection();
  tcpServer.emitNewConnection();
  CT_BOOST_TEST(server.currentConnection() == connection);
}

CT_BOOST_AUTO_TEST_CASE(handleConnectionShouldSetCurrentConnectionSocket) {
  auto& tcpServer = server.getServer();
  CT_BOOST_TEST(server.currentConnection() == nullptr);
  tcpServer.emitNewConnection();
  CT_BOOST_TEST(server.currentConnection() != nullptr);
}

CT_BOOST_AUTO_TEST_SUITE_END()

#include "DeckServerTests.moc"

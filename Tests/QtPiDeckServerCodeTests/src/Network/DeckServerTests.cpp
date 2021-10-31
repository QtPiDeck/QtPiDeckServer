#define BOOST_TEST_MODULE DeckServerTests // NOLINT
#include "BoostUnitTest.hpp"

#include "Network/DeckServer.inl"
#include "Utilities/Literals.hpp"
#include "Utilities/Logging.hpp"

auto main(int argc, char* argv[]) -> int {
  QtPiDeck::Utilities::initLogging("DeckServerTests");
  return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}

CT_BOOST_AUTO_TEST_SUITE(DeckServer)

using QtPiDeck::Bus::Message;
using QtPiDeck::Network::DeckMessages;
using QtPiDeck::Network::detail::DeckServerPrivate;
using QtPiDeck::Services::IMessageBus;
using QtPiDeck::Utilities::Connection;

namespace {
namespace common {
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
}
namespace constructor {
class DeckServerImpl : public DeckServerPrivate<DeckServerImpl> {
  friend DeckServerPrivate<DeckServerImpl>;
  using DeckServerPrivate<DeckServerImpl>::DeckServerPrivate;

public:
  auto getService() noexcept -> std::shared_ptr<IMessageBus> { return service<IMessageBus>(); }
};
}
}

CT_BOOST_AUTO_TEST_CASE(constructorShouldSetServices) {
  auto messageBus = std::make_shared<common::EmptyIMessageBusImpl>();
  auto server = constructor::DeckServerImpl(nullptr, messageBus);
  auto serviceInServer = server.getService();
  CT_BOOST_TEST(messageBus.get() == serviceInServer.get());
}

namespace {
namespace start {
class TrackableMessageBus : public common::EmptyIMessageBusImpl {
public:
  auto subscribe(QObject* /*context*/, const std::function<void(const Message&)>& /*method*/,
                 uint64_t messageType) noexcept -> Connection override {
    m_subscribedTypes.push_back(messageType);
    return {};
  }

  [[nodiscard]] auto subscribedTypes() const -> const std::vector<uint64_t>& { return m_subscribedTypes; }

private:
  std::vector<uint64_t> m_subscribedTypes{};
};
namespace common {
class TcpSocket {};
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
}

namespace success {
class TcpServer : public common::TcpServer {
public:
  [[nodiscard]] auto listen(QHostAddress::SpecialAddress /*address*/, qint16 /*port*/) -> bool { // NOLINT
    return true;
  }
};
class DeckServerImpl : public DeckServerPrivate<DeckServerImpl, TcpServer, common::TcpSocket> {
  friend DeckServerPrivate<DeckServerImpl, TcpServer, common::TcpSocket>;
  using DeckServerPrivate<DeckServerImpl, TcpServer, common::TcpSocket>::DeckServerPrivate;

public:
  [[nodiscard]] auto getServer() -> TcpServer& { return server(); }
};
}
}
}

CT_BOOST_AUTO_TEST_CASE(startShouldInitServerSuccess) {
  auto messageBus = std::make_shared<start::TrackableMessageBus>();
  auto server = start::success::DeckServerImpl(nullptr, messageBus);
  server.start();
  auto& tcpServer = server.getServer();
  tcpServer.emitNewConnection();
  CT_BOOST_TEST(server.currentConnection() != nullptr);
  CT_BOOST_TEST(messageBus->subscribedTypes().at(0) == DeckMessages::PingReceived);
}

namespace {
namespace start::failure {
class TcpServer : public common::TcpServer {
public:
  [[nodiscard]] auto listen(QHostAddress::SpecialAddress /*address*/, qint16 /*port*/) -> bool { // NOLINT
    return false;
  }
};
class DeckServerImpl : public DeckServerPrivate<DeckServerImpl, TcpServer, common::TcpSocket> {
  friend DeckServerPrivate<DeckServerImpl, TcpServer, common::TcpSocket>;
  using DeckServerPrivate<DeckServerImpl, TcpServer, common::TcpSocket>::DeckServerPrivate;

public:
  [[nodiscard]] auto getServer() -> TcpServer& { return server(); }
};
}
}

CT_BOOST_AUTO_TEST_CASE(startShouldInitServerFailure) {
  auto messageBus = std::make_shared<start::TrackableMessageBus>();
  auto server = start::failure::DeckServerImpl(nullptr, messageBus);
  server.start();
  auto& tcpServer = server.getServer();
  tcpServer.emitNewConnection();
  CT_BOOST_TEST(server.currentConnection() != nullptr);
  CT_BOOST_TEST(messageBus->subscribedTypes().at(0) == DeckMessages::PingReceived);
}

CT_BOOST_AUTO_TEST_SUITE_END()

#include "DeckServerTests.moc"
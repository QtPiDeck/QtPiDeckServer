#pragma once

#include <future>
#include <optional>

#include <QString>

#include "Services/ServiceInterface.hpp"

namespace QtPiDeck::Services {

class IWebSocket : public ServiceInterface {
public:
  enum class ConnectionError { RefusedConnection, Unspecified };

  enum class SendingError { NotConnected, Unspecified };

  using MessageReceivedHandler = std::function<void(QByteArray)>;
  using ConnectedHandler = std::function<void()>;
  using FailHandler = std::function<void(ConnectionError)>;

  virtual void connect(QStringView address) noexcept = 0;
  virtual void disconnect() noexcept = 0;
  [[nodiscard]] virtual auto send(QByteArray message) noexcept -> std::optional<SendingError> = 0;
  virtual void setMessageReceivedHandler(MessageReceivedHandler handler) noexcept = 0;
  virtual void setConnectedHandler(ConnectedHandler handler) noexcept = 0;
  virtual void setFailHandler(FailHandler handler) noexcept = 0;
};
}

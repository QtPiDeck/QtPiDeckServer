#pragma once

#include <QString>

#include "Services/ServiceInterface.hpp"

namespace QtPiDeck::Services {
class IObsMessageIdGenerator : public ServiceInterface {
public:
    [[nodiscard]] virtual auto generateId() const noexcept -> QString = 0;
    [[nodiscard]] virtual auto generateId(const QString& requestName) const noexcept -> QString = 0;
};
}

#pragma once

#include <QUuid>

#include "Services/IObsMessageIdGenerator.hpp"

namespace QtPiDeck::Services {
class ObsMessageIdGenerator final : public IObsMessageIdGenerator {
public:
    ObsMessageIdGenerator() = default;
    ObsMessageIdGenerator(const ObsMessageIdGenerator&) = delete;
    ObsMessageIdGenerator(ObsMessageIdGenerator&&) = default;
    ~ObsMessageIdGenerator() final = default;

    auto operator=(const ObsMessageIdGenerator&) -> ObsMessageIdGenerator& = delete;
    auto operator=(ObsMessageIdGenerator&&) -> ObsMessageIdGenerator& = default;

    [[nodiscard]] auto generateId() const noexcept -> QString final {
        return QUuid::createUuid().toString();
    }

    [[nodiscard]] auto generateId(const QString& requestName) const noexcept -> QString final {
        return QString{"%1-%2"}.arg(requestName, generateId());
    }
};
}

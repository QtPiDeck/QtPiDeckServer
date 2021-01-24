#pragma once

#include <optional>

#include <QString>

#include "ObsRequests.hpp"

namespace QtPiDeck::Network::Obs {
struct ObsResponseStatus {
    QString status{};
    std::optional<QString> error{};
};

inline auto isRequestSuccessful(const ObsResponseStatus& responseStatus) noexcept -> bool {
    return responseStatus.status == "ok"_qs;
}
}

#pragma once

#include <optional>

#include <QString>

#include "ObsRequests.hpp"

namespace QtPiDeck::Network::Obs {
struct ObsResponseStatus {
    QString status{};
    std::optional<QString> error{};

    static const inline QLatin1String statusField = "status"_qls;
    static const inline QLatin1String errorField = "error"_qls;

    static const inline QString successStatus = "ok"_qs;
};

inline auto isRequestSuccessful(const ObsResponseStatus& responseStatus) noexcept -> bool {
    return responseStatus.status == ObsResponseStatus::successStatus;
}
}

#pragma once

#include <optional>

#include <QString>

#include "Network/Obs/ObsRequests.hpp"
#include "Utilities/Literals.hpp"
#include "QtDefinitions.hpp"

namespace QtPiDeck::Network::Obs::Models {
using namespace QtPiDeck::Utilities::literals;
struct ObsResponseStatus {
    QString status{};
    std::optional<QString> error{};
    // false if there was an error during parsing
    // (like lack of mandatory field in json)
    bool parseSuccessful{true};

    static const inline QLatin1String statusField = "status"_qls;
    static const inline QLatin1String errorField = "error"_qls;

    static const inline QString successStatus = CT_QStringLiteral("ok");
};

inline auto isRequestSuccessful(const ObsResponseStatus& responseStatus) noexcept -> bool {
    return responseStatus.status == ObsResponseStatus::successStatus;
}
}

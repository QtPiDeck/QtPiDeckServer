#pragma once

#include <optional>

#include <QJsonDocument>
#include <QJsonObject>

#include "ObsRequests.hpp"
#include "ObsObjectsParsing.hpp"
#include "ObsResponseStatus.hpp"

namespace QtPiDeck::Network::Obs {
struct GetAuthRequiredResponse : ObsResponseStatus {
    bool authRequired{};
    std::optional<QString> challenge{};
    std::optional<QString> salt{};
};

template <>
inline auto ParseObsResponse<General::GetAuthReqired>(QStringView json) {
    return withJsonObject<GetAuthRequiredResponse>(QJsonDocument::fromJson(json.toUtf8()).object())
            .parse(&GetAuthRequiredResponse::authRequired, "authRequered"_qls)
            .parse(&GetAuthRequiredResponse::challenge, "challenge"_qls)
            .parse(&GetAuthRequiredResponse::salt, "salt"_qls)
            .getResult();
}
}

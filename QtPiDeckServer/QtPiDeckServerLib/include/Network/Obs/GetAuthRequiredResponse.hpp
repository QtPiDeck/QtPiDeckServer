#pragma once

#include <optional>

#include <QJsonDocument>
#include <QJsonObject>

#include "ObsObjectsParsing.hpp"
#include "ObsRequests.hpp"
#include "ObsResponseStatus.hpp"

namespace QtPiDeck::Network::Obs {
struct GetAuthRequiredResponse : ObsResponseStatus {
  bool authRequired{};
  std::optional<QString> challenge{};
  std::optional<QString> salt{};

  static auto fromJson(QStringView json) noexcept -> GetAuthRequiredResponse {
    return withJsonObject<GetAuthRequiredResponse>(QJsonDocument::fromJson(json.toUtf8()).object())
        .parse(&GetAuthRequiredResponse::authRequired, "authRequered"_qls)
        .parse(&GetAuthRequiredResponse::challenge, "challenge"_qls)
        .parse(&GetAuthRequiredResponse::salt, "salt"_qls)
        .getResult();
  }
};
}

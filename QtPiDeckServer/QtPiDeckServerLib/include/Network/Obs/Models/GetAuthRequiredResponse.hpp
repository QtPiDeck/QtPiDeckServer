#pragma once

#include <optional>

#include <QJsonDocument>
#include <QJsonObject>

#include "Network/Obs/ObsObjectsParsing.hpp"
#include "Network/Obs/ObsRequests.hpp"
#include "ObsResponseStatus.hpp"

namespace QtPiDeck::Network::Obs::Models {
struct GetAuthRequiredResponse : ObsResponseStatus {
  bool authRequired{};
  std::optional<QString> challenge{};
  std::optional<QString> salt{};

  static const inline QLatin1String authRequiredField = "authRequired"_qls;
  static const inline QLatin1String challengeField = "challenge"_qls;
  static const inline QLatin1String saltField = "salt"_qls;

  static auto fromJson(QStringView json) noexcept -> GetAuthRequiredResponse {
    return withJsonObject<GetAuthRequiredResponse>(QJsonDocument::fromJson(json.toUtf8()).object())
        .parse(&GetAuthRequiredResponse::authRequired, authRequiredField)
        .parse(&GetAuthRequiredResponse::challenge, challengeField)
        .parse(&GetAuthRequiredResponse::salt, saltField)
        .getResult();
  }
};
}

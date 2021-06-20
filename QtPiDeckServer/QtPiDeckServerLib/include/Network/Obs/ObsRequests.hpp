#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <variant>

#include <QJsonObject>
#include <QString>

#include "QtPiDeckServerDefinitions.hpp"

namespace QtPiDeck::Network::Obs {
enum class General : uint16_t { GetAuthReqired, End };
enum class MediaControl : uint16_t { PlayPauseMedia = static_cast<uint16_t>(General::End), End };
using ObsRequest = std::variant<General, MediaControl>;

inline constexpr std::array RequestTypesRaw = {"GetAuthRequired"};

namespace detail {
inline auto typesRawToQString() {
  std::array<QString, RequestTypesRaw.size()> ret;
  std::transform(std::begin(RequestTypesRaw), std::end(RequestTypesRaw), std::begin(ret),
                 [](const char* text) { return QString(text); });
  return ret;
}
}

inline const std::array RequestTypes = detail::typesRawToQString();
}

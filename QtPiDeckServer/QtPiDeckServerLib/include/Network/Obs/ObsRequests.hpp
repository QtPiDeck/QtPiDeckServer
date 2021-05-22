#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include <QJsonObject>
#include <QString>

#include "QtPiDeckServerDefines.hpp"

namespace QtPiDeck::Network::Obs {
enum class General : uint16_t { GetAuthReqired, End };
enum class MediaControl : uint16_t { PlayPauseMedia = static_cast<uint16_t>(General::End), End };

#if !defined(APPLE_CLANG)
template<class T>
concept CONCEPT_BOOL ObsRequest = std::is_same_v<T, General> || std::is_same_v<T, MediaControl>;
#else
#define ObsRequest class
#endif

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

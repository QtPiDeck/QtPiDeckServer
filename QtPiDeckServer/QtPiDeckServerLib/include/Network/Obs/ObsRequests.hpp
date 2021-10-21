#pragma once

#include <array>
#include <cstdint>
#if defined(RANGESV3)
#else
#include <algorithm>
#endif
#include <variant>

#include <QString>

namespace QtPiDeck::Network::Obs {
enum class General : uint16_t { GetAuthRequired, End };
enum class MediaControl : uint16_t { PlayPauseMedia = static_cast<uint16_t>(General::End), End };
using ObsRequest = std::variant<General, MediaControl>;

inline constexpr std::array RequestTypesRaw = {"GetAuthRequired"};

namespace detail {
inline auto typesRawToQString() noexcept {
  std::array<QString, RequestTypesRaw.size()> ret;
#if defined(RANGESV3)
  std::transform(std::cbegin(RequestTypesRaw), std::cend(RequestTypesRaw), std::begin(ret),
                 [](const char* text) { return QString(text); });
#else
  std::ranges::transform(RequestTypesRaw, std::begin(ret), [](const char* text) { return QString(text); });
#endif
  return ret;
}
}

inline const std::array RequestTypes = detail::typesRawToQString();
}

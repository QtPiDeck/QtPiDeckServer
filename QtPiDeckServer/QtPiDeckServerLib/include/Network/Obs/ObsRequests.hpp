#pragma once

#include <array>
#include <cstdint>
#if defined(RANGESV3)
#include <range/v3/algorithm/transform.hpp>
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
#if !defined(RANGESV3)
  namespace ranges = std::ranges;
#endif
  ranges::transform(RequestTypesRaw, std::begin(ret), [](const char* text) { return QString(text); });
  return ret;
}
}

inline const std::array RequestTypes = detail::typesRawToQString();
}

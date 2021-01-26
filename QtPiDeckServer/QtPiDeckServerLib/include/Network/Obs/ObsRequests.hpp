#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include <QJsonObject>
#include <QString>

namespace QtPiDeck::Network::Obs {

// I believe I had this suffix before
// But I still need to move it to common
inline auto operator"" _qs(const char* str, size_t /*unused*/) -> QString { return QString{str}; }

inline auto operator"" _qls(const char* str, size_t /*unused*/) -> QLatin1String { return QLatin1String{str}; }

enum class General : uint16_t { GetAuthReqired, End };
enum class MediaControl : uint16_t { PlayPauseMedia = static_cast<uint16_t>(General::End), End };

#if __cpp_concepts >= 201907L || (defined(_MSC_VER) && __cpp_concepts >= 201811L) || defined(__APPLE__) // because reasons
#define CONCEPT_BOOL
#else
#define CONCEPT_BOOL bool
#endif

template<class T>
concept CONCEPT_BOOL ObsRequest = std::is_same_v<T, General> || std::is_same_v<T, MediaControl>;

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

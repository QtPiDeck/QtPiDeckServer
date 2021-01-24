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

enum General : uint16_t { GetAuthReqired, End };

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

template <uint16_t ObjectId>
auto ParseObsResponse(QStringView json);
}

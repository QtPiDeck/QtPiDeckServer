#pragma once

#include <cstdint>
#include <array>
#include <optional>

#include <QString>
#include <QJsonObject>

namespace QtPiDeck::Network::Obs {

// I believe I had this suffix before
// But I still need to move it to common
inline auto operator"" _qs(const char* str, size_t /*unused*/) -> QString {
    return QString{str};
}

inline auto operator"" _qls(const char* str, size_t /*unused*/) -> QLatin1String {
    return QLatin1String{str};
}

enum General : uint16_t {
    GetAuthReqired,
    End
};

const std::array RequestTypes = {
    "GetAuthRequired"_qs
};

template <uint16_t ObjectId>
auto ParseObsResponse(QStringView json);
}

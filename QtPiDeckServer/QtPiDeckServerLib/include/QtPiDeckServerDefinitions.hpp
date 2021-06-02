#pragma once
#include <cstdint>

#include <QString>
#include <QUrl>

#if __cpp_concepts
#if __cpp_concepts >= 201907L || (defined(_MSC_VER) && __cpp_concepts >= 201811L) // because reasons
#define CONCEPT_BOOL
#else
#define CONCEPT_BOOL bool
#endif
#endif

inline auto operator"" _qs(const char* str, size_t /*unused*/) -> QString { return QString{str}; }
inline auto operator"" _qls(const char* str, size_t /*unused*/) -> QLatin1String { return QLatin1String{str}; }
inline auto operator"" _qurl(const char* str, size_t /*unused*/) -> QUrl { return QUrl{QLatin1String{str}}; }
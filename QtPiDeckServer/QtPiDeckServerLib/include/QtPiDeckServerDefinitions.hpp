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
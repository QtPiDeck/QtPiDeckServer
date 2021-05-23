#pragma once

#include <optional>
#include <type_traits>

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "QtPiDeckServerDefines.hpp"

namespace QtPiDeck::Network::Obs {
template<class T>
struct is_optional : std::false_type {};

template<class T>
struct is_optional<std::optional<T>> : std::false_type {};

template<class T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template<class T>
struct field_type {
  using type = T;
};

template<class T>
struct field_type<std::optional<T>> : field_type<T> {};

template<class T>
using field_type_t = typename field_type<T>::type;

#if !defined(APPLE_CLANG)
template<class T>
concept CONCEPT_BOOL Field = std::is_same_v<field_type_t<T>, QString> || std::is_same_v<field_type_t<T>, bool>;
#else
#define Field class
#endif

template<Field TField>
void setValue(TField& field, const QJsonValue& value) noexcept {
  if constexpr (std::is_same_v<field_type_t<TField>, QString>) {
    field = value.toString();
  } else {
    field = value.toBool();
  }
}

template<Field TField>
void setValue(TField& field, const QJsonObject& object, const QLatin1String& key) noexcept {
  if (!object.contains(key)) {
    if constexpr (is_optional_v<TField>) {
      qWarning("no value for non-optional key '%s'", key); // NOLINT
    }

    return;
  }

  setValue(field, object[key]);
}

template<class TObsObj>
struct [[nodiscard]] withJsonObject {
  explicit withJsonObject(const QJsonObject& jsonObject) noexcept : m_jsonObject(&jsonObject) {
    setValue(m_obsObj.status, jsonObject, QLatin1String{"status"});
    setValue(m_obsObj.error, jsonObject, QLatin1String{"error"});
    m_isOk = isRequestSuccessful(m_obsObj);
  }

  explicit withJsonObject(QStringView json) noexcept
      : withJsonObject(QJsonDocument::fromJson(json.toUtf8()).object()) {}

  template<class TField>
  [[nodiscard]] auto parse(TField TObsObj::*field, const QLatin1String& key) noexcept -> withJsonObject& {
    if (m_isOk) {
      setValue(m_obsObj.*field, *m_jsonObject, key);
    }

    return *this;
  }

  [[nodiscard]] auto getResult() const noexcept -> const TObsObj& { return m_obsObj; }

private:
  const QJsonObject* m_jsonObject;
  TObsObj m_obsObj;
  bool m_isOk;
};
}

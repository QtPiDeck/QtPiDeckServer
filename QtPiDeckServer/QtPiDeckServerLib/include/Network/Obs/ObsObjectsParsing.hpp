#pragma once

#include <optional>
#include <type_traits>

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "QtPiDeckServerDefinitions.hpp"

namespace QtPiDeck::Network::Obs {
template<class T>
struct is_optional : std::false_type {};

template<class T>
struct is_optional<std::optional<T>> : std::true_type {};

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

#if __cpp_concepts
template<class T>
concept CONCEPT_BOOL Field = std::is_same_v<field_type_t<T>, QString> || std::is_same_v<field_type_t<T>, bool>;
#endif

#if __cpp_concepts
template<Field TField>
#else
template<class TField>
#endif
void setValue(TField& field, const QJsonValue& value) noexcept {
  if constexpr (std::is_same_v<field_type_t<TField>, QString>) {
    field = value.toString();
  } else {
    field = value.toBool();
  }
}

#if __cpp_concepts
template<Field TField>
#else
template<class TField>
#endif
void setValue(TField& field, const QJsonObject& object, const QLatin1String& key) noexcept {
  if (const auto& value = object[key]; value.isUndefined() || value.isNull()) {
    if constexpr (!is_optional_v<TField>) {
      qWarning().nospace() << "no value for non-optional key " << key << " (" << typeid(TField).name() << ")"; // NOLINT
    }
  } else {
    setValue(field, value);
  }
}

template<class TObsObj>
struct [[nodiscard]] withJsonObject {
  explicit withJsonObject(const QJsonObject& jsonObject) noexcept : m_jsonObject(&jsonObject), m_isOk(false) {
    setValue(m_obsObj.status, jsonObject, TObsObj::statusField);
    setValue(m_obsObj.error, jsonObject, TObsObj::errorField);
    m_isOk = isRequestSuccessful(m_obsObj);
  }

  explicit withJsonObject(QStringView json) noexcept // NOLINT(cppcoreguidelines-pro-type-member-init)
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

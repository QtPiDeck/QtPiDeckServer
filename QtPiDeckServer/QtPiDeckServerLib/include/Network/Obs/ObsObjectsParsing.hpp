#pragma once

#include <type_traits>
#include <optional>

#include <QString>
#include <QJsonObject>

namespace QtPiDeck::Network::Obs {
template <class T>
struct is_optional : std::false_type {};

template <class T>
struct is_optional<std::optional<T>> : std::false_type {};

template <class T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template <class T>
struct field_type {
    using type = T;
};

template <class T>
struct field_type<std::optional<T>> : field_type<T> {};

template <class T>
using field_type_t = typename field_type<T>::type;

template<class TLeft, class TRight>
using enable_this = std::enable_if_t<std::is_same_v<field_type_t<TLeft>, TRight>, bool>;

template <class TField, enable_this<TField, QString> = true>
void setValue(TField& field, const QJsonValue& value) noexcept {
    field = value.toString();
}

template <class TField, enable_this<TField, bool> = true>
void setValue(TField& field, const QJsonValue& value) noexcept {
    field = value.toBool();
}

template<class TField>
void setValue(TField& field, const QJsonObject& object, const QLatin1String& key) noexcept {
    if (!object.contains(key)) {
        if constexpr (is_optional_v<TField>) {
            qWarning("no value for non-optional key '%s'", key); // NOLINT
        }

        return;
    }

    setValue(field, object[key]);
}

template <class TObsObj>
struct [[nodiscard]] withJsonObject {
    explicit withJsonObject(const QJsonObject& jsonObject) noexcept : m_jsonObject(&jsonObject) {
        setValue(m_obsObj.status, jsonObject, QLatin1String{"status"});
        setValue(m_obsObj.error, jsonObject, QLatin1String{"error"});
        m_isOk = isRequestSuccessful(m_obsObj);
    }

    template<class TField>
    [[nodiscard]] auto parse(TField TObsObj::* field, const QLatin1String& key) noexcept -> withJsonObject& {
        if (m_isOk) {
            setValue(m_obsObj.*field, *m_jsonObject, key);
        }

        return *this;
    }

    [[nodiscard]] auto getResult() const noexcept -> const TObsObj& {
        return m_obsObj;
    }
private:
    const QJsonObject* m_jsonObject;
    TObsObj m_obsObj;
    bool m_isOk;
};
}

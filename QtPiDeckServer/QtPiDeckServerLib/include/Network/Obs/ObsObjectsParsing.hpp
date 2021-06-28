#pragma once

#include <optional>
#include <type_traits>
#include <variant>

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "QtPiDeckServerDefinitions.hpp"

namespace QtPiDeck::Network::Obs {
void setValue(QString& field, const QJsonObject& object, const QLatin1String& key) noexcept;
void setValue(std::optional<QString>& field, const QJsonObject& object, const QLatin1String& key) noexcept;
void setValue(bool& field, const QJsonObject& object, const QLatin1String& key) noexcept;
void setValue(std::optional<bool>& field, const QJsonObject& object, const QLatin1String& key) noexcept;

template<class TObsObj>
struct [[nodiscard]] withJsonObject {
  explicit withJsonObject(const QJsonObject& jsonObject) noexcept : m_jsonObject(jsonObject) {
    setValue(m_obsObj.status, jsonObject, TObsObj::statusField);
    setValue(m_obsObj.error, jsonObject, TObsObj::errorField);
    m_isOk = isRequestSuccessful(m_obsObj);
  }

  explicit withJsonObject(const QString& json) noexcept // NOLINT(cppcoreguidelines-pro-type-member-init)
      : withJsonObject(QJsonDocument::fromJson(json.toUtf8()).object()) {}

  template<class TField>
  [[nodiscard]] auto parse(TField TObsObj::*field, const QLatin1String& key) noexcept -> withJsonObject& {
    if (m_isOk) {
      setValue(m_obsObj.*field, m_jsonObject, key);
    }

    return *this;
  }

  [[nodiscard]] auto getResult() const noexcept -> const TObsObj& { return m_obsObj; }

private:
  const QJsonObject m_jsonObject;
  TObsObj m_obsObj;
  bool m_isOk;
};
}

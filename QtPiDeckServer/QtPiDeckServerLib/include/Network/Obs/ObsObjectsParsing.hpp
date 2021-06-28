#pragma once

#include <optional>
#include <type_traits>
#include <variant>

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include "QtPiDeckServerDefinitions.hpp"
#include "Utilities/Logging.hpp"

namespace QtPiDeck::Network::Obs {
using MessageField = std::variant<QString*, std::optional<QString>*, bool*, std::optional<bool>*>;

[[nodiscard]] auto setValue(MessageField field, const QJsonObject& object, const QLatin1String& key) noexcept -> bool;

template<class TObsObj>
struct [[nodiscard]] withJsonObject {
  explicit withJsonObject(const QJsonObject& jsonObject) noexcept : m_jsonObject(jsonObject) {
    Utilities::initLogger(m_slg, "withJsonObject");
    BOOST_LOG_SEV(m_slg, Utilities::severity::trace) << "Parsing " << typeid(TObsObj).name();
    m_obsObj.parseSuccessful &= setValue(&m_obsObj.status, jsonObject, TObsObj::statusField);
    //m_obsObj.parseSuccessful &= setValue(&m_obsObj.error, jsonObject, TObsObj::errorField);
    m_isOk = isRequestSuccessful(m_obsObj);
  }

  explicit withJsonObject(const QString& json) noexcept // NOLINT(cppcoreguidelines-pro-type-member-init)
      : withJsonObject(QJsonDocument::fromJson(json.toUtf8()).object()) {}

  template<class TField>
  [[nodiscard]] auto parse(TField TObsObj::*field, const QLatin1String& key) noexcept -> withJsonObject& {
    if (m_isOk) {
      //m_obsObj.parseSuccessful &= setValue(&(m_obsObj.*field), m_jsonObject, key);
    }

    return *this;
  }

  [[nodiscard]] auto getResult() const noexcept -> const TObsObj& { return m_obsObj; }

private:
  const QJsonObject m_jsonObject;
  TObsObj m_obsObj;
  bool m_isOk;

  mutable boost::log::sources::severity_logger<boost::log::trivial::severity_level> m_slg;
};
}

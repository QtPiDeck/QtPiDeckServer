#include "Network/Obs/ObsObjectsParsing.hpp"

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include "Utilities/Logging.hpp"

namespace QtPiDeck::Network::Obs {
namespace {
BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT( // NOLINT
    obs_parser_logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>) {
  boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level> lg;
  Utilities::initLogger(lg, "obs_parsing");
  return lg;
}

template<class T>
struct is_optional : std::false_type {};

template<class T>
struct is_optional<std::optional<T>> : std::true_type {};

template<class T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template<class T>
[[nodiscard]] auto checkValue(const QLatin1String& key, const QJsonValue& value) -> bool {
  const bool hasValue = !(value.isUndefined() || value.isNull());
  if constexpr (!is_optional_v<T>) {
    if (!hasValue) {
      BOOST_LOG_SEV(obs_parser_logger::get(), Utilities::severity::warning)
          << "no value for non-optional key " << key.data() << " (" << typeid(T).name() << ")";
    }
  }

  return hasValue;
}

[[nodiscard]] auto toString(const QJsonValue& value) -> QString { return value.toString(); }
[[nodiscard]] auto toBool(const QJsonValue& value) -> bool { return value.toBool(); }

template<class T, class TConverter>
[[nodiscard]] auto setValue(T& field, const QJsonObject& object, const QLatin1String& key,
                            TConverter&& converter) noexcept -> bool {
  if (const auto& value = object.value(key); checkValue<T>(key, value)) {
    field = converter(value);
    return true;
  }

  return is_optional_v<T>;
}
}

[[nodiscard]] auto setValue(QString& field, const QJsonObject& object, const QLatin1String& key) noexcept -> bool {
  return setValue(field, object, key, toString);
}

[[nodiscard]] auto setValue(std::optional<QString>& field, const QJsonObject& object, const QLatin1String& key) noexcept
    -> bool {
  return setValue(field, object, key, toString);
}

[[nodiscard]] auto setValue(bool& field, const QJsonObject& object, const QLatin1String& key) noexcept -> bool {
  return setValue(field, object, key, toBool);
}

[[nodiscard]] auto setValue(std::optional<bool>& field, const QJsonObject& object, const QLatin1String& key) noexcept
    -> bool {
  return setValue(field, object, key, toBool);
}
}
#include "Network/Obs/ObsObjectsParsing.hpp"

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>

#include "Utilities/Logging.hpp"

namespace QtPiDeck::Network::Obs {
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(obs_parser_logger, boost::log::sources::logger_mt);

namespace {
template<class T>
struct is_optional : std::false_type {};

template<class T>
struct is_optional<std::optional<T>> : std::true_type {};

template<class T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template<class T>
auto checkValue(const QLatin1String& key, const QJsonValue& value) -> bool {
  auto& lg = obs_parser_logger::get();
  const bool hasValue = !(value.isUndefined() || value.isNull());
  if constexpr (is_optional_v<T>) {
    if (!hasValue) {
      BOOST_LOG_STREAM_WITH_PARAMS(lg, (boost::log::keywords::severity = Utilities::severity::warning))
          << "no value for non-optional key " << key.data() << " (" << typeid(typename T::value_type).name() << ")";
    }
  }

  return hasValue;
}

auto toString(const QJsonValue& value) -> QString { return value.toString(); }
auto toBool(const QJsonValue& value) -> bool { return value.toBool(); }

template<class T, class TConverter>
void setValue(T& field, const QJsonObject& object, const QLatin1String& key, TConverter&& converter) {
  if (const auto& value = object[key]; checkValue<T>(key, value)) {
    field = converter(value);
  }
}
}

void setValue(QString& field, const QJsonObject& object, const QLatin1String& key) noexcept {
  setValue(field, object, key, toString);
}

void setValue(std::optional<QString>& field, const QJsonObject& object, const QLatin1String& key) noexcept {
  setValue(field, object, key, toString);
}

void setValue(bool& field, const QJsonObject& object, const QLatin1String& key) noexcept {
  setValue(field, object, key, toBool);
}

void setValue(std::optional<bool>& field, const QJsonObject& object, const QLatin1String& key) noexcept {
  setValue(field, object, key, toBool);
}
}
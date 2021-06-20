#define BOOST_TEST_MODULE ObsObjectsParsingTests // NOLINT
#include "BoostUnitTest.hpp"

#include "Network/Obs/ObsObjectsParsing.hpp"
#include "Network/Obs/ObsResponseStatus.hpp"
#include "Utilities/Literals.hpp"
#include "Utilities/Logging.hpp"

auto main(int argc, char* argv[]) -> int {
  QtPiDeck::Utilities::initLogging("ObsObjectsParsingTests");
  return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}

namespace std {
auto operator<<(std::ostream& ostr, const std::optional<bool>& right) -> std::ostream& {
  return operator<<(ostr, right.has_value() ? static_cast<int8_t>(right.value()) : int8_t{-1});
}

template<class T>
auto operator==(const std::optional<T>& left, const std::optional<T>& right) -> bool {
  if (left.has_value() && right.has_value()) {
    return left.value() == right.value();
  }

  return left.has_value() == right.has_value();
}
}

auto operator<<(std::ostream& ostr, const std::optional<QString>& right) -> std::ostream& {
  using namespace QtPiDeck::Utilities::literals;
  return operator<<(ostr, right.value_or("(nil)"_qs).toStdString());
}

CT_BOOST_AUTO_TEST_SUITE(ObsObjectsParsingTests)

using namespace QtPiDeck::Utilities::literals;

struct TestObject : QtPiDeck::Network::Obs::ObsResponseStatus {
  std::optional<bool> importantValue;
};

CT_BOOST_AUTO_TEST_CASE(ObjectSpecificFieldShouldBeSkippedIfErrorOccured) {
  const auto errorStr = "ERROR"_qs;
  const auto valueKey = "val"_qls;
  const auto json = [&]() {
    QJsonObject obj;
    obj[valueKey] = true;
    obj[TestObject::statusField] = "error"_qs;
    obj[TestObject::errorField] = errorStr;
    return QString{QJsonDocument{obj}.toJson()};
  }();

  auto parser = QtPiDeck::Network::Obs::withJsonObject<TestObject>(json);
  const auto objWithBasicParsing = parser.getResult();
  const auto objWithFullParsing = parser.parse(&TestObject::importantValue, valueKey).getResult();
  CT_BOOST_TEST(objWithBasicParsing.error == objWithFullParsing.error);
  CT_BOOST_TEST(objWithBasicParsing.error == errorStr);
  CT_BOOST_TEST(objWithBasicParsing.status == objWithFullParsing.status);
  CT_BOOST_TEST(!objWithBasicParsing.importantValue.has_value());
  CT_BOOST_TEST(objWithBasicParsing.importantValue == objWithFullParsing.importantValue);
}

CT_BOOST_AUTO_TEST_SUITE_END()
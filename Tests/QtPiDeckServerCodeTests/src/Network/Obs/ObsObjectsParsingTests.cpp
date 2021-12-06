#include "BoostUnitTest.hpp"

#include "Network/Obs/Models/ObsResponseStatus.hpp"
#include "Network/Obs/ObsObjectsParsing.hpp"
#include "Utilities/Literals.hpp"

namespace std {
auto operator<<(std::ostream& ostr, const std::optional<bool>& right) -> std::ostream& {
  return operator<<(ostr, right.has_value() ? static_cast<int8_t>(right.value()) : int8_t{-1});
}
}

/*auto operator<<(std::ostream& ostr, const std::optional<QString>& right) -> std::ostream& {
  return operator<<(ostr, right.value_or(CT_QStringLiteral("(nil)")).toStdString());
}*/

CT_BOOST_AUTO_TEST_SUITE(ObsObjectsParsingTests)

using namespace QtPiDeck::Utilities::literals;

struct TestObject : public QtPiDeck::Network::Obs::Models::ObsResponseStatus {
  std::optional<bool> importantValue;
};

CT_BOOST_AUTO_TEST_CASE(objectSpecificFieldShouldBeSkippedIfErrorOccured) {
  const auto errorStr = CT_QStringLiteral("ERROR");
  const auto valueKey = "val"_qls;
  const auto json = [&]() {
    QJsonObject obj;
    obj[valueKey] = true;
    obj[TestObject::statusField] = CT_QStringLiteral("error");
    obj[TestObject::errorField] = errorStr;
    return QString{QJsonDocument{obj}.toJson()};
  }();

  auto parser = QtPiDeck::Network::Obs::withJsonObject<TestObject>(json);
  const auto objWithBasicParsing = parser.getResult();
  const auto objWithFullParsing = parser.parse(&TestObject::importantValue, valueKey).getResult();
  CT_BOOST_TEST(*objWithBasicParsing.error == *objWithFullParsing.error);
  CT_BOOST_TEST(*objWithBasicParsing.error == errorStr);
  CT_BOOST_TEST(objWithBasicParsing.status == objWithFullParsing.status);
  CT_BOOST_TEST(!objWithBasicParsing.importantValue.has_value());
  CT_BOOST_TEST(!objWithFullParsing.importantValue.has_value());
}

CT_BOOST_AUTO_TEST_CASE(parseOptionalBool) {
  const auto valueKey = "val"_qls;
  const auto json = [&]() {
    QJsonObject obj;
    obj[valueKey] = true;
    obj[TestObject::statusField] = TestObject::successStatus;
    return QString{QJsonDocument{obj}.toJson()};
  }();

  const auto objWithFullParsing =
      QtPiDeck::Network::Obs::withJsonObject<TestObject>(json).parse(&TestObject::importantValue, valueKey).getResult();
  CT_BOOST_TEST(objWithFullParsing.importantValue.has_value());
  CT_BOOST_TEST(objWithFullParsing.importantValue.value() == true);
}

struct SecondTestObject : public QtPiDeck::Network::Obs::Models::ObsResponseStatus {
  bool importantValue{};
};

CT_BOOST_AUTO_TEST_CASE(handleMissingMandatoryField) {
  const auto valueKey = "val"_qls;
  const auto json = [&]() {
    QJsonObject obj;
    obj[TestObject::statusField] = TestObject::successStatus;
    return QString{QJsonDocument{obj}.toJson()};
  }();

  const auto objWithFullParsing = QtPiDeck::Network::Obs::withJsonObject<SecondTestObject>(json)
                                      .parse(&SecondTestObject::importantValue, valueKey)
                                      .getResult();
  CT_BOOST_TEST(!objWithFullParsing.parseSuccessful);
}

CT_BOOST_AUTO_TEST_SUITE_END()
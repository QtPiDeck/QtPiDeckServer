#include "BoostUnitTest.hpp"

#include "Network/Obs/Models/GetAuthRequiredResponse.hpp"
#include "Utilities/Literals.hpp"

auto operator<<(std::ostream& ostr, const std::optional<QString>& right) -> std::ostream& {
  using namespace QtPiDeck::Utilities::literals;
  return operator<<(ostr, right.value_or(CT_QStringLiteral("(nil)")).toStdString());
}

CT_BOOST_AUTO_TEST_SUITE(GetAuthRequiredResponseTests)

using namespace QtPiDeck::Network::Obs;
using namespace QtPiDeck::Network::Obs::Models;

CT_BOOST_AUTO_TEST_CASE(GetAuthRequiredResponseShouldBeParsedFromJson) {
  using namespace QtPiDeck::Utilities::literals;
  const auto challengeString = CT_QStringLiteral("12345");
  const auto json = [&challengeString]() {
    QJsonObject obj;
    obj[GetAuthRequiredResponse::authRequiredField] = false;
    obj[GetAuthRequiredResponse::challengeField] = challengeString;
    obj[GetAuthRequiredResponse::saltField] = QJsonValue::Null;
    obj[GetAuthRequiredResponse::statusField] = GetAuthRequiredResponse::successStatus;
    // omitted (no key in json)
    // obj[GetAuthRequiredResponse::errorField] = QJsonValue::Null;
    return QString{QJsonDocument{obj}.toJson()};
  }();

  const auto parsedObject = GetAuthRequiredResponse::fromJson(json);

  CT_BOOST_TEST(parsedObject.authRequired == false);
  CT_BOOST_TEST(parsedObject.challenge == challengeString);
  CT_BOOST_TEST(!parsedObject.salt.has_value());
  CT_BOOST_TEST(parsedObject.status == GetAuthRequiredResponse::successStatus);
  CT_BOOST_TEST(!parsedObject.error.has_value());
}

CT_BOOST_AUTO_TEST_SUITE_END()
#include "BoostUnitTest.hpp"

#include "Services/ObsMessageIdGenerator.hpp"

using namespace QtPiDeck::Services;

namespace {
struct Fixture {
  std::unique_ptr<ObsMessageIdGenerator> obsMessageIdGenerator{std::make_unique<ObsMessageIdGenerator>()};
};
}

CT_BOOST_FIXTURE_TEST_SUITE(ObsMessageIdGeneratorTests, Fixture)

CT_BOOST_AUTO_TEST_CASE(differentValueEveryTime) {
  const auto val1 = obsMessageIdGenerator->generateId();
  const auto val2 = obsMessageIdGenerator->generateId();
  CT_BOOST_TEST(val1 != val2);
}


CT_BOOST_AUTO_TEST_CASE(differentValueEveryTimeDifferentRequests) {
  const QString request1{"request1"};
  const QString request2{"request2"};
  const auto val1 = obsMessageIdGenerator->generateId(request1);
  const auto val2 = obsMessageIdGenerator->generateId(request2);
  CT_BOOST_TEST(val1 != val2);
}

CT_BOOST_AUTO_TEST_CASE(differentValueEveryTimeSameRequests) {
  const QString request{"request"};
  const auto val1 = obsMessageIdGenerator->generateId(request);
  const auto val2 = obsMessageIdGenerator->generateId(request);
  CT_BOOST_TEST(val1 != val2);
}

CT_BOOST_AUTO_TEST_SUITE_END()
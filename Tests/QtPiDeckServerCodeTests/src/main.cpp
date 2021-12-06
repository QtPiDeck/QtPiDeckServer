#define BOOST_TEST_MODULE QtPiDeckServerCodeTests // NOLINT
#include "BoostUnitTest.hpp"

#include "Utilities/Logging.hpp"

struct GlobalInit {
  GlobalInit() { QtPiDeck::Utilities::initLogging("QtPiDeckServerCodeTests"); }
};

const GlobalInit init;

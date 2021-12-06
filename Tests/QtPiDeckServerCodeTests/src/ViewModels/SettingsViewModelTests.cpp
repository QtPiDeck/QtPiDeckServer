#include "BoostUnitTest.hpp"

#include "QtDefinitions.hpp"
#include "Utilities/Literals.hpp"
#include "ViewModels/SettingsViewModel.hpp"

#include <array>

#include "SignalCatcher.hpp"

using namespace QtPiDeck::Tests;
using namespace QtPiDeck::ViewModels;
using namespace QtPiDeck::Utilities::literals;
using namespace QtPiDeck::Services;

struct Fixture {
  SettingsViewModel settingsViewModel{};
};

CT_BOOST_AUTO_TEST_SUITE(SettingsViewModelTests)

class TestServerSettingsStorage final : public IServerSettingsStorage {
public:
  [[nodiscard]] auto deckServerAddress() const noexcept -> QString final { return CT_QStringLiteral("server_address"); }
  void setDeckServerAddress(const QString& deckServerAddress) noexcept final {
    m_deckServerAddress = deckServerAddress;
  }
  [[nodiscard]] auto deckServerPort() const noexcept -> QString final { return CT_QStringLiteral("server_port"); }
  void setDeckServerPort(const QString& deckServerPort) noexcept final { m_deckServerPort = deckServerPort; }
  [[nodiscard]] auto obsWebsocketAddress() const noexcept -> QString final { return CT_QStringLiteral("obs_address"); }
  void setObsWebsocketAddress(const QString& obsWebsocketAddress) noexcept final {
    m_obsWebsocketAddress = obsWebsocketAddress;
  }
  [[nodiscard]] auto obsWebsocketPort() const noexcept -> QString final { return CT_QStringLiteral("obs_port"); }
  void setObsWebsocketPort(const QString& obsWebsocketPort) noexcept final { m_obsWebsocketPort = obsWebsocketPort; }
  [[nodiscard]] auto read(const QString& /*key*/, const QVariant& /*defaultValue*/ = QVariant{}) const noexcept
      -> QVariant final {
    return QVariant{};
  };
  void store(const QString& /*key*/, const QVariant& /*value*/) noexcept final{};

  [[nodiscard]] auto getNewDeckServerAddress() const noexcept -> QString { return m_deckServerAddress; }
  [[nodiscard]] auto getNewDeckServerPort() const noexcept -> QString { return m_deckServerPort; }
  [[nodiscard]] auto getNewObsWebsocketAddress() const noexcept -> QString { return m_obsWebsocketAddress; }
  [[nodiscard]] auto getNewObsWebsocketPort() const noexcept -> QString { return m_obsWebsocketPort; }

private:
  QString m_deckServerAddress{};
  QString m_deckServerPort{};
  QString m_obsWebsocketAddress{};
  QString m_obsWebsocketPort{};
};

CT_BOOST_AUTO_TEST_CASE(initWithSettingsStorage) {
  auto settingsStorage = std::shared_ptr<IServerSettingsStorage>(new TestServerSettingsStorage());
  SettingsViewModel settingsViewModel{nullptr, settingsStorage};

  CT_BOOST_TEST(settingsViewModel.deckServerAddress() == settingsStorage->deckServerAddress());
  CT_BOOST_TEST(settingsViewModel.deckServerPort() == settingsStorage->deckServerPort());
  CT_BOOST_TEST(settingsViewModel.obsWebsocketAddress() == settingsStorage->obsWebsocketAddress());
  CT_BOOST_TEST(settingsViewModel.obsWebsocketPort() == settingsStorage->obsWebsocketPort());
}

CT_BOOST_AUTO_TEST_CASE(saveSettings) {
  auto settingsStorage = std::shared_ptr<IServerSettingsStorage>(new TestServerSettingsStorage());
  SettingsViewModel settingsViewModel{nullptr, settingsStorage};
  const std::array values{CT_QStringLiteral("1.1.1.1"), CT_QStringLiteral("2000"), CT_QStringLiteral("3.3.3.3"),
                          CT_QStringLiteral("4000")};
  settingsViewModel.setDeckServerAddress(values[0]);
  settingsViewModel.setDeckServerPort(values[1]);
  settingsViewModel.setObsWebsocketAddress(values[2]);
  settingsViewModel.setObsWebsocketPort(values[3]);
  settingsViewModel.saveSettings();
  const auto testSettingsStorage = std::dynamic_pointer_cast<TestServerSettingsStorage>(settingsStorage);
  CT_BOOST_TEST(testSettingsStorage->getNewDeckServerAddress() == values[0]);
  CT_BOOST_TEST(testSettingsStorage->getNewDeckServerPort() == values[1]);
  CT_BOOST_TEST(testSettingsStorage->getNewObsWebsocketAddress() == values[2]);
  CT_BOOST_TEST(testSettingsStorage->getNewObsWebsocketPort() == values[3]);
}

CT_BOOST_AUTO_TEST_SUITE_END()

CT_BOOST_FIXTURE_TEST_SUITE(SettingsViewModelTests, Fixture)

CT_BOOST_AUTO_TEST_CASE(saveSettingsWithoutService) { settingsViewModel.saveSettings(); }

CT_BOOST_AUTO_TEST_CASE(setNewDeckServerAddressValue) {
  const QString firstValue = "123.123.123.123";
  const QString secondValue = "12.12.12.12";
  settingsViewModel.setDeckServerAddress(firstValue);
  CT_BOOST_TEST(firstValue == settingsViewModel.deckServerAddress());
  SignalCatcher deckServerAddressChangedCatcher{&settingsViewModel, &SettingsViewModel::deckServerAddressChanged};
  settingsViewModel.setDeckServerAddress(secondValue);
  CT_BOOST_TEST(secondValue == settingsViewModel.deckServerAddress());
  CT_BOOST_TEST(deckServerAddressChangedCatcher.count() == 1);
}

CT_BOOST_AUTO_TEST_CASE(setSameDeckServerAddressValue) {
  const QString value = "123.123.123.123";
  settingsViewModel.setDeckServerAddress(value);
  CT_BOOST_TEST(value == settingsViewModel.deckServerAddress());
  SignalCatcher deckServerAddressChangedCatcher{&settingsViewModel, &SettingsViewModel::deckServerAddressChanged};
  settingsViewModel.setDeckServerAddress(value);
  CT_BOOST_TEST(value == settingsViewModel.deckServerAddress());
  CT_BOOST_TEST(deckServerAddressChangedCatcher.count() == 0);
}

CT_BOOST_AUTO_TEST_CASE(setNewDeckServerPortValue) {
  const QString firstValue = "12345";
  const QString secondValue = "1234";
  settingsViewModel.setDeckServerPort(firstValue);
  CT_BOOST_TEST(firstValue == settingsViewModel.deckServerPort());
  SignalCatcher deckServerPortChangedCatcher{&settingsViewModel, &SettingsViewModel::deckServerPortChanged};
  settingsViewModel.setDeckServerPort(secondValue);
  CT_BOOST_TEST(secondValue == settingsViewModel.deckServerPort());
  CT_BOOST_TEST(deckServerPortChangedCatcher.count() == 1);
}

CT_BOOST_AUTO_TEST_CASE(setSameDeckServerPortValue) {
  const QString value = "12345";
  settingsViewModel.setDeckServerPort(value);
  CT_BOOST_TEST(value == settingsViewModel.deckServerPort());
  SignalCatcher deckServerPortChangedCatcher{&settingsViewModel, &SettingsViewModel::deckServerPortChanged};
  settingsViewModel.setDeckServerPort(value);
  CT_BOOST_TEST(value == settingsViewModel.deckServerPort());
  CT_BOOST_TEST(deckServerPortChangedCatcher.count() == 0);
}

CT_BOOST_AUTO_TEST_CASE(setNewObsWebsocketAddressValue) {
  const QString firstValue = "123.123.123.123";
  const QString secondValue = "12.12.12.12";
  settingsViewModel.setObsWebsocketAddress(firstValue);
  CT_BOOST_TEST(firstValue == settingsViewModel.obsWebsocketAddress());
  SignalCatcher obsWebsocketAddressChangedCatcher{&settingsViewModel, &SettingsViewModel::obsWebsocketAddressChanged};
  settingsViewModel.setObsWebsocketAddress(secondValue);
  CT_BOOST_TEST(secondValue == settingsViewModel.obsWebsocketAddress());
  CT_BOOST_TEST(obsWebsocketAddressChangedCatcher.count() == 1);
}

CT_BOOST_AUTO_TEST_CASE(setSameObsWebsocketAddressValue) {
  const QString value = "123.123.123.123";
  settingsViewModel.setObsWebsocketAddress(value);
  CT_BOOST_TEST(value == settingsViewModel.obsWebsocketAddress());
  SignalCatcher obsWebsocketAddressChangedCatcher{&settingsViewModel, &SettingsViewModel::obsWebsocketAddressChanged};
  settingsViewModel.setObsWebsocketAddress(value);
  CT_BOOST_TEST(value == settingsViewModel.obsWebsocketAddress());
  CT_BOOST_TEST(obsWebsocketAddressChangedCatcher.count() == 0);
}

CT_BOOST_AUTO_TEST_CASE(setNewObsWebsocketPortValue) {
  const QString firstValue = "12345";
  const QString secondValue = "1234";
  settingsViewModel.setObsWebsocketPort(firstValue);
  CT_BOOST_TEST(firstValue == settingsViewModel.obsWebsocketPort());
  SignalCatcher obsWebsocketPortChangedCatcher{&settingsViewModel, &SettingsViewModel::obsWebsocketPortChanged};
  settingsViewModel.setObsWebsocketPort(secondValue);
  CT_BOOST_TEST(secondValue == settingsViewModel.obsWebsocketPort());
  CT_BOOST_TEST(obsWebsocketPortChangedCatcher.count() == 1);
}

CT_BOOST_AUTO_TEST_CASE(setSameObsWebsocketPortValue) {
  const QString value = "12345";
  settingsViewModel.setObsWebsocketPort(value);
  CT_BOOST_TEST(value == settingsViewModel.obsWebsocketPort());
  SignalCatcher obsWebsocketPortChangedCatcher{&settingsViewModel, &SettingsViewModel::obsWebsocketPortChanged};
  settingsViewModel.setObsWebsocketPort(value);
  CT_BOOST_TEST(value == settingsViewModel.obsWebsocketPort());
  CT_BOOST_TEST(obsWebsocketPortChangedCatcher.count() == 0);
}

#if QT_VERSION_MAJOR == 6
CT_BOOST_AUTO_TEST_CASE(bindableDeckServerAddress) {
  const QString value = "12345";
  settingsViewModel.setDeckServerAddress(value);
  const auto binding = settingsViewModel.bindableDeckServerAddress();
  CT_BOOST_TEST(binding.value() == value);
}

CT_BOOST_AUTO_TEST_CASE(bindableDeckServerPort) {
  const QString value = "12345";
  settingsViewModel.setDeckServerPort(value);
  const auto binding = settingsViewModel.bindableDeckServerPort();
  CT_BOOST_TEST(binding.value() == value);
}

CT_BOOST_AUTO_TEST_CASE(bindableObsWebsocketAddress) {
  const QString value = "12345";
  settingsViewModel.setObsWebsocketAddress(value);
  const auto binding = settingsViewModel.bindableObsWebsocketAddress();
  CT_BOOST_TEST(binding.value() == value);
}

CT_BOOST_AUTO_TEST_CASE(bindableObsWebsocketPort) {
  const QString value = "12345";
  settingsViewModel.setObsWebsocketPort(value);
  const auto binding = settingsViewModel.bindableObsWebsocketPort();
  CT_BOOST_TEST(binding.value() == value);
}
#endif

CT_BOOST_AUTO_TEST_SUITE_END()
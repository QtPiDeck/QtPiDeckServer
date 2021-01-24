#include <QtTest>

#include "Services/ObsMessageIdGenerator.hpp"

namespace QtPiDeck::Services::Tests {
class ObsMessageIdGeneratorTests : public QObject {
    Q_OBJECT // NOLINT

private slots:
    void init();
    void differentValueEveryTime();
    void differentValueEveryTimeDifferentRequests();
    void differentValueEveryTimeSameRequests();

private: // NOLINT(readability-redundant-access-specifiers)
    std::unique_ptr<ObsMessageIdGenerator> m_obsMessageIdGenerator;
};

void ObsMessageIdGeneratorTests::init() {
    m_obsMessageIdGenerator = std::make_unique<ObsMessageIdGenerator>();
}

void ObsMessageIdGeneratorTests::differentValueEveryTime() {
    const auto val1 = m_obsMessageIdGenerator->generateId();
    const auto val2 = m_obsMessageIdGenerator->generateId();
    QVERIFY(val1 != val2);
}

void ObsMessageIdGeneratorTests::differentValueEveryTimeDifferentRequests() {
    const QString request1{"request1"};
    const QString request2{"request2"};
    const auto val1 = m_obsMessageIdGenerator->generateId(request1);
    const auto val2 = m_obsMessageIdGenerator->generateId(request2);
    QVERIFY(val1 != val2);
}

void ObsMessageIdGeneratorTests::differentValueEveryTimeSameRequests() {
    const QString request{"request"};
    const auto val1 = m_obsMessageIdGenerator->generateId(request);
    const auto val2 = m_obsMessageIdGenerator->generateId(request);
    QVERIFY(val1 != val2);
}

}

QTEST_APPLESS_MAIN(QtPiDeck::Services::Tests::ObsMessageIdGeneratorTests) // NOLINT

#include "main.moc"

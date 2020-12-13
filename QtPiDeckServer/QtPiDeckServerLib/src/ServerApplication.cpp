#include "ServerApplication.hpp"

namespace QtPiDeck::Server {
ServerApplication::ServerApplication() : QtPiDeck::Application() {

}

auto ServerApplication::mainPage() -> QUrl {
    return QUrl{QStringLiteral("qrc:/qml/pages/main.qml")};
}
}

#include "Network/DeckServer.hpp"

#include <QTcpSocket>


#include "Application.hpp"
#include "Services/IMessageBus.hpp"
#include "Network/MessageHeader.hpp"
#include "Network/DeckDataStream.hpp"

namespace QtPiDeck::Network {

// TODO:
// better name
// add overload with const Bus::Message& arg passed
// move to separate file
template<class T>
auto wrap(T* context, void(T::* function)()) {
    return [context, function](const Bus::Message& /*message*/) {(context->*function)();};
}

DeckServer::DeckServer(QObject *parent) : QObject(parent) {
}

void DeckServer::start() {
    connectToServerSignal();
    Application::current()->ioc().resolveService<Services::IMessageBus>()->subscribe(this, wrap(this, &DeckServer::sendPong), DeckMessages::PingReceived);
    // default port and address will be in global config
    constexpr qint16 defaultPort = 13000;
    if(!m_server.listen(QHostAddress::LocalHost, defaultPort)) {
        qWarning() << "Failed to start DeckServer";
    }
}

void DeckServer::sendPong() {
    const QtPiDeck::Network::MessageHeader response{0, QtPiDeck::Network::MessageId::Pong};
    QByteArray qba;
    DeckDataStream outStream{&qba, QIODevice::WriteOnly};
    outStream << response;
    m_socket->write(qba);
    m_socket->flush();
}

void DeckServer::handleConnection() {
    qDebug() << "New client";
    // only one connection at the same time
    disconnect(*m_serverConnection);
    m_serverConnection.reset();
    m_socket = m_server.nextPendingConnection();
    connect(m_socket, &QTcpSocket::disconnected, this, &DeckServer::connectToServerSignal);
    connect(m_socket, &QTcpSocket::readyRead, this, &DeckServer::readData);
}

namespace {
void processMessage(const QtPiDeck::Network::MessageHeader & header) {
    switch (header.messageId) {
    case QtPiDeck::Network::MessageId::Ping:
        qDebug() << "Got ping";
        Application::current()->ioc().resolveService<Services::IMessageBus>()->sendMessage(Bus::Message{DeckMessages::PingReceived});
        break;
    default:
        qWarning() << "Unhandled message";
        break;
    }
}

template<class T>
std::optional<T> readObject(QTcpSocket * socket) {
    DeckDataStream inStream{socket};
    T object{};

    inStream.startTransaction();
    inStream >> object;
    if (inStream.commitTransaction()) {
        return object;
    }

    return std::nullopt;
}
}

void DeckServer::readData() {
    const auto header = readObject<QtPiDeck::Network::MessageHeader>(m_socket);

    if (header) {
        processMessage(*header);
    }
}

void DeckServer::connectToServerSignal() {
    if (m_serverConnection) {
        disconnect(*m_serverConnection);
    }

    m_serverConnection = connect(&m_server, &QTcpServer::newConnection, this, &DeckServer::handleConnection);
}
}

#include "Network/DeckServer.inl"

#include <QTcpSocket>

template class QtPiDeck::Network::detail::DeckServerPrivate<QtPiDeck::Network::DeckServer, QTcpServer, QTcpSocket>;

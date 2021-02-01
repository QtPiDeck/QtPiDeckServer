#pragma once

#include <cstdint>

namespace QtPiDeck::Bus {
enum ObsMessages : uint64_t {
    GetAuthRequiredResponseReceived = 0x400
};
}

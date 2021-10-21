#include <algorithm>
#include <array>
#include <ranges>

int main() {
  const std::array<int, 1> in{0};
  std::array<int, 1> out{0};
  std::ranges::transform(in, std::begin(out), [](int val) { return val; });
  return 0;
}
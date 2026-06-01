#include <cstddef>
#include <queue>
#include <vector>
class puzzle_state {
  // a state is defined as a M x N array with -1 used to represent an empty
  // space, and an integer used to represent a colour.
public:
  size_t rows;
  size_t col;
  size_t cost = 0;
  std::vector<int> state;

  explicit puzzle_state(int rows, int col)
      : rows(rows), state(rows * col, -1) {}
  bool operator<(puzzle_state &rhs) { return this->cost < rhs.cost; }

private:
};

int main(int argc, char *argv[]) {
  std::priority_queue<puzzle_state> states;
  return 0;
}

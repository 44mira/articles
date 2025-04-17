#include <memory>
#include <string>
#include <vector>

using namespace std;

struct Node {
  string name;
  vector<shared_ptr<Node>> neighbors;
  size_t neighbor_count = 0;

  Node(string _name): name(_name), neighbors(vector<shared_ptr<Node>>()) {}
  Node(string _name, vector<shared_ptr<Node>> _neighbors): name(_name) {
    for (shared_ptr<Node> neighbor: _neighbors) {
      this->connect(neighbor);
    }
  }

  Node& connect(shared_ptr<Node> other) {
    this->neighbor_count++;
    other->neighbor_count++;

    this->neighbors.push_back(other);
    return *this;
  }
};

#include "finiteAutomaton.cpp"
#include <bits/stdc++.h> 

using namespace std;

void testEps() {
  finiteAutomaton<int, char> foo(5, 0, std::vector<int>({2, 3}));
  foo.insertEdge(0, 1, 'x');
  foo.insertEdge(1, 2, 'y');
  foo.insertEdge(0, 3, '.');
  foo.insertEdge(3, 4, '.');
  foo.insertEdge(4, 2, 'z');
  foo = foo.eraseZeroEdges('.');
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(make_pair(make_pair(0, 1), 'x'));
  d.push_back(make_pair(make_pair(0, 2), 'z'));
  d.push_back(make_pair(make_pair(1, 2), 'y'));
  d.push_back(make_pair(make_pair(3, 2), 'z'));
  d.push_back(make_pair(make_pair(4, 2), 'z'));
  std::vector<int> t = {0, 2, 3};
  assert(foo.getEdges() == d);
  assert(foo.getTerminals() == t);
}

void testFull() {
  finiteAutomaton<int, char> foo(2, 0, std::vector<int>({1}));
  foo.insertEdge(0, 1, 'a');
  foo = foo.makeFull(std::vector<char>({'a', 'b'}));
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(make_pair(make_pair(0, 1), 'a'));
  d.push_back(make_pair(make_pair(0, 2), 'b'));
  d.push_back(make_pair(make_pair(1, 2), 'a'));
  d.push_back(make_pair(make_pair(1, 2), 'b'));
  d.push_back(make_pair(make_pair(2, 2), 'a'));
  d.push_back(make_pair(make_pair(2, 2), 'b'));
  std::vector<int> t = {1};
  assert(foo.getEdges() == d);
  assert(foo.getTerminals() == t);
}

void testDet() {
  finiteAutomaton<int, char> foo(3, 0, std::vector<int>({1}));
  foo.insertEdge(0, 1, 'x');
  foo.insertEdge(0, 2, 'x');
  foo.insertEdge(1, 2, 'y');
  foo.insertEdge(1, 2, 's');
  foo = foo.determine();
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(make_pair(make_pair(0, 1), 'a'));
  d.push_back(make_pair(make_pair(0, 2), 'b'));
  d.push_back(make_pair(make_pair(1, 2), 'a'));
  d.push_back(make_pair(make_pair(1, 2), 'b'));
  d.push_back(make_pair(make_pair(2, 2), 'a'));
  d.push_back(make_pair(make_pair(2, 2), 'b'));
  std::vector<int> t = {1};
  foo.print();
  //assert(foo.getEdges() == d);
  //assert(foo.getTerminals() == t);
}

int main() {
  testEps();
  testFull();
  testDet();
  return 0;
}

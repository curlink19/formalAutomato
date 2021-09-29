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
  foo.insertEdge(2, 1, 's');
  foo = foo.determine();
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(make_pair(make_pair(0, 1), 'x'));
  d.push_back(make_pair(make_pair(1, 2), 'y'));
  d.push_back(make_pair(make_pair(1, 3), 's'));
  d.push_back(make_pair(make_pair(2, 3), 's'));
  d.push_back(make_pair(make_pair(3, 2), 'y'));
  std::vector<int> t = {1, 3};
  assert(foo.getEdges() == d);
  assert(foo.getTerminals() == t);
}

void testNeg() {
  finiteAutomaton<int, char> foo(2, 0, std::vector<int>({0}));
  foo.insertEdge(0, 1, 'a');
  foo.insertEdge(1, 0, 'b');
  foo = foo.makeFull(std::vector<char>({'a', 'b'}));
  foo = foo.negatate();
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(make_pair(make_pair(0, 1), 'a'));
  d.push_back(make_pair(make_pair(0, 2), 'b'));
  d.push_back(make_pair(make_pair(1, 0), 'b'));
  d.push_back(make_pair(make_pair(1, 2), 'a'));
  d.push_back(make_pair(make_pair(2, 2), 'a'));
  d.push_back(make_pair(make_pair(2, 2), 'b'));
  std::vector<int> t = {1, 2};
  assert(foo.getEdges() == d);
  assert(foo.getTerminals() == t);
}

void testMin() {
  finiteAutomaton<int, char> foo(4, 0, std::vector<int>({3}));
  foo.insertEdge(0, 1, 'a');
  foo.insertEdge(0, 2, 'b');
  foo.insertEdge(1, 3, 'b');
  foo.insertEdge(2, 3, 'a');
  foo.insertEdge(3, 0, '.');
  foo = foo.eraseZeroEdges('.');
  foo = foo.determine();
  foo = foo.makeFull(std::vector<char>({'a', 'b'}));
  foo = foo.minimize();
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(make_pair(make_pair(0, 2), 'a'));
  d.push_back(make_pair(make_pair(0, 3), 'b'));
  d.push_back(make_pair(make_pair(1, 2), 'a'));
  d.push_back(make_pair(make_pair(1, 3), 'b'));
  d.push_back(make_pair(make_pair(2, 1), 'b'));
  d.push_back(make_pair(make_pair(2, 4), 'a'));
  d.push_back(make_pair(make_pair(3, 1), 'a'));
  d.push_back(make_pair(make_pair(3, 4), 'b'));
  d.push_back(make_pair(make_pair(4, 4), 'a'));
  d.push_back(make_pair(make_pair(4, 4), 'b'));
  std::vector<int> t = {1};
  assert(foo.getEdges() == d);
  assert(foo.getTerminals() == t);
}

int main() {
  testEps();
  testFull();
  testDet();
  testNeg();
  testMin();
  return 0;
}

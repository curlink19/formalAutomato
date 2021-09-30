#include "finiteAutomaton.cpp"
#include <gtest/gtest.h>

class TestFiniteAutomaton: public ::testing::Test {
protected:
  finiteAutomaton<int, char>* foo;

  void SetUp() {
    foo = nullptr;
  }

  void TearDown() {
    if (foo != nullptr) { 
      delete foo;
    }
  }
};

TEST_F(TestFiniteAutomaton, testEpsFirst) {
  foo = new finiteAutomaton<int, char>(5, 0, std::vector<int>({2}));
  foo->insertEdge(0, 1, '.');
  foo->insertEdge(1, 2, '.');
  foo->insertEdge(0, 4, 'x');
  foo->insertEdge(4, 3, '.');
  foo->insertEdge(3, 2, 'y');
  auto result = foo->eraseZeroEdges('.');
  std::vector<std::pair<std::pair<int, int>, char>> d;
  std::vector<int> t = {0, 1, 2};
  d.push_back(std::make_pair(std::make_pair(0, 4), 'x'));
  d.push_back(std::make_pair(std::make_pair(3, 2), 'y'));
  d.push_back(std::make_pair(std::make_pair(4, 2), 'y'));
  ASSERT_EQ(result.getEdges(), d);
  ASSERT_EQ(result.getTerminals(), t);
}

TEST_F(TestFiniteAutomaton, testEpsSecond) {
  foo = new finiteAutomaton<int, char>(5, 0, std::vector<int>({2, 3}));
  foo->insertEdge(0, 1, 'x');
  foo->insertEdge(1, 2, 'y');
  foo->insertEdge(0, 3, '.');
  foo->insertEdge(3, 4, '.');
  foo->insertEdge(4, 2, 'z');
  auto result = foo->eraseZeroEdges('.');
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(std::make_pair(std::make_pair(0, 1), 'x'));
  d.push_back(std::make_pair(std::make_pair(0, 2), 'z'));
  d.push_back(std::make_pair(std::make_pair(1, 2), 'y'));
  d.push_back(std::make_pair(std::make_pair(3, 2), 'z'));
  d.push_back(std::make_pair(std::make_pair(4, 2), 'z'));
  std::vector<int> t = {0, 2, 3};
  ASSERT_EQ(result.getEdges(), d);
  ASSERT_EQ(result.getTerminals(), t);
}

TEST_F(TestFiniteAutomaton, testFullFirst) {
  foo = new finiteAutomaton<int, char>(2, 0, std::vector<int>({1}));
  foo->insertEdge(0, 1, 'a');
  auto result = foo->makeFull(std::vector<char>({'a', 'b'}));
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(std::make_pair(std::make_pair(0, 1), 'a'));
  d.push_back(std::make_pair(std::make_pair(0, 2), 'b'));
  d.push_back(std::make_pair(std::make_pair(1, 2), 'a'));
  d.push_back(std::make_pair(std::make_pair(1, 2), 'b'));
  d.push_back(std::make_pair(std::make_pair(2, 2), 'a'));
  d.push_back(std::make_pair(std::make_pair(2, 2), 'b'));
  std::vector<int> t = {1};
  ASSERT_EQ(result.getEdges(), d);
  ASSERT_EQ(result.getTerminals(), t);
}

TEST_F(TestFiniteAutomaton, testDetFirst) {
  foo = new finiteAutomaton<int, char>(3, 0, std::vector<int>({1}));
  foo->insertEdge(0, 1, 'x');
  foo->insertEdge(0, 2, 'x');
  foo->insertEdge(1, 2, 'y');
  foo->insertEdge(2, 1, 's');
  auto result = foo->determine();
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(std::make_pair(std::make_pair(0, 1), 'x'));
  d.push_back(std::make_pair(std::make_pair(1, 2), 'y'));
  d.push_back(std::make_pair(std::make_pair(1, 3), 's'));
  d.push_back(std::make_pair(std::make_pair(2, 3), 's'));
  d.push_back(std::make_pair(std::make_pair(3, 2), 'y'));
  std::vector<int> t = {1, 3};
  ASSERT_EQ(result.getEdges(), d);
  ASSERT_EQ(result.getTerminals(), t);
}

TEST_F(TestFiniteAutomaton, testNegFirst) {
  foo = new finiteAutomaton<int, char>(2, 0, std::vector<int>({0}));
  foo->insertEdge(0, 1, 'a');
  foo->insertEdge(1, 0, 'b');
  auto result = foo->makeFull(std::vector<char>({'a', 'b'}));
  result = result.negatate();
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(std::make_pair(std::make_pair(0, 1), 'a'));
  d.push_back(std::make_pair(std::make_pair(0, 2), 'b'));
  d.push_back(std::make_pair(std::make_pair(1, 0), 'b'));
  d.push_back(std::make_pair(std::make_pair(1, 2), 'a'));
  d.push_back(std::make_pair(std::make_pair(2, 2), 'a'));
  d.push_back(std::make_pair(std::make_pair(2, 2), 'b'));
  std::vector<int> t = {1, 2};
  assert(result.getEdges() == d);
  assert(result.getTerminals() == t);
}

TEST_F(TestFiniteAutomaton, testMinFirst) {
  foo = new finiteAutomaton<int, char>(4, 0, std::vector<int>({3}));
  foo->insertEdge(0, 1, 'a');
  foo->insertEdge(0, 2, 'b');
  foo->insertEdge(1, 3, 'b');
  foo->insertEdge(2, 3, 'a');
  foo->insertEdge(3, 0, '.');
  auto result = foo->eraseZeroEdges('.');
  result = result.determine();
  result = result.makeFull(std::vector<char>({'a', 'b'}));
  result = result.minimize();
  std::vector<std::pair<std::pair<int, int>, char>> d;
  d.push_back(std::make_pair(std::make_pair(0, 2), 'a'));
  d.push_back(std::make_pair(std::make_pair(0, 3), 'b'));
  d.push_back(std::make_pair(std::make_pair(1, 2), 'a'));
  d.push_back(std::make_pair(std::make_pair(1, 3), 'b'));
  d.push_back(std::make_pair(std::make_pair(2, 1), 'b'));
  d.push_back(std::make_pair(std::make_pair(2, 4), 'a'));
  d.push_back(std::make_pair(std::make_pair(3, 1), 'a'));
  d.push_back(std::make_pair(std::make_pair(3, 4), 'b'));
  d.push_back(std::make_pair(std::make_pair(4, 4), 'a'));
  d.push_back(std::make_pair(std::make_pair(4, 4), 'b'));
  std::vector<int> t = {1};
  assert(result.getEdges() == d);
  assert(result.getTerminals() == t);
}

int main(int args, char *argv[]) {
  ::testing::InitGoogleTest(&args, argv);
  return RUN_ALL_TESTS();
}

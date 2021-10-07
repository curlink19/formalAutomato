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

TEST_F(TestFiniteAutomaton, zeroEdgesTest_checkingTerminals) {
  foo = new finiteAutomaton<int, char>(6, 0, std::vector<int>({3, 4}));
  foo->insertEdge(0, 1, '.');
  foo->insertEdge(1, 2, 'x');
  foo->insertEdge(2, 3, '.');
  foo->insertEdge(1, 4, '.');
  foo->insertEdge(4, 5, 'y');
  foo->insertEdge(5, 3, 'z');
  auto answer = foo->eraseZeroEdges('.');
  ASSERT_EQ(answer.getHash(), "0>x>2,0>y>5,1>x>2,1>y>5,4>y>5,5>z>3|0,1,2,3,4");
}

TEST_F(TestFiniteAutomaton, zeroEdgesTest_checkingCycles) {
  foo = new finiteAutomaton<int, char>(7, 0, std::vector<int>({2, 6}));
  foo->insertEdge(0, 1, '.');
  foo->insertEdge(1, 2, '.');
  foo->insertEdge(2, 3, '.');
  foo->insertEdge(3, 4, '.');
  foo->insertEdge(4, 2, '.');
  foo->insertEdge(3, 5, 'x');
  foo->insertEdge(4, 6, 'y');
  auto answer = foo->eraseZeroEdges('.');
  ASSERT_EQ(answer.getHash(), "0>x>5,0>y>6,1>x>5,1>y>6,2>x>5,2>y>6,3>x>5,3>y>6,4>x>5,4>y>6|0,1,2,3,4,6");
}

TEST_F(TestFiniteAutomaton, determine_checkingSource) {
  foo = new finiteAutomaton<int, char>(4, 2, std::vector<int>({3}));
  foo->insertEdge(0, 1, 'x');
  foo->insertEdge(0, 2, 'x');
  foo->insertEdge(2, 3, 'p');
  auto answer = foo->determine();
  ASSERT_EQ(answer.getHash(), "0>p>1|1");
}

TEST_F(TestFiniteAutomaton, determine_checkingBigLineTest) {
  foo = new finiteAutomaton<int, char>(8, 0, std::vector<int>({7}));
  foo->insertEdge(0, 1, 'x');
  foo->insertEdge(0, 2, 'x');
  foo->insertEdge(2, 3, 'y');
  foo->insertEdge(2, 4, 'y');
  foo->insertEdge(3, 5, 'z');
  foo->insertEdge(3, 6, 'z');
  foo->insertEdge(5, 7, 'a');
  foo->insertEdge(6, 7, 'b');
  auto answer = foo->determine();
  ASSERT_EQ(answer.getHash(), "0>x>1,1>y>2,2>z>3,3>a>4,3>b>4|4");
}

TEST_F(TestFiniteAutomaton, determine_checkingBigCycleTest) {
  foo = new finiteAutomaton<int, char>(4, 0, std::vector<int>({2}));
  foo->insertEdge(0, 1, 'x');
  foo->insertEdge(0, 2, 'x');
  foo->insertEdge(1, 2, 'a');
  foo->insertEdge(2, 3, 'a');
  foo->insertEdge(1, 3, 'b');
  foo->insertEdge(3, 1, 'b');
  auto answer = foo->determine();
  ASSERT_EQ(answer.getHash(), "0>x>1,1>a>2,1>b>3,2>a>3,2>b>4,3>b>4,4>b>3,4>a>5,5>a>3|1,2,5");
}

TEST_F(TestFiniteAutomaton, makeFull_bigTest) {
 foo = new finiteAutomaton<int, char>(4, 0, std::vector<int>({3}));
 foo->insertEdge(0, 1, 'a');
 foo->insertEdge(0, 3, 'b');
 foo->insertEdge(1, 2, 'a');
 foo->insertEdge(1, 3, 'b');
 auto answer = foo->makeFull(std::vector<char>({'a', 'b'}));
 ASSERT_EQ(answer.getHash(), "0>a>1,0>b>3,1>a>2,1>b>3,2>a>4,2>b>4,3>a>4,3>b>4,4>a>4,4>b>4|3");
}

TEST_F(TestFiniteAutomaton, negatate_simpleTest) {
  foo = new finiteAutomaton<int, char>(2, 0, std::vector<int>({0}));
  foo->insertEdge(0, 1, 'a');
  foo->insertEdge(1, 0, 'a');
  auto answer = foo->negatate();
  ASSERT_EQ(answer.getHash(), "0>a>1,1>a>0|1");
}

TEST_F(TestFiniteAutomaton, minimize_simpleTest) {
  foo = new finiteAutomaton<int, char>(4, 0, std::vector<int>({3}));
  foo->insertEdge(0, 1, 'a');
  foo->insertEdge(0, 2, 'b');
  foo->insertEdge(1, 3, 'c');
  foo->insertEdge(2, 3, 'c');
  auto answer = foo->minimize();
  ASSERT_EQ(answer.getHash(), "0>a>2,0>b>2,2>c>1|1");
}

TEST_F(TestFiniteAutomaton, minimize_bigTest) {
  foo = new finiteAutomaton<int, char>(6, 0, std::vector<int>({2}));
  foo->insertEdge(0, 1, 'a');
  foo->insertEdge(1, 2, 'b');
  foo->insertEdge(0, 5, 'b');
  foo->insertEdge(5, 2, 'a');
  foo->insertEdge(0, 3, '.');
  foo->insertEdge(3, 2, '.');
  foo->insertEdge(3, 4, 'a');
  foo->insertEdge(3, 4, 'b');
  foo->insertEdge(4, 3, 'a');
  foo->insertEdge(4, 3, 'b');
  auto answer = foo->eraseZeroEdges('.');
  answer = answer.determine();
  answer = answer.minimize();
  ASSERT_EQ(answer.getHash(), "0>a>1,0>b>1,1>a>0,1>b>0|1");
}

TEST_F(TestFiniteAutomaton, determinator_getSubsetGraph) {
  using Tcort = std::vector<int>;
  foo = new finiteAutomaton<int, char>(1, 0, std::vector<int>({0}));
  finiteAutomaton_determinator<int, char> fooDeterminator(*foo);
  std::vector<Tcort> subsetsArray = {{1}, {1, 2}, {1, 2, 3}};
  std::vector<Tcort> terms = {{1, 2}};
  std::map<Tcort, std::vector<std::pair<Tcort, char>>> graph;
  graph[{1}] = {{{1, 2}, 'x'}};
  graph[{1, 2}] = {{{1, 2, 3}, 'y'}};
  ASSERT_EQ(fooDeterminator.getSubsetGraph(subsetsArray, terms, graph).getHash(), "0>x>1,1>y>2|1");
}

TEST_F(TestFiniteAutomaton, minimizer_getClassGraph) {
  foo = new finiteAutomaton<int, char>(4, 0, std::vector<int>({3}));
  foo->insertEdge(0, 1, 'a');
  foo->insertEdge(0, 2, 'b');
  foo->insertEdge(1, 3, 'x');
  foo->insertEdge(2, 3, 'x');
  finiteAutomaton_minimizer<int, char> fooMinimizer(*foo);
  ASSERT_EQ(fooMinimizer.getClassGraph(std::vector<int>({0, 1, 1, 2}), 3).getHash(), "0>a>1,0>b>1,1>x>2|2");
}

int main(int args, char *argv[]) {
  ::testing::InitGoogleTest(&args, argv);
  return RUN_ALL_TESTS();
}

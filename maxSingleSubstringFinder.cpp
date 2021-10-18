#include "finiteAutomatonArithmetic.cpp" 
#include <stack> 

class maxSingleSubstringFinder {
public: //must be private, public only for easy-testing
  finiteAutomaton<int, char> base;

  maxSingleSubstringFinder(std::string str): base(finiteAutomaton<int, char>(1, 0, std::vector<int>({0}))) {
    std::stack<finiteAutomaton<int, char>> elements;
    for (char letter: str) {
      if (letter == '*') {
        auto lastElement = elements.top();
        elements.pop();
        elements.push(closure<int, char>(lastElement));
        continue;
      }
      if (letter == '+') {
        auto secondTerm = elements.top();
        elements.pop();
        auto firstTerm = elements.top();
        elements.pop();
        elements.push(sum<int, char>(firstTerm, secondTerm));
        continue;
      }
      if (letter == '.') {
        auto secondTerm = elements.top();
        elements.pop();
        auto firstTerm = elements.top();
        elements.pop();
        elements.push(concatenation<int, char>(firstTerm, secondTerm));
        continue;
      }
      finiteAutomaton<int, char> current(2, 0, std::vector<int>({1}));
      current.insertEdge(0, 1, (letter == '1') ? defaultZeroLetter<char>() : letter);
      elements.push(current);
    }
    base = elements.top();
    elements.pop();
    assert(elements.empty());
    base = base.eraseZeroEdges(defaultZeroLetter<char>());
    base = base.makeFull(std::vector<char>({'a', 'b', 'c'}));
    base = base.determine();
    base = base.minimize();
  }

  void dfsFindPossibleStarts(int vertex, std::vector<bool>& isVertexUsed) {
    isVertexUsed[vertex] = true;
    for (auto adjacentEdgesIterator = base.getBegin(vertex); adjacentEdgesIterator.valid(); adjacentEdgesIterator.next()) {
      int adjacentVertex = adjacentEdgesIterator.getFinish();
      if (!isVertexUsed[adjacentVertex]) {
        dfsFindPossibleStarts(adjacentVertex, isVertexUsed);
      }
    }
  }

  int execute(char letter) {
    std::vector<bool> isVertexReachableFromSource(base.vertexCount(), false);
    dfsFindPossibleStarts(base.getSource(), isVertexReachableFromSource);
    std::vector<int> bestPathLength(base.vertexCount(), -1);
    for (auto vertex: base.getTerminals()) {
      bestPathLength[vertex] = 0;
    }
    int numberOfIterations = 2 * base.vertexCount() + 12;
    while (numberOfIterations--) {
      for (auto edge: base.getEdges()) {
        if (bestPathLength[edge.finish] == -1) {
          continue;
        }
        if (edge.letter == letter) {
          bestPathLength[edge.start] = std::max(bestPathLength[edge.start], bestPathLength[edge.finish] + 1);
        } else {
          bestPathLength[edge.start] = std::max(bestPathLength[edge.start], 0);
        }
      }
    }
    int answer = 0;
    for (int vertex = 0; vertex < base.vertexCount(); ++vertex) {
      if (isVertexReachableFromSource[vertex]) {
        answer = std::max(answer, bestPathLength[vertex]);
      }
    }
    if (answer > base.vertexCount()) {
      return -1;
    }
    return answer;
  }
};

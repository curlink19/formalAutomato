#include <string> 
#include <iostream>
#include <map> 
#include <vector> 
#include <functional> 
#include <cassert> 
#include <queue> 
#include <algorithm> 

template<typename Tletter>
std::vector<Tletter> defaultAlphabetLetters() {
  std::vector<Tletter> alphabetArray;
  for (char alphabetLetter = 'a'; alphabetLetter <= 'z'; ++alphabetLetter) {
    alphabetArray.push_back(static_cast<Tletter>(alphabetLetter));
  }
  return alphabetArray;
}

template<typename Tletter>
Tletter defaultZeroLetter() {
  return static_cast<Tletter>(0);
}

template<>
char defaultZeroLetter() {
  return '.';
}

template<typename Tvertex, typename Tletter>
class finiteAutomaton_determinator;

template<typename Tvertex, typename Tletter>
class finiteAutomaton_minimizer;

template<typename Tvertex, typename Tletter>
class finiteAutomaton {
public:
  class Edge {
  public:
    Tvertex start;
    Tvertex finish;
    Tletter letter;

    explicit Edge(Tvertex startVertex, Tvertex finishVertex, Tletter edgeLetter):
      start(startVertex),
      finish(finishVertex),
      letter(edgeLetter) {} 
    
    bool operator<(const Edge& anotherEdge) const {
      return std::make_pair(std::make_pair(start, finish), letter) <
             std::make_pair(std::make_pair(anotherEdge.start, anotherEdge.finish), anotherEdge.letter);
    }
  };

  std::vector<std::vector<Edge>> adjencyList_;
  std::vector<bool> isTerminal_;
  Tvertex source_;
  
  explicit finiteAutomaton(std::vector<std::vector<Edge>> sameAdjencyList, Tvertex sameSource, std::vector<bool> sameIsTerminal):
    adjencyList_(sameAdjencyList),
    isTerminal_(sameIsTerminal),
    source_(sameSource) {
      assert(sameIsTerminal.size() == sameAdjencyList.size());
    }

  explicit finiteAutomaton(size_t vertexCount, Tvertex sameSource, std::vector<bool> sameIsTerminal):
    adjencyList_(vertexCount),
    isTerminal_(sameIsTerminal),
    source_(sameSource) {
      assert(sameIsTerminal.size() == vertexCount);
    }

  explicit finiteAutomaton(size_t vertexCount, Tvertex sameSource, std::vector<Tvertex> listOfTerminals):
    adjencyList_(vertexCount),
    isTerminal_(vertexCount, false),
    source_(sameSource) {
      for (auto vertex: listOfTerminals) {
        isTerminal_[vertex] = true;
      }
    }
  
  size_t vertexCount() const {
    return adjencyList_.size();
  }

  Tvertex getSource() const {
    return source_;
  }

  class OutgoingEdgesIterator {
  private:
    finiteAutomaton& network_;
    size_t index_;
    Tvertex vertex_;
  
  public:
    explicit OutgoingEdgesIterator(finiteAutomaton& networkReference, size_t position, Tvertex vertex):
      network_(networkReference),
      index_(position),
      vertex_(vertex) {}

    bool valid() const {
      return (vertex_ >= 0) && 
             (static_cast<size_t>(vertex_) < network_.vertexCount()) &&
             (index_ < network_.adjencyList_[vertex_].size()); 
    }

    void next() {
      if (valid()) {
        ++index_;
      }
    }

    Edge getEdge() const {
      return network_.adjencyList_[vertex_][index_];
    }

    Tvertex getStart() const {
      return getEdge().start;
    }

    Tvertex getFinish() const {
      return getEdge().finish;
    }

    Tletter getLetter() const {
      return getEdge().letter;
    }
  };

  OutgoingEdgesIterator getBegin(Tvertex vertex) {
    return OutgoingEdgesIterator(*this, 0, vertex);
  }

  void insertEdge(Tvertex startVertex, Tvertex finishVertex, Tletter edgeLetter) {
    adjencyList_[startVertex].push_back(Edge(startVertex, finishVertex, edgeLetter));
  }

private:
  void addVertex() {
    adjencyList_.push_back({});
    isTerminal_.push_back(false);
  }

  void dfsZeroLetter(Tvertex vertex, Tvertex startVertex, std::vector<bool>& isVertexUsed, 
                     Tletter& zeroLetter, finiteAutomaton<Tvertex, Tletter>& answer) {
    isVertexUsed[vertex] = true;
    if (isTerminal_[vertex]) {
      answer.isTerminal_[startVertex] = true;
    } 
    for (auto adjacentEdgesIterator = getBegin(vertex); adjacentEdgesIterator.valid(); adjacentEdgesIterator.next()) {
      Tvertex adjacentVertex = adjacentEdgesIterator.getFinish();
      Tletter letter = adjacentEdgesIterator.getLetter();
      if (!isVertexUsed[adjacentVertex] && (letter == zeroLetter)) {
        dfsZeroLetter(adjacentVertex, startVertex, isVertexUsed, zeroLetter, answer);
      }
      if (letter != zeroLetter) {
        answer.insertEdge(startVertex, adjacentVertex, letter);
      }
    }
  };

public:  
  finiteAutomaton<Tvertex, Tletter> eraseZeroEdges(Tletter zeroLetter) {
    finiteAutomaton<Tvertex, Tletter> answer(vertexCount(), source_, isTerminal_);
    for (Tvertex vertex = 0; static_cast<size_t>(vertex) < vertexCount(); ++vertex) {
      std::vector<bool> isVertexUsed(vertexCount(), false);
      dfsZeroLetter(vertex, vertex, isVertexUsed, zeroLetter, answer);
    }
    return answer;
  }

  finiteAutomaton<Tvertex, Tletter> determine() { 
    finiteAutomaton_determinator<Tvertex, Tletter> algorithmInstance(*this);
    return algorithmInstance.execute();
  }

  void printAllTerminals() const {
    for (Tvertex vertex = 0; static_cast<size_t>(vertex) < vertexCount(); ++vertex) {
      if (isTerminal_[vertex]) {
        std::cout << vertex << " ";
      }
    }
    std::cout << "\n";
  }

  void print() {
    for (Tvertex vertex = 0; static_cast<size_t>(vertex) < vertexCount(); ++vertex) {
      std::cout << vertex << ": ";
      for (auto adjacentEdgesIterator = getBegin(vertex); adjacentEdgesIterator.valid(); adjacentEdgesIterator.next()) {
        std::cout << adjacentEdgesIterator.getFinish() << " " << adjacentEdgesIterator.getLetter() << " , ";
      }
      std::cout << "\n";
    }
    std::cout << "Terminals:\n";
    printAllTerminals();
  }

  std::vector<Edge> getEdges() {
    std::vector<Edge> listOfEdges;
    for (size_t vertex = 0; vertex < vertexCount(); ++vertex) {
      for (auto adjacentEdgesIterator = getBegin(vertex); adjacentEdgesIterator.valid(); adjacentEdgesIterator.next()) {
        auto currentEdge = Edge(adjacentEdgesIterator.getStart(), adjacentEdgesIterator.getFinish(), adjacentEdgesIterator.getLetter());
        listOfEdges.push_back(currentEdge);
      }
    }
    sort(listOfEdges.begin(), listOfEdges.end());
    return listOfEdges;
  }

  std::vector<Tvertex> getTerminals() {
    std::vector<Tvertex> listOfTerminals;
    for (size_t vertex = 0; vertex < vertexCount(); ++vertex) {
      if (isTerminal_[vertex]) {
        listOfTerminals.push_back(static_cast<Tvertex>(vertex));
      }
    }
    return listOfTerminals;
  }

  std::string getHash() {
    auto answerEdges = getEdges();
    auto answerTerminals = getTerminals();
    std::string hashString = "";
    for (auto edge: answerEdges) {
      if (!hashString.empty()) {
        hashString += ",";
      }
      hashString += std::to_string(edge.start);
      hashString += ">";
      hashString += std::string(1, edge.letter);
      hashString += ">";
      hashString += std::to_string(edge.finish);
    }
    hashString += "|";
    for (auto vertex: answerTerminals) {
      if (hashString.back() != '|') {
        hashString += ",";
      }
      hashString += std::to_string(vertex);
    }
    return hashString;
  }

  finiteAutomaton<Tvertex, Tletter> makeFull(std::vector<Tletter> alphabetLetters = defaultAlphabetLetters<Tletter>()) {
    finiteAutomaton<Tvertex, Tletter> answer(adjencyList_, source_, isTerminal_);
    answer.addVertex();
    for (size_t vertex = 0; vertex < answer.vertexCount(); ++vertex) {
      std::map<Tletter, bool> isLetterUsed;
      for (auto adjacentEdgesIterator = answer.getBegin(vertex); adjacentEdgesIterator.valid(); adjacentEdgesIterator.next()) {
        isLetterUsed[adjacentEdgesIterator.getLetter()] = true;
      }
      for (Tletter letter: alphabetLetters) {
        if (!isLetterUsed[letter]) {
          answer.insertEdge(static_cast<Tvertex>(vertex), static_cast<Tvertex>(vertexCount()), letter);
        }
      }
    }
    return answer;
  }

  finiteAutomaton<Tvertex, Tletter> negatate() {
    std::vector<bool> answerTerminal = isTerminal_;
    for (size_t vertex = 0; vertex < vertexCount(); ++vertex) {
      answerTerminal[vertex] = !isTerminal_[vertex];
    }
    return finiteAutomaton(adjencyList_, source_, answerTerminal);
  }

  finiteAutomaton<Tvertex, Tletter> minimize() {
    finiteAutomaton_minimizer<Tvertex, Tletter> algorithmInstance(*this);
    return algorithmInstance.execute();
  }

  class edgeWithStringAsLetter {
  public:
    std::string letter;
    int vertex;

    explicit edgeWithStringAsLetter(std::string sameLetter, int edgeVertex):
      letter(sameLetter),
      vertex(edgeVertex) {} 
  };

  std::string getExpression() {
    std::vector<std::vector<edgeWithStringAsLetter>> graph(vertexCount() + 1);
    for (size_t vertex = 0; vertex < vertexCount(); ++vertex) {
      for (auto adjacentEdgesIterator = getBegin(vertex); adjacentEdgesIterator.valid(); adjacentEdgesIterator.next()) {
        std::string currentEdgeLetter = std::string(1, static_cast<char>(adjacentEdgesIterator.getLetter()));
        int currentEdgeVertex = static_cast<int>(adjacentEdgesIterator.getFinish());
        graph[vertex].push_back(edgeWithStringAsLetter(currentEdgeLetter, currentEdgeVertex));
      }
      if (isTerminal_[vertex]) {
        graph[vertex].push_back(edgeWithStringAsLetter("", vertexCount()));
      }
    }
    std::vector<bool> isVertexDeleted(graph.size(), false);
    for (size_t vertex = 0; vertex + 1 < graph.size(); ++vertex) {
      if (static_cast<Tvertex>(vertex) == source_) {
        continue;
      }
      for (size_t adjacentVertex = 0; adjacentVertex < graph.size(); ++adjacentVertex) {
        if (isVertexDeleted[adjacentVertex] || (adjacentVertex == vertex)) { 
          continue;
        }
        for (auto adjacentEdgesIterator: graph[adjacentVertex]) {
          if (adjacentEdgesIterator.vertex == static_cast<int>(vertex)) {
            std::string firstPart = adjacentEdgesIterator.letter;
            std::string cyclePart = "";
            for (auto cycleEdgesIterator: graph[vertex]) {
              if (cycleEdgesIterator.vertex == static_cast<int>(vertex)) {
                if (cycleEdgesIterator.letter.empty()) {
                  continue;
                }
                if (!cyclePart.empty()) {
                  cyclePart += "+";
                }
                cyclePart += "(" + cycleEdgesIterator.letter + ")";
              }
            }
            for (auto lastPartEdgesIterator: graph[vertex]) {
              if (isVertexDeleted[lastPartEdgesIterator.vertex]) {
                continue;
              }
              std::string secondPart = lastPartEdgesIterator.letter;
              if (cyclePart.empty()) {
                graph[adjacentVertex].push_back(edgeWithStringAsLetter(firstPart + secondPart, lastPartEdgesIterator.vertex));
              } else {
                graph[adjacentVertex].push_back(edgeWithStringAsLetter(firstPart + "(" + cyclePart + ")*" + secondPart, lastPartEdgesIterator.vertex));
              }
            }
          }
        }
      }
      isVertexDeleted[vertex] = true;
    }
    int source = static_cast<int>(source_);
    int lastVertex= static_cast<int>(graph.size()) - 1;
    std::string sourceToLast = "";
    std::string lastToSource = "";
    std::string cycleSource = "";
    std::string cycleLast = "";
    for (auto adjacentEdgesIterator: graph[source]) {
      if (adjacentEdgesIterator.vertex == source) {
        if (adjacentEdgesIterator.letter.empty()) {
          continue;
        }
        if (!cycleSource.empty()) {
          cycleSource += "+";
        }
        if (adjacentEdgesIterator.letter.empty()) {
          cycleSource += "(@)";
        } else {
          cycleSource += "(" + adjacentEdgesIterator.letter +")";
        }
      } 
      if (adjacentEdgesIterator.vertex == lastVertex) {
        if (!sourceToLast.empty()) {
          sourceToLast += "+";
        }
        if (adjacentEdgesIterator.letter.empty()) {
          sourceToLast += "(@)";
        } else {
          sourceToLast += "(" + adjacentEdgesIterator.letter + ")";
        }
      }
    }
    for (auto adjacentEdgesIterator: graph[lastVertex]) {
      if (adjacentEdgesIterator.vertex == lastVertex) {
        if (adjacentEdgesIterator.letter.empty()) {
          continue;
        }
        if (!cycleLast.empty()) {
          cycleLast += "+";
        }
        if (adjacentEdgesIterator.letter.empty()) {
          cycleLast += "(@)";
        } else {
          cycleLast += "(" + adjacentEdgesIterator.letter + ")";
        }
      }
      if (adjacentEdgesIterator.vertex == source) {
        if (!lastToSource.empty()) {
          lastToSource += "+";
        }
        if (adjacentEdgesIterator.letter.empty()) {
          lastToSource += "(@)";
        } else {
          lastToSource += "(" + adjacentEdgesIterator.letter + ")";
        }
      }
    }
    std::string firstPart = "";
    firstPart += "(";
    if (!cycleSource.empty()) { 
      firstPart += "(" + cycleSource + ")*";
    }
    if (!sourceToLast.empty()) {
      firstPart += "(" + sourceToLast + ")";
    }
    if (!cycleLast.empty()) {
      firstPart += "(" + cycleLast + ")*";
    }
    if (!lastToSource.empty()) {
      firstPart += "(" + lastToSource + ")";
    }
    if (!cycleSource.empty()) {
      firstPart += "(" + cycleSource + ")*";
    }
    firstPart += ")*";
    if (firstPart.size() == 3) {
      firstPart = "";
    }
    if (!cycleSource.empty()) {
      firstPart += "(" + cycleSource + ")*";
    }
    std::string result = firstPart;
    if (!sourceToLast.empty()) {
      result += "(" + sourceToLast + ")";
    }
    if (!cycleLast.empty()) {
      result += "(" + cycleLast + ")*";
    }
    if (isTerminal_[source_] && !firstPart.empty()) {
      return firstPart + "+" + result;
    }
    return result;
  }

  friend finiteAutomaton_determinator<Tvertex, Tletter>;
};


template<typename Tvertex, typename Tletter>
class finiteAutomaton_determinator {
public: // Must be private, public only for easy-testing
  using TvertexSubset = std::vector<Tvertex>;
  
  class subsetsGraphEdge {
  public:
    TvertexSubset finish;
    Tletter letter;

    explicit subsetsGraphEdge(TvertexSubset adjacentVertex, Tletter sameLetter):
      finish(adjacentVertex),
      letter(sameLetter) {}

    bool operator<(const subsetsGraphEdge& anotherEdge) {
      return std::make_pair(finish, letter) < std::make_pair(anotherEdge.finish, anotherEdge.letter);
    }
  };

  std::map<TvertexSubset, std::vector<subsetsGraphEdge>> graph;
  std::map<TvertexSubset, bool> isVertexTagged;
  std::vector<TvertexSubset> arrayOfSubsets;
  std::vector<TvertexSubset> terminals;
  std::queue<TvertexSubset> subsetsQueue;

  finiteAutomaton<Tvertex, Tletter>& network_;

  explicit finiteAutomaton_determinator(finiteAutomaton<Tvertex, Tletter>& networkReference):
    network_(networkReference) {} 

  finiteAutomaton<Tvertex, Tletter> getSubsetGraph() {
    std::map<TvertexSubset, Tvertex> index;
    std::vector<Tvertex> indexOfTerminals;
    for (size_t position = 0; position < arrayOfSubsets.size(); ++position) {
      index[arrayOfSubsets[position]] = static_cast<Tvertex>(position);
    }
    for (size_t position = 0; position < terminals.size(); ++position) {
      indexOfTerminals.push_back(index[terminals[position]]);
    }
    finiteAutomaton<Tvertex, Tletter> answer(arrayOfSubsets.size(), index[arrayOfSubsets[0]], indexOfTerminals);
    for (auto adjacentEdgesIterator: graph) {
      for (auto edge: adjacentEdgesIterator.second) {
        answer.insertEdge(index[adjacentEdgesIterator.first], index[edge.finish], edge.letter);
      }
    }
    return answer;
  }

  void searchEdges(TvertexSubset& vertex, TvertexSubset& adjacentVertex, size_t position, std::vector<std::vector<bool>>& isIndexUsed, 
                   typename finiteAutomaton<Tvertex, Tletter>::OutgoingEdgesIterator& adjacentEdgesIterator) {
    for (size_t innerPostion = position; innerPostion < vertex.size(); ++innerPostion) {
      size_t innerCounter = 0;
      for (auto innerAdjacentEdgesIterator = network_.getBegin(vertex[innerPostion]); 
                innerAdjacentEdgesIterator.valid(); innerAdjacentEdgesIterator.next(), ++innerCounter) {
        if (isIndexUsed[innerPostion][innerCounter]) {
          continue;
        }
        if (adjacentEdgesIterator.getLetter() == innerAdjacentEdgesIterator.getLetter()) {
          isIndexUsed[innerPostion][innerCounter] = true;
          adjacentVertex.push_back(innerAdjacentEdgesIterator.getFinish());
          break;
        }
      }
    }
  }

  finiteAutomaton<Tvertex, Tletter> execute() {
    arrayOfSubsets.push_back({network_.source_});
    subsetsQueue.push(arrayOfSubsets[0]);
    while (!subsetsQueue.empty()) {
      TvertexSubset vertex = subsetsQueue.front();
      isVertexTagged[vertex] = true;
      subsetsQueue.pop();
      std::vector<std::vector<bool>> isIndexUsed(vertex.size());
      for (size_t position = 0; position < vertex.size(); ++position) {
        isIndexUsed[position].resize(network_.adjencyList_[vertex[position]].size(), false);
      }
      for (size_t position = 0; position < vertex.size(); ++position) {
        if (network_.isTerminal_[vertex[position]]) {
          terminals.push_back(vertex);
          break;
        }
      }
      for (size_t position = 0; position < vertex.size(); ++position) {
        size_t counter = 0;
        for (auto adjacentEdgesIterator = network_.getBegin(vertex[position]); 
                  adjacentEdgesIterator.valid(); adjacentEdgesIterator.next(), ++counter) {
          if (isIndexUsed[position][counter]) {
            continue;
          }
          isIndexUsed[position][counter] = true;
          TvertexSubset adjacentVertex = {adjacentEdgesIterator.getFinish()};
          searchEdges(vertex, adjacentVertex, position, isIndexUsed, adjacentEdgesIterator);
          sort(adjacentVertex.begin(), adjacentVertex.end());
          adjacentVertex.erase(std::unique(adjacentVertex.begin(), adjacentVertex.end()), adjacentVertex.end());
          graph[vertex].push_back(subsetsGraphEdge(adjacentVertex, adjacentEdgesIterator.getLetter()));
          if (!isVertexTagged[adjacentVertex]) {
            isVertexTagged[adjacentVertex] = true;
            arrayOfSubsets.push_back(adjacentVertex);
            subsetsQueue.push(adjacentVertex); 
          }
        }
      }
    }
    return getSubsetGraph();
  }

  friend finiteAutomaton<Tvertex, Tletter>;
};

template<typename Tvertex, typename Tletter>
class finiteAutomaton_minimizer {
public:// Must be private, public only for easy-testing
  finiteAutomaton<Tvertex, Tletter>& network_;

  explicit finiteAutomaton_minimizer(finiteAutomaton<Tvertex, Tletter>& networkReference):
    network_(networkReference) {} 

  class bidirectionalEdge {
  public:
    int start;
    int finish;
    Tletter letter;

    explicit bidirectionalEdge(int startVertex, int finishVertex, Tletter edgeLetter):
      start(startVertex),
      finish(finishVertex),
      letter(edgeLetter) {}

    bool operator<(const bidirectionalEdge& anotherEdge) const {
      return std::make_pair(std::make_pair(start, finish), letter) <
             std::make_pair(std::make_pair(anotherEdge.start, anotherEdge.finish), anotherEdge.letter);
    }
  };

  finiteAutomaton<Tvertex, Tletter> getClassesGraph(std::vector<int> classNumber, int currentClassNumber) {
    std::vector<bool> answerTerminal(currentClassNumber, false);
    for (size_t vertex = 0; vertex < classNumber.size(); ++vertex) {
      if (network_.isTerminal_[vertex]) {
        answerTerminal[classNumber[vertex]] = true;
      }
    }
    finiteAutomaton<Tvertex, Tletter> answer(currentClassNumber, classNumber[network_.source_], answerTerminal);
    std::map<bidirectionalEdge, bool> isEdgeUsed;
    for (size_t vertex = 0; vertex < classNumber.size(); ++vertex) {
      for (auto adjacentEdgesIterator = network_.getBegin(vertex); adjacentEdgesIterator.valid(); adjacentEdgesIterator.next()) {
        auto currentEdge = bidirectionalEdge(classNumber[vertex], classNumber[adjacentEdgesIterator.getFinish()], adjacentEdgesIterator.getLetter());
        if (isEdgeUsed[currentEdge]) {
          continue;
        }
        isEdgeUsed[currentEdge] = true;
        Tvertex startVertex = static_cast<Tvertex>(currentEdge.start);
        Tvertex finishVertex = static_cast<Tvertex>(currentEdge.finish);
        answer.insertEdge(startVertex, finishVertex, currentEdge.letter);
      }
    }
    return answer;
  }

  class monodirectionalEdge {
  public:
    Tletter letter;
    int vertex;

    explicit monodirectionalEdge(Tletter edgeLetter, int edgeVertex):
      letter(edgeLetter),
      vertex(edgeVertex) {}

    bool operator<(const monodirectionalEdge& anotherEdge) const {
      return std::make_pair(letter, vertex) < std::make_pair(anotherEdge.letter, anotherEdge.vertex);
    }

    bool operator==(const monodirectionalEdge& anotherEdge) const {
      return std::make_pair(letter, vertex) == std::make_pair(anotherEdge.letter, anotherEdge.vertex);
    }
  };

  finiteAutomaton<Tvertex, Tletter> execute() {
    std::vector<int> classNumber(network_.vertexCount(), 0);
    for (size_t vertex = 0; vertex < network_.vertexCount(); ++vertex) {
      if (network_.isTerminal_[vertex]) {
        classNumber[vertex] = 1;
      }
    }
    auto oldClassNumber = classNumber;
    int currentClassNumber = 2;
    int numberOfIterations = network_.vertexCount();
    while (numberOfIterations--) {
      std::vector<std::vector<monodirectionalEdge>> adjacentEdges(classNumber.size());
      for (size_t vertex = 0; vertex < classNumber.size(); ++vertex) {
        for (auto adjacentEdgesIterator = network_.getBegin(vertex); adjacentEdgesIterator.valid(); adjacentEdgesIterator.next()) {
          adjacentEdges[vertex].push_back(monodirectionalEdge(adjacentEdgesIterator.getLetter(), classNumber[adjacentEdgesIterator.getFinish()]));
        }
        std::sort(adjacentEdges[vertex].begin(), adjacentEdges[vertex].end());
        adjacentEdges[vertex].erase(std::unique(adjacentEdges[vertex].begin(), adjacentEdges[vertex].end()), adjacentEdges[vertex].end());
      }
      bool checkIteration = false;
      for (size_t vertex = 0; vertex < classNumber.size(); ++vertex) {
        bool isFinded = false;
        bool isConflict = false;
        for (size_t adjacentVertex = 0; adjacentVertex < vertex; ++adjacentVertex) {
          if ((adjacentEdges[adjacentVertex] == adjacentEdges[vertex]) && (oldClassNumber[adjacentVertex] == oldClassNumber[vertex])) {
            isFinded = true;
            classNumber[vertex] = classNumber[adjacentVertex];
          } else if (classNumber[adjacentVertex] == classNumber[vertex]) {
            isConflict = true;
          }
        }
        if (!isFinded && isConflict) {
          classNumber[vertex] = currentClassNumber++;
          checkIteration = true;
        }
      }
      if (!checkIteration) {
        break;
      }
      oldClassNumber = classNumber;
    }
    return getClassesGraph(classNumber, currentClassNumber);
  }

  friend finiteAutomaton<Tvertex, Tletter>;
};

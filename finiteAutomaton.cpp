#include <string> 
#include <iostream>
#include <map> 
#include <vector> 
#include <functional> 
#include <cassert> 
#include <queue> 
#include <algorithm> 

template<typename Tletter>
std::vector<Tletter> defaultEdgeValues() {
  std::vector<Tletter> alphabetArray;
  for (char alphabetLetter = 'a'; alphabetLetter <= 'z'; ++alphabetLetter) {
    alphabetArray.push_back(static_cast<Tletter>(alphabetLetter));
  }
  return alphabetArray;
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
  
  explicit finiteAutomaton(std::vector<std::vector<Edge>> adjacent_edge, Tvertex sourceVertex, std::vector<bool> isterm):
    adjencyList_(adjacent_edge),
    isTerminal_(isterm),
    source_(sourceVertex) {
      assert(isterm.size() == adjacent_edge.size());
    }

  explicit finiteAutomaton(size_t vertexCount, Tvertex sourceVertex, std::vector<bool> isterm):
    adjencyList_(vertexCount),
    isTerminal_(isterm),
    source_(sourceVertex) {
      assert(isterm.size() == vertexCount);
    }

  explicit finiteAutomaton(size_t vertexCount, Tvertex sourceVertex, std::vector<Tvertex> terms):
    adjencyList_(vertexCount),
    isTerminal_(vertexCount, false),
    source_(sourceVertex) {
      for (auto vertex: terms) {
        isTerminal_[vertex] = true;
      }
    }
  
  size_t vertexCount() const {
    return adjencyList_.size();
  } 

  class OutgoingEdgesIterator {
  private:
    finiteAutomaton& network_;
    size_t index_;
    Tvertex vertex_;
  
  public:
    explicit OutgoingEdgesIterator(finiteAutomaton& atm, size_t position, Tvertex vertex):
      network_(atm),
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

  void dfsZeroLetter(Tvertex vertex, Tvertex startPoint, std::vector<bool>& usedVertex, 
                     Tletter& zeroLetter, finiteAutomaton<Tvertex, Tletter>& answer) {
    usedVertex[vertex] = true;
    if (isTerminal_[vertex]) {
      answer.isTerminal_[startPoint] = true;
    } 
    for (auto edgesIterator = getBegin(vertex); edgesIterator.valid(); edgesIterator.next()) {
      Tvertex neighborVertex = edgesIterator.getFinish();
      Tletter letter = edgesIterator.getLetter();
      if (!usedVertex[neighborVertex] && (letter == zeroLetter)) {
        dfsZeroLetter(neighborVertex, startPoint, usedVertex, zeroLetter, answer);
      }
      if (letter != zeroLetter) {
        answer.insertEdge(startPoint, neighborVertex, letter);
      }
    }
  };

public:  
  finiteAutomaton<Tvertex, Tletter> eraseZeroEdges(Tletter zeroLetter) {
    finiteAutomaton<Tvertex, Tletter> answer(vertexCount(), source_, isTerminal_);
    for (Tvertex vertex = 0; static_cast<size_t>(vertex) < vertexCount(); ++vertex) {
      std::vector<bool> usedVertex(vertexCount(), false);
      dfsZeroLetter(vertex, vertex, usedVertex, zeroLetter, answer);
    }
    return answer;
  }

  finiteAutomaton<Tvertex, Tletter> determine() { 
    finiteAutomaton_determinator<Tvertex, Tletter> algo(*this);
    return algo.execute();
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
      for (auto edgesIterator = getBegin(vertex); edgesIterator.valid(); edgesIterator.next()) {
        std::cout << edgesIterator.getFinish() << " " << edgesIterator.getLetter() << " , ";
      }
      std::cout << "\n";
    }
    std::cout << "Terminals:\n";
    printAllTerminals();
  }

  std::vector<Edge> getEdges() {
    std::vector<Edge> result;
    for (size_t vertex = 0; vertex < vertexCount(); ++vertex) {
      for (auto edgesIterator = getBegin(vertex); edgesIterator.valid(); edgesIterator.next()) {
        auto current = Edge(edgesIterator.getStart(), edgesIterator.getFinish(), edgesIterator.getLetter());
        result.push_back(current);
      }
    }
    sort(result.begin(), result.end());
    return result;
  }

  std::vector<Tvertex> getTerminals() {
    std::vector<Tvertex> result;
    for (size_t vertex = 0; vertex < vertexCount(); ++vertex) {
      if (isTerminal_[vertex]) {
        result.push_back(static_cast<Tvertex>(vertex));
      }
    }
    return result;
  }

  std::string getHash() {
    auto answerEdges = getEdges();
    auto answerTerminals = getTerminals();
    std::string str = "";
    for (auto edge: answerEdges) {
      if (!str.empty()) {
        str += ",";
      }
      str += std::to_string(edge.start);
      str += ">";
      str += std::string(1, edge.letter);
      str += ">";
      str += std::to_string(edge.finish);
    }
    str += "|";
    for (auto vertex: answerTerminals) {
      if (str.back() != '|') {
        str += ",";
      }
      str += std::to_string(vertex);
    }
    return str;
  }

  finiteAutomaton<Tvertex, Tletter> makeFull(std::vector<Tletter> edgeValues = defaultEdgeValues<Tletter>()) {
    finiteAutomaton<Tvertex, Tletter> answer(adjencyList_, source_, isTerminal_);
    answer.addVertex();
    for (size_t vertex = 0; vertex < answer.vertexCount(); ++vertex) {
      std::map<Tletter, bool> usedEdge;
      for (auto edgesIterator = answer.getBegin(vertex); edgesIterator.valid(); edgesIterator.next()) {
        usedEdge[edgesIterator.getLetter()] = true;
      }
      for (Tletter letter: edgeValues) {
        if (!usedEdge[letter]) {
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
    finiteAutomaton_minimizer<Tvertex, Tletter> algo(*this);
    return algo.execute();
  }

  class edgeWord {
    std::string str;
    int vertex;

    edgeWord(std::string letter, int edgeVertex):
      str(letter),
      vertex(edgeVertex) {} 
  };

  std::string getExpression() {
    std::vector<std::vector<edgeWord>> graph(vertexCount() + 1);
    for (size_t vertex = 0; vertex < vertexCount(); ++vertex) {
      for (auto edgesIterator = getBegin(vertex); edgesIterator.valid(); edgesIterator.next()) {
        graph[vertex].push_back(edgeWord(std::string(1, static_cast<char>(edgesIterator.getLetter())), static_cast<int>(edgesIterator.getFinish())));
      }
      if (isTerminal_[vertex]) {
        graph[vertex].push_back(edgeWord("", vertexCount()));
      }
    }
    std::vector<bool> del(graph.size(), false);
    for (size_t vertex = 0; vertex + 1 < graph.size(); ++vertex) {
      if (static_cast<Tvertex>(vertex) == source_) {
        continue;
      }
      for (size_t neighborVertex = 0; neighborVertex < graph.size(); ++neighborVertex) {
        if (del[neighborVertex] || (neighborVertex == vertex)) { 
          continue;
        }
        for (auto edgesIterator: graph[neighborVertex]) {
          if (edgesIterator.vertex == static_cast<int>(vertex)) {
            std::string firstPart = edgesIterator.str;
            std::string cycle = "";
            for (auto itcycle: graph[vertex]) {
              if (itcycle.vertex == static_cast<int>(vertex)) {
                if (itcycle.str.empty()) {
                  continue;
                }
                if (!cycle.empty()) {
                  cycle += "+";
                }
                cycle += "(" + itcycle.str + ")";
              }
            }
            for (auto edgesIteratorOut: graph[vertex]) {
              if (del[edgesIteratorOut.vertex]) {
                continue;
              }
              std::string secondPart = edgesIteratorOut.str;
              if (cycle.empty()) {
                graph[neighborVertex].push_back(std::make_pair(firstPart + secondPart, edgesIteratorOut.vertex));
              } else {
                graph[neighborVertex].push_back(std::make_pair(firstPart + "(" + cycle + ")*" + secondPart, edgesIteratorOut.vertex));
              }
            }
          }
        }
      }
      del[vertex] = true;
    }
    int source = static_cast<int>(source_);
    int last = static_cast<int>(graph.size()) - 1;
    std::string sourceToLast = "";
    std::string lastToSource = "";
    std::string cycleSource = "";
    std::string cycleLast = "";
    for (auto edgesIterator: graph[source]) {
      if (edgesIterator.vertex == source) {
        if (edgesIterator.str.empty()) {
          continue;
        }
        if (!cycleSource.empty()) {
          cycleSource += "+";
        }
        if (edgesIterator.str.empty()) {
          cycleSource += "(@)";
        } else {
          cycleSource += "(" + edgesIterator.str +")";
        }
      } 
      if (edgesIterator.vertex == last) {
        if (!sourceToLast.empty()) {
          sourceToLast += "+";
        }
        if (edgesIterator.str.empty()) {
          sourceToLast += "(@)";
        } else {
          sourceToLast += "(" + edgesIterator.str + ")";
        }
      }
    }
    for (auto edgesIterator: graph[last]) {
      if (edgesIterator.vertex == last) {
        if (edgesIterator.str.empty()) {
          continue;
        }
        if (!cycleLast.empty()) {
          cycleLast += "+";
        }
        if (edgesIterator.str.empty()) {
          cycleLast += "(@)";
        } else {
          cycleLast += "(" + edgesIterator.str + ")";
        }
      }
      if (edgesIterator.vertex == source) {
        if (!lastToSource.empty()) {
          lastToSource += "+";
        }
        if (edgesIterator.str.empty()) {
          lastToSource += "(@)";
        } else {
          lastToSource += "(" + edgesIterator.str + ")";
        }
      }
    }
    std::string onepart = "";
    onepart += "(";
    if (!cycleSource.empty()) { 
      onepart += "(" + cycleSource + ")*";
    }
    if (!sourceToLast.empty()) {
      onepart += "(" + sourceToLast + ")";
    }
    if (!cycleLast.empty()) {
      onepart += "(" + cycleLast + ")*";
    }
    if (!lastToSource.empty()) {
      onepart += "(" + lastToSource + ")";
    }
    if (!cycleSource.empty()) {
      onepart += "(" + cycleSource + ")*";
    }
    onepart += ")*";
    if (onepart.size() == 3) {
      onepart = "";
    }
    if (!cycleSource.empty()) {
      onepart += "(" + cycleSource + ")*";
    }
    std::string result = onepart;
    if (!sourceToLast.empty()) {
      result += "(" + sourceToLast + ")";
    }
    if (!cycleLast.empty()) {
      result += "(" + cycleLast + ")*";
    }
    if (isTerminal_[source_] && !onepart.empty()) {
      return onepart + "+" + result;
    }
    return result;
  }

  friend finiteAutomaton_determinator<Tvertex, Tletter>;
};


template<typename Tvertex, typename Tletter>
class finiteAutomaton_determinator {
public: // Must be private, public only for easy-testing
  using TvertexSubset = std::vector<Tvertex>;
  
  struct subsetEdge {
    TvertexSubset finish;
    Tletter letter;

    subsetEdge(TvertexSubset neighborVertex, Tletter letter):
      finish(neighborVertex),
      letter(letter) {}

    bool operator<(const subsetEdge& anotherEdge) {
      return std::make_pair(finish, letter) < std::make_pair(anotherEdge.finish, anotherEdge.letter);
    }
  };

  std::map<TvertexSubset, std::vector<subsetEdge>> graph;
  std::map<TvertexSubset, bool> tagged;
  std::vector<TvertexSubset> subsetsArray;
  std::vector<TvertexSubset> terms;
  std::queue<TvertexSubset> subsetsQueue;

  finiteAutomaton<Tvertex, Tletter>& network_;

  finiteAutomaton_determinator(finiteAutomaton<Tvertex, Tletter>& net):
    network_(net) {} 

  finiteAutomaton<Tvertex, Tletter> getSubsetGraph() {
    std::map<TvertexSubset, Tvertex> index;
    std::vector<Tvertex> indexTerms;
    for (size_t position = 0; position < subsetsArray.size(); ++position) {
      index[subsetsArray[position]] = static_cast<Tvertex>(position);
    }
    for (size_t position = 0; position < terms.size(); ++position) {
      indexTerms.push_back(index[terms[position]]);
    }
    finiteAutomaton<Tvertex, Tletter> answer(subsetsArray.size(), index[subsetsArray[0]], indexTerms);
    for (auto edgesIterator: graph) {
      for (auto edgePair: edgesIterator.second) {
        answer.insertEdge(index[edgesIterator.first], index[edgePair.finish], edgePair.letter);
      }
    }
    return answer;
  }

  void searchEdges(TvertexSubset& vertex, TvertexSubset& neighborVertex, size_t position, std::vector<std::vector<bool>>& usedPosition, 
                   typename finiteAutomaton<Tvertex, Tletter>::OutgoingEdgesIterator& edgesIterator) {
    for (size_t newPositon = position; newPositon < vertex.size(); ++newPositon) {
      size_t newCounter = 0;
      for (auto newedgesIterator = network_.getBegin(vertex[newPositon]); newedgesIterator.valid(); newedgesIterator.next(), ++newCounter) {
        if (usedPosition[newPositon][newCounter]) {
          continue;
        }
        if (edgesIterator.getLetter() == newedgesIterator.getLetter()) {
          usedPosition[newPositon][newCounter] = true;
          neighborVertex.push_back(newedgesIterator.getFinish());
          break;
        }
      }
    }
  }

  finiteAutomaton<Tvertex, Tletter> execute() {
    subsetsArray.push_back({network_.source_});
    subsetsQueue.push(subsetsArray[0]);
    while (!subsetsQueue.empty()) {
      TvertexSubset vertex = subsetsQueue.front();
      tagged[vertex] = true;
      subsetsQueue.pop();
      std::vector<std::vector<bool>> usedPosition(vertex.size());
      for (size_t position = 0; position < vertex.size(); ++position) {
        usedPosition[position].resize(network_.adjencyList_[vertex[position]].size(), false);
      }
      for (size_t position = 0; position < vertex.size(); ++position) {
        if (network_.isTerminal_[vertex[position]]) {
          terms.push_back(vertex);
          break;
        }
      }
      for (size_t position = 0; position < vertex.size(); ++position) {
        size_t counter = 0;
        for (auto edgesIterator = network_.getBegin(vertex[position]); edgesIterator.valid(); edgesIterator.next(), ++counter) {
          if (usedPosition[position][counter]) {
            continue;
          }
          usedPosition[position][counter] = true;
          TvertexSubset neighborVertex = {edgesIterator.getFinish()};
          searchEdges(vertex, neighborVertex, position, usedPosition, edgesIterator);
          sort(neighborVertex.begin(), neighborVertex.end());
          neighborVertex.erase(std::unique(neighborVertex.begin(), neighborVertex.end()), neighborVertex.end());
          graph[vertex].push_back(subsetEdge(neighborVertex, edgesIterator.getLetter()));
          if (!tagged[neighborVertex]) {
            tagged[neighborVertex] = true;
            subsetsArray.push_back(neighborVertex);
            subsetsQueue.push(neighborVertex); 
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

  finiteAutomaton_minimizer(finiteAutomaton<Tvertex, Tletter>& net):
    network_(net) {} 

  class integerDoubleEdge {
  public:
    int start;
    int finish;
    Tletter letter;

    integerDoubleEdge(int startVertex, int finishVertex, Tletter edgeLetter):
      start(startVertex),
      finish(finishVertex),
      letter(edgeLetter) {}

    bool operator<(const integerDoubleEdge& anotherEdge) const {
      return std::make_pair(std::make_pair(start, finish), letter) <
             std::make_pair(std::make_pair(anotherEdge.start, anotherEdge.finish), anotherEdge.letter);
    }
  };

  finiteAutomaton<Tvertex, Tletter> getClassGraph(std::vector<int> classNumber, int currentClassNumber) {
    std::vector<bool> answerTerminal(currentClassNumber, false);
    for (size_t vertex = 0; vertex < classNumber.size(); ++vertex) {
      if (network_.isTerminal_[vertex]) {
        answerTerminal[classNumber[vertex]] = true;
      }
    }
    finiteAutomaton<Tvertex, Tletter> answer(currentClassNumber, classNumber[network_.source_], answerTerminal);
    std::map<integerDoubleEdge, bool> usedEdge;
    for (size_t vertex = 0; vertex < classNumber.size(); ++vertex) {
      for (auto edgesIterator = network_.getBegin(vertex); edgesIterator.valid(); edgesIterator.next()) {
        auto currentEdge = integerDoubleEdge(classNumber[vertex], classNumber[edgesIterator.getFinish()], edgesIterator.getLetter());
        if (usedEdge[currentEdge]) {
          continue;
        }
        usedEdge[currentEdge] = true;
        Tvertex startVertex = static_cast<Tvertex>(currentEdge.start);
        Tvertex finishVertex = static_cast<Tvertex>(currentEdge.finish);
        answer.insertEdge(startVertex, finishVertex, currentEdge.letter);
      }
    }
    return answer;
  }

  class integerMonoEdge {
  public:
    Tletter letter;
    int vertex;

    integerMonoEdge(Tletter edgeLetter, int edgeVertex):
      letter(edgeLetter),
      vertex(edgeVertex) {}

    bool operator<(const integerMonoEdge& anotherEdge) const {
      return std::make_pair(letter, vertex) < std::make_pair(anotherEdge.letter, anotherEdge.vertex);
    }

    bool operator==(const integerMonoEdge& anotherEdge) const {
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
      std::vector<std::vector<integerMonoEdge>> adjacent_edge(classNumber.size());
      for (size_t vertex = 0; vertex < classNumber.size(); ++vertex) {
        for (auto edgesIterator = network_.getBegin(vertex); edgesIterator.valid(); edgesIterator.next()) {
          adjacent_edge[vertex].push_back(integerMonoEdge(edgesIterator.getLetter(), classNumber[edgesIterator.getFinish()]));
        }
        std::sort(adjacent_edge[vertex].begin(), adjacent_edge[vertex].end());
        adjacent_edge[vertex].erase(std::unique(adjacent_edge[vertex].begin(), adjacent_edge[vertex].end()), adjacent_edge[vertex].end());
      }
      bool checkIteration = false;
      for (size_t vertex = 0; vertex < classNumber.size(); ++vertex) {
        bool isFinded = false;
        bool isConflict = false;
        for (size_t neighborVertex = 0; neighborVertex < vertex; ++neighborVertex) {
          if ((adjacent_edge[neighborVertex] == adjacent_edge[vertex]) && (oldClassNumber[neighborVertex] == oldClassNumber[vertex])) {
            isFinded = true;
            classNumber[vertex] = classNumber[neighborVertex];
          } else if (classNumber[neighborVertex] == classNumber[vertex]) {
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
    return getClassGraph(classNumber, currentClassNumber);
  }

  friend finiteAutomaton<Tvertex, Tletter>;
};

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
class finiteAutomaton {
public:
  class Edge {
  public:
    Tvertex start;
    Tvertex finish;
    Tletter value;

    explicit Edge(Tvertex startVertex, Tvertex finishVertex, Tletter edgeLetter):
      start(startVertex),
      finish(finishVertex),
      value(edgeLetter) {} 
    
    bool operator<(const Edge& anotherEdge) const {
      return std::make_pair(std::make_pair(start, finish), value) <
             std::make_pair(std::make_pair(anotherEdge.start, anotherEdge.finish), anotherEdge.value);
    }
  };

  std::vector<std::vector<Edge>> adjencyList_;
  std::vector<bool> isTerminal_;
  Tvertex source_;
  
  explicit finiteAutomaton(std::vector<std::vector<Edge>> adlist, Tvertex sourceVertex, std::vector<bool> isterm):
    adjencyList_(adlist),
    isTerminal_(isterm),
    source_(sourceVertex) {
      assert(isterm.size() == adlist.size());
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
      for (auto v: terms) {
        isTerminal_[v] = true;
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
    explicit OutgoingEdgesIterator(finiteAutomaton& atm, size_t position, Tvertex v):
      network_(atm),
      index_(position),
      vertex_(v) {}

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
      return getEdge().value;
    }
  };

  OutgoingEdgesIterator getBegin(Tvertex v) {
    return OutgoingEdgesIterator(*this, 0, v);
  }

  void insertEdge(Tvertex startVertex, Tvertex finishVertex, Tletter edgeLetter) {
    adjencyList_[startVertex].push_back(Edge(startVertex, finishVertex, edgeLetter));
  }

private:
  void addVertex() {
    adjencyList_.push_back({});
    isTerminal_.push_back(false);
  }

  void dfsZeroLetter(Tvertex u, Tvertex startPoint, std::vector<bool>& used, 
                     Tletter& zeroLetter, finiteAutomaton<Tvertex, Tletter>& answer) {
    used[u] = true;
    if (isTerminal_[u]) {
      answer.isTerminal_[startPoint] = true;
    } 
    for (auto it = getBegin(u); it.valid(); it.next()) {
      Tvertex v = it.getFinish();
      Tletter c = it.getLetter();
      if (!used[v] && (c == zeroLetter)) {
        dfsZeroLetter(v, startPoint, used, zeroLetter, answer);
      }
      if (c != zeroLetter) {
        answer.insertEdge(startPoint, v, c);
      }
    }
  };

public:  
  finiteAutomaton<Tvertex, Tletter> eraseZeroEdges(Tletter zeroLetter) {
    finiteAutomaton<Tvertex, Tletter> answer(vertexCount(), source_, isTerminal_);
    for (Tvertex v = 0; static_cast<size_t>(v) < vertexCount(); ++v) {
      std::vector<bool> used(vertexCount(), false);
      dfsZeroLetter(v, v, used, zeroLetter, answer);
    }
    return answer;
  }

private:
  using Tcort = std::vector<Tvertex>;

public:
  finiteAutomaton<Tvertex, Tletter> getSubsetGraph(std::vector<Tcort> subsetsArray, std::vector<Tcort> terms,
                                    std::vector<std::pair<Tcort, Tletter>> graph) {
    std::map<Tcort, Tvertex> index;
    std::vector<Tvertex> indexTerms;
    for (size_t i = 0; i < subsetsArray.size(); ++i) {
      index[subsetsArray[i]] = static_cast<Tvertex>(i);
    }
    for (size_t i = 0; i < terms.size(); ++i) {
      indexTerms.push_back(index[terms[i]]);
    }
    finiteAutomaton<Tvertex, Tletter> answer(subsetsArray.size(), index[subsetsArray[0]], indexTerms);
    for (auto it: graph) {
      for (auto p: it.second) {
        answer.insertEdge(index[it.first], index[p.first], p.second);
      }
    }
    return answer;
  }

  finiteAutomaton<Tvertex, Tletter> determine() { 
    std::map<Tcort, std::vector<std::pair<Tcort, Tletter>>> graph;
    std::map<Tcort, bool> tagged;
    std::vector<Tcort> subsetsArray;
    std::vector<Tcort> terms;
    subsetsArray.push_back({source_});
    std::queue<Tcort> q;
    q.push(subsetsArray[0]);
    while (!q.empty()) {
      Tcort v = q.front();
      tagged[v] = true;
      q.pop();
      std::vector<std::vector<bool>> adlist(v.size());
      for (size_t i = 0; i < v.size(); ++i) {
        adlist[i].resize(adjencyList_[v[i]].size(), false);
      }
      for (size_t i = 0; i < v.size(); ++i) {
        if (isTerminal_[v[i]]) {
          terms.push_back(v);
          break;
        }
      }
      for (size_t i = 0; i < v.size(); ++i) {
        size_t counter = 0;
        for (auto it = getBegin(v[i]); it.valid(); it.next(), ++counter) {
          if (adlist[i][counter]) {
            continue;
          }
          adlist[i][counter] = true;
          Tcort u = {it.getFinish()};
          for (size_t j = i; j < v.size(); ++j) {
            size_t new_counter = 0;
            for (auto new_it = getBegin(v[j]); new_it.valid(); new_it.next(), ++new_counter) {
              if (adlist[j][new_counter]) {
                continue;
              }
              if (it.getLetter() == new_it.getLetter()) {
                adlist[j][new_counter] = true;
                u.push_back(new_it.getFinish());
                break;
              }
            }
          }
          sort(u.begin(), u.end());
          u.erase(std::unique(u.begin(), u.end()), u.end());
          graph[v].push_back(std::make_pair(u, it.getLetter()));
          if (!tagged[u]) {
            tagged[u] = true;
            subsetsArray.push_back(u);
            q.push(u);
          }
        }
      }
    }
    return getSubsetGraph(subsetsArray, terms, graph);
  }

  void printAllTerminals() const {
    for (Tvertex v = 0; static_cast<size_t>(v) < vertexCount(); ++v) {
      if (isTerminal_[v]) {
        std::cout << v << " ";
      }
    }
    std::cout << "\n";
  }

  void print() {
    for (Tvertex v = 0; static_cast<size_t>(v) < vertexCount(); ++v) {
      std::cout << v << ": ";
      for (auto it = getBegin(v); it.valid(); it.next()) {
        std::cout << it.getFinish() << " " << it.getLetter() << " , ";
      }
      std::cout << "\n";
    }
    std::cout << "Terminals:\n";
    printAllTerminals();
  }

  std::vector<Edge> getEdges() {
    std::vector<Edge> result;
    for (size_t u = 0; u < vertexCount(); ++u) {
      for (auto it = getBegin(u); it.valid(); it.next()) {
        auto current = Edge(it.getStart(), it.getFinish(), it.getLetter());
        result.push_back(current);
      }
    }
    sort(result.begin(), result.end());
    return result;
  }

  std::vector<Tvertex> getTerminals() {
    std::vector<Tvertex> result;
    for (size_t v = 0; v < vertexCount(); ++v) {
      if (isTerminal_[v]) {
        result.push_back(static_cast<Tvertex>(v));
      }
    }
    return result;
  }

  std::string getHash() {
    auto answerEdges = getEdges();
    auto answerTerminals = getTerminals();
    std::string str = "";
    for (auto e: answerEdges) {
      if (!str.empty()) {
        str += ",";
      }
      str += std::to_string(e.start);
      str += ">";
      str += std::string(1, e.value);
      str += ">";
      str += std::to_string(e.finish);
    }
    str += "|";
    for (auto v: answerTerminals) {
      if (str.back() != '|') {
        str += ",";
      }
      str += std::to_string(v);
    }
    return str;
  }

  finiteAutomaton<Tvertex, Tletter> makeFull(std::vector<Tletter> edgeValues = defaultEdgeValues<Tletter>()) {
    finiteAutomaton<Tvertex, Tletter> answer(adjencyList_, source_, isTerminal_);
    answer.addVertex();
    for (size_t v = 0; v < answer.vertexCount(); ++v) {
      std::map<Tletter, bool> used;
      for (auto it = answer.getBegin(v); it.valid(); it.next()) {
        used[it.getLetter()] = true;
      }
      for (Tletter c: edgeValues) {
        if (!used[c]) {
          answer.insertEdge(static_cast<Tvertex>(v), static_cast<Tvertex>(vertexCount()), c);
        }
      }
    }
    return answer;
  }

  finiteAutomaton<Tvertex, Tletter> negatate() {
    std::vector<bool> answerTerminal = isTerminal_;
    for (size_t v = 0; v < vertexCount(); ++v) {
      answerTerminal[v] = !isTerminal_[v];
    }
    return finiteAutomaton(adjencyList_, source_, answerTerminal);
  }

  finiteAutomaton<Tvertex, Tletter> minimize() {
    std::vector<int> classNumber(vertexCount(), 0);
    for (size_t v = 0; v < vertexCount(); ++v) {
      if (isTerminal_[v]) {
        classNumber[v] = 1;
      }
    }
    auto oldClassNumber = classNumber;
    int currentClassNumber = 2;
    int numberOfIterations = vertexCount();
    while (numberOfIterations--) {
      std::vector<std::vector<std::pair<Tletter, int>>> used(classNumber.size());
      for (size_t v = 0; v < classNumber.size(); ++v) {
        for (auto it = getBegin(v); it.valid(); it.next()) {
          used[v].push_back(std::make_pair(it.getLetter(), classNumber[it.getFinish()]));
        }
        std::sort(used[v].begin(), used[v].end());
        used[v].erase(std::unique(used[v].begin(), used[v].end()), used[v].end());
      }
      bool checkIteration = false;
      for (size_t v = 0; v < classNumber.size(); ++v) {
        bool isFinded = false;
        bool isConflict = false;
        for (size_t u = 0; u < v; ++u) {
          if ((used[u] == used[v]) && (oldClassNumber[u] == oldClassNumber[v])) {
            isFinded = true;
            classNumber[v] = classNumber[u];
          } else if (classNumber[u] == classNumber[v]) {
            isConflict = true;
          }
        }
        if (!isFinded && isConflict) {
          classNumber[v] = currentClassNumber++;
          checkIteration = true;
        }
      }
      if (!checkIteration) {
        break;
      }
      oldClassNumber = classNumber;
    }
    std::vector<bool> answerTerminal(currentClassNumber, false);
    for (size_t v = 0; v < classNumber.size(); ++v) {
      if (isTerminal_[v]) {
        answerTerminal[classNumber[v]] = true;
      }
    }
    finiteAutomaton<Tvertex, Tletter> answer(currentClassNumber, classNumber[source_], answerTerminal);
    std::map<std::pair<std::pair<int, int>, Tletter>, bool> used;
    for (size_t v = 0; v < classNumber.size(); ++v) {
      for (auto it = getBegin(v); it.valid(); it.next()) {
        auto currentEdge = std::make_pair(std::make_pair(classNumber[v], classNumber[it.getFinish()]), it.getLetter());
        if (used[currentEdge]) {
          continue;
        }
        used[currentEdge] = true;
        Tvertex stU = static_cast<Tvertex>(currentEdge.first.first);
        Tvertex stV = static_cast<Tvertex>(currentEdge.first.second);
        answer.insertEdge(stU, stV, currentEdge.second);
      }
    }
    return answer;
  }

  std::string getExpression() {
    std::vector<std::vector<std::pair<std::string, int>>> graph(vertexCount() + 1);
    for (size_t v = 0; v < vertexCount(); ++v) {
      for (auto it = getBegin(v); it.valid(); it.next()) {
        graph[v].push_back(std::make_pair(std::string(1, static_cast<char>(it.getLetter())), static_cast<int>(it.getFinish())));
      }
      if (isTerminal_[v]) {
        graph[v].push_back(std::make_pair("", vertexCount()));
      }
    }
    std::vector<bool> del(graph.size(), false);
    for (size_t v = 0; v + 1 < graph.size(); ++v) {
      if (static_cast<Tvertex>(v) == source_) {
        continue;
      }
      for (size_t u = 0; u < graph.size(); ++u) {
        if (del[u] || (u == v)) { 
          continue;
        }
        for (auto itu: graph[u]) {
          if (itu.second == static_cast<int>(v)) {
            std::string firstPart = itu.first;
            std::string cycle = "";
            for (auto itcycle: graph[v]) {
              if (itcycle.second == static_cast<int>(v)) {
                if (itcycle.first.empty()) {
                  continue;
                }
                if (!cycle.empty()) {
                  cycle += "+";
                }
                cycle += "(" + itcycle.first + ")";
              }
            }
            for (auto ito: graph[v]) {
              if (del[ito.second]) {
                continue;
              }
              std::string secondPart = ito.first;
              if (cycle.empty()) {
                graph[u].push_back(std::make_pair(firstPart + secondPart, ito.second));
              } else {
                graph[u].push_back(std::make_pair(firstPart + "(" + cycle + ")*" + secondPart, ito.second));
              }
            }
          }
        }
      }
      del[v] = true;
    }
    int source = static_cast<int>(source_);
    int last = static_cast<int>(graph.size()) - 1;
    std::string sourceToLast = "";
    std::string lastToSource = "";
    std::string cycleSource = "";
    std::string cycleLast = "";
    for (auto it: graph[source]) {
      if (it.second == source) {
        if (it.first.empty()) {
          continue;
        }
        if (!cycleSource.empty()) {
          cycleSource += "+";
        }
        if (it.first.empty()) {
          cycleSource += "(@)";
        } else {
          cycleSource += "(" + it.first +")";
        }
      } 
      if (it.second == last) {
        if (!sourceToLast.empty()) {
          sourceToLast += "+";
        }
        if (it.first.empty()) {
          sourceToLast += "(@)";
        } else {
          sourceToLast += "(" + it.first + ")";
        }
      }
    }
    for (auto it: graph[last]) {
      if (it.second == last) {
        if (it.first.empty()) {
          continue;
        }
        if (!cycleLast.empty()) {
          cycleLast += "+";
        }
        if (it.first.empty()) {
          cycleLast += "(@)";
        } else {
          cycleLast += "(" + it.first + ")";
        }
      }
      if (it.second == source) {
        if (!lastToSource.empty()) {
          lastToSource += "+";
        }
        if (it.first.empty()) {
          lastToSource += "(@)";
        } else {
          lastToSource += "(" + it.first + ")";
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
};

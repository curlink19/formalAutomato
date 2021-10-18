#include "finiteAutomaton.cpp"

template<typename Tvertex, typename Tletter>
finiteAutomaton<Tvertex, Tletter> sum(finiteAutomaton<Tvertex, Tletter> firstTerm, 
                                      finiteAutomaton<Tvertex, Tletter> secondTerm,
                                      Tletter zeroLetter = defaultZeroLetter<Tletter>()) {
  Tvertex answerSource = static_cast<Tvertex>(0);
  Tvertex answerTerminal = static_cast<Tvertex>(1);
  finiteAutomaton<Tvertex, Tletter> answer(2 + firstTerm.vertexCount() + secondTerm.vertexCount(),
                                           answerSource, std::vector<Tvertex>({answerTerminal}));
  using TEdge = typename finiteAutomaton<Tvertex, Tletter>::Edge;
  std::vector<TEdge> firstTermEdges = firstTerm.getEdges();
  std::vector<TEdge> secondTermEdges = secondTerm.getEdges();
  std::vector<Tvertex> firstTermTerminals = firstTerm.getTerminals();
  std::vector<Tvertex> secondTermTerminals = secondTerm.getTerminals();
  Tvertex shiftFirstTerm = static_cast<Tvertex>(2);
  Tvertex shiftSecondTerm = static_cast<Tvertex>(firstTerm.vertexCount() + 2);
  answer.insertEdge(answerSource, firstTerm.getSource() + shiftFirstTerm, zeroLetter);
  answer.insertEdge(answerSource, secondTerm.getSource() + shiftSecondTerm, zeroLetter);
  for (auto edge: firstTermEdges) {
    answer.insertEdge(edge.start + shiftFirstTerm, edge.finish + shiftFirstTerm, edge.letter);
  }
  for (auto edge: secondTermEdges) {
    answer.insertEdge(edge.start + shiftSecondTerm, edge.finish + shiftSecondTerm, edge.letter);
  }
  for (auto terminalVertex: firstTermTerminals) {
    answer.insertEdge(terminalVertex + shiftFirstTerm, answerTerminal, zeroLetter);
  }
  for (auto terminalVertex: secondTermTerminals) {
    answer.insertEdge(terminalVertex + shiftSecondTerm, answerTerminal, zeroLetter);
  }
  return answer;
}

template<typename Tvertex, typename Tletter>
finiteAutomaton<Tvertex, Tletter> concatenation(finiteAutomaton<Tvertex, Tletter> firstTerm, 
                                      finiteAutomaton<Tvertex, Tletter> secondTerm,
                                      Tletter zeroLetter = defaultZeroLetter<Tletter>()) {
  Tvertex answerSource = static_cast<Tvertex>(0);
  Tvertex answerTerminal = static_cast<Tvertex>(1);
  finiteAutomaton<Tvertex, Tletter> answer(2 + firstTerm.vertexCount() + secondTerm.vertexCount(),
                                           answerSource, std::vector<Tvertex>({answerTerminal}));
  using TEdge = typename finiteAutomaton<Tvertex, Tletter>::Edge;
  std::vector<TEdge> firstTermEdges = firstTerm.getEdges();
  std::vector<TEdge> secondTermEdges = secondTerm.getEdges();
  std::vector<Tvertex> firstTermTerminals = firstTerm.getTerminals();
  std::vector<Tvertex> secondTermTerminals = secondTerm.getTerminals();
  Tvertex shiftFirstTerm = static_cast<Tvertex>(2);
  Tvertex shiftSecondTerm = static_cast<Tvertex>(firstTerm.vertexCount() + 2);
  answer.insertEdge(answerSource, firstTerm.getSource() + shiftFirstTerm, zeroLetter);
  for (auto edge: firstTermEdges) {
    answer.insertEdge(edge.start + shiftFirstTerm, edge.finish + shiftFirstTerm, edge.letter);
  }
  for (auto edge: secondTermEdges) {
    answer.insertEdge(edge.start + shiftSecondTerm, edge.finish + shiftSecondTerm, edge.letter);
  }
  for (auto terminalVertex: firstTermTerminals) {
    answer.insertEdge(terminalVertex + shiftFirstTerm, secondTerm.getSource() + shiftSecondTerm, zeroLetter);
  }
  for (auto terminalVertex: secondTermTerminals) {
    answer.insertEdge(terminalVertex + shiftSecondTerm, answerTerminal, zeroLetter);
  }
  return answer;
}

template<typename Tvertex, typename Tletter>
finiteAutomaton<Tvertex, Tletter> closure(finiteAutomaton<Tvertex, Tletter> base,
                                      Tletter zeroLetter = defaultZeroLetter<Tletter>()) {  
  Tvertex answerSource = static_cast<Tvertex>(0);
  Tvertex answerTerminal = static_cast<Tvertex>(1);
  finiteAutomaton<Tvertex, Tletter> answer(2 + base.vertexCount(), answerSource, std::vector<Tvertex>({answerTerminal}));
  using TEdge = typename finiteAutomaton<Tvertex, Tletter>::Edge;
  Tvertex shift = static_cast<Tvertex>(2);
  std::vector<TEdge> baseEdges = base.getEdges();
  std::vector<Tvertex> baseTerminals = base.getTerminals();
  answer.insertEdge(answerSource, base.getSource() + shift, zeroLetter);
  for (auto edge: baseEdges) {
    answer.insertEdge(edge.start + shift, edge.finish + shift, edge.letter);
  }
  for (auto terminalVertex: baseTerminals) {
    answer.insertEdge(terminalVertex + shift, answerTerminal, zeroLetter);
  }
  answer.insertEdge(answerTerminal, answerSource, zeroLetter);
  return answer;
}

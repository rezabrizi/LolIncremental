#include <vector>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

struct Node {

  Node (int id, std::vector<int> formula, int raw_value, char operation): id(id), formula(formula), raw_value(raw_value), operation(operation), is_na(false) {}

  void compute(const std::unordered_map<int, Node*>& nodes){
    int new_val = raw_value;

    for (int u: formula){
      if (nodes.at(u)->is_na){
        is_na = true;
        return;
      }

      if (operation == '+') {
        new_val += nodes.at(u)->value;
      } else if (operation == '-'){
        new_val -= nodes.at(u)->value;
      } else if (operation == '*'){
        new_val *= nodes.at(u)->value;
      } else if (operation == '/'){
        if (nodes.at(u)->value == 0){
          is_na = true;
          return;
        }
      }
    }
    value = new_val;
    std::cout << "node: " << id << "  value: " << value << std::endl;
  }

  int id;
  std::vector<int> formula;
  int raw_value;
  int value;
  char operation;
  bool is_na;
};

class Graph {

public:
  bool addNode(int id, std::vector<int> formula, int initialValue, char operation){
    // check node already exists
    if (nodes.find(id) != nodes.end()){
      return false;
    }

    // check if dependencies exist and the node is not itself a dependency
    for (const int u: formula){
      if (nodes.find(u) == nodes.end() || u == id){
        return false;
      }
    }

    // add the node to our graph
    for (const int u: formula){
      dependencies[u].insert(id);
    }

    nodes[id] = new Node(id, formula, initialValue, operation);
    nodes[id]->compute(nodes);
    return true;
  }

  void changeNodeFormula(int id, std::vector<int> newFormula, int initialValue){
    if (nodes.find(id) == nodes.end()){
      std::cout << "Node doesn't exist" << std::endl;
      return;
    }

    for (int u: newFormula){
      if (nodes.find(u) == nodes.end()){
        std::cout << "Dependency doesn't exist" << std::endl;
        return;
      }
      if (u == id){
        std::cout << "Self loop!" << std::endl;
        return;
      }
    }
    updateDependencies(id, newFormula);
    nodes[id]->raw_value = initialValue;
    if (!getNodeOrdering()){
      std::cout<< "There is a cycle! FIX THE FORMULA" << std::endl;
      return;  
    }

    for (int i: nodeOrdering){
      nodes[i]->compute(nodes);
    }
    std::cout << "Graph Updated!" << std::endl;
  }

private:

  void updateDependencies(int id, const std::vector<int>& newFormula){
    // remove old formula from the dependency
    std::unordered_set<int> oldDepenencies (nodes[id]->formula.begin(), nodes[id]->formula.end());
    for (const auto& u: oldDepenencies){
      dependencies[u].erase(id);
    }

    // add the new formula to the dependencies
    nodes[id]->formula = newFormula;
    for (const auto& u: newFormula){
      dependencies[u].insert(id);
    }
  }

  bool getNodeOrdering(){
    std::vector<int> newNodeOrdering;
    std::unordered_map<int, int> visited;

    for (const auto& it: nodes){
      if (visited.find(it.first) == visited.end() && topoSort(visited, it.first, newNodeOrdering)){
        return false;
      }
    }
    reverse(newNodeOrdering.begin(), newNodeOrdering.end());
    nodeOrdering = newNodeOrdering;
    return true;
  }

  bool topoSort(std::unordered_map<int, int>& visited, int source, std::vector<int>& newNodeOrdering){
    // 0, 1, 2 pattern
    // false means we are visiting (1)
    visited[source] = false;

    for (const auto& v: dependencies[source]){
      if (visited.find(v) != visited.end() && !visited[v]){
        return true;

      // v not being in visited means the node has not been visited yet (0)
      } else if (visited.find(v) == visited.end() && topoSort(visited, v, newNodeOrdering)){
        return true;
      }
    }
    // true means visited (2)
    visited[source] = true;
    newNodeOrdering.push_back(source);
    return false;
  }


private:
  std::unordered_map<int, std::unordered_set<int>> dependencies;
  std::unordered_map<int, Node*> nodes;
  std::vector<int> nodeOrdering;
};


int main(){
  Graph test;

  test.addNode(0, {}, 2, '+');
  test.addNode(1, {}, 3, '+');
  test.addNode(2, {0, 1}, 0, '+');

  test.changeNodeFormula(2, {1}, 0);
  return 0;
}

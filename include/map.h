#ifndef MAP_H
#define MAP_H

#include "sparsematrix.h"
#include <fstream>
#include <sstream>
#include <random>

// a position on the map
using Node = std::pair<int,int>;
// a path in the map
using Path = std::vector<std::pair<int,int>>;
// set moves
const std::vector<Node> moves = {Node(1,0),Node(0,1),Node(-1,0),Node(0,-1)};

class Map {
public:
  Map(int & n, int & m)
  : values(SparseMatrix<double>(n,m))
  {}

  Path A_star_path();

  inline int get_n(){
    return this->values.get_n();
  }

  inline int get_m(){
    return this->values.get_m();
  }

  inline double value(Node node){
    return values[node];
  }

  bool read_map(std::string file);
  bool write_map(std::string file);
  bool generate_map(std::vector<double> move_probs,double switching_prob = 0.1, bool non_intersecting = true);

  inline double operator() (int x, int y)
  {
    return values(x,y);
  }

  inline Node get_start(){
    return map_start;
  }

  inline Node get_end(){
    return map_end;
  }

  // distance functions for A*
  inline int A_star_h(Node node){
    return std::abs(node.first-this->map_start.first)+std::abs(node.second-this->map_start.second);
  }

  inline int A_star_d(Node u, Node v){
    return std::abs(u.first-v.first)+std::abs(u.second-v.second);
  }

  inline bool inbounds(Node node);
  inline bool possible_path_node(Node node);
  std::vector<Node> A_star_neighbours(Node node);

  inline int length() {return values.length();}

  // gets the map in sparse form, useful for efficiently drawing non-zero values
  inline std::map<std::pair<int,int>,double> get_non_zero() {return values.get_non_zero();}

  enum type {nothing,valid,start,end};
private:
  SparseMatrix<double> values;
  Node map_start;
  Node map_end;

  inline bool set_value(int x, int y, double value){values.set_value(x,y,value);}
};
#endif

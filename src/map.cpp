#include "map.h"
#include "constants.h"
#include <limits>
#include <algorithm>
#include <functional>
#include <chrono>

inline Node add(Node u, Node v){
  return Node(u.first+v.first,u.second+v.second);
}

Path reconstruct_path(std::map<Node,Node> cameFrom, Node current){
  Path path = {current};
  while (cameFrom.find(current) != cameFrom.end()){
    current = (cameFrom.find(current))->second;
    path.push_back(current);
  }
  return path;
}

// A* (non-iterative, CF pathfinder.cpp)
// https://en.wikipedia.org/wiki/A*_search_algorithm
Path Map::A_star_path(){

  std::vector<Node> open = {map_start};
  std::map<Node,Node> cameFrom;
  std::map<Node,double> gscore;
  gscore.insert(std::pair<Node,double>(map_start,0.));

  std::map<Node,double> fscore;
  fscore.insert(std::pair<Node,double>(map_start,0.));

  Node current;

  while (open.size() > 0){
    current = open.back();

    if (current == map_end){
      return reconstruct_path(cameFrom,current);
    }

    open.pop_back();

    std::vector<Node> N = A_star_neighbours(current);


    for (int i = 0; i < N.size(); i++){
      double t_score = gscore.find(current)->second + A_star_d(current,N[i]);

      if (gscore.find(N[i]) == gscore.end()){
        gscore.insert(std::pair<Node,double>(N[i],INF));
      }
      if (t_score < gscore.find(N[i])->second){
        cameFrom[N[i]] = current;
        gscore[N[i]] = t_score;
        fscore[N[i]] = gscore[N[i]] + A_star_h(N[i]);

        if (std::find(open.begin(),open.end(),N[i]) == open.end()){
          // this little struct is needed for the sorting on L110
          struct less {
            std::map<Node,double> scores;
            less(std::map<Node,double> s)
            : scores(s)
            {}
            bool operator()(Node u, Node v) {
              return scores[u] > scores[v];
            }
          };
          open.push_back(N[i]);
          std::sort(open.begin(),open.end(),less(fscore));
        }
      }
    }
  }
  return Path {map_start};
}

// the neighbours are nodes which we can access
// by taking one of the moves, being careful
// to check the neighbours are in bounds
std::vector<Node> Map::A_star_neighbours(Node node){
  std::vector<Node> N;

  for (int i = 0; i < moves.size(); i++){
    // proposal neighbour
    Node u = add(node,moves[i]);

    // check it is both within bounds ([0,n-1] x [0,m-1]) and the map value is a 1 not a 0
    if (Map::possible_path_node(u)){
      N.push_back(u);
    }
  }

  return N;
}

// check node is both within bounds ([0,n-1] x [0,m-1])
inline bool Map::inbounds(Node node){
  if (node.first >= 0 && node.second >= 0 && node.first < get_n() && node.second < get_m()){
    return true;
  }
  return false;
}

// check node's map value is a 1 not a 0 and it's inbounds
inline bool Map::possible_path_node(Node node){
  if (Map::inbounds(node) && Map::value(node) == Map::valid || Map::value(node) == Map::end)
    return true;

  return false;
}

// writes the map
// Specification:
/*

Given a map of size n=80 by m=60, with a start at [52,0] and an end at
(8,59) and 272 nodes in the path (not start or end) the file (truncated) is:
80,60
52,0
8,59
272
0,8,1
0,9,1
0,10,1
0,11,1
0,12,1
0,13,1
.
.
.

*/
bool Map::write_map(std::string filename){
  std::ofstream f;
  f.open(map_path+filename);
  if (f.is_open()){
    f << get_n() << ", " << get_m() << std::endl;
    f << map_start.first << ", " << map_start.second << std::endl;
    f << map_end.first << ", " << map_end.second << std::endl;
    std::map<std::pair<int,int>,double> p = values.get_non_zero();
    f << p.size() << std::endl;
    std::map<std::pair<int,int>,double>::iterator iter;
    for (iter = p.begin(); iter != p.end(); iter++){
      f << iter->first.first << ", " << iter->first.second << ", " << iter->second << std::endl;
    }
    f.close();
    return true;
  }
  else{
    std::cout << "@warn could not open file: " << map_path+filename;
    return false;
  }
}

// read a map file (see spec on L115)
bool Map::read_map(std::string file){
  values.clear();
  std::ifstream f;
  f.open(map_path+file);
  Node f_start, f_end;
  int l = -1;
  std::vector<std::pair<Node,double>> f_nodes;
  if (f.is_open()){
    std::string line;
    int f_n, f_m, x;
    int k = 0;
    while (std::getline(f,line)){
      std::replace(line.begin(), line.end(), ',', ' ');
      std::istringstream iss(line);
      if (k == 0){
        iss >> f_n >> f_m;
        if (f_n != get_n() || f_m != get_m()){
          std::cout << "Map file dimensions incorrect got: " << f_n << ", " << f_m;
          std::cout << "\nExpected: " << get_n() << ", " << get_m() << std::endl;
          return false;
        }
      }
      if (k == 1){
        iss >> f_n >> f_m;
        f_start = Node(f_n,f_m);
      }
      if (k == 2){
        iss >> f_n >> f_m;
        f_end = Node(f_n,f_m);
      }
      if (k == 3){
        iss >> l;
      }
      if (k > 3){
        iss >> f_n >> f_m >> x;
        f_nodes.push_back(std::pair<Node,double>(Node(f_n,f_m),x));
      }
      k += 1;
    }
    if (f_nodes.size() != l){
      std::cout << "Map file has missing entries, expected: " << l << " got: " << f_nodes.size() << std::endl;
      return false;
    }
    map_start = f_start;
    map_end = f_end;
    values.clear();
    for (int i = 0; i < l; i++){
      values.set_value(f_nodes[i].first.first,f_nodes[i].first.second,f_nodes[i].second);
    }
    return true;
  }
  else{
    std::cout << "unable to read map file: " << file << std::endl;
    return false;
  }
}

// stochastic process to generate maps at random:
// move from random start in (direction) dir (up-down-left-right)
// flip a coin of prob p to change direction (if heads change, tails keep current)
// false means accept intersecting paths
// move_probs allow for a bias in direction, e.g [1,1,1,5] means
// move 4 is heavily favoured above the others

// can fail a lot, but trials are quite fast
bool Map::generate_map(std::vector<double> move_probs,double switching_prob, bool non_intersecting){
  values.clear();
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);
  std::discrete_distribution<int> move_distribution(move_probs.begin(),move_probs.end());

  std::uniform_int_distribution<int> start_dist(0,get_m());
  int start = start_dist(generator);

  std::uniform_real_distribution<double> coin(0.0,1.0);

  std::vector<Node> new_map = {Node(0,start)};

  Node current_direction = moves[0];
  Node current_position = Node(0,start);
  for (int t = 0; t < 50000; t++){
    if (coin(generator) < switching_prob){
      // change dir
      current_direction = moves[move_distribution(generator)];
    }

    Node proposal = add(current_direction,current_position);
    if (Map::inbounds(proposal)){
      if (non_intersecting == false || (non_intersecting && std::find(new_map.begin(),new_map.end(),proposal) == new_map.end())) {
        new_map.push_back(proposal);
        current_position = proposal;
      }
    }

    if (current_position.first == get_n()-1){
      // valid map
      map_start = new_map[0];
      map_end = current_position;
      set_value(map_start.first,map_start.second,Map::start);
      set_value(map_end.first,map_end.second,Map::end);
      for (int i = 1; i < new_map.size()-1; i++){
        set_value(new_map[i].first,new_map[i].second,Map::valid);
      }
      return true;
    }
  }

  return false;
}

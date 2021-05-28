#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "map.h"
#include "constants.h"
#include <algorithm>

// a path finding agent
// could be abstracted so A* is a subclass
class PathFinder {
public:
  PathFinder(Map M)
  : map(M)
  {}

  Path get_path();
  Path get_next_path();
  inline bool in_seen(Node node) {
    if (std::find(seen.begin(),seen.end(),node) != seen.end()){
      return true;
    }
    return false;
  }
  inline std::vector<Node> get_seen() {return this->seen;}
  inline int n_seen() {return seen.size();}
  inline bool is_finished() {return finished;}
  inline int progress() {return map.A_star_d(current_path[0],map.get_end());}

  inline Map get_map() {return map;};

  void clear();
  void set_map(Map & new_map);
private:
  Map map;
  Path current_path;
  std::vector<Node> open;
  std::map<Node,Node> cameFrom;
  std::map<Node,double> gscore;
  std::map<Node,double> fscore;
  std::vector<Node> seen;

  bool finished = false;

  void A_star_iteration();
  Path reconstruct_path(std::map<Node,Node> cameFrom, Node current);
};

#endif

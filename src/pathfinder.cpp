#include "pathfinder.h"
#include <algorithm>
#include <functional>

// reset all the data!
void PathFinder::clear(){
  current_path.clear();
  open.clear();
  cameFrom.clear();
  gscore.clear();
  fscore.clear();
  seen.clear();

  finished = false;
}

void PathFinder::set_map(Map & new_map){
  map = new_map;
}

// scan back along the path to the start and then return it
Path PathFinder::reconstruct_path(std::map<Node,Node> cameFrom, Node current){
  Path path = {current};
  while (cameFrom.find(current) != cameFrom.end()){
    current = (cameFrom.find(current))->second;
    path.push_back(current);
  }
  return path;
}

Path PathFinder::get_path(){
  return this->current_path;
}

Path PathFinder::get_next_path(){
  A_star_iteration();
  return this->current_path;
}

// one iteration of A*
// https://en.wikipedia.org/wiki/A*_search_algorithm
void PathFinder::A_star_iteration(){
  Node start = map.get_start();
  Node end = map.get_end();
  gscore[start] = 0.0;
  fscore[start] = map.A_star_d(start,end);

  Node current = map.get_start();
  if (current_path.size() > 0){
      // if already started, continue
      current = current_path[0];
  }

  // handle case of 1st iteratio
  if (open.size() == 0){
    open.push_back(current);
  }

  current = open.back();

  if (current == map.get_end()){
    if (std::find(seen.begin(),seen.end(),current) == seen.end()){
      // make sure the end is part of the path!
      seen.push_back(current);
    }
    current_path = reconstruct_path(cameFrom,current);
    // we are done
    finished = true;
    return;
  }

  seen.push_back(current);

  open.pop_back();
  // scan the neighbours
  std::vector<Node> N = map.A_star_neighbours(current);
  for (int i = 0; i < N.size(); i++){
    if (std::find(seen.begin(),seen.end(),N[i]) != seen.end()){
      // no need to expand
      continue;
    }
    double t_score = gscore.find(current)->second + map.A_star_d(current,N[i]);

    // defualt values are INF
    if (gscore.find(N[i]) == gscore.end()){
      gscore.insert(std::pair<Node,double>(N[i],INF));
      fscore.insert(std::pair<Node,double>(N[i],INF));
    }
    if (t_score < gscore.find(N[i])->second){
      cameFrom[N[i]] = current;
      gscore[N[i]] = t_score;
      fscore[N[i]] = gscore[N[i]] + map.A_star_d(N[i],end);

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
        if (std::find(open.begin(),open.end(),N[i]) == open.end()){
          open.push_back(N[i]);
          std::sort(open.begin(),open.end(),less(fscore));
        }
      }
    }
  }

  current_path = reconstruct_path(cameFrom,current);
}

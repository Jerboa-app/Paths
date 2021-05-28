#include "display.h"
#include "pathfinder.h"

// helper to set colours to c
void set_quad_colour (sf::VertexArray & v, int start, sf::Color c){
  v[start].color = c;
  v[start+1].color = c;
  v[start+2].color = c;
  v[start+3].color = c;
}

// go through all quads and set their positions, colours, and dimensions
void setup_map(sf::VertexArray & map, Map & M, Path & p, float dx, float dy){
  // do need the double loop for initial setup
  int k = 0;
  for (int i = 0; i < M.get_n(); i++){
    for (int j = 0; j < M.get_m(); j++){
      map[k].position = sf::Vector2f(i*dx,(j+1)*dy);
      map[k+1].position = sf::Vector2f(i*dx,j*dy);
      map[k+2].position = sf::Vector2f((i+1)*dx,j*dy);
      map[k+3].position = sf::Vector2f((i+1)*dx,(j+1)*dy);
      if (M(i,j)==Map::valid){
        if (std::find(p.begin(),p.end(),Node(i,j)) != p.end()){
          set_quad_colour(map,k,sf::Color::Yellow);
        }
        else{
          set_quad_colour(map,k,sf::Color::White);
        }
      }
      else if (M(i,j)==Map::start){
        set_quad_colour(map,k,sf::Color::Green);
      }
      else if (M(i,j)==Map::end){
        set_quad_colour(map,k,sf::Color::Red);
      }
      else{
        set_quad_colour(map,k,sf::Color::Black);
      }
      k += 4;
    }
  }
}

// acutally draw the quads given a map and path
// will compute new colours for the quads
void render_map(sf::RenderWindow & window, Map & M, sf::VertexArray & map, Path & p){
  std::map<Node,double> non_zero = M.get_non_zero();
  std::map<Node,double>::iterator iter;
  int i,j;
  double v;
  int k = 0;
  for (int i = 0; i < M.get_n(); i++){
    for (int j = 0; j < M.get_m(); j++){
      if (M(i,j)==Map::valid){
        if (std::find(p.begin(),p.end(),Node(i,j)) != p.end()){
          set_quad_colour(map,k,sf::Color::Yellow);
        }
        else{
          set_quad_colour(map,k,sf::Color::White);
        }
      }
      else if (M(i,j)==Map::start){
        set_quad_colour(map,k,sf::Color::Green);
      }
      else if (M(i,j)==Map::end){
        set_quad_colour(map,k,sf::Color::Red);
      }
      else{
        set_quad_colour(map,k,sf::Color::Black);
      }
      k += 4;
    }
  }

  window.draw(map);
}

// same as above but for a PathFinder agent
// also a bit more efficient by only looping over the non_zero values
// of the map (0s will always be black so no need to set them again)
void render_map(sf::RenderWindow & window, Map & M, sf::VertexArray & map, PathFinder & agent){
  Path p = agent.get_path();
  std::map<Node,double> non_zero = M.get_non_zero();
  std::map<Node,double>::iterator iter;
  int i,j,k;
  double v;
  for (iter = non_zero.begin(); iter != non_zero.end(); iter++){
    i = iter->first.first;
    j = iter->first.second;
    v = iter->second;
    // flattened index
    k = 4*(M.get_m()*i+j);
    if (M(i,j)==Map::valid){
      if (std::find(p.begin(),p.end(),Node(i,j)) != p.end()){
        set_quad_colour(map,k,sf::Color::Yellow);
      }
      else if (agent.in_seen(Node(i,j)))
      {
        set_quad_colour(map,k,sf::Color::Blue);
      }
      else{
        set_quad_colour(map,k,sf::Color::White);
      }
    }
    else if (M(i,j)==Map::start){
      set_quad_colour(map,k,sf::Color::Green);
    }
    else if (M(i,j)==Map::end){
      set_quad_colour(map,k,sf::Color::Red);
    }
    else{
      set_quad_colour(map,k,sf::Color::Black);
    }
  }

  window.draw(map);
}

// short hand for rendering text
void render_text(sf::RenderWindow & window, std::string text, sf::Vector2f position, sf::Color colour, sf::Font font, int pixels){
  sf::Text t;
  t.setFont(font);
  t.setPosition(position);
  t.setColor(colour);
  t.setCharacterSize(pixels);
  t.setString(text);
  window.draw(t);
}

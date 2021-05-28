#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include "SFML/Window.hpp"
#include <SFML/Graphics.hpp>
#include "map.h"
#include "pathfinder.h"

void setup_map(sf::VertexArray & map, Map & M, Path & p, float dx, float dy);
void render_map(sf::RenderWindow & window, Map & M, sf::VertexArray & map, Path & p);
void render_map(sf::RenderWindow & window, Map & M, sf::VertexArray & map, PathFinder & agent);
void render_text(sf::RenderWindow & window, std::string text, sf::Vector2f position, sf::Color colour, sf::Font font, int pixels);
#endif

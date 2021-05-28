#include "main.h"
#include <iostream>
#include <fstream>
#include <string>

#include "SFML/Window.hpp"
#include <SFML/Graphics.hpp>

#include "constants.h"
#include "display.h"
#include "sound.h"


int main(int argc, char ** argv){

  // load the font
  sf::Font font;
  if (!font.loadFromFile("resources/fonts/sansation.ttf")){
      std::cout << "@warn could not load font\n";
  }
  // vector of sound buffers and sounds for audibilisation
  // this helps to make sure the code only attemps to play a
  // certain number of sounds (CF CONSTANTS_H)
  std::vector<sf::SoundBuffer> sound_buffers(max_sounds,sf::SoundBuffer());
  std::vector<sf::Sound> sounds(max_sounds,sf::Sound());

  // sounds are manually generated cosine forms (L34)
  std::vector<std::vector<sf::Int16>> tones;

  double domega = (max_tone-min_tone) / n_tones;
  for (int i = 0; i < n_tones; i++){
    // generate a tone with freqs in [min_tone,max_tone] see CONSTANTS_H
    // each last 0.1 seconds
    tones.push_back(generate_tone(min_tone+domega*i,0.1,sample_rate,bits));
  }

  int n = 96;
  int m = 54;
  double dx = 10;
  // hard code a reserved region for the text
  int reserved = 30;

  // map (a sparse matrix of dimension n x m)
  Map M(n,m);

  double prob = 0.2;
  int trials = 5000;
  int t = 0;
  std::vector<double> move_probs = {1,5,1,5};
  sf::Clock clock;
  clock.restart();

  // trial a stochastic process
  // move from random start in dir (up-down-left-right)
  // flip a coin of prob p to change dir
  // false means accept intersecting paths
  // move_probs allow for a bias in direction (CF MAP_H -> const std::vector<Node> moves = {Node(1,0),Node(0,1),Node(-1,0),Node(0,-1)};)
  while (t < trials){
    // not always going to succeed (hit right hand side)
    if (M.generate_map(move_probs,prob,false))
      break;

    t++;
  }

  if (t == trials){
    std::cout << "@warn did not succeed in " << t << " trials\n";
  }

  std::cout << "@info " << float(t+1) / float(clock.getElapsedTime().asSeconds())
            << " map trials per second\n";

  std::cout << "@info " << t+1 << " trials needed\n";

  Path p;

  sf::RenderWindow window(
    sf::VideoMode(960,540+reserved),
    "SFML",
    sf::Style::Default);
  window.setFramerateLimit(60);

  // quads to draw the sparse array quickly
  sf::VertexArray map_states(sf::Quads,4*n*m);

  // sets positions dimensions and initial colours for the quads
  setup_map(map_states,M,p,dx,dx);

  // the pathfinding algorithm (here just A*)
  PathFinder agent(M);
  Path q;

  // some stats to track
  double difficulty;
  double frac_explored = 0.0;
  double frac_in_path = 0.0;
  double elapsed = 0.0;

  // will use to hold the window once the path has been found
  sf::Clock timeout;
  bool hold = false;

  while (window.isOpen())
  {
      // Process events
      sf::Event event;
      while (window.pollEvent(event))
      {
          // Close window: exit
          if (event.type == sf::Event::Closed)
              window.close();
          // Escape key: exit
          if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
              window.close();
      }
      if (agent.is_finished() && hold && timeout.getElapsedTime().asSeconds() > 0.0){
        q = agent.get_path();
        difficulty = float(agent.n_seen())/float(q.size());
        int psuedo_uuid = clock.getElapsedTime().asMicroseconds() % 10; // will mean there are at most 10 maps saved for each difficulty
        std::string filename = std::to_string(difficulty)+"-"+std::to_string(psuedo_uuid)+".map";
        agent.get_map().write_map(filename);
        // if done and held window for 3 s or more generate the next map
        t = 0;
        while (t < trials){
          if (M.generate_map(move_probs,prob))
            break;

          t++;
        }

        if (t == trials){
          std::cout << "@warn did not succeed in " << t << " trials\n";
        }

        // update agent and quads
        agent.clear();
        agent.set_map(M);
        setup_map(map_states,M,q,dx,dx);

        hold = false;
        elapsed = 0.0;
      }

      if (agent.is_finished() && hold == false){
        // hold frame for 3 secs
        hold = true;
        timeout.restart();
      }

      window.clear(sf::Color::Black);

      clock.restart();
      // one iteration of the path finding algorithm
      q = agent.get_next_path();
      double runtime = clock.getElapsedTime().asSeconds();
      if (agent.is_finished() == false){
        // record how long it's taking
        elapsed += runtime;
      }

      if (agent.is_finished() == false){
        // if not finished we may play a tone
        // to audibilise how close the algorithm is
        int tone = n_tones - agent.progress();
        if (tone < 0){
          // clip the tone
          tone = 0;
        }

        if (tone >= n_tones){
          // clip the tone
          tone = n_tones-1;
        }
        int s = 0;
        // examine the sound queue
        while (s < max_sounds){
          if (sounds[s].getStatus() == sf::SoundSource::Stopped){
            // this sound is available, play using it
            sound_buffers[s].loadFromSamples(&tones[tone][0], tones[tone].size(), 1, sample_rate);
            sounds[s].setBuffer(sound_buffers[s]);
            sounds[s].play();
            break;
          }
          s++;
        }
      }

      // draw quads
      render_map(window,M,map_states,agent);

      // collect stats and render some information
      difficulty = float(agent.n_seen())/float(q.size());
      frac_explored = float(agent.n_seen())/float(M.length());
      frac_in_path = float(q.size())/float(M.length());

      render_text(window,"Explored: "+std::to_string(agent.n_seen())+" ("+std::to_string(int(std::floor(frac_explored*100)))+" %)",
        sf::Vector2f(10,(m-1)*dx+reserved/2.0),sf::Color::Blue,font,16);

      render_text(window,"Current Path Length: "+std::to_string(q.size())+" ("+std::to_string(int(std::floor(frac_in_path*100)))+" %)",
        sf::Vector2f(200,(m-1)*dx+reserved/2.0),sf::Color::Yellow,font,16);

      render_text(window,"CPU time: "+std::to_string(elapsed)+" s",
        sf::Vector2f(450,(m-1)*dx+reserved/2.0),sf::Color::White,font,16);

      // display!
      window.display();
  }

  return 0;
}

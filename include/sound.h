#ifndef SOUND_H
#define SOUND_H

#include <SFML/Audio.hpp>
std::vector<sf::Int16> generate_tone(float omega,float time, float sample_rate=44100.0, int bits=16);

#endif

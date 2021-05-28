#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits>

// maths
# define M_PI           3.14159265358979323846

// misc
const double INF = std::numeric_limits<double>::infinity();
const std::string map_path = "resources/maps/";

// sounds
const int max_sounds = 128;
const int n_tones = 128;
const int sample_rate = 44100;
const int bits = 16;
const double min_tone = 10.0;
const double max_tone = 100.0;

#endif

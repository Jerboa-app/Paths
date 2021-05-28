#include "sound.h"
#include "constants.h"

// based upon https://panthema.net/2013/sound-of-sorting/

double ADSR(size_t i, double duration, double a = 0.025, double d = 0.1, double s = 0.9, double r = 0.3){
    double x = double(i) / duration;
    if (x > 1.0) x = 1.0;
    if (x < a)
        return 1.0 / a * x;

    if (x < a + d)
        return 1.0 - (x - a) / d * (1.0 - s);

    if (x < 1.0 - r)
        return s;

    return s / r * (1.0 - x);
}

std::vector<sf::Int16> generate_tone(float omega,float time, float sample_rate, int bits){
  int n = int(time*sample_rate);
  int b = std::pow(2,bits-1)-1;
  std::vector<sf::Int16> samples(n,0);

  double dtheta = 2.0*M_PI / n;
  for (int i = 0; i < n; i++){
    samples[i] = sf::Int16(std::floor(ADSR(i,n)*std::cos(omega * i * dtheta) * b));
  }

  return samples;
}

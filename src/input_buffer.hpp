#ifndef INPUT_BUFFER_HPP
#define INPUT_BUFFER_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>

// change to use heap, then remove template.
template<int SIZE>
struct input_buffer_t {
  float d_r[SIZE];
  float d_i[SIZE];
  float d_power[SIZE];
  private:
  float han_window[SIZE]; // should make this const
  public:

  // initialize
  input_buffer_t() {
    for (int i=0; i<SIZE; ++i) {
      han_window[i] = 0.54 - 0.46 * cos(2*M_PI*i / (float)SIZE);
    }
  }

  // lowpass
  void do_lowpass() {
    // skip for now
  }

  // clear d_i
  void clear_d_i() {
    for (int i=0; i<SIZE; ++i) {
      d_i[i] = 0;
    }
  }

  // apply han window
  void apply_han_window() {
    for (int i=0; i<SIZE; ++i) {
      d_r[i] *= han_window[i];
    }
  }

  // calculate the power vector from the real and imaginary components
  void calculate_power() {
    for (int i=0; i<SIZE; ++i) {
      d_power[i] = d_r[i]*d_r[i] + d_i[i]*d_i[i];
    }
  }

  // find index of max power
  int get_max_power_index() {
    float max_power = 0;
    int max_power_index = 0;
    for (int i=0; i<SIZE; ++i) {
      if (d_power[i] > max_power) {
        max_power = d_power[i];
        max_power_index = i;
      }
    }
    return max_power_index;
  }
};

#endif


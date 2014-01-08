#ifndef INPUT_BUFFER_HPP
#define INPUT_BUFFER_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>

// change to use heap, then remove template.
class input_buffer_t {
  public:
  const int size;
  float* d_r;
  float* d_i;
  float* d_power;
  float* han_window; // should make this const

  // singleton, so don't allow these
  input_buffer_t(const input_buffer_t&);
  input_buffer_t& operator=(const input_buffer_t&);

  // initialize
  input_buffer_t(int p_size) :
           size(p_size), d_r(NULL), d_i(NULL), d_power(NULL), han_window(NULL) {
    d_r = new float[size];
    d_i = new float[size];
    d_power = new float[size];
    han_window = new float[size];

    for (int i=0; i<size; ++i) {
      han_window[i] = 0.54 - 0.46 * cos(2*M_PI*i / (float)size);
    }
  }

  ~input_buffer_t() {
    delete d_r;
    delete d_i;
    delete d_power;
    delete han_window;
  }

  // lowpass
  void do_lowpass() {
    // skip for now
  }

  // clear d_i
  void clear_d_i() {
    for (int i=0; i<size; ++i) {
      d_i[i] = 0;
    }
  }

  // apply han window
  void apply_han_window() {
    for (int i=0; i<size; ++i) {
      d_r[i] *= han_window[i];
    }
  }

  // calculate the power vector from the real and imaginary components
  void calculate_power() {
    for (int i=0; i<size; ++i) {
      d_power[i] = d_r[i]*d_r[i] + d_i[i]*d_i[i];
    }
  }

  // find index of max power
  int get_max_power_index() {
    float max_power = 0;
    int max_power_index = 0;
    for (int i=0; i<size; ++i) {
      if (d_power[i] > max_power) {
        max_power = d_power[i];
        max_power_index = i;
      }
    }
    return max_power_index;
  }
};

#endif


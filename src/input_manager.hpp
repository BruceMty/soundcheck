#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
//#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "portaudio_manager.hpp"
#include "fft_manager.hpp"
#include "input_buffer.hpp"

class input_manager_t {

  const int sample_rate;
  const int fft_exponent_size;
  const int sample_size;

  // singleton resources
  fft_manager_t fft_manager;
  input_buffer_t input_buffer;
  portaudio_manager_t portaudio_manager;

  public:
  input_manager_t() :
        sample_rate(8000),
        fft_exponent_size(13), // 2^13=8192
        sample_size(1<<fft_exponent_size),

        fft_manager(fft_exponent_size),
        input_buffer(sample_size),
        portaudio_manager(sample_rate, sample_size) {
  }

  // get frequency, later, should get &power
  int get_frequency() {
    
    // read sound data from input device
    portaudio_manager.read(input_buffer.d_r, sample_size);

    // clear the input_buffer.d_i portion
    input_buffer.clear_d_i();

    // apply lowpass filter (why?)
    input_buffer.do_lowpass();

    // apply han window (why?)
    input_buffer.apply_han_window();

    // perform the transform on d_r to obtain d_r and d_i
    fft_manager.apply_fft(input_buffer.d_r, input_buffer.d_i);

    // calculate the power vector
    input_buffer.calculate_power();

    // find the index of maximum power
    int max_index = input_buffer.get_max_power_index();

    // calculate the frequency from the index
    int frequency = (int)((sample_rate * max_index) / ((float)sample_size));

    return frequency;
  }
};

#endif

/*
void computeSecondOrderLowPassParameters( float srate, float f, float *a, float *b )
{
   float a0;
   float w0 = 2 * M_PI * f/srate;
   float cosw0 = cos(w0);
   float sinw0 = sin(w0);
   //float alpha = sinw0/2;
   float alpha = sinw0/2 * sqrt(2);

   a0   = 1 + alpha;
   a[0] = (-2*cosw0) / a0;
   a[1] = (1 - alpha) / a0;
   b[0] = ((1-cosw0)/2) / a0;
   b[1] = ( 1-cosw0) / a0;
   b[2] = b[0];
}
float processSecondOrderFilter( float x, float *mem, float *a, float *b )
{
    float ret = b[0] * x + b[1] * mem[0] + b[2] * mem[1]
                         - a[0] * mem[2] - a[1] * mem[3] ;

		mem[1] = mem[0];
		mem[0] = x;
		mem[3] = mem[2];
		mem[2] = ret;

		return ret;
}
void signalHandler( int signum ) { running = false; }
*/


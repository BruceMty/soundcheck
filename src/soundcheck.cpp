
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

// allow graceful closure
void close_handler(int s) {
  std::cout << "Closing, signal " << s << "\n";
  std::exit(1);
}

// main
int main( int argc, char **argv ) {

  const int SAMPLE_RATE = 8000;
  const int FFT_EXP_SIZE = 13; // 2^13=8192
  const int SIZE = 1<<FFT_EXP_SIZE;

  // initialize singleton resources
  fft_manager_t fft_manager(FFT_EXP_SIZE);
  input_buffer_t<SIZE> input_buffer();
  portaudio_manager_t portaudio_manager(SAMPLE_RATE, SIZE);

  // allow clean termination for CRTL-c
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = close_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // loop reading from input stream and displaying spectrogram
  while (true) {
    
    // read sound data from input device
    portaudio_manager.read(input_buffer.d_r, SIZE);

    // clear the input_buffer.d_i portion
    input_buffer.clear_d_i();

    // apply lowpass filter (why?)
    input_buffer.do_lowpass();

    // apply han window (why?)
    input_buffer.apply_han_window();

    // perform the transform on d_r to obtain d_r and d_i
    fft_manager.read(input_buffer.d_r, input_buffer.d_i);

    // calculate the power vector
    input_buffer.calculate_power();

    // find the index of maximum power
    int max_index = input_buffer.get_max_power_index();

    // calculate the frequency from the index
    int frequency = (int)((SAMPLE_RATE * max_index) / ((float)SIZE));

    // report the frequency
    std::cout << "dominant frequency: " << frequency << "\n";
  }
}

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


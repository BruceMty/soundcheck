
#include <cstdio>
#include <cstdlib>
#include <iostream>
//#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include "libfft.h"
#include "portaudio.h"

// constants
static const int SAMPLE_RATE = 8000;
static const int FFT_EXP_SIZE = 13; // 2^13=8192
static const int FFT_SIZE = 1<<FFT_EXP_SIZE;

struct sound_data_t {
  float d_r[FFT_SIZE];
  float d_i[FFT_SIZE];
  float d_power[FFT_SIZE];
  private:
  float han_window[FFT_SIZE]; // should make this const
  public:

  // initialize
  sound_data_t() {
    for (int i=0; i<FFT_SIZE; ++i) {
      han_window[i] = 0.54 - 0.46 * cos(2*M_PI*i / (float)FFT_SIZE);
    }
  }

  // lowpass
  void do_lowpass() {
    // skip for now
  }

  // clear d_i
  void clear_d_i() {
    for (int i=0; i<FFT_SIZE; ++i) {
      d_i[i] = 0;
    }
  }

  // apply han window
  void apply_han_window() {
    for (int i=0; i<FFT_SIZE; ++i) {
      d_r[i] *= han_window[i];
    }
  }

  // calculate the power vector from the real and imaginary components
  void calculate_power() {
    for (int i=0; i<FFT_SIZE; ++i) {
      d_power[i] = d_r[i]*d_r[i] + d_i[i]*d_i[i];
    }
  }

  // find index of max power
  int get_max_power_index() {
    float max_power = 0;
    int max_power_index = 0;
    for (int i=0; i<FFT_SIZE; ++i) {
      if (d_power[i] > max_power) {
        max_power = d_power[i];
        max_power_index = i;
      }
    }
    return max_power_index;
  }
};

// variables for main
static PaStreamParameters input_stream_parameters;
static PaStream* input_stream;
static PaError pa_error;
static sound_data_t sound_data;
static void* fft = NULL;

// allow graceful closure
void close_resources() {
  if (input_stream) {
    Pa_AbortStream(input_stream);
    Pa_CloseStream(input_stream);
  }
  destroyfft(fft);
  Pa_Terminate();
}
void close_handler(int s) {
  std::cout << "Closing, signal " << s << "\n";
  close_resources();
  std::exit(1);
}

// main
int main( int argc, char **argv ) {
std::cout << "fft size " << FFT_SIZE << "\n";

  // initialize FFT
  fft = initfft( FFT_EXP_SIZE );

  // initialize portaudio to stream from the default audio device
  pa_error = Pa_Initialize();
  if (pa_error != 0) {
    std::cout << "failure initializing portaudio " << pa_error << " \n";
    std::exit(1);
  }
  input_stream_parameters.device = Pa_GetDefaultInputDevice();
  if (input_stream_parameters.device == paNoDevice) {
    std::cerr << "No device.  Aborting.\n";
    exit(1);
  }
  input_stream_parameters.channelCount = 1;
  input_stream_parameters.sampleFormat = paFloat32;
  input_stream_parameters.suggestedLatency =
     Pa_GetDeviceInfo(input_stream_parameters.device)->defaultHighInputLatency;
  input_stream_parameters.hostApiSpecificStreamInfo = NULL;

  std::cout << "Opening input "
            << Pa_GetDeviceInfo(input_stream_parameters.device)->name
            << "\n";

  pa_error = Pa_OpenStream(&input_stream,
                           &input_stream_parameters,
                           NULL,
                           SAMPLE_RATE,
                           FFT_SIZE,
                           paClipOff, // ?
                           NULL,
                           NULL);
  if (pa_error != 0) {
    std::cout << "failure opening input stream " << pa_error << " \n";
    std::exit(1);
  }

  // start the input stream
  pa_error = Pa_StartStream(input_stream);
  if (pa_error != 0) {
    std::cout << "failure starting input stream " << pa_error << " \n";
    std::exit(1);
  }

  // allow clean termination for CRTL-c
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = close_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // loop reading from input stream and displaying spectrogram
  while (true) {
    
    // read sound data from input device
    pa_error = Pa_ReadStream(input_stream, sound_data.d_r, FFT_SIZE);
    if (pa_error != 0) {
      std::cout << "failure reading from input stream " << pa_error << " \n";
      std::exit(1);
    }

    // clear the sound_data.d_i portion
    sound_data.clear_d_i();

    // apply lowpass filter (why?)
    sound_data.do_lowpass();

    // apply han window (why?)
    sound_data.apply_han_window();

    // perform the transform on d_r to obtain d_r and d_i
    applyfft(fft, sound_data.d_r, sound_data.d_i, false);

    // calculate the power vector
    sound_data.calculate_power();

    // find the index of maximum power
    int max_index = sound_data.get_max_power_index();

    // calculate the frequency from the index
    int frequency = (int)((SAMPLE_RATE * max_index) / ((float)FFT_SIZE));

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


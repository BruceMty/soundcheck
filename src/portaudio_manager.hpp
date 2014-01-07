#ifndef PORTAUDIO_MANAGER_HPP
#define PORTAUDIO_MANAGER_HPP

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include "portaudio.h"

class portaudio_manager_t {
  private:
  const int sample_rate;
  const int size;

  PaStreamParameters input_stream_parameters;
  PaStream* input_stream;

  public:
  portaudio_manager_t(int p_sample_rate, int p_size) :
          sample_rate(p_sample_rate), size(p_size),
          input_stream_parameters(), input_stream(NULL) {

    // initialize portaudio to stream from the default audio device
    PaError pa_error;
    pa_error = Pa_Initialize();
    if (pa_error != 0) {
      std::ostringstream ss;
      ss << "failure initializing portaudio " << pa_error;
      throw std::runtime_error(ss.str());
    }
    input_stream_parameters.device = Pa_GetDefaultInputDevice();
    if (input_stream_parameters.device == paNoDevice) {
      throw std::runtime_error("No device.  Cannot continue.");
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
                             sample_rate,
                             size,
                             paClipOff, // ?
                             NULL,
                             NULL);
    if (pa_error != 0) {
      std::ostringstream ss2;
      ss2 << "failure opening input stream " << pa_error;
      throw std::runtime_error(ss2.str());
    }

    // start the input stream
    pa_error = Pa_StartStream(input_stream);
    if (pa_error != 0) {
      std::ostringstream ss3;
      ss3 << "failure starting input stream " << pa_error;
      throw std::runtime_error(ss3.str());
    }
  }

  void read(float* data, int count) {
    PaError pa_error;
    pa_error = Pa_ReadStream(input_stream, data, count);
    if (pa_error != 0) {
      std::ostringstream ss;
      ss << "failure reading from input stream " << pa_error;
      throw std::runtime_error(ss.str());
    }
  }

  ~portaudio_manager_t() {
    if (input_stream) {
      Pa_AbortStream(input_stream);
      Pa_CloseStream(input_stream);
    }
    Pa_Terminate();
  }

  // this manager is a singleton
// sorry, no c++11 for this 
//  portaudio_manager_t(const portaudio_manager_t&) = delete;
//  portaudio_manager_t& operator=(const portaudio_manager_t&) = delete;

  private:
  portaudio_manager_t(const portaudio_manager_t&);
  portaudio_manager_t& operator=(const portaudio_manager_t&);
};

#endif


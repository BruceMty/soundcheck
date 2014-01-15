#include <input_manager.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
//#include <stdio.h>
#include <signal.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include "portaudio_manager.hpp"
//#include "fft_manager.hpp"
//#include "input_buffer.hpp"

// allow graceful closure
void close_handler(int s) {
  std::cout << "Closing, signal " << s << "\n";
  std::exit(1);
}

// main
int main( int argc, char **argv ) {

  // allow clean termination for CRTL-c
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = close_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  // create the input manager
  input_manager_t input_manager;

  // loop reading from input stream and displaying spectrogram
  while (true) {
    int frequency = input_manager.get_frequency();
    
    // report the frequency
    std::cout << "dominant frequency: " << frequency << "\n";
  }
}


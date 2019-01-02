//*****************************************//
//  midiout.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI output.
//
//*****************************************//

#include <iostream>
#include <cstdlib>
#include "RtMidi.h"
#include "pigpio.h"

// Platform-dependent sleep routines.
#if defined(WIN32)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidiOut *rtmidi );
RtMidiOut *setupRtmidi();

#define LOW   0
#define HIGH  1

#define SOUND_ONE 1
#define SOUND_TWO 2

#define MIDI

class midi_state {
  public:
    int cur_state;
    int last_msg_sent;
 };

void button_callback1(int gpio, int level, uint32_t tick, void *user) {
  midi_state *state = (midi_state *) user;
  if (level == HIGH) {
    switch(state->last_msg_sent) {
      case SOUND_ONE:
        state->cur_state = 2;
        break;
      case SOUND_TWO:
        state->cur_state = 1;
        break;
    }
  }
}


int main( void )
{
  std::vector<unsigned char> message;
  std::vector<unsigned char> message1;
  std::vector<unsigned char> message2;
  RtMidiOut *midiout = setupRtmidi();
  if (midiout == NULL)
    exit(EXIT_FAILURE);
  if (gpioInitialise() == PI_INIT_FAILED)
    exit(EXIT_FAILURE);


  // GPIO test
  gpioSetMode(4, PI_INPUT);
  gpioSetMode(17, PI_OUTPUT);

  // Program change: 192, 5
  message.push_back( 192 );
  message.push_back( 5 );
  midiout->sendMessage( &message );

  SLEEP( 500 );

  // Note On: 144, 64, 90
  message[0] = 144;
  message[1] = 64;
  //message[2] = 90;
  message.push_back( 90 );
  midiout->sendMessage( &message );

  SLEEP( 500 );

  // Note Off: 128, 64, 40
  message[0] = 128;
  message[1] = 64;
  message[2] = 40;
  midiout->sendMessage( &message );

  midi_state *state = new midi_state();
  state->cur_state = 0;
  state->last_msg_sent = SOUND_ONE;
  gpioSetAlertFuncEx(4, button_callback1, state);

  message1.push_back(192);
  message1.push_back(5);

  message2.push_back(192);
  message2.push_back(57);

  while(1) {
    switch(state->cur_state) {
      case 0:
        usleep(200);
        break;
      case 1:
        // Send message 1
        state->last_msg_sent = SOUND_ONE; //state->cur_state;
        state->cur_state = 0;
        midiout->sendMessage(&message1);
        break;
      case 2:
        // Send message 2
        state->last_msg_sent = SOUND_TWO; //state->cur_state;
        state->cur_state = 0;
        midiout->sendMessage(&message2);
        break;
    }
  }
  // Clean up
 cleanup:
  delete midiout;
  gpioTerminate();
  return 0;
}

RtMidiOut *setupRtmidi() {
  RtMidiOut *midiout = 0;

  try {
    midiout = new RtMidiOut();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    return NULL;
  }

  // Call function to select port.
  try {
    if ( chooseMidiPort( midiout ) == false ) {
      delete midiout;
      return NULL;
    }
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    return NULL;
  }
  return midiout;
}

bool chooseMidiPort( RtMidiOut *rtmidi )
{
  //std::cout << "\nWould you like to open a virtual output port? [y/N] ";

  //std::string keyHit;
  //std::getline( std::cin, keyHit );
  //if ( keyHit == "y" ) {
  //  rtmidi->openVirtualPort();
  //  return true;
  //}

  std::string portName;
  unsigned int i = 0, nPorts = rtmidi->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No output ports available!" << std::endl;
    return false;
  }

  if ( nPorts == 1 ) {
    std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
  }
  else {
    for ( i=0; i<nPorts; i++ ) {
      portName = rtmidi->getPortName(i);
      std::cout << "  Output port #" << i << ": " << portName << '\n';
    }

    do {
      std::cout << "\nChoose a port number: ";
      std::cin >> i;
    } while ( i >= nPorts );
  }

  std::cout << "\n";
  rtmidi->openPort( i );

  return true;
}

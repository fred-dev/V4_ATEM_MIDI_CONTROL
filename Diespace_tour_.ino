/*****************
 * Example: ATEM Basic Control
 * Connects to the Atem Switcher and outputs changes to Preview and Program on the Serial monitor (at 9600 baud)
 * Uses digital inputs 2 and 3 (active High) to select input 1 and 2 on Preview Bus
 * Uses digital input 7 (active High) to "Cut" (Preview and Program swaps)
 * Uses digital outputs 4 and 5 for Tally LEDs for inp  ut 1 and 2 (Active LOW)
 * See file "Breadboard circuit for ATEM basic control.pdf" for suggested breadboard schematics (find it under ATEM/examples/ATEMbasicControl/ in this library)
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */

//commit
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <MIDI.h>
#include <ATEM.h>
//#define LED  9
// MAC address and IP address for this *particular* Ethernet Shield!
// MAC address is printed on the shield
// IP address is an available address you choose on your subnet where the switcher is also present:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xEB, 0x7E };		// <= SETUP
IPAddress ip(192, 168, 10, 50);			// <= SETUP
// Include ATEM library and make an instance:
// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
//ATEM AtemSwitcher(IPAddress(192, 168, 10, 240), 56417); // <= SETUP (the IP address of the ATEM switcher)

int tBarPos;
int bankSelected;
int bankSelectFine;
int programToInput;
int faderSide=1;
int inputKeeper;
int fadeToBlkCh; //Midi cc channel ftb
int rotSelect;
boolean AtemOnline = false;
ATEM AtemSwitcher;
void resetSelector(){
  bankSelected=0;
}


// Necaed to find what the mixer outputs when the transformer buttons are pushed
void HandleControlChange(byte channel, byte number, byte value){
  if (number>0&& number<10){
    AtemSwitcher.changePreviewInput(number);
    }
  if (number==10){
    AtemSwitcher.changeTransitionType(value); // 0=MIX, 1=DIP, 2=WIPE, 3=DVE, 4=STING
    }
  if (number==0 && value==80){
    rotSelect=1;
    }
    
  if (number==11) {
    if (faderSide==1){
      tBarPos=value;
      }
    if (faderSide==2){
      tBarPos=127-value;
      }
    AtemSwitcher.changeTransitionPosition(int(map(tBarPos,0,127,1,1000)));
    
    // Keep track of fader side
    if (tBarPos==0){
      AtemSwitcher.changeTransitionPositionDone();
       faderSide=2;
    }

    if (tBarPos==127){
      AtemSwitcher.changeTransitionPositionDone();
      faderSide=1;
    }
  }

  if (number==0 && value==0){
    bankSelected=1;
  }

  if (number==0&&value==1){
    bankSelected=2;
  }
  if (number==12&&faderSide==1){
    AtemSwitcher.changeProgramInput(5);
  }
  if (number==12&&faderSide==2){
    AtemSwitcher.changePreviewInput(5);
  }
  if (number==13&&faderSide==1){
    AtemSwitcher.changePreviewInput(5);
  }
  if (number==13&&faderSide==2){
    AtemSwitcher.changeProgramInput(5);
  }

  if (number==fadeToBlkCh){
    inputKeeper = AtemSwitcher.getPreviewInput();
    AtemSwitcher.changePreviewInput(0);
    AtemSwitcher.changeTransitionPosition(int(map(value,0,64,0,1000)));
    if (value==64){
      AtemSwitcher.changeTransitionPositionDone();
      AtemSwitcher.changePreviewInput(inputKeeper);
    }
  } 
}



void HandleProgramChange(byte channel, byte number){
  // V4 input buttons to ATEM input selector, the message for these is sent as three parts, this selects 1 to 4
  //programToInput = number+1; 
  if (rotSelect==1){
    AtemSwitcher.changeAuxState(3, number);
    rotSelect=0;
    
   }
  if (bankSelected==1 && faderSide==1){
    AtemSwitcher.changeProgramInput(number+1);
    resetSelector();
  }

  if (bankSelected==1  && faderSide==2){
    AtemSwitcher.changePreviewInput(number+1);
    resetSelector();
  }

  if (bankSelected==2  && faderSide==2){
    AtemSwitcher.changeProgramInput(number+1);
    resetSelector();
  }

  if (bankSelected==2 && faderSide==1){
    AtemSwitcher.changeProgramInput(number+1);
    resetSelector();
  }
}     
// //Assuming the transition type and effect type row a and b send note on signals
//void HandleNoteOn(byte channel, byte pitch, byte velocity){
//  AtemSwitcher.changeAuxState(3, pitch);
//  AtemSwitcher.changeTransitionType(pitch+1); // 0=MIX, 1=DIP, 2=WIPE, 3=DVE, 4=STING
//  AtemSwitcher.changeProgramInput(pitch);
//}

void setup() {
 fadeToBlkCh=14;
  // Initiate MIDI communications, listen to all channels
     
  //MIDI.setHandleNoteOn(HandleNoteOn);
  

  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip); 
  AtemSwitcher.begin(IPAddress(192, 168, 10, 240), 56417);;
  AtemSwitcher.serialOutput(false);
  AtemSwitcher.connect();
  AtemSwitcher.delay(50);
  
   MIDI.begin(MIDI_CHANNEL_OMNI); 
  //Serial.begin(34800);
  // Initialize a connection to the switcher:
  MIDI.setHandleControlChange(HandleControlChange);
  MIDI.setHandleProgramChange(HandleProgramChange);
}

void loop() {
  MIDI.read();
  AtemSwitcher.runLoop();
  if (AtemSwitcher.isConnectionTimedOut())  {
     AtemSwitcher.connect();
  }
    

}


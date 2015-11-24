#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>

#define DEBUG 0

#define NUM_MOTORS 8

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED} ;

// NOTE: Alternatively, you can assign a fixed IP to configure your
//       Ethernet shield.
byte ip[] = { 192, 168, 0, 100 };


int serverPort  = 10000; //TouchOSC (incoming port)
int destPort = 9000;    //TouchOSC (outgoing port)

// motors Pins

int motorPinA[NUM_MOTORS] = {3, 5, 7, 9, 13, 15, 17, 19};
int motorPinB[NUM_MOTORS] = {4, 6, 8, 12, 14, 16, 18, 20};


//switc Pins
int switchPinA[NUM_MOTORS] = {22, 24, 26, 28, 30, 32, 34, 36};
int switchPinB[NUM_MOTORS] = {23, 25, 27, 29, 31, 33, 35, 37};


//Create UDP message object
EthernetUDP Udp;


typedef enum motorStates { // enum with motor States, just a list of Names constants
  FORWARD,
  BACKWARD,
  STOP
};

motorStates actualState;




void setup() {


  actualState = STOP;  // start with STOP state

  // start the Ethernet connection

  Ethernet.begin(mac, ip);
  Udp.begin(serverPort);

  //two pins to control the motor
  for (int i = 0; i < NUM_MOTORS; i++) {
    pinMode(motorPinA[i], OUTPUT);
    pinMode(motorPinB[i], OUTPUT);
    digitalWrite(motorPinA[i], HIGH);
    digitalWrite(motorPinB[i], HIGH);
    //two switchs INPUT,

    pinMode(switchPinA[i], INPUT);
    pinMode(switchPinB[i], INPUT);
  }



#ifdef  DEBUG
  //Debug Mode enable in case DEBUG equal 1
  Serial.begin(19200);
#endif


}

void loop()
{

  //process received messages
  OSCMsgReceive();

  //constantly check the keys independent of the OSC messages
  //now check all motors and turn the motor off in case off the switchs are on

  for (int motorID = 0; motorID < NUM_MOTORS; motorID++) {

    boolean switchF = digitalRead(switchPinA[motorID]); // if switch is ON for both it stop and send back erro
    boolean switchB = digitalRead(switchPinB[motorID]);
    //if either switchs are on throw erro

    //
    if (switchF == 1 || switchB == 1) {
      sendMotorCommand(STOP,motorID); // motorID will be the index
#ifdef  DEBUG
      Serial.println("STOP");
#endif
    }

  }

}

void OSCMsgReceive() {
  OSCMessage msgIN;
  int size;
  if ((size = Udp.parsePacket()) > 0) {
    while (size--)
      msgIN.fill(Udp.read());
    if (!msgIN.hasError()) {

      msgIN.route("/MotorIn/Forward", motorForward);
      msgIN.route("/MotorIn/Backward", motorBackward);
      msgIN.route("/MotorIn/Stop", motorStop);

    }
  }
}

void motorForward(OSCMessage &msg, int addrOffset) {

#ifdef  DEBUG
  Serial.println("FORWARD_OSC");
#endif

  //receive the motor ID from the OSC message
  int motorID;
  boolean error;

  if (msg.isInt(0)) //only if theres a number
  {
    motorID = msg.getInt(0); //get an integer from the OSC message
    //receive FORWARD update generalState and send back TRUE OSC message
    error = sendMotorCommand(FORWARD, motorID); //motor return 1 for OK 0 for error

  } else {

    error = 0; //trow an error
  }


#ifdef  DEBUG
  Serial.print("FORWARD_OSC_ERROR: ");
  Serial.println(error);
#endif


  String msgText = "/MotorOut/Forward/" + motorID;
  OSCMessage msgOUT(msgText.c_str());
  
  msgOUT.add(error); // send TRUE we got the Foward Message


  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}

void motorBackward(OSCMessage &msg, int addrOffset ) {

#ifdef  DEBUG
  Serial.println("BACKWARD_OSC");
#endif

  //receive the motor ID from the OSC message
  int motorID;
  boolean error;

  if (msg.isInt(0)) //only if theres a number
  {

    motorID = msg.getInt(0); //get an integer from the OSC message
    //receive BACKWARD update generalState and send back TRUE OSC message
    error = sendMotorCommand(BACKWARD, motorID); //motor return 1 for OK 0 for error

  } else {
    error = 0; //trow an error
  }

#ifdef  DEBUG
  Serial.print("BACKWARD_OSC_ERROR: ");
  Serial.println(error);
#endif

  String msgText = "/MotorOut/Backward/" + motorID;
  OSCMessage msgOUT(msgText.c_str());
  
  msgOUT.add(error); // send TRUE we got the Foward Message

  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}

void motorStop(OSCMessage &msg, int addrOffset ) {
  //receive STOP update generalState and send back TRUE OSC message
#ifdef  DEBUG
  Serial.println("STOP_OSC");
#endif

  int motorID;
  boolean error;

  if (msg.isInt(0)) //only if theres a number
  {

    motorID = msg.getInt(0); //get an integer from the OSC message
    //receive BACKWARD update generalState and send back TRUE OSC message
    error = sendMotorCommand(STOP, motorID); //motor return 1 for OK 0 for error

  } else {
    error = 0; //trow an error
  }


#ifdef  DEBUG
  Serial.print("STOP_OSC_ERROR: ");
  Serial.println(error);
#endif

  String msgText = "/MotorOut/Stop/" + motorID;
  OSCMessage msgOUT(msgText.c_str());
  msgOUT.add(error); // send TRUE we got the Foward Message


  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}


boolean sendMotorCommand(enum motorStates state, int motorID) {

  //first thig to check before sending new motor commands is.
  //the switches are pressed/
  //if any of then are pressed, just turnOFF the motor and return

  boolean switchF = digitalRead(switchPinA[motorID]); // if switch is ON for both it stop and send back erro
  boolean switchB = digitalRead(switchPinB[motorID]);

  //if either switchs are on throw erro
  if (switchF == 1 || switchB == 1) {
    actualState = STOP; // should stop

    //send motors to off
    digitalWrite(motorPinA[motorID], HIGH);
    digitalWrite(motorPinB[motorID], HIGH);


    return 0; //return 0 //error message

  }


  //only send motor messages if it' a new OSC message a new state

  if (actualState != state) {
    actualState = state; // update actual state

    switch (state) {
      case FORWARD:
        digitalWrite(motorPinA[motorID], LOW);
        digitalWrite(motorPinB[motorID], HIGH);

        break;
      case BACKWARD:

        //check this, not sure how to make your motor go Backward
        digitalWrite(motorPinA[motorID], HIGH);
        digitalWrite(motorPinB[motorID], LOW);
        break;
      case STOP:
        //dont need to check switchs to make it stop..

        //check how to make the motor STOP
        digitalWrite(motorPinA[motorID], HIGH);
        digitalWrite(motorPinB[motorID], HIGH);

        break;

    }

  }

  //read switchs and return error in case


  //if it gets here I hope everything is ok

  return 1; //return 1 for OK


}

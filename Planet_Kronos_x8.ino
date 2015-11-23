#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>

#define DEBUG 0

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED} ;

// NOTE: Alternatively, you can assign a fixed IP to configure your
//       Ethernet shield.
byte ip[] = { 192, 168, 0, 100 };


int serverPort  = 10000; //TouchOSC (incoming port)
int destPort = 9000;    //TouchOSC (outgoing port)

// motors Pins
int motor1Pin1 = 3;
int motor1Pin2 = 4;
int motor2Pin1 = 5;
int motor2Pin2 = 6;
int motor3Pin1 = 7;
int motor3Pin2 = 8;
int motor4Pin1 = 9;
int motor4Pin2 = 12;
int motor5Pin1 = 13;
int motor5Pin2 = 14;
int motor6Pin1 = 15;
int motor6Pin2 = 16;
int motor7Pin1 = 17;
int motor7Pin2 = 18;
int motor8Pin1 = 19;
int motor8Pin2 = 20;

//switc Pins
int switch1PinA = 22;
int switch1PinB = 23;
int switch2PinA = 23;
int switch2PinB = 24;
int switch3PinA = 25;
int switch3PinB = 26;
int switch4PinA = 27;
int switch4PinB = 28;
int switch5PinA = 29;
int switch5PinB = 30;
int switch6PinA = 31;
int switch6PinB = 32;
int switch7PinA = 33;
int switch7PinB = 34;
int switch8PinA = 35;
int switch8PinB = 36;


//Create UDP message object
EthernetUDP Udp;


typedef enum motor1States { // enum with motor States, just a list of Names constants
  FORWARD1,
  BACKWARD1,
  STOP1
};

motor1States actualState1;


typedef enum motor2States { // enum with motor States, just a list of Names constants
  FORWARD2,
  BACKWARD2,
  STOP2
};

motor2States actualState2;


void setup() {


  actualState1 = STOP1;  // start with STOP state motor 1
  actualState2 = STOP2; // start with STOP state motor 2

  // start the Ethernet connection

  Ethernet.begin(mac, ip);
  Udp.begin(serverPort);

  //pins to control the motors
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  
  // switchs INPUT,
  pinMode(switch1PinA, INPUT);
  pinMode(switch1PinB, INPUT);
  pinMode(switch2PinA, INPUT);
  pinMode(switch2PinB, INPUT);

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
  boolean switchA = digitalRead(switch1PinA); // if switch is ON for both it stop and send back erro
  boolean switchB = digitalRead(switch1PinB);  //if either switchs are on throw erro
  boolean switchC = digitalRead(switch2PinA);
  boolean switchD = digitalRead(switch2PinB);
  

  //
  if (switchA == 1 || switchB == 1) {
    sendMotor1Command(STOP1);
#ifdef  DEBUG
    Serial.println("STOP");
#endif
  }

 if (switchC == 1 || switchD == 1) {
    sendMotor2Command(STOP2);
#ifdef  DEBUG
    Serial.println("STOP");
#endif
  }
  
}

void OSCMsgReceive() {
  OSCMessage msgIN;
  int size;
  if ((size = Udp.parsePacket()) > 0) {
    while (size--)
      msgIN.fill(Udp.read());
    if (!msgIN.hasError()) {

      msgIN.route("/Motor1/Forward", motor1Forward);
      msgIN.route("/Motor1/Backward", motor1Backward);
      msgIN.route("/Motor1/Stop", motor1Stop);
      msgIN.route("/Motor2/Forward", motor2Forward);
      msgIN.route("/Motor2/Backward", motor2Backward);
      msgIN.route("/Motor2/Stop", motor2Stop);

    }
  }
}

////OSC for motor 1 ////
void motor1Forward(OSCMessage &msg, int addrOffset) {

#ifdef  DEBUG
      Serial.println("FORWARD_OSC");
#endif
  
  //receive FORWARD update generalState and send back TRUE OSC message

  boolean error = sendMotor1Command(FORWARD1); //motor return 1 for OK 0 for error

#ifdef  DEBUG
      Serial.print("FORWARD_OSC_ERROR: ");
      Serial.println(error);
#endif

  OSCMessage msgOUT("/MotorOut1/Forward");
  msgOUT.add(error); // send TRUE we got the Foward Message


  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}

void motor1Backward(OSCMessage &msg, int addrOffset ) {

#ifdef  DEBUG
      Serial.println("BACKWARD_OSC");
#endif
  
  //receive BACKWARD update generalState and send back TRUE OSC message
  boolean error = sendMotor1Command(BACKWARD1);

#ifdef  DEBUG
      Serial.print("BACKWARD_OSC_ERROR: ");
      Serial.println(error);
#endif

  OSCMessage msgOUT("/MotorOut1/Backward");
  msgOUT.add(error); // send TRUE we got the Foward Message

  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}

void motor1Stop(OSCMessage &msg, int addrOffset ) {
  //receive STOP update generalState and send back TRUE OSC message
#ifdef  DEBUG
      Serial.println("STOP_OSC");
#endif
  
  boolean error = sendMotor1Command(STOP1);
  
#ifdef  DEBUG
      Serial.print("STOP_OSC_ERROR: ");
      Serial.println(error);
#endif
  
  OSCMessage msgOUT("/MotorOut1/Stop");
  msgOUT.add(error); // send TRUE we got the Foward Message


  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}

///OSC for motor 2 ////
void motor2Forward(OSCMessage &msg, int addrOffset) {

#ifdef  DEBUG
      Serial.println("FORWARD_OSC");
#endif
  
  //receive FORWARD update generalState and send back TRUE OSC message

  boolean error = sendMotor2Command(FORWARD2); //motor return 1 for OK 0 for error

#ifdef  DEBUG
      Serial.print("FORWARD_OSC_ERROR: ");
      Serial.println(error);
#endif

  OSCMessage msgOUT("/MotorOut2/Forward");
  msgOUT.add(error); // send TRUE we got the Foward Message


  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}

void motor2Backward(OSCMessage &msg, int addrOffset ) {

#ifdef  DEBUG
      Serial.println("BACKWARD_OSC");
#endif
  
  //receive BACKWARD update generalState and send back TRUE OSC message
  boolean error = sendMotor2Command(BACKWARD2);

#ifdef  DEBUG
      Serial.print("BACKWARD_OSC_ERROR: ");
      Serial.println(error);
#endif

  OSCMessage msgOUT("/MotorOut2/Backward");
  msgOUT.add(error); // send TRUE we got the Foward Message

  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}

void motor2Stop(OSCMessage &msg, int addrOffset ) {
  //receive STOP update generalState and send back TRUE OSC message
#ifdef  DEBUG
      Serial.println("STOP_OSC");
#endif
  
  boolean error = sendMotor2Command(STOP2);
  
#ifdef  DEBUG
      Serial.print("STOP_OSC_ERROR: ");
      Serial.println(error);
#endif
  
  OSCMessage msgOUT("/MotorOut2/Stop");
  msgOUT.add(error); // send TRUE we got the Foward Message


  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp);
  Udp.endPacket();
  msgOUT.empty();
}

//boolean state of motor 1 ///
boolean sendMotor1Command(enum motor1States state) {

  //first thig to check before sending new motor commands is.
  //the switches are pressed/
  //if any of then are pressed, just turnOFF the motor and return

  boolean switchA = digitalRead(switch1PinA); // if switch is ON for both it stop and send back erro
  boolean switchB = digitalRead(switch1PinB);

  //if either switchs are on throw erro
  if (switchA == 1 || switchB == 1) {
    actualState1 = STOP1; // should stop

    //send motors to off
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);


    return 0; //return 0 //error message

  }


  //only send motor messages if it' a new OSC message a new state

  if (actualState1 != state) {
    actualState1 = state; // update actual state

    switch (state) {
      case FORWARD1:
        digitalWrite(motor1Pin1, HIGH);
        digitalWrite(motor1Pin2, LOW);

        break;
      case BACKWARD1:

        //check this, not sure how to make your motor go Backward
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, HIGH);
        break;
      case STOP1:
        //dont need to check switchs to make it stop..

        //check how to make the motor STOP
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, LOW);

        break;

    }

  }

  //read switchs and return error in case


  //if it gets here I hope everything is ok

  return 1; //return 1 for OK


}


///boolean states of motor 2 ////

boolean sendMotor2Command(enum motor2States state) {

  //first thig to check before sending new motor commands is.
  //the switches are pressed/
  //if any of then are pressed, just turnOFF the motor and return

  boolean switchC = digitalRead(switch2PinA); // if switch is ON for both it stop and send back erro
  boolean switchD = digitalRead(switch2PinB);

  //if either switchs are on throw erro
  if (switchC == 1 || switchD == 1) {
    actualState2 = STOP2; // should stop

    //send motors to off
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);


    return 0; //return 0 //error message

  }


  //only send motor messages if it' a new OSC message a new state

  if (actualState2 != state) {
    actualState2 = state; // update actual state

    switch (state) {
      case FORWARD2:
        digitalWrite(motor2Pin1, HIGH);
        digitalWrite(motor2Pin2, LOW);

        break;
      case BACKWARD2:

        //check this, not sure how to make your motor go Backward
        digitalWrite(motor2Pin1, LOW);
        digitalWrite(motor2Pin2, HIGH);
        break;
      case STOP2:
        //dont need to check switchs to make it stop..

        //check how to make the motor STOP
        digitalWrite(motor2Pin1, LOW);
        digitalWrite(motor2Pin2, LOW);

        break;

    }

  }

  //read switchs and return error in case


  //if it gets here I hope everything is ok

  return 1; //return 1 for OK


}

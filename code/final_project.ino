
#define threshold 550
int enA = 10;
int enB = 9;
int in1 = 2;
int in2 = 3;
int in3 = 5;
int in4 = 4;
int motorSpeedForwardL = 150;
int motorSpeedForwardR = 90;
int pinValue = A0;
int pinValue2 = A1;
float NMleft;
float NMright;
int count = 0;
int flag = 0;
int updated = 0;
int obstacles = 0;

const int triggerPIN = 8; 
const int echoPIN = 7; 

float soundDuration, distToObjectInCM; 


void setup()
{
  Serial.begin(9600);
  pinMode(enA,OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT); 
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(triggerPIN, OUTPUT);
  pinMode(echoPIN, INPUT);
  digitalWrite(triggerPIN, LOW);

}


void loop() {
  
  float rightSensorValue;
  float leftSensorValue;

  rightSensorValue = analogRead(pinValue);
  leftSensorValue = analogRead(pinValue2);

  //Storing left and right sensor values 
  

  Serial.println("Left sensor value " + String(leftSensorValue));
  Serial.println("Right sensor value " + String(rightSensorValue));

  NMleft = leftSensorValue/(leftSensorValue + rightSensorValue);
  NMright = rightSensorValue/(leftSensorValue + rightSensorValue);

  if(NMleft<=0.6 && NMleft>=0.4 && NMright<=0.6 && NMright>=0.4) {
    // if the left and right are approximately equal, i.e. 
    // the normalized measurement is close to 0.5
    if(leftSensorValue > 250 && rightSensorValue > 400){
      moveForward();
      digitalWrite(LED_BUILTIN, LOW);       
    }
    else {
      moveCircle();
    }
 }
  else if(NMleft<NMright && rightSensorValue > 400) {
    moveRight();
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if(NMleft>NMright && leftSensorValue > 250) {
    count+=1;
    if(count!=2) {
      moveLeft();
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  else {
      moveCircle();
    }
  digitalWrite(triggerPIN, LOW);
  soundDuration = pulseIn(echoPIN,HIGH);
  unsigned long current_pulse = millis();
  unsigned long start_pulse = millis();
  while(current_pulse-start_pulse<=60){
    current_pulse = millis();
  }
  digitalWrite(triggerPIN, HIGH);
  distToObjectInCM =  (soundDuration*0.0343)/2;
  Serial.println(distToObjectInCM);
  if (distToObjectInCM <=30 && distToObjectInCM>=5) {
    // obstacle detected
    flag += 1;
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (distToObjectInCM <=5) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    flag = 0;
    updated = 0;
    if(obstacles!=2) digitalWrite(LED_BUILTIN, HIGH);
    else digitalWrite(LED_BUILTIN, LOW);
  }
  if(flag==1 && updated==0){
    // updating obstacles if not yet updated
    obstacles += 1;
    updated = 1;
  }
  else digitalWrite(LED_BUILTIN, LOW); 
  
}

//functions for movement 
//to make code modular 

void moveForward() {

  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  analogWrite(enA,motorSpeedForwardR);
  
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(enB,motorSpeedForwardL);
  
}

void moveCircle() {

  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  analogWrite(enA,motorSpeedForwardR);
  
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  analogWrite(enB,motorSpeedForwardL);
  
}


void moveLeft() {
  
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  analogWrite(enA,motorSpeedForwardR + 30);
  
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(enB,motorSpeedForwardL);
  
}


void moveRight() {
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  analogWrite(enA,motorSpeedForwardR);
  
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(enB,motorSpeedForwardL + 30);
  
}

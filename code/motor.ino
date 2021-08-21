
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
int init_time = millis();
unsigned long current_time;
#define threshold 550
unsigned long start_time; 
float NMleft;
float NMright;

void setup()
{
  Serial.begin(9600);
  pinMode(enA,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT); 
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  start_time = millis();

}


void loop() {
  current_time = millis();
  //resetting the start_time time to toggle the LED in case 600ms have passed and it has not been 
  //reset by the LED function
  if (current_time - start_time > 600) {
    start_time = millis();
  }
  
  float rightSensorValue;
  float leftSensorValue;

  rightSensorValue = analogRead(pinValue);
  leftSensorValue = analogRead(pinValue2);

  Serial.println("Left sensor value " + String(leftSensorValue));
  Serial.println("Right sensor value " + String(rightSensorValue));

  NMleft = leftSensorValue/(leftSensorValue + rightSensorValue);
  NMright = rightSensorValue/(leftSensorValue + rightSensorValue);

    //if both sensors have approximately the same amount of light falling upon them
    if (abs(NMleft - NMright) < 0.10) {

      //if both sensors have low values, it is confused
        if(rightSensorValue < threshold && leftSensorValue < threshold) {
            Serial.println("Confused, moving in circles");
            moveCircle();
         }
        else {
          Serial.println("Moving forward"); 
          moveForward();
        }
      }
    //if there is light on the left side
    else if (NMleft - NMright > 0.10 && leftSensorValue > threshold) {
      Serial.println("Moving left");
      moveLeft();
    }
    //if there is light on the right side
    else if(NMright - NMleft > 0.10 && rightSensorValue > threshold) {
      Serial.println("Moving right");
      moveRight();
    }
    else {
      Serial.println("Confused, moving in Circles");
      moveCircle();
    }
}


void moveForward() {

  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  analogWrite(enA,motorSpeedForwardR);
  
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(enB,motorSpeedForwardL);
  
}

void moveCircle() {

  if((current_time-start_time) > 400 && (current_time-start_time) < 600) {
    if (digitalRead(LED_BUILTIN) == HIGH) digitalWrite(LED_BUILTIN, LOW);
    else digitalWrite(LED_BUILTIN, HIGH);
  }
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

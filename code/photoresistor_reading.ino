void setup() {
  // The following is to open Serial communication between the Arduino
  // and your computer so that the values of pin A0 can be read.
  // The results are going to be displaued on the IDE's Serial Monitor.
  // Look up what this command does, and what the 9600 means.
  Serial.begin(9600);
}

void loop() {

  // Arduino pin whose voltage will be read
  int pinValue = A0;
  int pinValue2 = A1;

  // A0Value is the variable in which the value read off oa A0 will be stored.
  float rightSensorValue;
  float leftSensorValue;
  float NMleft;
  float NMright;

  // Read the value of the A0 pin. Note here that we do not need
  // to declare earlier that pin A0 is an input pin. You can if you
  // want, it can make things clearer in more complex code.
  rightSensorValue = analogRead(pinValue);
  leftSensorValue = analogRead(pinValue2);

  NMleft = leftSensorValue/(leftSensorValue + rightSensorValue);
  NMright = rightSensorValue/(leftSensorValue + rightSensorValue);

  Serial.println("Right sensor value: " + String(rightSensorValue)); 
  Serial.println("Left sensor value: " + String(leftSensorValue)); 
  Serial.println("Normalized measurement Left: " + String(NMleft)); 
  Serial.println("Normalized measurement Right: " + String(NMright)); 
  Serial.println(" ");

  // Wait 1000ms so as not to flood the Serial Monitor with too much info!
  delay(1000);
}

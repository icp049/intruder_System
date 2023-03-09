#include <Servo.h>

Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created

int pos = 0;    // variable to store the servo position

int potPin = A0;  // select the input pin for the potentiometer
int potVal;       // variable to store the value coming from the potentiometer

void setup()
{
  myservo.attach(7);  // attaches the servo on pin 9 to the servo object
}

void loop()
{
  potVal = analogRead(potPin); // read the value from the potentiometer

  // check if the potentiometer is connected
  if (potVal > 0) {
    // use the potentiometer to control the servo
    int angle = map(potVal, 0, 1023, 0, 180); // map the potentiometer value to an angle between 0 and 180
    myservo.write(angle);  // tell servo to go to position based on potentiometer input
    delay(10);             // waits 10ms for the servo to reach the position
  }
  else {
    // use the code to move the servo on its own
    int angle = 90;
    // goes from 0 degrees to 180 degrees and back to 0 degrees
    for (pos = 90 - angle; pos <= 90 + angle; pos++)
    {
      myservo.write(pos);  // tell servo to go to position in variable 'pos'
      delay(10);           // waits 10ms for the servo to reach the position
    }

    for (pos = 90 + angle; pos >= 90 - angle; pos--)
    {
      myservo.write(pos);  // tell servo to go to position in variable 'pos'
      delay(10);           // waits 10ms for the servo to reach the position
    }
  }
}

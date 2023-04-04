/*
Student Name: Ian Jericho Pedeglorio
Student #: 200450851
CS 207 Project: INTRUDER ALERT SYSTEM
*/

//Start of program
//libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

const int TRIGGER_PIN = 6;    // Trigger pin of the ultrasonic sensor
const int ECHO_PIN = 5;       // Echo pin of the ultrasonic sensor
const int NUM_READINGS = 5;   // Number of readings to take for averaging
const int MAX_DISTANCE = 30;  // Maximum distance that the sensor can detect
const int MIN_DISTANCE = 4;   // Minimum distance that the sensor can detect
const int BUZZER = 4;         // Buzzer
const int RED_PIN = 3;        // red led pin
const int GREEN_PIN = 12;     // green led pin
const int TFT_CS = 10;        //TFT CS
const int TFT_RST = 8;        //TFT RESET
const int TFT_DC = 9;         //TFT A0

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

LiquidCrystal_I2C lcd(0x27, 16, 2);  //LCD address and size of lcd we will use
Servo myservo;                       //Declare the servo

//variables
unsigned long lastTime = 0;   //if last time is 0 to be used later on if statements
int pos = 0;                  //position of servo
bool objectDetected = false;  //boolean for condition if object is detected, we set it as false so we can make it true later



void setup() {

  Serial.begin(9600);
  //Ultrasonic Sensor Pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

//initalize the led lights (green and red)
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);

  //Buzzer pin
  pinMode(BUZZER, OUTPUT);

  //Initialize the 16x2 lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  //initialize servo at pin 7
  myservo.attach(7);

  

  //Initialize TFT LCD Screen
  tft.initR();             
  tft.fillScreen(ST7735_BLACK);                  //background
  tft.setRotation(1);                            //Rotate it facing user

  //Create fixed radar lines
  tft.drawCircle(80, 120, 30, ST7735_GREEN);     
  tft.drawCircle(80, 120, 50, ST7735_GREEN);
  tft.drawCircle(80, 120, 70, ST7735_GREEN);
  tft.drawCircle(80, 120, 90, ST7735_GREEN);

  //Create fixed servo p[osition indicator
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(1);
  tft.print("Servo position:");
}



//function for reading the distance from the ultrasonic sensor
float readDistance() {

  //initialize the trigger pin
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  //the duration will be how the echo pin remains high
  float duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration / 58.2;  //This formula is used to calculate the distance from pulse of the sensor that bounces off in an object divided by the speed of sound

  //prints the distance in serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  //return value of distance
  return distance;
}

//Function to calculate average distance
float getAverageDistance() {
  //initiate totald istance at 0
  float totalDistance = 0;

  //this for loop increments until the number of readings is 5
  for (int i = 0; i < NUM_READINGS; i++) {

    totalDistance += readDistance();  //Total distance is equal to the sum of the distance read
  }

  //We get the average to make a better distance measure precision
  float averageDistance = totalDistance / NUM_READINGS;  //Divide all the distance read and divide it by number of readings to get average

  //prints the Average distance in the serial monitor
  Serial.print("Average Distance: ");
  Serial.print(averageDistance);
  Serial.println(" cm");

  //return average distance value
  return averageDistance;
}

//fnction when there is no object in sight
void displayOutOfRange() {
 
  
  //print information in the 16x2 lcd
  lcd.setCursor(0, 1);
  lcd.print("Out of range");
  lcd.setCursor(12, 1);
  lcd.print("             ");
  lcd.setCursor(0, 0);
  lcd.print("                  ");

   //make the buzzer silent
  noTone(BUZZER);    
  fastFlash(false, RED_PIN, GREEN_PIN); //turn on green led
  
}

//fnction when object is less than 4 cm from sensor
void displayImpact() {

  //print information in the 16x2 lcd
  lcd.setCursor(15, 0);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print("IMPACT! IMPACT!");
  lcd.setCursor(0, 0);
  lcd.print("IMPACT! IMPACT!");

  //make the buzzer make an alarm
  tone(BUZZER, 500);

  fastFlash(true, RED_PIN,GREEN_PIN);  // Turn on the red LED

}
//fnction when object is within 0 to 30 cm and object is detected
void displayDistance(float distance) {

  //print information in the 16x2 lcd
  lcd.setCursor(15, 0);
  lcd.setCursor(0, 1);
  lcd.print(distance, 2);
  lcd.print(" cm");
  lcd.setCursor(7, 1);
  lcd.print("            ");
  lcd.setCursor(0, 0);
  lcd.print("OBJECT DETECTED!");

  //make the buzzer make an alarm
  tone(BUZZER, 100);

  fastFlash(true, RED_PIN, GREEN_PIN);  //still turn on red led
  
}

//function for servo mvement
void rotateServo() {
  static int direction = 0;        // initial direction of rotation
  int posr = myservo.read();       //store the servo position in a variable for tft lcd later


  //if an object is detected, stop the servo as it it will write its current position
  if (objectDetected) {
    myservo.write(pos);

    return;
  }


  unsigned long now = millis();  //we will use millis because using delay() function causes too much delay in the lcd display, ultrasonic sensor readings and servo movements
  if (now - lastTime >= 100) {
    lastTime = now;
    pos += direction;            //position will be position + direction
    if (pos >= 180) {
      direction = -10;           // if position reaches 180, decrement position by 20s
      pos = 180;                 //position will be 180 by then

    } else if (pos <= 0) {       // if position reaches 0, increment position by 20s
      direction = 10;
      pos = 0;                   //position will be 0 by then


    }  
  }
  //Move the servo to desired position
  myservo.write(pos);


  //While moving servo position, draw a line on the TFT LCD to indicate and position like a radar (This will get updated as servo moves)
  tft.fillRect(0, 20, 120, 10, ST7735_BLACK);
  tft.setCursor(0, 20);
  tft.setTextColor(ST7735_CYAN);
  tft.setTextSize(2);
  tft.print(posr); //Write servo position 

  //Variables for the line
  static float prev_x = 80;
  static float prev_y = 120;


  
  float x = map(pos, 0, 180, 30, 130);                     //Map the servo position which is 0 to 180, the 430 and 130 enables it to fit in the screen   
  float y = -sqrt(pow(50, 2) - pow(x - 80, 2)) + 120;      //formula for a circle, we use -sqrt because we want the lines on the other side of circle 

  tft.drawLine(80, 120, prev_x, prev_y, ST7735_BLACK);     //draw a black previous line to hide the previous position of servo
  

  tft.drawLine(80, 120, x, y, ST7735_BLUE);               // Draw current line
  
  //curren values are stored in the prev_x and prev_y to vve used later
  prev_x = x; 
  prev_y = y;

  delay(10);
}


//function for red led
void fastFlash(bool on, int redPin, int greenPin) {
  // setup of green led pin
  if (on) {
    digitalWrite(redPin, HIGH);  // turn on the red LED
    digitalWrite(greenPin, LOW); // turn off the green LED
  } else {
    digitalWrite(redPin, LOW);   // turn off the red LED
    digitalWrite(greenPin, HIGH); // turn on the green LED
  }
}




//loop function
void loop() {

  //variables used throughout the loop
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();

  //call the function for average distance
  float averageDistance = getAverageDistance();



  if (averageDistance > MAX_DISTANCE) {              //if average distance greater than 30 cm, it cant detect any object
    if (currentTime - lastTime >= 500) {            //subtitute to delay() function
      lastTime = currentTime;
      rotateServo();                                //call function to rotate the servo
      displayOutOfRange();                          //call function for lcd display
    }
  } else if (averageDistance < MIN_DISTANCE) {      //if average distance is less than 4 cm, impact will happen
    if (currentTime - lastTime >= 500) {            //subtitute to delay() function
      lastTime = currentTime;
      displayImpact();                              //call function that makes the servo stop, and display that impact is detected
    }
  } else {                                          //if average distance is less than 30 cm and greater than 4 cm, An object is detected
    if (currentTime - lastTime >= 200) {
      lastTime = currentTime;
      displayDistance(averageDistance);             //call function that displays real time distance in 16x2 lcd
    }
  }
}
//end of program
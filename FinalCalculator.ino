#define ROTARY_ANGLE_SENSOR A3 // The input pin used for the sensor
#define ADC_REF 5 // This matches the operating voltage we set to 5V
#define GROVE_VCC 5 // VCC of the grove interface which is often 5V
#define NUM_OF_LEDS 24 // Number of LEDs in the LED bar
#define MAX_ANGLE 300 // device rotates to 300 degrees

#include <Grove_LED_Bar.h>

Grove_LED_Bar LEDbar(6, 7, 0,LED_CIRCULAR_24); // clock pin D6, data pin, orientation
int button = 3; //Digital pin 4

//the basis of our calculator
int num1, num2 = 0;
String operation = "";
int result;
int tempNum;

int operationsComplete = 1; //once all three are complete we will display the results
int binaryResult[24]; // our array that represent what's getting lit
int buttonState;

void setup() 
{
  //set our devices
  pinMode(ROTARY_ANGLE_SENSOR, INPUT);
  pinMode(button,INPUT);
  attachInterrupt(digitalPinToInterrupt(button), changeState, FALLING);
  //where would we be without the Serial
  Serial.begin(9600);
  
  LEDbar.begin();
  
  //initialize all of the values to zero
  memset(binaryResult,0,sizeof(binaryResult));
  
  //for our random number generation
  randomSeed(analogRead(0));
}

void changeState() {
  buttonState = 1;
  delay(200);
}


float getLightLevel()
{
  // Take reading, measure voltage, and calculate 
  // number of LEDs which should be turned on
  int reading = analogRead(ROTARY_ANGLE_SENSOR); 
  float voltage = (float)reading*ADC_REF/1023;
  //get the number of degrees using our constants
  float degs = (voltage*MAX_ANGLE)/GROVE_VCC;
  //return the degrees to be set for another use
  return degs;
}

int * getBinaryString(int num) {
    int index = 0;
    //a basic decimal to binary algorithm,
    while (num > 0) {
      binaryResult[index] = num % 2;
      num = num /2;
      index++;
    }
    //cannot forget to reverse the binary string
    return reverseArray(binaryResult,24);
}

int * reverseArray(int * arr, int size)
{
    //loop through replacing the outermost values within the bounds 
    //until all have been reversed
    for (int i = 0; i < size/2; ++i)
    {
        int temp = arr[i];
        arr[i] = arr[size - 1 - i];
        arr[size - 1 - i] = temp;
    }
    //return the reversed array  
    return arr;
}

void setLightLevel(int *binaryNum) {
    //make the binaryResult
    Serial.println("The binary result: ");
    for (int i = 0; i < NUM_OF_LEDS; i++) {
       Serial.print(binaryNum[i]);
      if (binaryNum[i] == 1) {
        //set the appropriate leds
        LEDbar.setLed(i,1);
      }
    }
    Serial.println();
    //new line after the prints
}

void TurnOffLeds() {
    // Turn off the rest of the LEDs in reverse order
    for (int j = NUM_OF_LEDS; j > 0; j--) {
        // Turn off 
        LEDbar.setLed(j, 0);
        delay(100);
    }
}
//simple function that converts our degrees to a number between 0 and 250
int mapToNumber(float degs) {
  return map(degs,0,300,0,250);
}
//a simple function to get our operation from the angle sensor
String mapToOperation(float degs) {
  //four operations
  int mapping = map(degs,0,300,1,4);
  //send one back
  switch (mapping) {
    case 1:
      return "*";
    case 2:
      return "+";
    case 3:
      return "-";
    case 4:
      return "/";
  }
}

void loop() 
{
  //cases for each phase of the calculation process
  //1. Obtain the first number
  //2. Get the operand
  //3. The second number and perform the calculation
  //for the numbers, a random will be generated if the number is > 200
  tempNum = mapToNumber(getLightLevel());

  switch (operationsComplete)
  {
  case 1:
    Serial.println("Turn the dial to generate a number between 0 and 250");
    if (tempNum < 201) {
        Serial.println(mapToNumber(getLightLevel()));
    }
    else {
      Serial.println("Random number between 0 and 200");
    }
    
    break;
  case 2:
    Serial.println("Turn the dial to generate + * / or -");
    Serial.println(mapToOperation(getLightLevel()));
    break;
  case 3:
    Serial.println("Turn the dial to generate the second number between 0 and 250");
    if (tempNum < 201) {
        Serial.println(mapToNumber(getLightLevel()));
    }
    else {
      Serial.println("Random number between 0 and 200");
    }
    break;
  default:
    break;
  }
  

  //if the button has been pressed
  if (buttonState == 1) {

      switch(operationsComplete) {
        case 1:
          //we are saving the first number and are now going to move to the operand
          if (tempNum < 201) {
              num1 = tempNum;
          }
          else {
            num1 = random(200);
          }
          operationsComplete++;
          delay(1000);
          break;
        case 2:
          //save the operation and continue
          operation = mapToOperation(getLightLevel());
          operationsComplete++;
          delay(1000);
          break;
        case 3:
          //we are saving the first number and are now going to move to the operand
          if (tempNum < 201) {
              num2 = tempNum;
          }
          else {
            num2 = random(200);
          }
          Serial.print("You entered "); Serial.print(num1); Serial.print(operation + num2);
          Serial.println();
          TurnOffLeds();
          Serial.println(getTotal());
          setLightLevel(getBinaryString( abs ( getTotal() ) ) );
          operationsComplete = 1;
          //initialize all of the values to zero
          memset(binaryResult,0,sizeof(binaryResult));
          delay(1000);
          break;
        default:
          break;
  
      }
      buttonState = 0;
  } 
}

int getTotal() {
  if (operation == "*") {
    return num1 * num2;
  }
  else if (operation == "+") {
    return num1 + num2;
  }
  else if (operation == "/") {
    return num1 / num2;
  }
  else {
    return num1 - num2;
  }
}

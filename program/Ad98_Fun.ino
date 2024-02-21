#include <AD9833.h>   // LIbrary for AD9833 Module
#include <Wire.h> // Wire Library for OLED
#include <Adafruit_GFX.h> // Support Library for OLED
#include <Adafruit_SSD1306.h> // OLED library
#include <math.h> // Math Library
#define SCREEN_WIDATA_PINH 128 // OLED display Width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SET_FREQUENCY_HZ A2 // Pushbutton To Set Frequency In Hz
#define SET_FREQUENCY_KHZ A3 // Pushbutton To Set Frequency In Khz
#define SET_FREQUENCY_MHZ A6 // Pushbutton To Set Frequency In Mhz
#define ENABLE_DISABLE_OUTPUT_PIN A7 // Pushbutton To Enable / Disable the Output
#define FNC_PIN 4 // Fsync Required by the AD9833 Module
#define CLK_PIN 8 // Clock Pin of the Encoder
#define DATA_PIN 7 // Data Pin of the Encoder
#define BTN_PIN 9 // Internal Push Button on the Encoder
int counter = 1; // This Counter value will increas or decreas if when the rotarty encoder is turned
int clockPin; // Placeholder por pin status used by the rotary encoder
int clockPinState; // Placeholder por pin status used by the rotary encoder
unsigned long time = 0; // Used for debouncing
unsigned long moduleFrequency; // used to set output frequency

long debounce = 220; // Debounce delay
bool btn_state; // used to enable disable output of the AD98333 Module
bool set_frequency_hz = 1; // Defult frequency of the AD9833 Module
bool set_frequency_khz;
bool set_frequency_mhz;
String waveSelect = "SIN"; // Startup waveform of the module
int encoder_btn_count = 0; // used to check encoder button press
Adafruit_SSD1306 display(SCREEN_WIDATA_PINH, SCREEN_HEIGHT, &Wire, -1);
AD9833 gen(FNC_PIN);
void setup() {
  Serial.begin(9600);
  gen.Begin(); // This MUST be the first command after declaring the AD9833 object
  pinMode(CLK_PIN, INPUT);
  pinMode(DATA_PIN, INPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  clockPinState = digitalRead(CLK_PIN);
  pinMode(SET_FREQUENCY_HZ, INPUT);
  pinMode(SET_FREQUENCY_KHZ, INPUT);
  pinMode(SET_FREQUENCY_MHZ, INPUT);
  pinMode(ENABLE_DISABLE_OUTPUT_PIN, INPUT);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay(); // Clear the Screen
  display.setTextSize(2); // Set text Size
  display.setTextColor(WHITE); // set LCD Colour
  display.setCursor(10, 10); // Set Cursor Position
  display.println("Function Generator"); // Print the this Text
  display.setCursor(10, 25);  // Set Cursor Position
  display.println("Ankush Darade"); // Print the this Text
  display.setCursor(10, 35); // Set Cursor Position
  display.println("Shubhada Dandawate"); // Print the this Text
  display.setCursor(10, 45); // Set Cursor Position
  display.println("Hetal Kini"); // Print the this Text
  display.display(); // Update the Display
  delay(2000); // Delay of 2 SEC
  update_display(); // Call update_display Function
}
void loop()
{
  clockPin = digitalRead(CLK_PIN);
  if (clockPin != clockPinState  && clockPin == 1) {
    if (digitalRead(DATA_PIN) != clockPin) {
      counter --;
    }
    else {
      counter ++;// Encoder is rotating CW so increment
    }
    if (counter < 1 ) counter = 1;
    Serial.println(counter);
    update_display();
  }
  clockPinState = clockPin; // Remember last CLK_PIN state
  //If we detect LOW signal, button is pressed
  if ( digitalRead(BTN_PIN) == LOW && millis() - time > debounce) {
    encoder_btn_count++; // Increment the values
    if (encoder_btn_count > 2) // if value is grater that 2 reset it to 0
    {
      encoder_btn_count = 0;
    }
    if (encoder_btn_count == 0) { // if the value is 0 sine wave is selected
      waveSelect = "SIN"; // update the string variable with sin value
      update_display(); // update the display
    }
    if (encoder_btn_count == 1) { // if the value is 1 square wave is selected
      waveSelect = "SQR"; // update the string variable with SQR value
      update_display(); // update the display
    }
    if (encoder_btn_count == 2) { // if the value is 1 Triangular wave is selected
      waveSelect = "TRI";  // update the string variable with TRI value
      update_display();// update the display
    }
    time = millis(); // update the time variable
  }
  // Check buttton press action with analogread method
  // Put in a slight delay to help debounce the reading
  if (analogRead(SET_FREQUENCY_HZ) < 30 && millis() - time > debounce) { // check analogpin with debounce delay
    //update boolean  values
    set_frequency_hz = 1;
    set_frequency_khz = 0;
    set_frequency_mhz = 0;
    update_display();// update the display
    time = millis();// update the time variable
  }
  if (analogRead(SET_FREQUENCY_KHZ) < 30 && millis() - time > debounce) { // check analogpin with debounce delay
    //update boolean  values
    set_frequency_hz = 0;
    set_frequency_khz = 1;
    set_frequency_mhz = 0;
    moduleFrequency = counter * 1000;
    update_display();// update the display
    time = millis();// update the time variable
  }
  if (analogRead(SET_FREQUENCY_MHZ) < 30 && millis() - time > debounce ) { // check analogpin with debounce delay
    //update boolean  values
    set_frequency_hz = 0;
    set_frequency_khz = 0;
    set_frequency_mhz = 1;
    moduleFrequency = counter * 1000000;
    update_display();// update the display
    time = millis();// update the time variable
  }
  if (analogRead(ENABLE_DISABLE_OUTPUT_PIN) < 30 && millis() - time > debounce ) {// check analogpin with debounce delay
    btn_state = ! btn_state; // Invert the button state
    gen.EnableOutput(btn_state); // Enable / Disable output of the function generator depending on button state
    update_display();// update the display

    time = millis();// update the time variable
  }
}
void update_display()
{
  display.clearDisplay(); // FIrst clear the display
  display.setTextSize(1); //set text Size
  display.setCursor(10, 0); // Set cursor position
  display.println("Function Generator"); //print the text
  display.setTextSize(2);//set text Size
  display.setCursor(0, 20);//Set cursor position
  if (set_frequency_hz == 1 && set_frequency_khz == 0 && set_frequency_mhz == 0 ) { // check if button for setting the frequency in Hz is pressed
    moduleFrequency = counter; //updayte teh moduleFrequency variable with current counter value
  }
  if (set_frequency_hz == 0 && set_frequency_khz == 1 && set_frequency_mhz == 0 ) { // check if button for setting the frequency in KHz is pressed
    moduleFrequency = counter * 1000;//updayte teh moduleFrequency variable with current counter value but we multiply 1000 to set it on KHZ
  }
  if (set_frequency_hz == 0 && set_frequency_khz == 0 && set_frequency_mhz == 1) { // check if button for setting the frequency in MHz is pressed
    moduleFrequency = counter * 1000000;
    if (moduleFrequency > 12000000)
    {
      moduleFrequency = 12000000; // do not let the frequency to be grater that 12Mhz
      counter = 12;
    }
  }
  if (waveSelect == "SIN") { // Sine wave is selected
    display.println("SIN");
    gen.ApplySignal(SINE_WAVE, REG0, moduleFrequency);
    Serial.println(moduleFrequency);
  }
  if (waveSelect == "SQR") {// Sqr wave is selected
    display.println("SQR");
    gen.ApplySignal(SQUARE_WAVE, REG0, moduleFrequency);
    Serial.println(moduleFrequency);
  }
  if (waveSelect == "TRI" ) {// Tri wave is selected
    display.println("TRI");
    gen.ApplySignal(TRIANGLE_WAVE, REG0, moduleFrequency); // update the AD9833 module.
    Serial.println(moduleFrequency);
  }
  display.setCursor(45, 20);
  display.println(counter); // print the counter information on teh display.
  if (set_frequency_hz == 1 && set_frequency_khz == 0 && set_frequency_mhz == 0 ) {
    display.setCursor(90, 20);
    display.println("Hz"); // print Hz on the display
    display.display(); // when all set update the display
  }
  if (set_frequency_hz == 0 && set_frequency_khz == 1 && set_frequency_mhz == 0 ) {
    display.setCursor(90, 20);
    display.println("Khz");
    display.display(); // when all set update the display
  }
  if (set_frequency_hz == 0 && set_frequency_khz == 0 && set_frequency_mhz == 1) {
    display.setCursor(90, 20);
    display.println("Mhz");
    display.display(); // when all set update the display
  }
  if (btn_state) { 
    display.setTextSize(1);
    display.setCursor(65, 45);
    display.print("Output ON"); // print output on to the display
    display.display();
    display.setTextSize(2);
  }
  else {
    display.setTextSize(1);
    display.setCursor(65, 45);
    display.print("Output OFF"); // print output off to the display
    display.display();
    display.setTextSize(2);
  }
}

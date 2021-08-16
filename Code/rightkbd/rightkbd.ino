/* Author:  Ryan Robinson
 * Class:   ECE561
 * Prof:    Pascricha
 * 
 * File Description:
 * Code for keyboard right half, also acts as a slave
 */

#include <Bounce.h>
#include <i2c_t3.h>
#define del 1 // Delay for bounce and for loop
#define MEM_LEN 54 // 6 rows * 9 collumns = 54, 54 bytes


#define ROWS 6 // 6 rows
byte rowPins[ROWS] = {0, 1, 3, 4, 6, 8}; // row pins (INPUT_PULLUP)

#define COLS 9 // 9 columns
byte colPins[COLS] = {9,22,23,24,25,26,27,28,29}; // column pins (OUTPUT)

// Ugly and redundent, but bounce2 fails. Copy pins from colPins
Bounce b[COLS] = {Bounce(9, del), Bounce(22, del),Bounce(23, del),Bounce(24, del),Bounce(25, del),Bounce(26, del), Bounce(27, del), Bounce(28, del), Bounce(29, del)};


/*
// keys array indicates the keymap
int keys[ROWS][COLS] = {
  // col1     col2       col3       col4           col5           col6            col7             col8           col9
  {KEY_F7,    KEY_F8,    KEY_F9,    KEY_F10,       KEY_F11,       KEY_F12,        KEY_PRINTSCREEN, KEY_PAUSE,     KEY_DELETE},    // row1
  {KEY_7,     KEY_8,     KEY_9,     KEY_0,         KEY_MINUS,     KEY_EQUAL,      KEY_BACKSPACE,   KEY_BACKSPACE, KEY_HOME},      // row2
  {KEY_Y,     KEY_U,     KEY_I,     KEY_O,         KEY_P,         KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEY_PAGE_UP},   // row3
  {KEY_H,     KEY_J,     KEY_K,     KEY_L,         KEY_SEMICOLON, KEY_QUOTE,      KEY_ENTER,       KEY_ENTER,     KEY_PAGE_DOWN}, // row4
  {KEY_N,     KEY_M,     KEY_COMMA, KEY_PERIOD,    KEY_SLASH,     KEY_RIGHT_SHIFT,KEY_RIGHT_SHIFT, KEY_UP,        KEY_END},       // row5
  {KEY_SPACE, KEY_SPACE, KEY_SPACE, KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_RIGHT_CTRL, KEY_LEFT,        KEY_DOWN,      KEY_RIGHT}      // row6
};
*/

// kstatus indicates is used to tell when a key switch changes and how it changes, i.e. was it pressed or released?
char kstatus[ROWS][COLS]; // Has to be a char type to play nicely with i2c_t3

void requestEvent(void); // Prototype function

void setup() {  
  memset(kstatus,HIGH,MEM_LEN); // Pullup resistors used at inputs, so HIGH = not pressed, LOW = pressed

  Wire.begin(I2C_SLAVE, 0x66, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000); // Setup for Slave mode, address 0x66, pins 18/19, external pullups, 400kHz

  for(int j = 0; j < COLS; j++){
    pinMode(colPins[j],INPUT_PULLUP); // Attach debouncer to input pins and configure to pullup
    
  }
  for(int i = 0; i < ROWS; i++){ 
    pinMode(rowPins[i], OUTPUT);          // Configure output pins
    digitalWrite(rowPins[i], HIGH);       // Default output pins to HIGH
  }

  Wire.onRequest(requestEvent); // Register event
}

void loop() {
  // Row loop
  for(int i = 0; i < ROWS; i++){ 
    
    digitalWrite(rowPins[i], LOW); // Set active row to low
    delay(del); // Delay to let row pins settle
    
    // Column loop
    for(int j = 0; j < COLS; j++){
      
      b[j].update(); // Update bounce instance
      
      // Has there been a change?
      if((b[j].read() != kstatus[i][j])){
        
        // Is it being pressed or let go?
        if((b[j].read() == LOW)){
          kstatus[i][j] = LOW;          // Update status matrix
          //Keyboard.press(keys[i][j]);
        } else {
          kstatus[i][j] = HIGH;         // Update status matrix
          //Keyboard.release(keys[i][j]);
        }
      }
    }
    
    digitalWrite(rowPins[i], HIGH); // Set active row back to high    
  } // End of Matrix scan
} 

// Tx Event, outgoing I2C data
void requestEvent(void)
{
    Wire.write(&kstatus[0][0], MEM_LEN); // fill Tx buffer (send full mem)
    // Note: i2c_t3 does not like 2D array inputs/outputs, but you can trick it by feeding it an address.
}

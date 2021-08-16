/* Author:  Ryan Robinson
 * Class:   ECE561
 * Prof:    Pascricha
 * 
 * File Description:
 * Code for keyboard left half, also acts as a master
 */


#include <Bounce.h>
#include <i2c_t3.h>
#include <TeensyView.h>


#define ROWS 6 // 6 rows
byte rowPins[ROWS] = {0, 1, 3, 4, 6, 8}; // row pins (INPUT_PULLUP)

#define COLS 7 // 7 columns
byte colPins[COLS] = {9,22,23,24,25,26,27}; // column pins (OUTPUT)

// Ugly and redundent, but bounce2 fails. Copy pins from colPins
#define DEL 1 // Delay for bounce and for loop
Bounce b[COLS] = {Bounce(9, DEL), Bounce(22, DEL),Bounce(23, DEL),Bounce(24, DEL),Bounce(25, DEL),Bounce(26, DEL),Bounce(27, DEL)};

// kstatus indicates is used to tell when a key switch changes and how it changes, i.e. was it pressed or released?
char kstatus[ROWS][COLS];


#define LAYERS 2
int layer = 0; // Global variable that keeps track of the currect keyboard layer
int l_keys[LAYERS][ROWS][COLS] = {{
  // Layer = 0
  // col1             col2              col3              col4        col5        col6        col7
  {KEY_ESC,           KEY_F1,           KEY_F2,           KEY_F3,     KEY_F4,     KEY_F5,     KEY_F6},    // row1
  {KEY_TILDE,         KEY_1,            KEY_2,            KEY_3,      KEY_4,      KEY_5,      KEY_6},     // row2
  {KEY_TAB,           KEY_Q,            KEY_W,            KEY_E,      KEY_R,      KEY_T,      KEY_F20},   // row3
  {KEY_CAPS_LOCK,     KEY_A,            KEY_S,            KEY_D,      KEY_F,      KEY_G,      KEY_H},     // row4
  {MODIFIERKEY_SHIFT, KEY_Z,            KEY_X,            KEY_C,      KEY_V,      KEY_B,      KEY_N},     // row5
  {MODIFIERKEY_CTRL,  MODIFIERKEY_GUI,  MODIFIERKEY_ALT,  KEY_SPACE,  KEY_SPACE,  KEY_SPACE,  KEY_SPACE}  // row6
},

{
  // Layer = 1
  // col1             col2              col3              col4        col5        col6        col7
  {KEY_ESC,           KEY_F1,           KEY_F2,           KEY_F3,     KEY_F4,     KEY_F5,     KEY_F6},          // row1
  {KEY_TILDE,         KEY_1,            KEY_2,            KEY_3,      KEY_4,      KEY_5,      KEY_6},           // row2
  {KEY_TAB,           KEY_Q,            KEY_UP,           KEY_E,      KEY_R,      KEY_T,      KEY_F20},         // row3
  {MODIFIERKEY_SHIFT, KEY_LEFT,         KEY_DOWN,         KEY_RIGHT,  KEY_ENTER,  KEY_ENTER,  KEY_H},           // row4
  {MODIFIERKEY_SHIFT, KEY_Z,            KEY_X,            KEY_C,      KEY_V,      KEY_B,      KEY_RIGHT_SHIFT}, // row5
  {MODIFIERKEY_CTRL,  MODIFIERKEY_GUI,  MODIFIERKEY_ALT,  KEY_SPACE,  KEY_SPACE,  KEY_SPACE,  KEY_SPACE}        // row6
}};

// Keys that modify the state of the keyboard
byte l_special_keys[ROWS][COLS] = {
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0},
};

// Setup status matrix for right keyboard
#define MEM_LEN 54
#define R_ROWS 6
#define R_COLS 9
char r_kstatus_new[R_ROWS][R_COLS]; // array that the slave writes to
char r_kstatus_old[R_ROWS][R_COLS]; // array the is checked against to determine if a key is pressed or released
uint8_t target = 0x66; // target Slave address

int r_keys[LAYERS][R_ROWS][R_COLS] = {{
  // Layer = 0
  // col1     col2       col3       col4           col5           col6            col7             col8           col9
  {KEY_F7,    KEY_F8,    KEY_F9,    KEY_F10,       KEY_F11,       KEY_F12,        KEY_PRINTSCREEN, KEY_PAUSE,     KEY_DELETE},    // row1
  {KEY_7,     KEY_8,     KEY_9,     KEY_0,         KEY_MINUS,     KEY_EQUAL,      KEY_BACKSPACE,   KEY_BACKSPACE, KEY_HOME},      // row2
  {KEY_Y,     KEY_U,     KEY_I,     KEY_O,         KEY_P,         KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEY_PAGE_UP},   // row3
  {KEY_H,     KEY_J,     KEY_K,     KEY_L,         KEY_SEMICOLON, KEY_QUOTE,      KEY_ENTER,       KEY_ENTER,     KEY_PAGE_DOWN}, // row4
  {KEY_N,     KEY_M,     KEY_COMMA, KEY_PERIOD,    KEY_SLASH,     KEY_RIGHT_SHIFT,KEY_RIGHT_SHIFT, KEY_UP,        KEY_END},       // row5
  {KEY_SPACE, KEY_SPACE, KEY_SPACE, KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_RIGHT_CTRL, KEY_LEFT,        KEY_DOWN,      KEY_RIGHT}      // row6
},

{
  // Layer = 1
  // col1     col2       col3       col4           col5           col6            col7             col8           col9
  {KEY_F7,    KEY_F8,    KEY_F9,    KEY_F10,       KEY_F11,       KEY_F12,        KEY_PRINTSCREEN, KEY_PAUSE,     KEY_DELETE},    // row1
  {KEY_7,     KEY_8,     KEY_9,     KEY_0,         KEY_MINUS,     KEY_EQUAL,      KEY_BACKSPACE,   KEY_BACKSPACE, KEY_HOME},      // row2
  {KEY_Y,     KEY_U,     KEY_I,     KEY_O,         KEY_P,         KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_BACKSLASH, KEY_PAGE_UP},   // row3
  {KEY_H,     KEY_J,     KEY_K,     KEY_L,         KEY_SEMICOLON, KEY_QUOTE,      KEY_ENTER,       KEY_ENTER,     KEY_PAGE_DOWN}, // row4
  {KEY_N,     KEY_M,     KEY_COMMA, KEY_PERIOD,    KEY_SLASH,     KEY_RIGHT_SHIFT,KEY_RIGHT_SHIFT, KEY_UP,        KEY_END},       // row5
  {KEY_SPACE, KEY_SPACE, KEY_SPACE, KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_RIGHT_CTRL, KEY_LEFT,        KEY_DOWN,      KEY_RIGHT}      // row6
}};


// Defines used for joystick
#define JB 12 // Joystick button pin
#define JX 2  // Analog 2 input pin
#define JY 3  // Analog 3 input pin
#define MOUSE_RANGE 20
#define MOUSE_CENTER 10
#define A_READ_MAX 1023
#define A_READ_CENTER 511
int xoffst;
int yoffst;
int xjmax = 100;
int yjmax = 100;
//Bounce button0 = Bounce(JB, 10);

// TeensyView
#define PIN_RESET 15
#define PIN_DC    5
#define PIN_CS    10
#define PIN_SCK   13
#define PIN_MOSI  11
TeensyView oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);

// prototyping functions
void joy();
void rkbscan();
void lkbscan();
int boundval();
void oledset0();
void oledset1();

void setup() {  
  // Right keyboard (slave)
  memset(r_kstatus_new,HIGH,MEM_LEN); // Status matrix that the i2c communication writes to
  memset(r_kstatus_old,HIGH,MEM_LEN); // Status matrix that we check against to find changes
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
  Wire.setDefaultTimeout(200000); // 200ms, lets check this out some more later

  // Left keyboard (master)
  memset(kstatus,HIGH,sizeof(kstatus)); // Pullup resistors used at inputs, so HIGH = not pressed, LOW = pressed

  // Configure input column pins for left (this) keyboard
  for(int j = 0; j < COLS; j++){
    pinMode(colPins[j],INPUT_PULLUP); // Attach debouncer to input pins and configure to pullup
  }

  // Configure output row pins for left (this) keyboard
  for(int i = 0; i < ROWS; i++){ 
    pinMode(rowPins[i], OUTPUT);          // Configure output pins
    digitalWrite(rowPins[i], HIGH);       // Default output pins to HIGH
  }

  // Correcting code. Sadly, it seems the joystick is biased heavily
  yoffst = analogRead(JY) - A_READ_CENTER; // Note: will cause errors if powed on while joystick isn't in a neutral position.
  xoffst = analogRead(JX) - A_READ_CENTER;

  // TeensyView Setup
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.

  oledset(); // Set the starting oled screen

  // Serial test code
  //Serial.begin(9600);
  //delay(1000);
}





void loop() {

  joy();     // Handles joystick activity  
  rkbscan(); // Handles left keyboard activity
  lkbscan(); // Handles right keyboard activity

} // End of loop






void joy(){
  // Read joystick and apply correcting offset and value bounds.
  int xv = analogRead(JX) - xoffst;
  int yv = analogRead(JY) - yoffst;

  // Determine if calibration factors need to be updated
  if(xv > xjmax) xjmax = xv;
  if(yv > yjmax) yjmax = yv;

  // Apply calibration factors
  xv = (((xv - A_READ_CENTER) * A_READ_CENTER) / (xjmax - A_READ_CENTER)) + A_READ_CENTER;
  yv = (((yv - A_READ_CENTER) * A_READ_CENTER) / (yjmax - A_READ_CENTER)) + A_READ_CENTER;
  
  // Bound values
  xv = boundval(xv);
  yv = boundval(yv);

  // Activate joystick or mouse
  if(layer == 0){

    // Map large range onto smaller range
    int xmouse = map(xv, 0, A_READ_MAX, 0, MOUSE_RANGE)-MOUSE_CENTER;
    int ymouse = map(yv, 0, A_READ_MAX, 0, MOUSE_RANGE)-MOUSE_CENTER;
    Mouse.move(xmouse,ymouse);
    
  } else {
    Joystick.X(xv);
    Joystick.Y(yv);
  }
}

int boundval(int val){  
  if(val <= 0)               return 0;
  else if(val >= A_READ_MAX) return A_READ_MAX;
  else                       return val;
}

void rkbscan(){
  // Matrix scan
  for(int i = 0; i < ROWS; i++){ 
    digitalWrite(rowPins[i], LOW); // Set active row to low
    delay(DEL); // Delay to let row pins settle
    
    // col update
    for(int j = 0; j < COLS; j++){
      b[j].update();                                // Update bounce instance
      
      // Has there been a change?
      if((b[j].read() != kstatus[i][j])){
        
        // Is it being pressed or let go?
        if((b[j].read() == LOW)){
          
          // A key is being pressed
          if((l_special_keys[i][j] == 0)){
            Keyboard.press(l_keys[layer][i][j]);    // Send keyboard press
          }
          kstatus[i][j] = LOW;                      // Update status matrix for key being pressed
          
        } else { 
          
          // A key is being released
          // Is it a special key?
          if((l_special_keys[i][j] == 0)){

            // It is a normal keypress
            Keyboard.release(l_keys[layer][i][j]);      // Send keyboard release
            kstatus[i][j] = HIGH;                       // Update status matrix for key being released
          } else {

            // It is a special keypress, only registers on release
            Keyboard.releaseAll();                      // release all keys before changing layers
            memset(kstatus,HIGH,sizeof(kstatus));       // Update the status of left keys to that of released.
            memset(r_kstatus_old,HIGH,MEM_LEN);         // Update the status of left keys to that of released.
            
            // Moving to a special layer or going back to original?
            if(layer == l_special_keys[i][j]){
              layer = 0;                                // Return to first layer              
            } else {
              layer = l_special_keys[i][j];             // Set the layer
            }
            oledset(); // Set the oled screen for the respective layer
          }
        }
      } 
    }
    digitalWrite(rowPins[i], HIGH); // Set active row back to high    
  } // End of Matrix scan
}



void lkbscan(){
  // i2c read
  Wire.requestFrom(target, (size_t)MEM_LEN);
  if(!Wire.getError()){
    // If no error then read Rx data into buffer and print
    Wire.read(&r_kstatus_new[0][0], Wire.available());
    // Note: i2c_t3 does not like 2D array inputs/outputs, but you can trick it by feeding it an address.
  }

  // Check for changes in right keyboard
  for(int i = 0; i < R_ROWS; i++){
    for(int j = 0; j < R_COLS; j++){
      if((r_kstatus_new[i][j] != r_kstatus_old[i][j])){
        if(r_kstatus_new[i][j] == LOW){
          Keyboard.press(r_keys[layer][i][j]);
          r_kstatus_old[i][j] = LOW;
        } else {
          Keyboard.release(r_keys[layer][i][j]);
          r_kstatus_old[i][j] = HIGH;
        }
      }
    }
  } // End of right keyboard changes check
}






void oledset(){
  if(layer == 0){
    oled.clear(PAGE);  // Clear the page
    oled.setFontType(1);  // Set font to type 1
    oled.setCursor(5, 5); // move cursor
    oled.print("Layer: 0");  // Write a byte out as a character
    oled.setFontType(0);  // Set font to type 0
    oled.setCursor(10, 20); // move cursor
    oled.print("Mouse + Keyboard");  // Write a byte out as a character
    oled.display();  // Send the PAGE to the OLED memory
    return;
  }
  if(layer == 1){
    oled.clear(PAGE);  // Clear the page
    oled.setFontType(1);  // Set font to type 1
    oled.setCursor(5, 5); // move cursor
    oled.print("Layer: 1");  // Write a byte out as a character
    oled.setFontType(0);  // Set font to type 0
    oled.setCursor(10, 20); // move cursor
    oled.print("Joystick + Gamepad");  // Write a byte out as a character
    oled.display();  // Send the PAGE to the OLED memory
    return;
  }
}



 

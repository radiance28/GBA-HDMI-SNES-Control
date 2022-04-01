/*
  SNES -> GBA
  based on the code found at https://github.com/burks10/Arduino-SNES-Controller
*/

/*
  SNES Controller ->  Arduino
  -----------------\
  | 1 2 3 4 | 5 6 7 |
  -----------------/

  Pin 1: +5V
  Pin 2: Clock  ->  Arduino A0
  Pin 3: Latch  ->  Arduino A1
  Pin 4: Serial ->  Arduino A2
  Pin 7: GND
*/

int DATA_CLOCK    = A0;
int DATA_LATCH    = A1;
int DATA_SERIAL   = A2;

/*
    OGBA     	  -> 	    Arduino Nano
  --------      	      -------
   (GND)              		GND
   (Select, TP2)      		D2
   (Start,  TP3)      		D3
   (Up,     TP6)      		D4
   (Down,   TP7)      		D5
   (Left,   TP5)      		D6
   (Right,  TP4)      		D7
   (B,      TP1)      		D8
   (A,      TP0)      		D9
   (Y,     "BB")      		D12
   (X,     "AA")      		D13
   (L,      TP9)      		D10
   (R,      TP8)      		D11
   (RESET)            		A3

   For "turbo" function wire on arduino board:
   A4 to D8 pin,
   A5 to D9 pin.
*/

#define PIN_SELECT  2
#define PIN_START   3
#define PIN_UP      4
#define PIN_DOWN    5
#define PIN_LEFT    6
#define PIN_RIGHT   7
#define PIN_B       8
#define PIN_A       9
#define PIN_Y       12
#define PIN_X       13
#define PIN_L       10
#define PIN_R       11
#define PIN_RESET   17

int buttons_state[12];  // B,Y,Sel,Start,U,D,L,R,A,X,L,R
int output_pins[12] = { PIN_B, PIN_Y, PIN_SELECT, PIN_START, PIN_UP, PIN_DOWN,
                        PIN_LEFT, PIN_RIGHT, PIN_A, PIN_X, PIN_L, PIN_R
                      };

uint32_t timer, timer_2; // turbo timer
int mode = INPUT;
int mode_2 = INPUT;

void setup ()
{
  pinMode(DATA_CLOCK, OUTPUT);
  digitalWrite (DATA_CLOCK, HIGH);

  pinMode(DATA_LATCH, OUTPUT);
  digitalWrite(DATA_LATCH, LOW);

  pinMode(DATA_SERIAL, INPUT_PULLUP);

  pinMode(PIN_RESET, INPUT);
}

void loop ()
{
  // Read from the SNES controller

  /* Latch for 12us */
  digitalWrite(DATA_LATCH, HIGH);
  delayMicroseconds(12);
  digitalWrite(DATA_LATCH, LOW);
  delayMicroseconds(6);

  /* Read data bit by bit from DATA_SERIAL */
  for (int i = 0; i < 16; i++) {
    digitalWrite(DATA_CLOCK, LOW);
    delayMicroseconds(6);
    if (i <= 11)
      buttons_state[i] = digitalRead(DATA_SERIAL);
    digitalWrite(DATA_CLOCK, HIGH);
    delayMicroseconds(6);
  }

  // Output the level on our arduino digital pins
  // pulling them low if the equivalent SNES button
  // is pushed
  for (int i = 0; i <= 11; i++)
  {
    if (output_pins[i] != 0)
    {
      if (!buttons_state[i])
      {
        digitalWrite(output_pins[i], LOW);
        pinMode(output_pins[i], OUTPUT);
      }
      else
        pinMode(output_pins[i], INPUT);
    }
  }

  // Reset pulse by combination "L+R+Select+Start"
  if (buttons_state[2] == 0 && buttons_state[3] == 0 && buttons_state[10] == 0 && buttons_state[11] == 0) {
    pinMode(PIN_RESET, OUTPUT);
    digitalWrite(PIN_RESET, LOW);
  }
  else
    pinMode(PIN_RESET, INPUT);

  //turbo A by X, pin A5 (12.5 Hz)
  if (buttons_state[9] == 0) {
    pinMode(PIN_A, INPUT);
    if (millis() - timer >= 80) {
      timer = millis();
      if (mode == INPUT)
        mode = OUTPUT;
      else
        mode = INPUT;
      pinMode(19, mode);
    }
  } else
    pinMode(19, INPUT);

  //turbo B by Y, pin A4 (12.5 Hz)
  if (buttons_state[1] == 0) {
    pinMode(PIN_B, INPUT);
    if (millis() - timer_2 >= 80) {
      timer_2 = millis();
      if (mode_2 == INPUT)
        mode_2 = OUTPUT;
      else
        mode_2 = INPUT;
      pinMode(18, mode_2);
    }
  } else
    pinMode(18, INPUT);

  delay(5);
}

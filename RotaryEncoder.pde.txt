// This is just a code snippet, provided in case it helps you.
// My setup: This encoder has pin C (middle pin of the three on one side) and 
// one of the pair on the other side connected to ground.  The other pins are 
// connected as follows to my Arduino Mega:
//      'A': digital pin 22
//      'B': digital pin 24
//      'press': digital pin 26
// In "real" code, you'd want a state machine to keep track of the rotation,
// and only take note of the "forward" or "back" when the encoder is reporting
// 0 again.

// Digital pin definitions
enum enDigitalPins
{
  // Rotary encoder input lines
  dpInEncoderA=22,
  dpInEncoderB=24,
  dpInEncoderPress=26,
};


static void _ResetPins()
{
  // Rotary encoder input lines
  // Configure as input, turn on pullup resistors
  pinMode(dpInEncoderA, INPUT);
  digitalWrite(dpInEncoderA, HIGH);
  pinMode(dpInEncoderB, INPUT);
  digitalWrite(dpInEncoderB, HIGH);
  pinMode(dpInEncoderPress, INPUT);
  digitalWrite(dpInEncoderPress, HIGH);
}


void _lowlevel_ReadEncoder(int &rotate, int& press)
{
  rotate = (digitalRead(dpInEncoderB) * 2) + digitalRead(dpInEncoderA);
  press = digitalRead(dpInEncoderPress);
}


void ReadEncoder()
{
  int Position, Press;
  int isForward = 0;
  
  _ResetPins();
  Serial.println("Reading the encoder... press a key to abort.");
  _lowlevel_ReadEncoder(Position, Press);
  while (!Serial.available())
  {
    int Position2, Press2;
    do
    {
      _lowlevel_ReadEncoder(Position2, Press2);
    } while ((Position2 == Position) && (Press2 == Press));
    if (Position2 != Position)
    {
      // "Forward" is shown by the position going from (0 to 1) or (1 to 3)
      // or (3 to 2) or (2 to 0).  Anything else indicates that the user is
      // turning the device the other way.  Remember: this is Gray code, not
      // binary.
      int isFwd = ((Position == 0) && (Position2 == 1)) ||
                  ((Position == 1) && (Position2 == 3)) ||
                  ((Position == 3) && (Position2 == 2)) ||
                  ((Position == 2) && (Position2 == 0));
      Serial.println(isFwd ? "FWD" : "BWD");
    }
    if (Press2 != Press)
    {
      Serial.println(Press ? "Press" : "Release");
    }
    Position = Position2;
    Press = Press2;
  }
}


void setup()
{
  // configure the pins
  _ResetPins();

  // init serial communication
  Serial.begin(115200); 
  Serial.println("Ready to begin");
}


void loop()
{
  ReadEncoder();
}

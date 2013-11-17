/*This program puts the servo values into an array,
 reagrdless of channel number, polarity, ppm frame length, etc...
 You can even change these while scanning!*/

#include <LiquidCrystal.h>
#include <DFR_LCD_Keypad.h>

#define PPM_Pin 3  //this must be 2 or 3
int ppm[16];  //array for storing up to 16 servo signals
long frameTime; 
double frameRate;


LiquidCrystal lcd( 8, 9, 4, 5, 6, 7 );

// initialize the keypad
DFR_LCD_Keypad keypad(A0);
int last_key, key;
int channelDisplay;
int showStats;
int numChannels;

void setup()
{
  Serial.begin(115200);
  Serial.println("ready");
  lcd.begin(16, 2);

  pinMode(PPM_Pin, INPUT);
  attachInterrupt(PPM_Pin - 2, read_ppm, CHANGE);

  TCCR1A = 0;  //reset timer1
  TCCR1B = 0;
  TCCR1B |= (1 << CS11);  //set timer1 to increment every 0,5 us

  channelDisplay = 0;
  showStats = false;
  numChannels = 0;
}

void loop()
{
  int count;
  char lcdLine[17];

  last_key = keypad.get_last_key();
  key      = keypad.read_key();
  if (key != last_key) {
	  // key has changed

	  switch (key) {
	  case KEY_RIGHT:
		  break;

	  case KEY_UP:
		  showStats = false;
		  if(channelDisplay <= 4)
			  channelDisplay = 0;
		  else
			  channelDisplay = channelDisplay - 4;

		  lcd.clear();
		  break;

	  case KEY_DOWN:
		  showStats = false;
		  if(channelDisplay >= 12)
			  channelDisplay = 12;
		  else
			  channelDisplay = channelDisplay + 4;		  
		  
		  lcd.clear();
		  break;

	  case KEY_LEFT:
		  showStats = true;
		  lcd.clear();
		  break;

	  case KEY_SELECT:
		  break;

	  case KEY_NONE:
	  default:
		  break;
	  }
  }

  if(showStats)
  {
	  lcd.setCursor( 0, 0 );
	  
	  sprintf(lcdLine,"NumCh:%2d ",numChannels);
	  lcd.print(lcdLine);
	  if(frameTime > 0)
	  {
		frameRate = 1000000.0/(double)frameTime;
	  }
	  else
	  {
		  frameRate = 0.0;
	  }
	
	  dtostrf(frameRate, 5, 1, lcdLine);
	  lcd.print(lcdLine);

	  lcd.print("Hz");

	  lcd.setCursor( 0, 1);
	  sprintf(lcdLine," 0:%4dus",ppm[0]);
	  lcd.print(lcdLine);
	  
  }
  else
  {
	  lcd.setCursor( 0, 0 );
	  sprintf(lcdLine,"%2d:%4d %2d:%4d",channelDisplay, ppm[channelDisplay], channelDisplay+1, ppm[channelDisplay+1]);
	  lcd.print(lcdLine);

	  lcd.setCursor( 0, 1 );
	  sprintf(lcdLine,"%2d:%4d %2d:%4d",channelDisplay+2, ppm[channelDisplay+2], channelDisplay+3, ppm[channelDisplay+3]);
	  lcd.print(lcdLine);

  }

  // Reset data - will be filled in before delay expires
  numChannels = 0;
  frameTime = 0;
  for (int i=0;i<16;i++)
  {
	  ppm[i]=0;
  }
  
  delay(250);  
}



void read_ppm(){  //leave this alone
  static unsigned int pulse;
  static unsigned long counter;  
 
  static unsigned long frameStartCount;
  static unsigned long tmpl;
  static byte channel;

  counter = TCNT1;
  TCNT1 = 0;

  if(counter < 1020){  //must be a pulse if less than 510us
    pulse = counter;
  }
  else if(counter > 4500){  //sync pulses over 1910us
    channel = 0;
  }
  else{  //servo values between 510us and 2420us will end up here
    ppm[channel] = (counter + pulse)/2;
	
	if(channel == 0)
	{
		tmpl = micros();
		frameTime = (tmpl-frameStartCount);
		frameStartCount = tmpl;
	}
    
	channel++;
	numChannels = max(channel, numChannels);

  }
}


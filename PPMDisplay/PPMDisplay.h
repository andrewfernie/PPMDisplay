#define LCDKEYPAD_SHIELD 0
#define PERSONAL_KEYPAD  1
#define I2C_DISPLAY 1

#define LCD_COLUMNS 16
#define LCD_ROWS 2

#define DISPLAYMODE_READSTATS 0
#define DISPLAYMODE_READ4CHAN 1
#define DISPLAYMODE_SERVOOUT 2
#define DISPLAYMODE_MAX 2

#define SERVOOUT_CENTER 0
#define SERVOOUT_MINPPM 1
#define SERVOOUT_MAXPPM 2
#define SERVOOUT_CYCLE 3
#define SERVOOUT_VARIABLE 4
#define SERVOOUT_MAX 4

#define SERVOOUT_MAX_VALUE 2000
#define SERVOOUT_MIN_VALUE 1000
#define SERVOOUT_CENTER_VALUE 1500
#define SERVOOUT_CYCLE_MILLIS 4000				// Number of milliseconds for one cycle of the servo output CYCLE mode      

#define FAST_ITERATION_MILLIS 20				// milliseconds between fast band iteration starts
#define SLOW_ITERATION_MILLIS 100				// milliseconds between slow band iteration starts
#define SUPERSLOW_ITERATION_MILLIS 500				// milliseconds between slow band iteration starts
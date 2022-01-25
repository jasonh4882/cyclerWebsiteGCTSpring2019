/* 48V Battery Cycler

   Version 5.1.0
   Edited 10/15/19

   Written by
      Jason Hampshire
      Darin Kiefer

   GREEN CUBES TECHNOLOGY: MOTIVE POWER
*/

/*
   Library inclusion
*/
#include <Elegoo_GFX.h>    // Graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> // Touchscreen functionality
#include <SD.h> // Loading and saving to SD card
#include <Wire.h> //Communication with I2C
#include <Adafruit_ADS1015.h> // A-to-D
#include "Adafruit_MCP4725.h" // D-to-A
#include "max6675.h" //thermocouple library

/*
   Define constants
*/
#define VERSION_NO "5.1.0"

// Wiring for the LCD display
#define LCD_CS    A3
#define LCD_CD    A2
#define LCD_WR    A1
#define LCD_RD    A0
#define LCD_RESET A4

//pressure threshold
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// LCD display pin declarations
#define YP A2
#define XM A3
#define YM 8
#define XP 9

//Touch For New ILI9341 TP
#define TS_MINX 70
#define TS_MAXX 920

#define TS_MINY 120
#define TS_MAXY 900

//8 channel relay board
#define POWER_SUPPLY_PIN  22
#define CHARGE_PIN        23
#define DISCHARGE5_PIN    24
#define DISCHARGE33_PIN   25

//4 channel 30A relay board
#define CHARGE_POWER_1    30
#define CHARGE_POWER_2    31
#define CHARGE_POWER_3    32

//4 channel relay board
#define GND_RELAY             37 //Not used
#define SS_RELAY_GND          38 //turned on during charge, off after 15 min into chgwait
#define RELAY_GND_CONTROL     39 //controls if the 8 channel relay board is powered or not

//2 channel relay board
#define CYC_FAN         40
#define EXH_FAN         41

//define gas detector pin and threshold
#define GD_PIN A5
#define GD_THRESHOLD 700

//define thermocouple pins
#define TC_SO_PIN   34
#define TC_CS_PIN   35
#define TC_SCK_PIN  36

//definition of I2C channels
#define I2C_ADDR_CUR_CNTRL_1    0x60
#define I2C_ADDR_VOL_CNTRL_1    0x61
#define I2C_ADDR_CUR_CNTRL_2    0x62
#define I2C_ADDR_VOL_CNTRL_2    0x63

#define ADC_VOLTAGE_CHANNEL   0
#define ADC_CURRENT_CHANNEL   1
#define ADC_CURRENT2_CHANNEL  2

// Status bar
#define STATUS_X 65
#define STATUS_Y 10

//define system constants
#define WAIT      1000
#define MAX_CYCLE 2

#define MAX_DIS_TIME  36000  //10hrs(to be edited soon)
#define MAX_CHG_TIME  36000  //10hrs(to be edited soon)
#define D1WAIT_TIME   2100   //35min
#define D2WAIT_TIME   10800  //3hrs
#define CWAIT_TIME    10800  //3hrs
#define FWAIT_TIME    900    //15min

#define CURR_LOW_THRESH 5
#define VOLT_LOW_THRESH 11

//********************************************* */
#define SUPPLY_OUTPUT   210

#define HOUR    3600
#define MINUTE  60

#define ON  0
#define OFF 1

//define system states
#define BEFORE_CYCLE    0
#define STATE_START     1
#define STATE_DISCHARGE 2
#define STATE_DISWAIT   3
#define STATE_CHARGE    4
#define STATE_CHGWAIT   5
#define STATE_DONE      6

#define SCREEN_ENTRY        0
#define SCREEN_PULL         1
#define SCREEN_CHECK_SAVE   2
#define SCREEN_ENTRY_VOLTS  3
#define SCREEN_CAPACITY     4
#define SCREEN_SERIAL       5
#define SCREEN_SKIP         6
#define SCREEN_CALIBRATION  7
#define SCREEN_HOMEPAGE     8
#define SCREEN_SETTINGS     9
#define SCREEN_OPTIONS      10

// basic colors definded with their color codes
#define GREEN       0x0FC7
#define BLACK       0x0000      /*   0,   0,   0 */
#define LIGHTGREY   0xC618      /* 192, 192, 192 */
#define DARKGREY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */

#define BUFFPIXEL 10

//Model entry UI details
#define BUTTON_X 50
#define BUTTON_Y 90
#define BUTTON_W 80
#define BUTTON_H 25

#define BUTTON_SPACING_X  30
#define BUTTON_SPACING_Y  8
#define BUTTON_TEXTSIZE   2
#define BUTTON_BCOLOR     BLACK

// text box where numbers go
#define TEXT_LEN    4
#define TEXT_X      10
#define TEXT_Y      10
#define TEXT_W      300
#define TEXT_H      50
#define TEXT_TSIZE  3
#define TEXT_TCOLOR WHITE

//display spacing definitions
#define DARK_STRIPE_X   0
#define DARK_STRIPE_Y1  0
#define DARK_STRIPE_Y2  60
#define DARK_STRIPE_Y3  120
#define DARK_STRIPE_Y4  180
#define DARK_STRIPE_W   320
#define DARK_STRIPE_H   30

#define INFO_X        140
#define SUB_INFO_X    280
#define V_INFO_Y      5
#define SD_INFO_Y     17
#define BATTERY_X     7
#define BATTERY_Y     10
#define CAPACITY_X    17
#define CAPACITY_Y    40
#define SERIAL_X      17
#define SERIAL_Y      70
#define STATE_X       54
#define STATE_Y       100
#define TIME_X        66
#define TIME_Y        130
#define TEMPERATURE_X 66
#define TEMPERATURE_Y 160
#define VOLTAGE_X     30
#define VOLTAGE_Y     190
#define CURRENT_X     30
#define CURRENT_Y     220
#define CLEAR_INFO_X  180
#define CLEAR_INFO_Y  17
/*
   Global variables
*/
//declaration of the I2C variables
Adafruit_ADS1115 ads1115(0x48);
Adafruit_MCP4725 dac_voltage_cntrl;
Adafruit_MCP4725 dac_current_cntrl;

//declaration of thermocouple
MAX6675 tc(TC_SCK_PIN, TC_CS_PIN, TC_SO_PIN);

//LCD screen / touch screen declaration
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts(XP, YP, XM, YM, 300);

//declaration of the data is stored in a textfield
char combined[9] = "FBP-";
char textfield[TEXT_LEN + 1] = "";
char capText[4] = "";
char serialNum[9] = "";
char caliText[15] = "";
uint8_t textfield_i = 0;
uint8_t combined_i = 4;
uint8_t capText_i = 0;
uint8_t serialNum_i = 0;
uint8_t calibration_i = 0;

Elegoo_GFX_Button buttons[15];
Elegoo_GFX_Button YNBtns[2];
Elegoo_GFX_Button voltsBtns[6];
Elegoo_GFX_Button CSBtns[12];
Elegoo_GFX_Button SSBtns[9];
Elegoo_GFX_Button skip;
Elegoo_GFX_Button back;
Elegoo_GFX_Button homeBtns[3];
Elegoo_GFX_Button setBtns[5];
Elegoo_GFX_Button optBtns[11];

char buttonlabels[15][6] =  {"", "Clear", "Enter",
                             "1", "2", "3",
                             "4", "5", "6",
                             "7", "8", "9",
                             "Back", "0", ""
                            };
                            
char CellLbls[7][7] =       {"Switch", "24", "36", "48", "80"};

char CSBtnLbls[13][6] =     {"1", "2", "3",
                             "4", "5", "6",
                             "7", "8", "9",
                             "0", "Clear", "Enter", "."
                            };
                            
char SSBtnLbls[9][9] =      {"Start", "Dis1", "DisWait1",
                             "Chg1", "ChgWait", "Dis2",
                             "DisWait2", "Chg2"
                            };

uint16_t buttoncolors[15] = {DARKGREY, DARKGREY, RED,
                             BLUE, BLUE, BLUE,
                             BLUE, BLUE, BLUE,
                             BLUE, BLUE, BLUE,
                             RED, BLUE, DARKGREY
                            };
                            
uint16_t CSBtnClrs[12] =    {BLUE, BLUE, BLUE,
                             BLUE, BLUE, BLUE,
                             BLUE, BLUE, BLUE,
                             BLUE, DARKGREY, RED
                            };
                            
uint16_t SSBtnClrs[9] =     {RED, BLUE, BLUE,
                             BLUE, BLUE, BLUE,
                             BLUE, BLUE
                            };

//declaration of system variables
bool chgState = true;
bool manualEntry = false;
bool sSkip = false;
bool bSkip = false;
bool vConnect;
bool cConnect;
bool gasTripped = true;
bool saveSD = true;
bool isSD = false;
bool isCal = false;

double cData = 0;
double vData = 0;
double calibrationC = 1;
double calibrationV = 1;
double noise = 0;

float xVolts;
float xCurrent;

unsigned long loopDelay;
unsigned long cycleTime;
unsigned long doneTime = 0;
unsigned long nowClock;
unsigned long delayCycle = 0;
unsigned long totalTime = 0;
unsigned long sNumber = 00000000;
unsigned long now = millis();


uint16_t batteryVoltage;
uint16_t capacity;
uint16_t dcCycle;
uint16_t delayTimer;
uint16_t stateTimer;
uint16_t bulkTime;
uint16_t voltage_adc_raw = 0;
uint16_t current_adc_raw = 0;
uint8_t systemState;
uint8_t screenPage;
uint8_t place = 0;
uint8_t cycleType = 0;

String fileName;
String fileLoc = "";
String tempData;
String line;
String doneLbl;

File SDlogs;

/*
   Function prototypes
*/


void drawSkip(void);
void drawEntry(void);
void drawCapacity(void);
void drawSerialNum(void);
void drawVolts(void);
void drawOptions(void);
void drawVerify(void);
void printDirectory(File dir, int numTabs);
void drawPull(void);
void drawCalibration(void);
bool drawBMP(String BMPname, int x, int y);
uint16_t read16(File f);
uint32_t read32(File f);
void drawHome(void);
void drawSettings(void);
void setup(void);
void initLCD(void);
void initLog(void);
void printFile(String fileTag);
bool idle(bool delayIdle);
void checkSave(void);
void homeEntry(void);
void settingsEntry(void);
void isModel(String mod);
void printAll(File dir);
void pullPage(void);
void modelEntry(void);
void capacityEntry(void);
void serialEntry(void);
void voltsEntry(void);
void calibrationEntry(void);
void skipEntry(void);
void optionsEntry(void);
void startingPosition(void);
void checkSkip(void);
void disResist(float resPoint);
double calibrate(uint8_t chanNum);
float checkADC(uint8_t chanNum, bool noiseCheck = false);
void saveToSD(String st, float volt, float curr, float temp);
void checkGas(void);
String formatTime(void);
void printInfo(float volt, float curr, float temper);
void start(void);
void discharge(void);
void disWait(void);
void charge(void);
void chgWait(void);
void loop(void);
void sendData(String state, float volt, float curr, float  temp);

//draws yes and no buttons on the tft screen
void drawSkip(void) {
  Serial.print(F("Drawing skip screen..."));
  tft.fillScreen(LIGHTGREY);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(55, 15);
  tft.print("Choose Stage");
  tft.setTextSize(2);
  // create buttons, row-by-row
  SSBtns[0].initButton(&tft, 160,  BUTTON_Y - 5, // x, y, w, h, outline, fill, text
                       BUTTON_W * 2, BUTTON_H + 5, BUTTON_BCOLOR, SSBtnClrs[0], TEXT_TCOLOR,
                       "FromStart", BUTTON_TEXTSIZE);
  SSBtns[0].drawButton();

  SSBtns[1].initButton(&tft, BUTTON_X, BUTTON_Y + (BUTTON_H + 5 + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                       BUTTON_W + 20, BUTTON_H + 5, BUTTON_BCOLOR, SSBtnClrs[1], TEXT_TCOLOR,
                       SSBtnLbls[1], BUTTON_TEXTSIZE);
  SSBtns[1].drawButton();

  SSBtns[2].initButton(&tft, BUTTON_X + (BUTTON_W + BUTTON_SPACING_X),
                       BUTTON_Y + (BUTTON_H + 5 + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                       BUTTON_W + 20, BUTTON_H + 5, BUTTON_BCOLOR, SSBtnClrs[2], TEXT_TCOLOR,
                       SSBtnLbls[2], BUTTON_TEXTSIZE);
  SSBtns[2].drawButton();

  SSBtns[3].initButton(&tft, BUTTON_X + 2 * (BUTTON_W + BUTTON_SPACING_X),
                       BUTTON_Y + (BUTTON_H + 5 + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                       BUTTON_W + 20, BUTTON_H + 5, BUTTON_BCOLOR, SSBtnClrs[3], TEXT_TCOLOR,
                       SSBtnLbls[3], BUTTON_TEXTSIZE);
  SSBtns[3].drawButton();

  SSBtns[4].initButton(&tft, BUTTON_X, BUTTON_Y + 2 * (BUTTON_H + 5 + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                       BUTTON_W + 20, BUTTON_H + 5, BUTTON_BCOLOR, SSBtnClrs[4], TEXT_TCOLOR,
                       SSBtnLbls[4], BUTTON_TEXTSIZE);
  SSBtns[4].drawButton();

  SSBtns[5].initButton(&tft, BUTTON_X + (BUTTON_W + BUTTON_SPACING_X),
                       BUTTON_Y + 2 * (BUTTON_H + 5 + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                       BUTTON_W + 20, BUTTON_H + 5, BUTTON_BCOLOR, SSBtnClrs[5], TEXT_TCOLOR,
                       SSBtnLbls[5], BUTTON_TEXTSIZE);
  SSBtns[5].drawButton();

  SSBtns[6].initButton(&tft, BUTTON_X + 2 * (BUTTON_W + BUTTON_SPACING_X),
                       BUTTON_Y + 2 * (BUTTON_H + 5 + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                       BUTTON_W + 20, BUTTON_H + 5, BUTTON_BCOLOR, SSBtnClrs[6], TEXT_TCOLOR,
                       SSBtnLbls[6], BUTTON_TEXTSIZE);
  SSBtns[6].drawButton();

  SSBtns[7].initButton(&tft, BUTTON_X, BUTTON_Y + 3 * (BUTTON_H + 5 + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                       BUTTON_W + 20, BUTTON_H + 5, BUTTON_BCOLOR, SSBtnClrs[7], TEXT_TCOLOR,
                       SSBtnLbls[7], BUTTON_TEXTSIZE);
  SSBtns[7].drawButton();

  //back button
  back.initButton(&tft, 215, 205, BUTTON_W * 2.5, BUTTON_H + 5, BLACK, BLUE, TEXT_TCOLOR, "Back", 2);
  back.drawButton();
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  Serial.println(F("Finished"));
  screenPage = SCREEN_SKIP;
}

//draws the buttons on the tft screen for serial # input
void drawEntry(void) {
  Serial.print(F("Drawing model entry screen..."));
  tft.fillScreen(LIGHTGREY);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);

  // create buttons, row-by-row
  for (uint8_t row = 0; row < 5; row++) {
    for (uint8_t col = 0; col < 3; col++) {
      if ((col + row * 3) == 14 || (((col + row * 3) == 0) && (manualEntry || isSD == false))) //leaves a blank space
      {}
      else if ((col + row * 3) == 0) //creates the switch input types button
      {
        buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                          BUTTON_Y + row * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                          BUTTON_W, BUTTON_H, BUTTON_BCOLOR, buttoncolors[col + row * 3], TEXT_TCOLOR,
                                          "Switch", BUTTON_TEXTSIZE);
        buttons[col + row * 3].drawButton();
      }
      else //creates the rest of the buttons
      {
        buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                          BUTTON_Y + row * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                          BUTTON_W, BUTTON_H, BUTTON_BCOLOR, buttoncolors[col + row * 3], TEXT_TCOLOR,
                                          buttonlabels[col + row * 3], BUTTON_TEXTSIZE);
        buttons[col + row * 3].drawButton();
      }
    }
  }
  // create 'text field'
  tft.fillRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, BLACK);

  tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
  tft.setTextColor(TEXT_TCOLOR, BLACK);
  tft.setTextSize(TEXT_TSIZE);

  combined_i = 4;
  tft.print(combined);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  nowClock = millis();
  Serial.println(F("Finished"));
  screenPage = SCREEN_ENTRY;
}

//prints the capacity entry screen to the tft display
void drawCapacity(void) {
  Serial.print(F("Drawing capacity entry screen..."));
  tft.fillScreen(LIGHTGREY);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  // create buttons, row-by-row
  for (uint8_t row = 0; row < 4; row++)
  {
    for (uint8_t col = 0; col < 3; col++)
    {
      buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                        BUTTON_Y + (row + 1) * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                        BUTTON_W, BUTTON_H, BUTTON_BCOLOR, CSBtnClrs[col + row * 3], TEXT_TCOLOR,
                                        CSBtnLbls[col + row * 3], BUTTON_TEXTSIZE);
      buttons[col + row * 3].drawButton();
    }
  }
  // create 'text field'
  tft.fillRect(TEXT_X, TEXT_Y + 45, TEXT_W, TEXT_H, BLACK);

  tft.setTextColor(TEXT_TCOLOR);
  tft.setTextSize(TEXT_TSIZE);
  tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
  tft.print("Battery Capacity:");
  tft.setTextColor(TEXT_TCOLOR, BLACK);
  tft.setCursor(TEXT_X + 2, TEXT_Y + 55);
  tft.print(capText);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  Serial.println(F("Finished"));
  screenPage = SCREEN_CAPACITY;
}

//prints the serial entry screen to the tft display
void drawSerialNum(void) {
  Serial.print(F("Drawing serial number entry screen..."));
  tft.fillScreen(LIGHTGREY);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  // create buttons, row-by-row
  for (uint8_t row = 0; row < 4; row++)
  {
    for (uint8_t col = 0; col < 3; col++)
    {
      buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                        BUTTON_Y + (row + 1) * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                        BUTTON_W, BUTTON_H, BUTTON_BCOLOR, CSBtnClrs[col + row * 3], TEXT_TCOLOR,
                                        CSBtnLbls[col + row * 3], BUTTON_TEXTSIZE);
      buttons[col + row * 3].drawButton();
    }
  }
  // create 'text field'
  tft.fillRect(TEXT_X, TEXT_Y + 45, TEXT_W, TEXT_H, BLACK);

  tft.setTextColor(TEXT_TCOLOR);
  tft.setTextSize(TEXT_TSIZE);
  tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
  tft.print("Serial Number:");
  tft.setTextColor(TEXT_TCOLOR, BLACK);
  tft.setCursor(TEXT_X + 2, TEXT_Y + 55);
  tft.print(serialNum);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  Serial.println(F("Finished"));
  screenPage = SCREEN_SERIAL;
}

//prints buttons on tft screen for cell entry input
void drawVolts(void) {
  Serial.print(F("Drawing voltage entry screen..."));
  tft.fillScreen(LIGHTGREY);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  //Switch states button
  if(isSD && isCal == false){
    voltsBtns[0].initButton(&tft, BUTTON_X, BUTTON_Y - 10,
                            BUTTON_W, BUTTON_H, BUTTON_BCOLOR,
                            DARKGREY, TEXT_TCOLOR, CellLbls[0], BUTTON_TEXTSIZE);
    voltsBtns[0].drawButton();
  }
  //24 volt button
  voltsBtns[1].initButton(&tft, BUTTON_X + 35, BUTTON_Y + 45,
                          BUTTON_W + 65, BUTTON_H + 40, BUTTON_BCOLOR,
                          BLUE, TEXT_TCOLOR, CellLbls[1], BUTTON_TEXTSIZE);
  voltsBtns[1].drawButton();
  //36 volt button
  voltsBtns[2].initButton(&tft, BUTTON_X + 185, BUTTON_Y + 45,
                          BUTTON_W + 65, BUTTON_H + 40, BUTTON_BCOLOR,
                          BLUE, TEXT_TCOLOR, CellLbls[2], BUTTON_TEXTSIZE);
  voltsBtns[2].drawButton();
  //48 volt button
  voltsBtns[3].initButton(&tft, BUTTON_X + 35, BUTTON_Y + 115,
                          BUTTON_W + 65, BUTTON_H + 40, BUTTON_BCOLOR,
                          BLUE, TEXT_TCOLOR, CellLbls[3], BUTTON_TEXTSIZE);
  voltsBtns[3].drawButton();
  //80 volt button
  voltsBtns[4].initButton(&tft, BUTTON_X + 185, BUTTON_Y + 115,
                          BUTTON_W + 65, BUTTON_H + 40, BUTTON_BCOLOR,
                          DARKGREY, LIGHTGREY, CellLbls[4], BUTTON_TEXTSIZE);
  voltsBtns[4].drawButton();
  voltsBtns[5].initButton(&tft, BUTTON_X + 2*(BUTTON_W + BUTTON_SPACING_X),
                                    BUTTON_Y - 10, BUTTON_W, BUTTON_H, BUTTON_BCOLOR, RED, TEXT_TCOLOR,
                                    "Back", BUTTON_TEXTSIZE);
  voltsBtns[5].drawButton();

  tft.fillRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, BLACK);

  tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
  tft.setTextColor(TEXT_TCOLOR, BLACK);
  tft.setTextSize(TEXT_TSIZE);
  //displays text in textfield space
  tft.print("Battery Voltage:");
  manualEntry = true;
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  nowClock = millis();
  Serial.println(F("Finished"));
  screenPage = SCREEN_ENTRY_VOLTS;

}

void drawOptions(void){
  Serial.print(F("Drawing options screen..."));
  tft.fillScreen(LIGHTGREY);
  tft.setTextColor(WHITE);
  
  tft.fillRect(DARK_STRIPE_X, 0, DARK_STRIPE_W, DARK_STRIPE_H-5, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, 0, DARK_STRIPE_W, DARK_STRIPE_H-5, BLACK);
  tft.fillRect(DARK_STRIPE_X, 52, DARK_STRIPE_W, (DARK_STRIPE_H-5)*2, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, 52, DARK_STRIPE_W, (DARK_STRIPE_H-5)*2, BLACK);
  tft.fillRect(DARK_STRIPE_X, 148, DARK_STRIPE_W, (DARK_STRIPE_H-5)*2, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, 148, DARK_STRIPE_W, (DARK_STRIPE_H-5)*2, BLACK);
  
  tft.setTextSize(2);
  tft.setCursor(53,8);
  tft.print("Model #: "); tft.print(combined);
  tft.setCursor(41,34);
  tft.print("Serial #: "); tft.print(serialNum);
  tft.setCursor(113,56);
  tft.print("Logging: ");
  tft.setCursor(90,104);
  tft.print("Delay start: ");
  tft.setCursor(97,152);
  tft.print("Cycle type: ");
  
  optBtns[0].initButton(&tft, 85, 220,
                  BUTTON_W * 1.5, BUTTON_H + 5, BUTTON_BCOLOR,
                  DARKGREY, TEXT_TCOLOR, "Back", BUTTON_TEXTSIZE);
  optBtns[0].drawButton();
  optBtns[1].initButton(&tft, 240, 220,
                  BUTTON_W * 1.5, BUTTON_H + 5, BUTTON_BCOLOR,
                  RED, TEXT_TCOLOR, "Continue", BUTTON_TEXTSIZE);
  optBtns[1].drawButton();
  
  if(saveSD){
    optBtns[8].initButton(&tft, 160, 86,
                    BUTTON_W, BUTTON_H, BUTTON_BCOLOR,
                    GREEN, TEXT_TCOLOR, "Yes", BUTTON_TEXTSIZE);
    optBtns[8].drawButton();
    optBtns[2].initButton(&tft, 50, 86,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    DARKGREY, LIGHTGREY, "<", BUTTON_TEXTSIZE);
    optBtns[2].drawButton();
    optBtns[3].initButton(&tft, 270, 86,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    BLUE, TEXT_TCOLOR, ">", BUTTON_TEXTSIZE);
    optBtns[3].drawButton();
  }else{
    optBtns[8].initButton(&tft, 160, 86,
                    BUTTON_W, BUTTON_H, BUTTON_BCOLOR,
                    RED, TEXT_TCOLOR, "No", BUTTON_TEXTSIZE);
    optBtns[8].drawButton();
    optBtns[2].initButton(&tft, 50, 86,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    BLUE, TEXT_TCOLOR, "<", BUTTON_TEXTSIZE);
    optBtns[2].drawButton();
    optBtns[3].initButton(&tft, 270, 86,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    DARKGREY, LIGHTGREY, ">", BUTTON_TEXTSIZE);
    optBtns[3].drawButton();
  }
  
  if(delayCycle == 0){
    optBtns[9].initButton(&tft, 160, 134,
                    BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                    GREEN, TEXT_TCOLOR, "No Delay", BUTTON_TEXTSIZE);
    optBtns[9].drawButton();
    optBtns[4].initButton(&tft, 50, 134,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    DARKGREY, LIGHTGREY, "<", BUTTON_TEXTSIZE);
    optBtns[4].drawButton();
    optBtns[5].initButton(&tft, 270, 134,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    BLUE, TEXT_TCOLOR, ">", BUTTON_TEXTSIZE);
    optBtns[5].drawButton();
  }else{
    optBtns[4].initButton(&tft, 50, 134,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    BLUE, TEXT_TCOLOR, "<", BUTTON_TEXTSIZE);
    optBtns[4].drawButton();
    if(delayCycle == 43200){
      optBtns[9].initButton(&tft, 160, 134,
                      BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                      RED, TEXT_TCOLOR, "12hours", BUTTON_TEXTSIZE);
      optBtns[9].drawButton();
      optBtns[5].initButton(&tft, 270, 134,
                      BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                      DARKGREY, LIGHTGREY, ">", BUTTON_TEXTSIZE);
      optBtns[5].drawButton();
    }else{
      int intHours = delayCycle/36;
      String realHours;
      if(intHours % 100 == 0){
        realHours = String(intHours/100) + " hours";
      }else{
        realHours = String(intHours/100.0) + " hours";
      }
      char charHours[realHours.length() + 1];
      realHours.toCharArray(charHours, realHours.length() +1);
      optBtns[9].initButton(&tft, 160, 134,
                      BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                      YELLOW, BLACK, charHours, BUTTON_TEXTSIZE);
      optBtns[9].drawButton();
      optBtns[5].initButton(&tft, 270, 134,
                      BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                      BLUE, TEXT_TCOLOR, ">", BUTTON_TEXTSIZE);
      optBtns[5].drawButton();
    }
  }
  
  if(cycleType == 0){
    optBtns[10].initButton(&tft, 160, 182,
                    BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                    GREEN, TEXT_TCOLOR, "Default", BUTTON_TEXTSIZE);
    optBtns[10].drawButton();
    optBtns[6].initButton(&tft, 50, 182,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    DARKGREY, LIGHTGREY, "<", BUTTON_TEXTSIZE);
    optBtns[6].drawButton();
    optBtns[7].initButton(&tft, 270, 182,
                    BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                    DARKGREY, LIGHTGREY, ">", BUTTON_TEXTSIZE);
    optBtns[7].drawButton();
  }
  
  
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  nowClock = millis();
  Serial.println(F("Finished"));
  screenPage = SCREEN_OPTIONS;
}

//prints varification and extra options on the tft screen
void drawVerify(void) {
  Serial.print(F("Drawing verify screen..."));
  tft.fillScreen(LIGHTGREY);
  tft.setTextColor(WHITE);
  
  tft.fillRect(DARK_STRIPE_X, 28, DARK_STRIPE_W, DARK_STRIPE_H-5, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, 28, DARK_STRIPE_W, DARK_STRIPE_H-5, BLACK);
  tft.fillRect(DARK_STRIPE_X, 76, DARK_STRIPE_W, DARK_STRIPE_H-5, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, 76, DARK_STRIPE_W, DARK_STRIPE_H-5, BLACK);
  tft.fillRect(DARK_STRIPE_X, 124, DARK_STRIPE_W, DARK_STRIPE_H-5, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, 124, DARK_STRIPE_W, DARK_STRIPE_H-5, BLACK);
  tft.fillRect(DARK_STRIPE_X, 172, DARK_STRIPE_W, DARK_STRIPE_H-5, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, 172, DARK_STRIPE_W, DARK_STRIPE_H-5, BLACK);
  
  tft.setTextSize(2);
  tft.setCursor(53,10);
  tft.print("Model #: "); tft.print(combined);
  tft.setCursor(53,34);
  tft.print("Voltage: "); tft.print(batteryVoltage);
  tft.setCursor(41,58);
  tft.print("Capacity: "); tft.print(capacity);
  tft.setCursor(41,82);
  tft.print("Serial #: "); tft.print(serialNum);
  tft.setCursor(53,106);
  tft.print("Logging: ");
  if(saveSD){
    tft.print("Yes");
  }else{
    tft.print("No");
  }
  tft.setCursor(5,129);
  tft.print("Delay start: "); 
  if(delayCycle == 0){
    tft.print("No Delay"); 
  }else{
      int intHours = delayCycle/36;
      String realHours;
      if(intHours % 100 == 0){
        realHours = String(intHours/100) + " hours";
      }else{
        realHours = String(intHours/100.0) + " hours";
      }
      tft.print(realHours);
  }
  tft.setCursor(17,153);
  tft.print("Cycle type: ");
  if(cycleType == 0){
    tft.print("Default");
  }else if(cycleType == 1){
  }
  tft.setCursor(5,177);
  tft.print("Start point: ");
  if (place == 0){
    tft.print("Beginning");
  }else if (place == 1){
    tft.print("Discharge 1");
  }else if (place == 2){
    tft.print("Dis Wait 1");
  }else if (place == 3){
    tft.print("Charge 1");
  }else if (place == 4){
    tft.print("Chg Wait");
  }else if (place == 5){
    tft.print("Discharge 2");
  }else if (place == 6){
    tft.print("Dis Wait 2");
  }else if (place == 7){
    tft.print("Charge 2");
  }
  YNBtns[0].initButton(&tft, 85, 220,
                  BUTTON_W * 1.5, BUTTON_H + 5, BUTTON_BCOLOR,
                  DARKGREY, TEXT_TCOLOR, "Back", BUTTON_TEXTSIZE);
  YNBtns[0].drawButton();
  YNBtns[1].initButton(&tft, 240, 220,
                  BUTTON_W * 1.5, BUTTON_H + 5, BUTTON_BCOLOR,
                  RED, TEXT_TCOLOR, "Continue", BUTTON_TEXTSIZE);
  YNBtns[1].drawButton();
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  Serial.println(F("Finished"));
  screenPage = SCREEN_CHECK_SAVE;
}

//prints the file tree on the SD card to the serial monitor
void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println(F("/"));
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

//prints the pull screen to the tft display
void drawPull(void) {
  File root;
  Serial.print(F("Drawing pull screen..."));
  tft.fillScreen(LIGHTGREY);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(35, 15);
  tft.print("See Serial Monitor on");
  tft.setCursor(38, 40);
  tft.print("computer attached to");
  tft.setCursor(42, 65);
  tft.print("station for pulling");
  tft.setCursor(50, 90);
  tft.print("data from SD card");
  back.initButton(&tft, 160, 215,
                  BUTTON_W * 2, BUTTON_H + 5, BUTTON_BCOLOR,
                  BLUE, TEXT_TCOLOR, "Back", 2);
  back.drawButton();
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  Serial.println(F("Finished"));

  root = SD.open("/");
  Serial.println(root.name());
  printDirectory(root, 0);
  Serial.println(F("Enter a model number (FBP-****) or All to pull all logs"));
  root.close();

  screenPage = SCREEN_PULL;
}

//prints the calibration screen to the tft display
void drawCalibration(void) {
  Serial.println(F("Drawing calibration entry screen..."));
  tft.fillScreen(LIGHTGREY);

  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  // create buttons, row-by-row
  for (uint8_t row = 0; row < 4; row++)
  {
    for (uint8_t col = 0; col < 3; col++)
    {
      if ((col + row * 3) == 10) {

        back.initButton(&tft, BUTTON_X + col * (BUTTON_W / 1.3 + BUTTON_SPACING_X / 1.3),
                        BUTTON_Y + (row + 1) * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                        BUTTON_W / 2, BUTTON_H, BUTTON_BCOLOR, CSBtnClrs[col + row * 3], TEXT_TCOLOR,
                        ".", BUTTON_TEXTSIZE);
        back.drawButton();
        buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W / 1.3 + (BUTTON_W / 2) + BUTTON_SPACING_X / 1.3 + (BUTTON_SPACING_X / 2)),
                                          BUTTON_Y + (row + 1) * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                          BUTTON_W / 2, BUTTON_H, BUTTON_BCOLOR, CSBtnClrs[col + row * 3], TEXT_TCOLOR,
                                          "Cl", BUTTON_TEXTSIZE);
        buttons[col + row * 3].drawButton();
      }
      else {
        buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                          BUTTON_Y + (row + 1) * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                          BUTTON_W, BUTTON_H, BUTTON_BCOLOR, CSBtnClrs[col + row * 3], TEXT_TCOLOR,
                                          CSBtnLbls[col + row * 3], BUTTON_TEXTSIZE);
        buttons[col + row * 3].drawButton();
      }
    }
  }
  // create 'text field'
  tft.fillRect(TEXT_X, TEXT_Y + 45, TEXT_W, TEXT_H, BLACK);

  tft.setTextColor(TEXT_TCOLOR);
  tft.setTextSize(TEXT_TSIZE);
  tft.setCursor(TEXT_X + 2, TEXT_Y + 55);
  tft.print("Please Wait...");
  capacity = 1;
  isCal == false;
  noise = checkADC(place, true);
  Serial.print("Noise detected on channel " + String(place) + " : ");
  Serial.println(noise);
  switch (place) {
    case 0: {
        line = "Volt";
        break;
    } case 1: {
        digitalWrite(CYC_FAN, ON);
        digitalWrite(EXH_FAN, ON);
        digitalWrite(CHARGE_PIN, ON);
        digitalWrite(CHARGE_POWER_1, ON);
        digitalWrite(CHARGE_POWER_2, ON);
        digitalWrite(CHARGE_POWER_3, ON);
        vData = (3.6 * (batteryVoltage / 3) / 57.6) * 4095;
        cData = (capacity / 5.0 / SUPPLY_OUTPUT) * 4095;
        if (vData > 4095) {
          vData = 4095;
        } else if (vData < 683) {
          vData = 683;
        }
        if (cData > 4095) {
          cData = 4095;
        } else if (cData < 819) {
          cData = 819;
        }
        voltage_adc_raw = (0x0FFF) & String(vData).toInt();
        current_adc_raw = (0x0FFF) & String(cData).toInt();
        dac_voltage_cntrl.setVoltage( (0x0FFF) & voltage_adc_raw, false);
        dac_current_cntrl.setVoltage( (0x0FFF) & current_adc_raw, false);
        delay(4500);
        //delay for the power supply to turn ON
        digitalWrite(POWER_SUPPLY_PIN, ON);
        digitalWrite(SS_RELAY_GND, ON);
        delay(4500);
        line = "CCur";
        break;
    } case 2: {
        disResist(capacity / 5);
        digitalWrite(CYC_FAN, ON);
        digitalWrite(EXH_FAN, ON);
        delay(1000);
        line = "DCur";
      }
  }
  tft.fillRect(TEXT_X, TEXT_Y + 45, TEXT_W, TEXT_H, BLACK);
  tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
  tft.print(line + ":");
  line = "";
  tft.setTextColor(TEXT_TCOLOR, BLACK);
  tft.setCursor(TEXT_X + 2, TEXT_Y + 55);
  calibration_i = 0;
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  Serial.println(F("Finished"));
  screenPage = SCREEN_CALIBRATION;
}

//displays logo bitmap on startup
bool drawBMP(String BMPname = "logoGCT.bmp", int x = 26, int y = 75) {
  File     bmpFile;
  int      bmpWidth, bmpHeight;
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0;
  uint8_t  lcdidx = 0;
  boolean  first = true;
  if (SD.exists(BMPname)) {
    bmpFile = SD.open(BMPname);
    Serial.println(F("bitmap loaded"));
  }else{
    tft.setTextSize(1);
    tft.setTextColor(BLACK);
    tft.setCursor(x, y);
    Serial.println("Image not found");
    return false;
  }
  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.print(read32(bmpFile)); Serial.println(F("bytes"));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      Serial.print(F("Image size: "));
      Serial.print(bmpWidth);
      Serial.print('x');
      Serial.println(bmpHeight);
      rowSize = (bmpWidth * 3 + 3) & ~3;
      w = bmpWidth;
      h = bmpHeight;
      tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

      for (row = 0; row < h; row++) {
        pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
        if (bmpFile.position() != pos) {
          bmpFile.seek(pos);
          buffidx = sizeof(sdbuffer); // Force buffer reload
        }

        for (col = 0; col < w; col++) {
          if (buffidx >= sizeof(sdbuffer)) { // Indeed
            // Push LCD buffer to the display first
            if (lcdidx > 0) {
              tft.pushColors(lcdbuffer, lcdidx, first);
              lcdidx = 0;
              first  = false;
            }
            bmpFile.read(sdbuffer, sizeof(sdbuffer));
            buffidx = 0; // Set index to beginning
          }

          // Convert pixel from BMP to TFT format
          b = sdbuffer[buffidx++];
          g = sdbuffer[buffidx++];
          r = sdbuffer[buffidx++];
          lcdbuffer[lcdidx++] = tft.color565(r, g, b);
        } // end pixel
      } // end scanline
      // Write any remaining data to LCD
      if (lcdidx > 0) {
        tft.pushColors(lcdbuffer, lcdidx, first);
      }
    }
  }
  bmpFile.close();
  return true;
}

//for loading bitmap
uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

//for loading bitmap
uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void drawHome(){
  Serial.print(F("drawing Home screen..."));
  tft.fillScreen(LIGHTGREY);
  if(drawBMP("homeLogo.bmp", 26, 0) == false){
    tft.setTextSize(2);
    tft.setCursor(26,5);
    tft.setTextColor(WHITE);
    tft.print("Cycler Homepage");
  }
  //Yes button
  if(isSD){
    homeBtns[0].initButton(&tft, 160, 105,
                         150, BUTTON_H+10, BUTTON_BCOLOR,
                         BLUE, TEXT_TCOLOR, "Model No.", BUTTON_TEXTSIZE);
  }else{
    homeBtns[0].initButton(&tft, 160, 105,
                         150, BUTTON_H+10, BUTTON_BCOLOR,
                         DARKGREY, LIGHTGREY, "Model No.", BUTTON_TEXTSIZE);
  }
  homeBtns[0].drawButton();
  //No button
  homeBtns[1].initButton(&tft, 160, 158,
                       150, BUTTON_H+10, BUTTON_BCOLOR,
                       BLUE, TEXT_TCOLOR, "Voltage", BUTTON_TEXTSIZE);
  homeBtns[1].drawButton();

  homeBtns[2].initButton(&tft, 160, 211,
                       150, BUTTON_H+10, BUTTON_BCOLOR,
                       BLUE, TEXT_TCOLOR, "Settings", BUTTON_TEXTSIZE);
  homeBtns[2].drawButton();
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  Serial.println(F("Finished"));
  screenPage = SCREEN_HOMEPAGE;
}

void drawSettings(){
  Serial.print(F("drawing Settings screen..."));
  tft.fillScreen(LIGHTGREY);
  manualEntry = false;
  if(drawBMP("homeLogo.bmp", 26, 0) == false){
    tft.setTextSize(2);
    tft.setCursor(26,5);
    tft.setTextColor(WHITE);
    tft.print("Cycler Homepage");
  }
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(95, 75);
  tft.print("Calibration:");
  tft.setCursor(65, 145);
  tft.print("SD data log pull:");
  setBtns[0].initButton(&tft, 70, 120,
                       BUTTON_W, BUTTON_H + 5, BUTTON_BCOLOR,
                       GREEN, TEXT_TCOLOR, "Volt", BUTTON_TEXTSIZE);
  setBtns[0].drawButton();
  setBtns[1].initButton(&tft, 160, 120,
                       BUTTON_W, BUTTON_H + 5, BUTTON_BCOLOR,
                       YELLOW, BLACK, "CCurr", BUTTON_TEXTSIZE);
  setBtns[1].drawButton();
  setBtns[2].initButton(&tft, 250, 120,
                       BUTTON_W, BUTTON_H + 5, BUTTON_BCOLOR,
                       RED, TEXT_TCOLOR, "DCurr", BUTTON_TEXTSIZE);
  setBtns[2].drawButton();
  
  setBtns[3].initButton(&tft, 160, 185,
                       BUTTON_W, BUTTON_H + 5, BUTTON_BCOLOR,
                       BLUE, TEXT_TCOLOR, "Pull", BUTTON_TEXTSIZE);
  setBtns[3].drawButton();
  
  setBtns[4].initButton(&tft, 160, 220,
                       BUTTON_W * 2, BUTTON_H + 5, BUTTON_BCOLOR,
                       DARKGREY, TEXT_TCOLOR, "Back", BUTTON_TEXTSIZE);
  setBtns[4].drawButton();
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);
  Serial.println(F("Finished"));
  screenPage = SCREEN_SETTINGS;
}

//code that is run once at the beginning of the program
void setup(void) {
  byte error, address;
  int nDevices;

  Wire.begin();
  Serial.begin(9600);
  Serial2.begin(9600);
  while (!Serial);

  //prints lcd screen info
#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print(F("TFT size is ")); Serial.print(tft.width()); Serial.print(F("x")); Serial.println(tft.height());

  ////////////////////////////////////

  //checks to see if the SD card is read
  Serial.println(F("Initializing SD card..."));
  pinMode(10, OUTPUT);

  if (!SD.begin(10))
  {
    isSD = false;
    Serial.println(F("initialization failed!"));
  }
  else
  {
    isSD = true;
    Serial.println(F("initialization successful."));
    initLCD();
    tft.fillScreen(WHITE);
    
  if(drawBMP() == false){
    tft.setTextSize(2);
    tft.setCursor(26,75);
    tft.setTextColor(LIGHTGREY);
    tft.print("Green Cubes Technology");
  }
    tft.setTextColor(LIGHTGREY);
    tft.setTextSize(1);
    tft.setCursor(10, 220);
    tft.print("Version " + String(VERSION_NO));
    Serial.print(F("Loaded in "));
    Serial.print(millis() - now);
    Serial.println(" ms");
    now = millis();

    //checks for models.csv file to load the model types from
    Serial.print(F("Searching for models.csv..."));
    if (SD.exists("models.csv"))
    {
      Serial.println(F("models.csv found"));
      //draws the model # input screen
      isSD = true;
    }
    else
    {
      Serial.println(F("models.csv files is missing!"));
      isSD = false;
    }
  }


  ///////////////////////////////

  dcCycle = 1;
  loopDelay = 0;
  systemState = BEFORE_CYCLE;

  //initializes necessary pins
  pinMode(GD_PIN, INPUT);
  pinMode(DISCHARGE5_PIN, OUTPUT);
  pinMode(DISCHARGE33_PIN, OUTPUT);
  pinMode(CHARGE_PIN, OUTPUT);
  pinMode(POWER_SUPPLY_PIN, OUTPUT);
  pinMode(CHARGE_POWER_1, OUTPUT);
  pinMode(CHARGE_POWER_2, OUTPUT);
  pinMode(CHARGE_POWER_3, OUTPUT);
  //  pinMode(GND_RELAY, OUTPUT); //not used
  pinMode(SS_RELAY_GND, OUTPUT);
  pinMode(RELAY_GND_CONTROL, OUTPUT);
  pinMode(CYC_FAN, OUTPUT);
  pinMode(EXH_FAN, OUTPUT);

  //sets pins to the OFF state
  digitalWrite(POWER_SUPPLY_PIN, OFF);
  digitalWrite(CHARGE_PIN, OFF);
  digitalWrite(DISCHARGE5_PIN, OFF);
  digitalWrite(DISCHARGE33_PIN, OFF);

  digitalWrite(CHARGE_POWER_1, OFF);
  digitalWrite(CHARGE_POWER_2, OFF);
  digitalWrite(CHARGE_POWER_3, OFF);

  digitalWrite(CYC_FAN, OFF);
  digitalWrite(EXH_FAN, OFF);
  //  digitalWrite(GND_RELAY, OFF); //not used
  digitalWrite(SS_RELAY_GND, OFF);

  // turns the 8 channel relay on
  digitalWrite(RELAY_GND_CONTROL, ON);

  ///////////////////////////////

  //initializes the I2C connections
  Serial.println(F("Scanning for I2C devices..."));

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print(F("I2C device found at address 0x"));
      if (address < 16)
      {
        Serial.print(F("0"));
      }
      Serial.println(address, HEX);

      if (address == 0x48)
      {
        ads1115.begin();
        Serial.println(F("ADC connection established"));
      }
      else if (address == I2C_ADDR_CUR_CNTRL_1)
      {
        dac_current_cntrl.begin(I2C_ADDR_CUR_CNTRL_1);
        Serial.println(F("DAC current connection established"));
      }
      else if (address == I2C_ADDR_VOL_CNTRL_1)
      {
        dac_voltage_cntrl.begin(I2C_ADDR_VOL_CNTRL_1);
        Serial.println(F("DAC voltage connection established"));
      }
      else if (address == I2C_ADDR_CUR_CNTRL_2)
      {
        dac_current_cntrl.begin(I2C_ADDR_CUR_CNTRL_2);
        Serial.println(F("DAC current connection established"));
      }
      else if (address == I2C_ADDR_VOL_CNTRL_2)
      {
        dac_voltage_cntrl.begin(I2C_ADDR_VOL_CNTRL_2);
        Serial.println(F("DAC voltage connection established"));
      }

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print(F("Unknown error at address 0x"));
      if (address < 16)
      {
        Serial.print(F("0"));
      }
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
  {
    Serial.println(F("No I2C devices found\n"));
  }
  else
  {
    Serial.println("done\n");
  }


  ads1115.begin();
  voltage_adc_raw = 0;
  current_adc_raw = 0;
  dac_voltage_cntrl.setVoltage( (0x0FFF) & voltage_adc_raw, false);
  dac_current_cntrl.setVoltage( (0x0FFF) & current_adc_raw, false);
  while(millis() - now < WAIT*2){}
  initLCD();
  drawHome();
  ///////////////////////////////
}

//initializes or resets LCD screen
void initLCD(void) {
  tft.reset();

  //determines what lcd screen is attached
  uint16_t identifier = tft.readID();
  if (identifier == 0x0101)
  {
    Serial.println(F("Found 0x9341 LCD driver"));
  }
  else
  {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
  }
  identifier = 0x9341;
  tft.begin(identifier);
  tft.setRotation(1);
}

//initializes saving to an sd card
void initLog(void) {
  if (!isSD) {
    return;
  }
  bool file = true;
  int count = 0;
  //determines what the name of the file will be
  Serial.println(F("creating file"));
  if (SD.exists(combined))
  {
    Serial.println(F("Battery Model already exists, entering directory"));
  }
  else
  {
    Serial.println(F("Creating battery file"));
    SD.mkdir(String(combined));
  }
  if (SD.exists(String(sNumber)))
  {
    Serial.println(F("Serial number already exists, entering directory"));
  }
  else
  {
    Serial.println(F("Creating serial number file"));
    SD.mkdir(String(combined) + "/" + String(sNumber));
  }

  while (file)
  {
    fileName = String(combined) + "/" + String(sNumber) + "/" + String(count) + "log.gct";
    if (SD.exists(fileName))
    {
      count += 1;
    }
    else
    {
      file = false;
    }
  }

  //opens the file
  SDlogs = SD.open(fileName, FILE_WRITE);

  Serial.print("Writing headers to file " + fileName);
  Serial.print(F("..."));
  SDlogs.println("      CycleLogs: Model Serial# batteryVoltage Capacity");
  SDlogs.print("      BatteryInfo: ");
  Serial.println(F("done"));
  Serial.println(F("Starting data log to SD card..."));
  SDlogs.close();
}

//prints specific file to tlhe serial monitor
void printFile(String fileTag) {
  if (!isSD) {
    return;
  }
  File SDpull = SD.open(fileTag, FILE_READ);
  line = "";
  //  Serial.println(fileTag);
  bool keepGoing = true;
  while (keepGoing)
  {
    line = SDpull.readStringUntil('\n');
    if (line != "")
    {
      Serial.print(line);
    }
    else
    {
      keepGoing = false;
      SDpull.close();
    }
  }
}

//chills until there is user input
bool idle(bool delayIdle = false) {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if(delayIdle){
  now = millis();
    if(now - loopDelay >= WAIT){
      ++stateTimer;
      if(nowClock <= 0){
        return false;
      }
      nowClock = delayCycle - stateTimer;
      String formattedTime = formatTime();
      loopDelay = now;
        tft.setTextSize(3);
      tft.setTextColor(DARKGREY);
      tft.fillRect(50, 135, CLEAR_INFO_X+75, CLEAR_INFO_Y+25, WHITE);
      tft.setCursor((160 - ((formattedTime.length()/2)*18)), 140);
      tft.print(formattedTime);
    }
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      // scale from 0->1023 to tft.width
      p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
      p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
    }
  
      if (skip.contains(p.x, p.y)) {
        skip.press(true);  // tell the button it is pressed
      } else {
        skip.press(false);  // tell the button it is NOT pressed
      }
      if (skip.justReleased()) {
        skip.drawButton();  // draw normal
      }
      if (skip.justPressed()) {
        skip.drawButton(true);  // draw invert!
        nowClock = 0;
        stateTimer = 0;
        delay(100); // UI debouncing
        return false;
      }
      
      if (back.contains(p.x, p.y)) {
        back.press(true);  // tell the button it is pressed
      } else {
        back.press(false);  // tell the button it is NOT pressed
      }
      if (back.justReleased()) {
        back.drawButton();  // draw normal
      }
      if (back.justPressed()) {
        back.drawButton(true);  // draw invert!
        nowClock = 0;
        stateTimer = 0;
        initLCD();
        drawVerify();
        delay(100); // UI debouncing
        return false;
      }
  }else{
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      nowClock = millis();
      return false;
    }
  }
  return true;
}
      

//checks to see if the yes, no, or reset buttons have been pressed
void checkSave(void) {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 2; b++) {
    if (YNBtns[b].contains(p.x, p.y)) {
      YNBtns[b].press(true);  // tell the button it is pressed
    } else {
      YNBtns[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 2; b++) {
    if (YNBtns[b].justReleased()) {
      YNBtns[b].drawButton();  // draw normal
    }

    if (YNBtns[b].justPressed()) {
      YNBtns[b].drawButton(true);  // draw invert!

      //continue button
      if (b == 1)
      {
        if(saveSD){
          initLog();
        }
        startingPosition();
      }
      //back button
      else if (b == 0)
      {
        drawSkip();
      }
      delay(100); // UI debouncing
    }
  }
}

void homeEntry(void) {
  if (now - nowClock > 30000) {
    bool isIdle = true;
    initLCD();
    tft.fillScreen(WHITE);
    
  if(drawBMP() == false){
    tft.setTextSize(2);
    tft.setCursor(26,75);
    tft.setTextColor(LIGHTGREY);
    tft.print("Green Cubes Technology");
  }
    tft.setTextColor(LIGHTGREY);
    tft.setTextSize(1);
    tft.setCursor(10, 220);
    tft.print("Version " + String(VERSION_NO));
    while (isIdle) {
      isIdle = idle();
    }
    Serial.print(F("Idle for "));
    Serial.print(millis() - now);
    Serial.println(" ms");
    initLCD();
    drawHome();
  }
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); 
    nowClock = millis();
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 4; b++) {
    if (homeBtns[b].contains(p.x, p.y)) {
      homeBtns[b].press(true);  // tell the button it is pressed
    } else {
      homeBtns[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 4; b++) {
    if (homeBtns[b].justReleased()) {
      homeBtns[b].drawButton();  // draw normal
    }

    if (homeBtns[b].justPressed()) {
      homeBtns[b].drawButton(true);  // draw invert!
      if (b == 2) {
        initLCD();
        drawSettings();
      }else if (b == 1){
        initLCD();
        drawVolts();
      }else if (b == 0 && isSD){
        initLCD();
        drawEntry();
      }
      delay(100); // UI debouncing
    }
  }
}

void settingsEntry(void) {
  if (now - nowClock > 30000) {
    bool isIdle = true;
    initLCD();
    tft.fillScreen(WHITE);
    
  if(drawBMP() == false){
    tft.setTextSize(2);
    tft.setCursor(26,75);
    tft.setTextColor(LIGHTGREY);
    tft.print("Green Cubes Technology");
  }
    tft.setTextColor(LIGHTGREY);
    tft.setTextSize(1);
    tft.setCursor(10, 220);
    tft.print("Version " + String(VERSION_NO));
    while (isIdle) {
      isIdle = idle();
    }
    Serial.print(F("Idle for "));
    Serial.print(millis() - now);
    Serial.println(" ms");
    initLCD();
    drawSettings();
  }
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    nowClock = millis();
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 5; b++) {
    if (setBtns[b].contains(p.x, p.y)) {
      setBtns[b].press(true);  // tell the button it is pressed
    } else {
      setBtns[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 5; b++) {
    
    if (setBtns[b].justReleased()) {
      setBtns[b].drawButton();  // draw normal
    }

    if (setBtns[b].justPressed()) {
      setBtns[b].drawButton(true);  // draw invert!

      if(b == 0 || b == 1 || b == 2){
        isCal = true;
        place = b;
        initLCD();
        drawVolts();
      }else if(b == 3){
        initLCD();
        drawPull();
      }else if(b == 4){
        initLCD();
        drawHome();
      }

      delay(100); // UI debouncing
    }
  }
}

//searches for the inputed serial number from a file
void isModel(String mod) {
  if (!isSD) {
    return;
  }
  bool bat = true;
  String pos;
  String id;
  File models;
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  tft.print("Loading...");
  Serial.print(F("Loading ")); Serial.println(textfield);
  models = SD.open("models.csv", FILE_READ);
  Serial.print(F("Searching models.csv for battery "));
  Serial.print(mod);
  Serial.println(F("... "));
  //searches until the battery is found or there are no more entrys
  while (bat)
  {
    pos = models.readStringUntil('\n');
    if (pos != "")
    {
      id = pos.substring(0, 8);
      if (id == mod) {
        Serial.println(F("Battery model found"));
        batteryVoltage = ((pos.substring(9, 11)).toInt());
        Serial.println("Battery cell voltage: " + String(batteryVoltage));
        capacity = pos.substring(12, pos.length()).toInt();
        Serial.println("Capacity: " + String(capacity));
        initLCD();
        drawSerialNum();
        bat = false;
      }
    }
    else
    {
      //cancels the loading if the battery model is not found in the file
      Serial.println(F("Battery model not found"));
      tft.setCursor(STATUS_X, STATUS_Y);
      tft.print("Not found ");
      bat = false;
    }
  }
  //closes the file
  models.close();
}

//prints all data on sd card to the main serial monitor
void printAll(File dir) {
  if (!isSD) {
    return;
  }
  bool keepGoing = true;
  bool first = true;
  String filePath = "";
  File modelNo = dir.openNextFile();
  while (keepGoing)
  {
    if (! first)
    {
      if (modelNo.isDirectory())
      {
        filePath = String(modelNo.name());
        Serial.println(filePath);
        File serialNo = modelNo.openNextFile();
        while (keepGoing)
        {
          if (serialNo.isDirectory())
          {
            filePath = String(modelNo.name()) + "/" + String(serialNo.name());
            Serial.println(filePath);
            File Log = serialNo.openNextFile();
            while (keepGoing)
            {
              filePath = String(modelNo.name()) + "/" + String(serialNo.name()) + "/" + String(Log.name());
              Serial.println(filePath);
              printFile(filePath);
              Serial.println(F(""));
              Log = serialNo.openNextFile();
              if (! Log)
              {
                keepGoing = false;
              }
            }
            Log.close();
            keepGoing = true;
          }
          serialNo = modelNo.openNextFile();
          if (! serialNo)
          {
            keepGoing = false;
          }
        }
        serialNo.close();
        keepGoing = true;
      }
    }
    else
    {
      first = false;
    }
    modelNo = dir.openNextFile();
    if (! modelNo)
    {
      keepGoing = false;
    }
  }
  modelNo.close();
}

//watches the main serial port for responses to pulling data from SD card
void pullPage(void) {
  bool fileEx;
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  switch (place)
  {
    case 0: {
        while (Serial.available() > 0) {
          fileLoc = Serial.readStringUntil('\n');
        }
        if (fileLoc != "") {
          fileLoc.trim();
          fileLoc.toUpperCase();
          Serial.println(fileLoc);
          if (fileLoc == "ALL") {
            Serial.println(F("Pulling all data"));
            place = 2;
          }
          else if (fileLoc == "models.csv" || fileLoc == "models") {
            Serial.println(F("Loading models"));
            place = 3;
          }
          else {
            fileLoc = "FBP-" + fileLoc;
            if (SD.exists(fileLoc))
            {
              Serial.println(F("model found"));
              Serial.println(F("Enter a serial number (8 digits)"));
              place = 4;
            } else {
              Serial.println(F("model not found"));
              fileLoc = "";
            }
          }
        }
        while (Serial.available() > 0) {
          char t = Serial.read();
        }
        break;
      }
    case 1: {
        tft.setTextColor(YELLOW);
        tft.setTextSize(2);
        tft.fillRect(10, 115, 300, 75, LIGHTGREY);
        tft.setCursor(10, 140);
        tft.print("Pulling data from SD card");
        tft.setCursor(35, 165);
        tft.print("Do not remove computer");
        printFile(fileLoc);
        tft.fillRect(10, 115, 300, 75, LIGHTGREY);
        tft.setCursor(105, 140);
        tft.print("Complete");
        tft.setTextColor(WHITE);
        Serial.println(F(""));
        Serial.println(F("Enter a model number (FBP-****) or All to pull all logs"));
        fileLoc = "";
        place = 0;
        break;
      }
    case 2: {
        File root = SD.open("/");
        tft.setTextColor(YELLOW);
        tft.setTextSize(2);
        tft.fillRect(10, 115, 300, 75, LIGHTGREY);
        tft.setCursor(10, 140);
        tft.print("Pulling data from SD card");
        tft.setCursor(35, 165);
        tft.print("Do not remove computer");
        printAll(root);
        tft.fillRect(10, 115, 300, 75, LIGHTGREY);
        tft.setCursor(105, 140);
        tft.print("Complete");
        tft.setTextColor(WHITE);
        Serial.println(F(""));
        Serial.println(F("Enter a model number (FBP-****) or All to pull all logs"));
        fileLoc = "";
        place = 0;
        break;
      }
    case 3: {
        File modelPull = SD.open("models.csv", FILE_READ);
        line = "";
        Serial.println(F("models.csv"));
        bool keepGoing = true;
        while (keepGoing) {
          line = modelPull.readStringUntil('\n');
          if (line != "") {
            Serial.println(line);
          } else {
            keepGoing = false;
            modelPull.close();
          }
        }
        Serial.println(F("Enter a model number (FBP-****) or All to pull all logs"));
        fileLoc = "";
        place = 0;
        break;
      }
    case 4: {
        String temp;
        while (Serial.available() > 0) {
          temp = Serial.readStringUntil('\n');
        }
        if (temp != "") {
          temp.trim();
          temp = fileLoc + "/" + temp;
          Serial.println(temp);
          if (SD.exists(temp)) {
            Serial.println(F("serial number found"));
            Serial.println(F("Enter a log number (*log.gct)"));
            fileLoc = temp;
            place = 5;
          } else {
            Serial.println(F("serial number not found"));
            temp = "";
          }
        }
        while (Serial.available() > 0) {
          char t = Serial.read();
        }
        break;
      }
    case 5: {
        String temp;
        while (Serial.available() > 0) {
          temp = Serial.readStringUntil('\n');
        }
        if (temp != "") {
          temp.trim();
          temp = fileLoc + "/" + temp + "log.gct";
          Serial.println(temp);
          if (SD.exists(temp)) {
            Serial.println(F("file found"));
            fileLoc = temp;
            place = 1;
          } else {
            Serial.println(F("file not found"));
            temp = "";
          }
        }
        while (Serial.available() > 0) {
          char t = Serial.read();
        }
        break;
      }
  }
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
  }
  if (back.contains(p.x, p.y)) {
    back.press(true);  // tell the button it is pressed
  } else {
    back.press(false);  // tell the button it is NOT pressed
  }
  if (back.justPressed()) {
    back.drawButton(true);  // draw invert!
    tft.setTextColor(WHITE);
    place = 0;
    initLCD();
    drawSettings();
  }
}

//checks to see if any of the model # entry buttons have been pressed
void modelEntry(void) {
  if (now - nowClock > 30000) {
    bool isIdle = true;
    initLCD();
    tft.fillScreen(WHITE);
    
  if(drawBMP() == false){
    tft.setTextSize(2);
    tft.setCursor(26,75);
    tft.setTextColor(LIGHTGREY);
    tft.print("Green Cubes Technology");
  }
    tft.setTextColor(LIGHTGREY);
    tft.setTextSize(1);
    tft.setCursor(10, 220);
    tft.print("Version " + String(VERSION_NO));
    while (isIdle) {
      isIdle = idle();
    }
    Serial.print(F("Idle for "));
    Serial.print(millis() - now);
    Serial.println(" ms");
    initLCD();
    drawEntry();
  }
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    nowClock = millis();
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 15; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 15; b++) {
    if (buttons[b].justReleased() && (b != 0 || !manualEntry)) {
      buttons[b].drawButton();  // draw normal
    }

    if (buttons[b].justPressed() && (b != 0 || !manualEntry)) {
      buttons[b].drawButton(true);  // draw invert!

      // if a numberpad button, append the relevant # to the textfield
      if ((b >= 3 && b <= 11) || b == 13) {
        if (textfield_i < TEXT_LEN) {
          textfield[textfield_i] = buttonlabels[b][0];
          textfield_i++;
          textfield[textfield_i] = 0; // zero terminate
        }
      }

      // clr button! delete char
      if (b == 1) {

        textfield[textfield_i] = 0;
        if (textfield_i > 0) {
          textfield_i--;
          if (textfield_i < 0) {
            textfield_i = 0;
          }
          textfield[textfield_i] = ' ';
        }
      }

      // update the current text field
      tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
      tft.setTextColor(TEXT_TCOLOR, BLACK);
      tft.setTextSize(TEXT_TSIZE);

      for (combined_i; combined_i < 8; combined_i++)
      {
        combined[combined_i] = textfield[combined_i - 4];
      }
      combined_i = 4;
      tft.print(combined);
      //Enter button has been pressed
      if (b == 2 && textfield_i >= 4) {
        if (manualEntry) {
          initLCD();
          drawCapacity();
        }
        else {
          isModel(String(combined));
        }
      }
      //Switch button has been pressed
      if (b == 0)
      {
        initLCD();
        drawVolts();
      }
      if (b == 12)
      {
        initLCD();
        if (manualEntry) {
          drawVolts();
        }
        else {
          drawHome();;
        }
      }

      delay(100); // UI debouncing
    }
  }
}

//checks to see if any buttons on the capacity entry page have been pressed
void capacityEntry(void) {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 15; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 12; b++) {
    if (buttons[b].justReleased()) {
      buttons[b].drawButton();  // draw normal
    }

    if (buttons[b].justPressed()) {
      buttons[b].drawButton(true);  // draw invert!

      // if a numberpad button, append the relevant # to the textfield
      if (b <= 9) {

        Serial.println(capText);
        if (capText_i < TEXT_LEN) {
          capText[capText_i] = CSBtnLbls[b][0];
          capText_i++;
          capText[capText_i] = 0; // zero terminate
        }
      }

      // clr button! delete char
      if (b == 10) {

        capText[capText_i] = 0;
        if (capText_i > 0) {
          capText_i--;
          if (capText_i < 0) {
            capText_i = 0;
          }
          capText[capText_i] = ' ';
        }
      }

      // update the current text field
      tft.setCursor(TEXT_X + 2, TEXT_Y + 55);
      tft.setTextColor(TEXT_TCOLOR, BLACK);
      tft.setTextSize(TEXT_TSIZE);
      tft.print(capText);
      //Enter button has been pressed
      if (b == 11) {
        capacity = (String(capText)).toInt();
        Serial.println("Battery capacity set to " + String(capacity));
        initLCD();
        drawSerialNum();
      }

      delay(100); // UI debouncing
    }
  }
}

//checks to see if any buttons on the serial entry page have been pressed
void serialEntry(void) {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 15; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 12; b++) {
    if (buttons[b].justReleased()) {
      buttons[b].drawButton();  // draw normal
    }

    if (buttons[b].justPressed()) {
      buttons[b].drawButton(true);  // draw invert!

      // if a numberpad button, append the relevant # to the textfield
      if (b <= 9) {
        if (serialNum_i < TEXT_LEN + 4) {
          serialNum[serialNum_i] = CSBtnLbls[b][0];
          serialNum_i++;
          serialNum[serialNum_i] = 0; // zero terminate
        }
      }

      // clr button! delete char
      if (b == 10) {

        serialNum[serialNum_i] = 0;
        if (serialNum_i > 0) {
          serialNum_i--;
          if (serialNum_i < 0) {
            serialNum_i = 0;
          }
          serialNum[serialNum_i] = ' ';
        }
      }

      // update the current text field
      tft.setCursor(TEXT_X + 2, TEXT_Y + 55);
      tft.setTextColor(TEXT_TCOLOR, BLACK);
      tft.setTextSize(TEXT_TSIZE);

      tft.print(serialNum);
      //Enter button has been pressed
      if (b == 11 && serialNum_i >= 7) {
        sNumber = (String(serialNum)).toInt();
        Serial.println("Battery's serial number is " + String(sNumber));
        initLCD();
        drawOptions();
      }

      delay(100); // UI debouncing
    }
  }
}

//checks to see if any of the voltage entry buttons have been pressed
void voltsEntry(void) {
  if (now - nowClock > 30000) {
    bool isIdle = true;
    initLCD();
    tft.fillScreen(WHITE);
    
  if(drawBMP() == false){
    tft.setTextSize(2);
    tft.setCursor(26,75);
    tft.setTextColor(LIGHTGREY);
    tft.print("Green Cubes Technology");
  }
    tft.setTextColor(LIGHTGREY);
    tft.setTextSize(1);
    tft.setCursor(10, 220);
    tft.print("Version " + String(VERSION_NO));
    while (isIdle) {
      isIdle = idle();
    }
    Serial.print(F("Idle for "));
    Serial.print(millis() - now);
    Serial.println(" ms");
    initLCD();
    drawVolts();
  }
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
    nowClock = millis();
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 6; b++) {
    if (voltsBtns[b].contains(p.x, p.y)) {
      voltsBtns[b].press(true);  // tell the button it is pressed
    } else {
      voltsBtns[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 6; b++) {
    if (voltsBtns[b].justReleased()) {
      voltsBtns[b].drawButton();  // draw normal
    }

    if (voltsBtns[b].justPressed()) {
      voltsBtns[b].drawButton(true);  // draw invert!

      if (b == 1) {
        //24 volts button
        batteryVoltage = 24;
        Serial.println("Setting battery voltage to " + String(batteryVoltage));
        initLCD();
        if(isCal){
          drawCalibration();
        }else{
          drawEntry();
        }
      } else if (b == 2) {
        //36 volts button
        batteryVoltage = 36;
        Serial.println("Setting battery voltage to " + String(batteryVoltage));
        initLCD();
        if(isCal){
          drawCalibration();
        }else{
          drawEntry();
        }
      } else if (b == 3) {
        //48 volts button
        batteryVoltage = 48;
        Serial.println("Setting battery voltage to " + String(batteryVoltage));
        initLCD();
        if(isCal){
          drawCalibration();
        }else{
          drawEntry();
        }
      } 
//      else if (b == 4) {
//        //80 volts button
//        batteryVoltage = 78;
//        Serial.println("Setting battery voltage to " + String(batteryVoltage));
//        initLCD();
//        if(isCal){
//          drawCalibration();
//        }else{
//          drawEntry();
//        }
//      } 
      else if (b == 5) {
        initLCD();
        if(isCal){
          drawSettings();
        }else{
          drawHome();
        }
      } else if (b == 0) {
        //Switch mode button
        manualEntry = false;
        initLCD();
        drawEntry();
      }
      delay(100); // UI debouncing
    }
  }
}

//checks to see if any if the buttons on calibration page have been clicked
void calibrationEntry(void) {
  if(millis() - nowClock > 2000){
    float tempVal;
    tft.fillRect(TEXT_X + 100, TEXT_Y + 10, 200, 35, LIGHTGREY);
    tft.setCursor(TEXT_X + 100, TEXT_Y + 10);
    tft.setTextSize(3);
    if(calibration_i == 0){
      tft.setTextColor(DARKGREY);
      switch (place) {
          case 0: {
            calibrationV = calibrate(ADC_VOLTAGE_CHANNEL);
            tempVal = checkADC(ADC_VOLTAGE_CHANNEL);
            break;
          }case 1: {
            calibrationC = calibrate(ADC_CURRENT_CHANNEL);
            tempVal = checkADC(ADC_CURRENT_CHANNEL);
            break;
          }case 2: {
            calibrationC = calibrate(ADC_CURRENT2_CHANNEL);
            tempVal = checkADC(ADC_CURRENT2_CHANNEL);
            break;
          }
        }
      }else{
        switch (place) {
          case 0: {
            calibrationV = 1;
            calibrationV = checkADC(ADC_VOLTAGE_CHANNEL, true) / (String(caliText)).toDouble();
            tempVal = checkADC(ADC_VOLTAGE_CHANNEL);
            break;
          }case 1: {
            calibrationC = 1;
            calibrationC = checkADC(ADC_CURRENT_CHANNEL) / (String(caliText)).toDouble();
            tempVal = checkADC(ADC_CURRENT_CHANNEL);
            break;
          }case 2: {
            calibrationC = 1;
            calibrationC = checkADC(ADC_CURRENT2_CHANNEL) / (String(caliText)).toDouble();
            tempVal = checkADC(ADC_CURRENT2_CHANNEL);
            break;
          }
        }
        tft.setTextColor(WHITE);
    }
    tft.print(tempVal, 4);
    nowClock = millis();
    tft.setTextSize(2);
    }
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
  }

  if (back.contains(p.x, p.y)) {
    back.press(true);  // tell the button it is pressed
  } else {
    back.press(false);  // tell the button it is NOT pressed
  }

  if (back.justReleased()) {
    back.drawButton();  // draw normal
  }

  if (back.justPressed()) {
    back.drawButton(true);  // draw invert!
    bool noDec = true;
    for (uint8_t b = 0; b < 15; b ++) {
      if (caliText[b] == CSBtnLbls[12][0]) {
        noDec = false;
      }
    }
    if (noDec) {
      caliText[calibration_i] = CSBtnLbls[12][0];
      calibration_i++;
      caliText[calibration_i] = 0;
      tft.setCursor(TEXT_X + 2, TEXT_Y + 55);
      tft.setTextColor(TEXT_TCOLOR, BLACK);
      tft.setTextSize(TEXT_TSIZE);
      tft.print(caliText);
    }
    delay(100);
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 15; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 12; b++) {
    if (buttons[b].justReleased()) {
      buttons[b].drawButton();  // draw normal
    }

    if (buttons[b].justPressed()) {
      buttons[b].drawButton(true);  // draw invert!

      // if a numberpad button, append the relevant # to the textfield
      if (b <= 9) {
        if (calibration_i < TEXT_LEN + 10) {
          caliText[calibration_i] = CSBtnLbls[b][0];
          calibration_i++;
          caliText[calibration_i] = 0; // zero terminate
        }
      }

      // clr button! delete char
      if (b == 10) {
        if(calibration_i == 0){
          place = 0;
          line = "";
          initLCD();
          drawSettings();
        }
        caliText[calibration_i] = 0;
        if (calibration_i > 0) {
          calibration_i--;
          if (calibration_i < 0) {
            calibration_i = 0;
          }
          caliText[calibration_i] = ' ';
        }
      }

      // update the current text field
      tft.setCursor(TEXT_X + 2, TEXT_Y + 55);
      tft.setTextColor(TEXT_TCOLOR, BLACK);
      tft.setTextSize(TEXT_TSIZE);

      tft.print(caliText);
      //Enter button has been pressed
      if (b == 11) {
        File rem = SD.open("station.csv", FILE_READ);
        uint8_t lineCount = 0;
        bool bat = true;
        line = "";
        while (bat) {
          line = rem.readStringUntil('\n');
          if (line != "") {
            lineCount++;
          } else {
            bat = false;
          }
        }
        rem.close();
        String strTemp[lineCount];
        rem = SD.open("station.csv", FILE_READ);
        for (uint8_t i = 0; i < lineCount; i++) {
          strTemp[i] = rem.readStringUntil('\n');
          strTemp[i].trim();
        }
        rem.close();
        SD.remove("station.csv");
        File calFile = SD.open("station.csv", FILE_WRITE);
        double temp;
        calibrationC = 1;
        calibrationV = 1;
        if (place == 0) {
          temp = checkADC(place, true);
        } else {
          temp = checkADC(place);
        }
        Serial.println("Actual reading entered in as " + String(caliText));
        Serial.println("ADC reading from channel " + String(place) + ": " + temp);
        temp = temp / ((String(caliText)).toDouble());
        switch (place) {
          case 0: {
              calFile.print("Calibration0,"); calFile.println(temp, 10);
              for (uint8_t i = 1; i < lineCount; i++) {
                calFile.println(strTemp[i]);
              }
              break;
            }
          case 1: {
              calFile.println(strTemp[0]);
              calFile.print("Calibration1,"); calFile.println(temp, 10);
              for (uint8_t i = 2; i < lineCount; i++) {
                calFile.println(strTemp[i]);
              }
              digitalWrite(POWER_SUPPLY_PIN, OFF);
              delay(500);
              voltage_adc_raw = 0;
              current_adc_raw = 0;
              dac_voltage_cntrl.setVoltage( (0x0FFF) & voltage_adc_raw, false);
              dac_current_cntrl.setVoltage( (0x0FFF) & current_adc_raw, false);
              digitalWrite(CHARGE_PIN, OFF);
              digitalWrite(CHARGE_POWER_1, OFF);
              digitalWrite(CHARGE_POWER_2, OFF);
              digitalWrite(CHARGE_POWER_3, OFF);
              digitalWrite(SS_RELAY_GND, OFF);
              digitalWrite(CYC_FAN, OFF);
              digitalWrite(EXH_FAN, OFF);
              break;
            }
          case 2: {
              calFile.println(strTemp[0]);
              calFile.println(strTemp[1]);
              calFile.print("Calibration2,"); calFile.println(temp, 10);
              for (uint8_t i = 3; i < lineCount; i++) {
                calFile.println(strTemp[i]);
              }
              digitalWrite(DISCHARGE5_PIN, OFF);
              digitalWrite(DISCHARGE33_PIN, OFF);
              digitalWrite(CYC_FAN, OFF);
              digitalWrite(EXH_FAN, OFF);
              break;
            }
        }
        calFile.close();
        Serial.println("ADC Channel " + String(place) + " has been calibrated");
        Serial.print(F("Calibration set to ")); Serial.println(temp, 10);
        place = 0;
        line = "";
        initLCD();
        drawSettings();

      }

      delay(100); // UI debouncing
    }
  }
}

//checks to see if any buttons on the skip entry page have been pressed
void skipEntry(void) {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
  }

  if (back.contains(p.x, p.y))
  {
    back.press(true);  // tell the button it is pressed
  }
  else
  {
    back.press(false);  // tell the button it is NOT pressed
  }

  if (back.justReleased())
  {
    // Serial.print(F("Released: ")); Serial.println(b);
    back.drawButton();  // draw normal
  }

  if (back.justPressed())
  {
    back.drawButton(true);  // draw invert!
    tft.setTextColor(WHITE);
    initLCD();
    drawOptions();
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 8; b++) {
    if (SSBtns[b].contains(p.x, p.y)) {
      SSBtns[b].press(true);  // tell the button it is pressed
    } else {
      SSBtns[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 8; b++) {
    if (SSBtns[b].justReleased()) {
      SSBtns[b].drawButton();  // draw normal
    }

    if (SSBtns[b].justPressed()) {
      SSBtns[b].drawButton(true);  // draw invert!

      vConnect = true;
      cConnect = true;
      loopDelay = millis();
      place = b;
      drawVerify();
      //Switch mode button
      delay(100); // UI debouncing
    }
  }
}


void optionsEntry(void){
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
  }

  // go thru all the buttons, checking if they were pressed
  for (uint8_t b = 0; b < 8; b++) {
    if (optBtns[b].contains(p.x, p.y)) {
      optBtns[b].press(true);  // tell the button it is pressed
    } else {
      optBtns[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 8; b++) {
    
    if (optBtns[b].justReleased()) {
      optBtns[b].drawButton();  // draw normal
    }
if (optBtns[b].justPressed()){
}
    if (optBtns[b].justPressed() && 
    ((b != 2 && saveSD) || (b != 3 && saveSD == false)) &&
    ((b == 4 && delayCycle != 0) || (b == 5 && delayCycle != 43200) || (b != 4 && b != 5)) && 
    ((b != 6) || (b != 7))) {
      optBtns[b].drawButton(true);  // draw invert!
      if(b == 0){
        initLCD();
        drawSerialNum();
      }else if(b == 1){
        initLCD();
        drawSkip();
      }else if(b == 2){
        optBtns[8].initButton(&tft, 160, 86,
                        BUTTON_W, BUTTON_H, BUTTON_BCOLOR,
                        GREEN, TEXT_TCOLOR, "Yes", BUTTON_TEXTSIZE);
        optBtns[8].drawButton();
        optBtns[2].initButton(&tft, 50, 86,
                        BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                        DARKGREY, LIGHTGREY, "<", BUTTON_TEXTSIZE);
        optBtns[2].drawButton();
        optBtns[3].initButton(&tft, 270, 86,
                        BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                        BLUE, TEXT_TCOLOR, ">", BUTTON_TEXTSIZE);
        optBtns[3].drawButton();
        saveSD = true;
      }else if(b == 3){
        optBtns[8].initButton(&tft, 160, 86,
                        BUTTON_W, BUTTON_H, BUTTON_BCOLOR,
                        RED, TEXT_TCOLOR, "No", BUTTON_TEXTSIZE);
        optBtns[8].drawButton();
        optBtns[2].initButton(&tft, 50, 86,
                        BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                        BLUE, TEXT_TCOLOR, "<", BUTTON_TEXTSIZE);
        optBtns[2].drawButton();
        optBtns[3].initButton(&tft, 270, 86,
                        BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                        DARKGREY, LIGHTGREY, ">", BUTTON_TEXTSIZE);
        optBtns[3].drawButton();
        saveSD = false;
      }else if(b == 4){
        if(delayCycle == 900 || delayCycle == 1800 || 
            delayCycle == 2700 || delayCycle == 3600){
          delayCycle = delayCycle - 900;
        }else if(delayCycle == 5400 || delayCycle == 7200 || 
                  delayCycle == 9000 || delayCycle == 10800){
          delayCycle = delayCycle - 1800;
        }else{
          delayCycle = delayCycle - 3600;
        }
        if(delayCycle == 0){
          optBtns[9].initButton(&tft, 160, 134,
                          BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                          GREEN, TEXT_TCOLOR, "No Delay", BUTTON_TEXTSIZE);
          optBtns[9].drawButton();
          optBtns[4].initButton(&tft, 50, 134,
                          BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                          DARKGREY, LIGHTGREY, "<", BUTTON_TEXTSIZE);
          optBtns[4].drawButton();
        }else{
          int intHours = delayCycle/36;
          String realHours;
          if(delayCycle <= 3600){
            if(intHours % 100 == 0){
              realHours = String(intHours/100) + " hour";
            }else{
              realHours = String(intHours/100.0) + " hour";
            }
            char charHours[realHours.length() + 1];
            realHours.toCharArray(charHours, realHours.length() +1);
            optBtns[9].initButton(&tft, 160, 134,
                            BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                            YELLOW, BLACK, charHours, BUTTON_TEXTSIZE);
            optBtns[9].drawButton();
          }else{
            if(intHours % 100 == 0){
              realHours = String(intHours/100) + " hours";
            }else{
              realHours = String(intHours/100.0) + " hours";
            }
            char charHours[realHours.length() + 1];
            realHours.toCharArray(charHours, realHours.length() +1);
            optBtns[9].initButton(&tft, 160, 134,
                            BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                            YELLOW, BLACK, charHours, BUTTON_TEXTSIZE);
            optBtns[9].drawButton();
          }
          optBtns[4].initButton(&tft, 50, 134,
                          BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                          BLUE, TEXT_TCOLOR, "<", BUTTON_TEXTSIZE);
          optBtns[4].drawButton();
        }
        optBtns[5].initButton(&tft, 270, 134,
                        BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                        BLUE, TEXT_TCOLOR, ">", BUTTON_TEXTSIZE);
        optBtns[5].drawButton();
      }else if(b == 5){
        if(delayCycle == 900 || delayCycle == 1800 || 
            delayCycle == 2700 || delayCycle == 0){
          delayCycle = delayCycle + 900;
        }else if(delayCycle == 5400 || delayCycle == 7200 || 
                  delayCycle == 9000 || delayCycle == 3600){
          delayCycle = delayCycle + 1800;
        }else{
          delayCycle = delayCycle + 3600;
        }
        if(delayCycle == 43200){
          optBtns[9].initButton(&tft, 160, 134,
                          BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                          RED, TEXT_TCOLOR, "12 hours", BUTTON_TEXTSIZE);
          optBtns[9].drawButton();
          optBtns[5].initButton(&tft, 270, 134,
                          BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                          DARKGREY, LIGHTGREY, ">", BUTTON_TEXTSIZE);
          optBtns[5].drawButton();
        }else{
          
          int intHours = delayCycle/36;
          String realHours;
          if(delayCycle <= 3600){
            if(intHours % 100 == 0){
              realHours = String(intHours/100) + " hour";
            }else{
              realHours = String(intHours/100.0) + " hour";
            }
            char charHours[realHours.length() + 1];
            realHours.toCharArray(charHours, realHours.length() +1);
            optBtns[9].initButton(&tft, 160, 134,
                            BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                            YELLOW, BLACK, charHours, BUTTON_TEXTSIZE);
            optBtns[9].drawButton();
          }else{
            if(intHours % 100 == 0){
              realHours = String(intHours/100) + " hours";
            }else{
              realHours = String(intHours/100.0) + " hours";
            }
            char charHours[realHours.length() + 1];
            realHours.toCharArray(charHours, realHours.length() +1);
            optBtns[9].initButton(&tft, 160, 134,
                            BUTTON_W * 2, BUTTON_H, BUTTON_BCOLOR,
                            YELLOW, BLACK, charHours, BUTTON_TEXTSIZE);
            optBtns[9].drawButton();
          }
          optBtns[5].initButton(&tft, 270, 134,
                          BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                          BLUE, TEXT_TCOLOR, ">", BUTTON_TEXTSIZE);
          optBtns[5].drawButton();
        }
        optBtns[4].initButton(&tft, 50, 134,
                        BUTTON_W/2, BUTTON_H, BUTTON_BCOLOR,
                        BLUE, TEXT_TCOLOR, "<", BUTTON_TEXTSIZE);
        optBtns[4].drawButton();
      }else if(b == 6){
      }else if(b == 7){
      }

      delay(100); // UI debouncing
    }
  }
}

void startingPosition(void){
      loopDelay = millis();
      now = millis();
      
      if(delayCycle > 0){
        initLCD();
        tft.fillScreen(WHITE);
    
        if(drawBMP("logoGCT.bmp", 26, 10) == false){
          tft.setTextSize(2);
          tft.setCursor(26,20);
          tft.setTextColor(LIGHTGREY);
          tft.print("Green Cubes Technology");
        }
        tft.setTextColor(LIGHTGREY);
        tft.setTextSize(1);
        tft.setCursor(10, 220);
        tft.print("Version " + String(VERSION_NO));
        tft.setTextSize(2);
        tft.setTextColor(DARKGREY);
        tft.setCursor(50, 115);
        tft.print("Cycle will start in");
        nowClock = delayCycle;
        stateTimer = 0;
        String formattedTime = formatTime();
        tft.setTextSize(3);
        tft.setCursor((160 - ((formattedTime.length()/2)*18)), 140);
        tft.print(formattedTime);
        back.initButton(&tft, 85, 200,
                        BUTTON_W * 1.5, BUTTON_H + 5, BUTTON_BCOLOR,
                        DARKGREY, TEXT_TCOLOR, "Back", BUTTON_TEXTSIZE);
        back.drawButton();
        skip.initButton(&tft, 240, 200,
                        BUTTON_W * 1.5, BUTTON_H + 5, BUTTON_BCOLOR,
                        RED, TEXT_TCOLOR, "Start", BUTTON_TEXTSIZE);
        skip.drawButton();
        bool isIdle = true;
        while(isIdle){
          isIdle = idle(true);
        }
      }
      
      vConnect = true;
      cConnect = true;
      if (place == 0){
        systemState = STATE_START;
      }else if (place == 1){
        start();
        systemState = STATE_DISCHARGE;
      }else if (place == 2){
        start();
        discharge();
        systemState = STATE_DISWAIT;
      }else if (place == 3){
        start();
        discharge();
        disWait();
        systemState = STATE_CHARGE;
      }else if (place == 4){
        start();
        discharge();
        disWait();
        charge();
      }else if (place == 5){
        start();
        chgWait();
        systemState = STATE_DISCHARGE;
      } else if (place == 6){
        dcCycle++;
        start();
        discharge();
        systemState = STATE_DISWAIT;
      }else if (place == 7){
        dcCycle++;
        start();
        discharge();
        disWait();
        systemState = STATE_CHARGE;
      }
}

//checks to see if the skip button has been pressed
void checkSkip() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = (tft.width() - map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0)); //
  }
  if (skip.contains(p.x, p.y))
  {
    skip.press(true);  // tell the button it is pressed
  }
  else
  {
    skip.press(false);  // tell the button it is NOT pressed
  }

  if (skip.justReleased())
  {
    skip.drawButton();  // draw normal
  }

  if (skip.justPressed())
  {
    skip.drawButton(true);  // draw invert!
    tft.setTextColor(WHITE);
    //checks what state the system is in and acts accordingly
    switch (systemState)
    {
      case STATE_DISCHARGE:
        {
          Serial.println(F("Discharge Skipped"));
          sSkip = true;
          break;
        }
      case STATE_DISWAIT:
        {
          Serial.println(F("Skipping Discharge Wait"));
          sSkip = true;
          break;
        }
      case STATE_CHARGE:
        {
          //switches from bulk to finish
          if (chgState)
          {
            bSkip = true;
          }
          else
          {
            Serial.print(F("Skipping Charge "));
            sSkip = true;
          }
          break;
        }
      case STATE_CHGWAIT:
        {
          Serial.println(F("Skipping Charge Wait"));
          sSkip = true;
          break;
        }
      case STATE_DONE:
        {
          Serial.println(F("Restarting"));
          dcCycle = 1;
          loopDelay = 0;
          systemState = BEFORE_CYCLE;
          gasTripped = true;
          chgState = true;
          manualEntry = false;
          sSkip = false;
          bSkip = false;
          voltage_adc_raw = 0;
          current_adc_raw = 0;
          cData = 0;
          vData = 0;
          place = 0;
          initLCD();
          drawHome();
          break;
        }
    }
    delay(100); // UI debouncing
  }
}

//determines which discharge relays to close
void disResist(float resPoint) {
  float bound1 = ((((batteryVoltage / 3) * 3.3) / .5) + ((((batteryVoltage / 3) * 3.3) / .33) * 2)) / 3;
  float bound2 = ((((batteryVoltage / 3) * 3.3) / .33) + ((((batteryVoltage / 3) * 3.3) / .2) * 2)) / 3;
  if (resPoint < bound1)
  {
    Serial.println(F(".5 Ohm Discharge"));
    digitalWrite(DISCHARGE5_PIN, ON);
  }
  else if (resPoint < bound2)
  {
    Serial.println(F(".33 Ohm Discharge"));
    digitalWrite(DISCHARGE33_PIN, ON);
  }
  else if (resPoint >= bound2)
  {
    Serial.println(F(".2 Ohm Discharge"));
    digitalWrite(DISCHARGE5_PIN, ON);
    digitalWrite(DISCHARGE33_PIN, ON);
  }
}

//returns calibration value saved on SD card
double calibrate(uint8_t chanNum) {
  if (isSD) {
    File properties;
    bool bat = true;
    line = "";
    String id;
    double cal;
    properties = SD.open("station.csv", FILE_READ);
    while (bat) {
      line = properties.readStringUntil('\n');
      if (line != "") {
        id = line.substring(0, 12);
        if (id == "Calibration" + String(chanNum)) {
          cal = (line.substring(13, line.length())).toDouble();
          Serial.print("Calibration for channel " + String(chanNum) + " is set to "); Serial.println(cal, 10);
          bat = false;
          properties.close();
          return cal;
        }
      } else {
        Serial.println("!!!!!CALIBRATION NEEDED!!!!!");
        bat = false;
        properties.close();
      }
    }
  }
  return 1;
}

//reads from the ADC channel and converts it to a double
float checkADC(uint8_t chanNum, bool noiseCheck) {
  int temp = 0;
  uint16_t adcC = ads1115.readADC_SingleEnded(chanNum);
  uint8_t count = 0;
  float ADCRangeLo = 0;
  float ADCRangeHi = 65535;
  if (chanNum == 0 && !noiseCheck) {
    ADCRangeLo = ((batteryVoltage / 3) * 2.5) * calibrationV;
    ADCRangeHi = ((batteryVoltage / 3) * 4.0) * calibrationV;
    //Serial.print("ADCRangeLo: "); Serial.println(ADCRangeLo);
    //Serial.print("ADCRangeHi: "); Serial.println(ADCRangeHi);
  }
  for (int i = 0; i < 40; i++)
  {
    temp = ads1115.readADC_SingleEnded(chanNum);
    if (temp >= ADCRangeLo && temp <= ADCRangeHi) {
//    Serial.print("(temp:" + String(temp) + " + adcC:" + String(adcC) + ")/2.0 =");
      adcC = ((temp + adcC) / 2.0);
//      Serial.println(adcC);
    }
    else {
//      Serial.println("REMOVEDVALUE");
    }
    //delay(5);              // A short delay between readings
    //        Serial.print(String(temp) + ",");
    if (temp == 0 || temp == 65535 || temp == -1) {
      count++;
    }
  }
  if (count == 40 && !noiseCheck) {
    if (chanNum == 0) {
      vConnect = false;
    } else {
      cConnect = false;
    }
    return 1;
  } else {
    Serial.println("ADC Mean: " + String(adcC));
    if (noiseCheck) {
      return adcC;
      //      return readings[10];
    } else if (chanNum == 0) {
      return ((adcC) / calibrationV);
      //      return ((readings[10])/calibrationV);
    } else {
      return ((adcC - noise) / calibrationC);
      //      return ((readings[10]-noise)/calibrationC);
    }
  }
}

//saves information to SD card
void saveToSD(String st, float volt, float curr, float temp) {
  if (!isSD) {
    return;
  }
  SDlogs = SD.open(fileName, FILE_WRITE);
  st.trim();
  SDlogs.print(" ");
  SDlogs.print(String(nowClock) + " ");
  SDlogs.print(String(volt) + " ");
  SDlogs.print(String(curr) + " ");
  SDlogs.print(String(temp) + " ");
  SDlogs.println(analogRead(GD_PIN));
  SDlogs.close();
}

//checks to see if the gas sensor has been tripped
void checkGas(void) {
      uint16_t gasLevel;
      for(int i = 0; i < 20; i++){
        gasLevel = analogRead(GD_PIN);
        if(gasLevel <= GD_THRESHOLD){
          gasTripped = true;
          return;
        }else{
          gasTripped = false;
        }
      }
      if(!gasTripped){
//        digitalWrite(DISCHARGE5_PIN, OFF);
//        digitalWrite(DISCHARGE33_PIN, OFF);
//        digitalWrite(CHARGE_PIN, OFF);
//        digitalWrite(CHARGE_POWER_1, OFF);
//        digitalWrite(CHARGE_POWER_2, OFF);
//        digitalWrite(CHARGE_POWER_3, OFF);
//        digitalWrite(POWER_SUPPLY_PIN, OFF);
//        digitalWrite(CYC_FAN, OFF);
//        digitalWrite(EXH_FAN, OFF);
        Serial.println("HARMFUL GAS DETECTED: " + String(gasLevel));
//        Serial.println(F("PROGRAM ENTERING IDLE STATE")); tft.setTextSize(1);
//        tft.setCursor(INFO_X , STATE_Y);
//        tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
//        tft.print(" Gas detected time-out");
//        doneLbl = "Gas detected time-out";
//        nowClock = 0;
//        systemState = STATE_DONE;
      }
}

//prints the time
String formatTime(void) {
  String currTime;
  if ((nowClock / HOUR) > 0)
  {
    currTime = String(nowClock / HOUR) + "h ";
    nowClock = nowClock - (((nowClock / HOUR)) * HOUR);
  }
  if ((nowClock / MINUTE) > 0)
  {
    currTime = currTime + String(nowClock / MINUTE);
    currTime = currTime + "m ";
    nowClock = nowClock - (((nowClock / MINUTE)) * MINUTE);
  }
  currTime = currTime + nowClock;
  currTime = currTime + "s";
  return currTime;
}

//prints info (time, temp, gas lvl, voltage, and current)
void printInfo(float volt, float curr, float temper) {
  String fTime = formatTime();
  uint16_t gasLvl = analogRead(GD_PIN);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);

  Serial.print(F("Time: ")); Serial.println(fTime);
  tft.fillRect(INFO_X, TIME_Y, CLEAR_INFO_X, CLEAR_INFO_Y, DARKGREY);
  tft.setCursor(INFO_X , TIME_Y);
  tft.print(fTime);

  Serial.print(F("Temperature: "));
  tft.fillRect(INFO_X, TEMPERATURE_Y, CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.setCursor(INFO_X, TEMPERATURE_Y);
  if (temper != 0)
  {
    Serial.println(temper);
    tft.print(String(temper) + " C");
  }
  else
  {
    Serial.println(F("No Connection"));
    tft.print("No Connection");
  }

  Serial.print(F("Voltage: "));
  tft.fillRect(INFO_X, VOLTAGE_Y, CLEAR_INFO_X, CLEAR_INFO_Y, DARKGREY);
  tft.setCursor(INFO_X , VOLTAGE_Y);
  if (vConnect)
  {
    Serial.println(volt);
    tft.print(String(volt) + " V");
  }
  else
  {
    Serial.println(F("No Connection"));
    tft.print("No Connection");
  }

  Serial.print(F("Current: "));
  tft.fillRect(INFO_X, CURRENT_Y, CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.setCursor(INFO_X , CURRENT_Y);
  if (cConnect)
  {
    Serial.println(curr);
    tft.print(String(curr) + " A");
  }
  else
  {
    Serial.println(F("No Connection"));
    tft.print("No Connection");
  }
  Serial.print(F("Gas Level: "));
  Serial.println(gasLvl);
  if (gasLvl >= GD_THRESHOLD && gasTripped) {
    checkGas();
  }
}

//code that initializes the screen that the cycles will be run on
void start(void) {
  Serial.println(F("Printing information screen"));
  totalTime = 0;
  stateTimer = MAX_DIS_TIME;
  delayTimer = D1WAIT_TIME;
  xVolts =  (batteryVoltage / 3) * 2.75;
  xCurrent = capacity / 3;
  noise = checkADC(ADC_CURRENT2_CHANNEL, true);
  Serial.print("Noise detected on channel " + String(ADC_CURRENT2_CHANNEL) + " : ");
  Serial.println(noise);
  calibrationV = calibrate(ADC_VOLTAGE_CHANNEL);
  calibrationC = calibrate(ADC_CURRENT2_CHANNEL);
  disResist(xCurrent);
  digitalWrite(CYC_FAN, ON);
  digitalWrite(EXH_FAN, ON);
  doneLbl = "Done";
  ++totalTime;
  initLCD();
  Serial.print(F("Printing stripes..."));
  tft.fillScreen(LIGHTGREY);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  //draws grey stripes
  tft.fillRect(DARK_STRIPE_X, DARK_STRIPE_Y1, DARK_STRIPE_W, DARK_STRIPE_H, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, DARK_STRIPE_Y1, DARK_STRIPE_W, DARK_STRIPE_H, BLACK);
  tft.fillRect(DARK_STRIPE_X, DARK_STRIPE_Y2, DARK_STRIPE_W, DARK_STRIPE_H, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, DARK_STRIPE_Y2, DARK_STRIPE_W, DARK_STRIPE_H, BLACK);
  tft.fillRect(DARK_STRIPE_X, DARK_STRIPE_Y3, DARK_STRIPE_W, DARK_STRIPE_H, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, DARK_STRIPE_Y3, DARK_STRIPE_W, DARK_STRIPE_H, BLACK);
  tft.fillRect(DARK_STRIPE_X, DARK_STRIPE_Y4, DARK_STRIPE_W, DARK_STRIPE_H, DARKGREY);
  tft.drawRect(DARK_STRIPE_X, DARK_STRIPE_Y4, DARK_STRIPE_W, DARK_STRIPE_H, BLACK);
  Serial.println(F("done"));

  //prints version & SD status
  Serial.print(F("Printing version and SD status..."));
  tft.setTextSize(1);
  tft.setCursor(SUB_INFO_X - 10, V_INFO_Y);
  tft.print("V"); tft.print(VERSION_NO);

  tft.setCursor(SUB_INFO_X, SD_INFO_Y);
  tft.print("SD:");
  if (saveSD)
  {
    tft.setTextColor(GREEN);
    tft.print("ON");
  }
  else
  {
    tft.setTextColor(RED);
    tft.print("OFF");
  }
  Serial.println(F("done"));

  Serial.print(F("Printing Battery label..."));
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  //prints the Battery tag
  tft.setCursor(BATTERY_X, BATTERY_Y);
  tft.print("Battery #:");
  tft.setCursor(INFO_X, BATTERY_Y);
  tft.print(combined);
  Serial.println(F("done"));

  //prints the Capacity tag
  Serial.print(F("Printing Capacity/Voltage label..."));
  tft.setCursor(CAPACITY_X, CAPACITY_Y);
  tft.print("Vol:");
  tft.print(batteryVoltage);
  tft.print("V");
  tft.print(" C:");
  tft.setCursor(INFO_X, CAPACITY_Y);
  tft.print(capacity);
  tft.print("Ah");
  Serial.println(F("done"));

  //draws the skip button
  Serial.print(F("Printing Skip button..."));
  skip.initButton(&tft, 275, CAPACITY_Y + 5, BUTTON_W, BUTTON_H, BLACK, DARKGREY, WHITE, "Skip", 2);
  skip.drawButton();
  Serial.println(F("done"));

  //prints the Serial   b# tag
  Serial.print(F("Printing Serial label..."));
  tft.setCursor(SERIAL_X, SERIAL_Y);
  tft.print("Serial #:");
  tft.setCursor(INFO_X, SERIAL_Y);
  tft.print(sNumber);
  Serial.println(F("done"));

  //prints the State tag
  Serial.print(F("Printing State label..."));
  tft.setCursor(STATE_X, STATE_Y);
  tft.print("State:");
  Serial.println(F("done"));

  //prints the Time tag
  Serial.print(F("Printing Time label..."));
  tft.setCursor(TIME_X, TIME_Y);
  tft.print("Time:");
  Serial.println(F("done"));

  //prints temp tag
  Serial.print(F("Printing Temperature label..."));
  tft.setCursor(TEMPERATURE_X, TEMPERATURE_Y);
  tft.print("Temp:");
  Serial.println(F("done"));

  //prints the Voltage tag
  Serial.print(F("Printing Voltage label..."));
  tft.setCursor(VOLTAGE_X, VOLTAGE_Y);
  tft.print("Voltage:");
  Serial.println(F("done"));

  //prints the Current tag
  Serial.print(F("Printing Current label..."));
  tft.setCursor(CURRENT_X, CURRENT_Y);
  tft.print("Current:");
  Serial.println(F("done"));
  Serial.println(F("Finished printing Information screen")); +

  Serial.print(F("Starting discharge ")); Serial.println(dcCycle);
  tempData = "";
  tempData = " Discharge " + String(dcCycle);
  delay(1000);
  tft.setCursor(INFO_X , STATE_Y);
  tft.print(tempData);
  if (saveSD)
  {
    SDlogs = SD.open(fileName, FILE_WRITE);
    SDlogs.print(String(combined) + " ");
    SDlogs.print(String(sNumber) + " ");
    SDlogs.print(String(batteryVoltage) + " ");
    SDlogs.println(String(capacity));
    SDlogs.println("");
    SDlogs.println(" Time(S) Voltage(V) Current(A) Temperature(C) GasLevel");
    SDlogs.close();
  }
  //print Discharge next to the system tag
}

//exit code for the discharge state of the cycle
void discharge(void) {
  sSkip = false;
  //turns the discharge pin OFF
  digitalWrite(DISCHARGE5_PIN, OFF);
  digitalWrite(DISCHARGE33_PIN, OFF);
  delay(1000);
  ++totalTime;
  noise = checkADC(ADC_CURRENT2_CHANNEL, true);
  Serial.print("Noise detected on channel " + String(ADC_CURRENT2_CHANNEL) + " : ");
  Serial.println(noise);
  //calculates how long the discharge took
  Serial.print(F("Discharge time: "));
  Serial.println(formatTime());
  Serial.print(F("Starting post-discharge wait ")); Serial.println(dcCycle);
  tempData = "";
  tempData = " Post-Discharge Wait " + String(dcCycle);
  tft.setTextSize(1);
  //clears the state on tft screen
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.print(tempData);
}

//exit code for the discharge wait state of the cycle
void disWait() {
  sSkip = false;
  stateTimer = MAX_CHG_TIME;
  noise = checkADC(ADC_CURRENT_CHANNEL, true);
  Serial.print("Noise detected on channel " + String(ADC_CURRENT_CHANNEL) + " : ");
  Serial.println(noise);
  calibrationC = calibrate(ADC_CURRENT_CHANNEL);
  digitalWrite(CYC_FAN, ON);
  digitalWrite(EXH_FAN, ON);
  //turns charge pin ON
  Serial.println(F("Charge pin turning ON"));
  digitalWrite(CHARGE_PIN, ON);
  Serial.println(F("Charge Power pin 1 turning ON"));
  digitalWrite(CHARGE_POWER_1, ON);
  Serial.println(F("Charge Power pin 2 turning ON"));
  digitalWrite(CHARGE_POWER_2, ON);
  Serial.println(F("Charge Power pin 3 turning ON"));
  digitalWrite(CHARGE_POWER_3, ON);

  chgState = true;
  xVolts = (batteryVoltage / 3) * 3.8;
  xCurrent = capacity / 10;
  Serial.print(F("Capacity: ")); Serial.println(capacity);
  Serial.print(F("Voltage High Threshold: ")); Serial.println(xVolts);
  Serial.print(F("Bulk Current Low Threshold: ")); Serial.println(xCurrent);
  //sets DAC output
  vData = (3.6 * (batteryVoltage / 3) / 57.6) * 4095;
  if (dcCycle == 1)
  {
    cData = (capacity / 5.0 / SUPPLY_OUTPUT) * 4095;
  }
  else
  {
    if ((batteryVoltage / 3) > 12 && capacity >= 540)
    {
      tft.setCursor(INFO_X, CAPACITY_Y);
      tft.fillRect(INFO_X , CAPACITY_Y , INFO_X/3, CLEAR_INFO_Y, LIGHTGREY);
      tft.setTextSize(2);
      tft.print(capacity);
      tft.print("Ah");
      tft.setTextSize(1);
      tft.print("*525");
      capacity = 525;
    }
    cData = (capacity / 5.0 / SUPPLY_OUTPUT) * 4095;  //<=============================================!!!!Change back to c/3 after move!!!!
  }

  if (vData > 4095) {
    vData = 4095;
  }
  else if (vData < 683) {
    vData = 683;
  }

  if (cData > 4095) {
    cData = 4095;
  }
  else if (cData < 819) {
    cData = 819;
  }

  Serial.println("vData: " + String(vData));
  Serial.println("cData: " + String(cData));
  voltage_adc_raw = (0x0FFF) & String(vData).toInt();
  current_adc_raw = (0x0FFF) & String(cData).toInt();
  dac_voltage_cntrl.setVoltage( (0x0FFF) & voltage_adc_raw, false);
  dac_current_cntrl.setVoltage( (0x0FFF) & current_adc_raw, false);
  Serial.print(F("vRaw: ")); Serial.println(voltage_adc_raw);
  Serial.print(F("cRaw: ")); Serial.println(current_adc_raw);
  tft.setTextSize(1);
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  Serial.println(F("Calibrating Current 1..."));

  //delay between turning the charge pin ON and the power supply pin ON
  Serial.println(F("Power supply pin turning ON"));
  tft.print("Turning Power Supply On.");
  delay(500); tft.print("."); delay(500); tft.print("."); delay(500);
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.print("Turning Power Supply On.");
  delay(500); tft.print("."); delay(500); tft.print("."); delay(500);
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.print("Turning Power Supply On.");
  delay(500); tft.print("."); delay(500); tft.print("."); delay(500);

  //delay for the power supply to turn ON
  digitalWrite(POWER_SUPPLY_PIN, ON);
  digitalWrite(SS_RELAY_GND, ON);
  tft.setTextSize(2);
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  Serial.println(F("Please wait..."));
  tft.print("Please Wait.");
  delay(500); tft.print("."); delay(500); tft.print("."); delay(500);
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.print("Please Wait.");
  delay(500); tft.print("."); delay(500); tft.print("."); delay(500);
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.print("Please Wait.");
  delay(500); tft.print("."); delay(500); tft.print("."); delay(500);

  totalTime = totalTime + 9;
  Serial.print(F("Starting charge ")); Serial.println(dcCycle);
  tempData = "";
  tempData = "Charge " + String(dcCycle);
  cycleTime = now;
  tft.setTextSize(2);
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.print(tempData + ":Bulk");
}

//exit code for the charge state of the cycle
void charge(void) {
  sSkip = false;
  bSkip = false;
  delay(1000);
  ++totalTime;
  //DAC to the power supply is set to 1 V
  //              voltage_adc_raw = 683;
  //              current_adc_raw = 819;
  digitalWrite(POWER_SUPPLY_PIN, OFF);
  delay(500);
  voltage_adc_raw = 0;
  current_adc_raw = 0;
  dac_voltage_cntrl.setVoltage( (0x0FFF) & voltage_adc_raw, false);
  dac_current_cntrl.setVoltage( (0x0FFF) & current_adc_raw, false);
  digitalWrite(CHARGE_PIN, OFF);
  //calculates how long charging took
  Serial.print(F("Charge time: "));
  Serial.println(formatTime());

  //if the maximum number of cycles has been reached, system is set to an idle state
  if (dcCycle < MAX_CYCLE)
  {
    delay(1000);
    ++totalTime;
    noise = checkADC(ADC_CURRENT_CHANNEL, true);
    Serial.print("Noise detected on channel " + String(ADC_CURRENT_CHANNEL) + " : ");
    Serial.println(noise);
    Serial.println(F("Starting post-charge wait..."));
    delayTimer = CWAIT_TIME;
    stateTimer = FWAIT_TIME;
    tft.setTextSize(1);
    tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
    tft.setCursor(INFO_X , STATE_Y);
    tft.print(" Post-Charge Wait");
    systemState = STATE_CHGWAIT;
  }
  else
  {
    Serial.println(F("Cycles finished"));
    digitalWrite(CHARGE_POWER_1, OFF);
    digitalWrite(CHARGE_POWER_2, OFF);
    digitalWrite(CHARGE_POWER_3, OFF);
    digitalWrite(SS_RELAY_GND, OFF);
    digitalWrite(CYC_FAN, OFF);
    digitalWrite(EXH_FAN, OFF);
    delay(1000);
    ++totalTime;
    noise = checkADC(ADC_CURRENT_CHANNEL, true);
    Serial.print("Noise detected on channel " + String(ADC_CURRENT_CHANNEL) + " : ");
    Serial.println(noise);
    chgState = true;
    tft.setCursor(INFO_X , STATE_Y);
    tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
    tft.print(" Done");
    nowClock = 0;
    systemState = STATE_DONE;
  }
}

//exit code for the charge wait state of the cycle
void chgWait(void) {
  sSkip = false;
  stateTimer = MAX_DIS_TIME;
  delayTimer = D2WAIT_TIME;
  noise = checkADC(ADC_CURRENT2_CHANNEL, true);
  Serial.print("Noise detected on channel " + String(ADC_CURRENT2_CHANNEL) + " : ");
  Serial.println(noise);
  calibrationC = calibrate(ADC_CURRENT2_CHANNEL);
  digitalWrite(CYC_FAN, ON);
  digitalWrite(EXH_FAN, ON);
  xVolts =  (batteryVoltage / 3) * 2.75;
  xCurrent = capacity / 5;
  dcCycle++;

  //turns the discharge pin ON

  Serial.print(F("Starting discharge ")); Serial.println(dcCycle);
  tft.setTextSize(2);
  tempData = "";
  tempData = " Discharge " +  String(dcCycle);
  //changes the displayed state on tft screen
  tft.setCursor(INFO_X , STATE_Y);
  tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
  tft.print(tempData);
  disResist(xCurrent);
  delay(1000);
  ++totalTime;
}

//code that will loop constantly
void loop(void) {
  //records the current system time
  now = millis();
  //system state for entering the model information
  if (systemState == BEFORE_CYCLE)
  {
    if (screenPage == SCREEN_ENTRY) //serial entry
    {
      modelEntry();
    }
    else if (screenPage == SCREEN_PULL) //draws yes no screen
    {
      pullPage();
    }
    else if (screenPage == SCREEN_CHECK_SAVE) //yes no reset screen
    {
      checkSave();
    }
    else if (screenPage == SCREEN_ENTRY_VOLTS) //voltage entry
    {
      voltsEntry();
    }
    else if (screenPage == SCREEN_CAPACITY)
    {
      capacityEntry();
    }
    else if (screenPage == SCREEN_SERIAL)
    {
      serialEntry();
    }
    else if (screenPage == SCREEN_SKIP)
    {
      skipEntry();
    }
    else if (screenPage == SCREEN_CALIBRATION) {
      calibrationEntry();
    }
    else if (screenPage == SCREEN_HOMEPAGE) {
      homeEntry();
    }
    else if (screenPage == SCREEN_SETTINGS) {
      settingsEntry();
    }
    else if (screenPage == SCREEN_OPTIONS) {
      optionsEntry();
    }
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  else
  {
    //if the alloted time has passed, execute the system states
    if (now - loopDelay > WAIT)
    {
      float voltage;
      float current;
      float temperature;
      vConnect = true;
      cConnect = true;
      loopDelay = now;

      switch (systemState)
      {
        case STATE_START: //initialized the discharge state and draws the tags on the Tft screen
          {
            start();
            systemState = STATE_DISCHARGE;
            cycleTime = now;
            break;
          }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case STATE_DISCHARGE: //state keeps the discharge pin ON until the battery reaches a set voltage
          {
            //checks if the gas reading is over a set threshold
            voltage = checkADC(ADC_VOLTAGE_CHANNEL);
            current = checkADC(ADC_CURRENT2_CHANNEL);
            temperature = tc.readCelsius();
            nowClock = MAX_DIS_TIME - stateTimer;
            if (nowClock % MINUTE == 0)
            {
              if (saveSD)
              {
                saveToSD("D" + (String(dcCycle)), voltage, (-1)*current, temperature);
              }
            }
            //prints to the tft and serial screens
            Serial.println("State: Discharge " + String(dcCycle));
            ++totalTime;
            sendData("Discharge " + String(dcCycle), voltage, (-1)*current, temperature);
            printInfo(voltage, (-1)*current, temperature);
            //if the voltage reaches a set level or the skip button has been pressed
            if (voltage < xVolts || sSkip)// || current < xCurrent)
            {
              discharge();
              systemState = STATE_DISWAIT;
            }
            //if the discharge took longer than the alloted time, system switches to a time-out state
            if (--stateTimer <= 0)
            {
              chgState = false;
              digitalWrite(DISCHARGE5_PIN, OFF);
              digitalWrite(DISCHARGE33_PIN, OFF);
              digitalWrite(CYC_FAN, OFF);
              digitalWrite(EXH_FAN, OFF);
              Serial.println(F("WARNING: Discharge taken longer than maximum allowed time"));
              Serial.println(F("Exiting cycle..."));
              tft.setTextSize(1);
              tft.setCursor(INFO_X , STATE_Y);
              tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
              tft.print(" Discharge time-out");
              doneLbl = "Charge time-out";
              nowClock = 0;
              systemState = STATE_DONE;
            }
            break;
          }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case STATE_DISWAIT: //waits a set time between discharging and charging
          {
            current = checkADC(ADC_CURRENT2_CHANNEL);
            voltage = checkADC(ADC_VOLTAGE_CHANNEL);
            temperature = tc.readCelsius();

            //determines which cycle the system is on
            if (dcCycle == 1)
            {
              nowClock = D1WAIT_TIME - delayTimer;
            }
            else
            {
              nowClock = D2WAIT_TIME - delayTimer;
            }
            if (nowClock >= FWAIT_TIME || sSkip)
            {
              digitalWrite(CYC_FAN, OFF);
              digitalWrite(EXH_FAN, OFF);
            }
            if (nowClock % MINUTE == 0)
            {
              if (saveSD)
              {
                saveToSD("PD" + String(dcCycle), voltage, current, temperature);
              }
            }
            //prints to the tft and serial screens
            Serial.println("State: Discharge Wait " + String(dcCycle));
            ++totalTime;
            sendData("Discharge Wait" + String(dcCycle), voltage, current, temperature);
            printInfo(voltage, current, temperature);
            //if the time as elapsed or the skip button has been pressed
            if (--delayTimer <= 0 || sSkip)
            {
              disWait();
              systemState = STATE_CHARGE;
            }
            break;
          }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        case STATE_CHARGE: //keeps the charge and power supply pin ON until a set voltage is reached
          {
            voltage = checkADC(ADC_VOLTAGE_CHANNEL);
            current = checkADC(ADC_CURRENT_CHANNEL);
            temperature = tc.readCelsius();
            nowClock = MAX_CHG_TIME - stateTimer;

            //saves to the SD card every 10 seconds
            Serial.print(F("State: "));
            if (chgState)
            {
              Serial.println("Bulk " + String(dcCycle));
              sendData("BulkCharge" + String(dcCycle), voltage, current, temperature);
              if (nowClock % MINUTE == 0)
              {
                if (saveSD)
                {
                  saveToSD("BC" + (String(dcCycle)), voltage, current, temperature);
                }
              }
            } else
            {
              Serial.println("Finish " + String(dcCycle));
              sendData("FinishCharge" + String(dcCycle), voltage, current, temperature);
              if (nowClock % MINUTE == 0)
              {
                if (saveSD)
                {
                  saveToSD("FC" + (String(dcCycle)), voltage, current, temperature);
                }
              }
            }
            //prints to the tft and serial screens
            ++totalTime;
            printInfo(voltage, current, temperature);

            //if the current drops below the set ammount, system state switched from bulk to finish
            if (current < xCurrent || bSkip)
            {
              if (chgState)
              {
                Serial.println(F("Switching to Finish..."));
                chgState = false;
                vData = (3.8 * (batteryVoltage / 3) / 57.6) * 4095;
                cData = (capacity / 10.0 / SUPPLY_OUTPUT) * 4095;
                if (vData > 4095) {
                  vData = 4095;
                }
                else if (vData < 683) {
                  vData = 683;
                }
                if (cData > 4095) {
                  cData = 4095;
                }
                else if (cData < 819) {
                  cData = 819;
                }
                Serial.println("vData: " + String(vData));
                Serial.println("cData: " + String(cData));
                voltage_adc_raw = (0x0FFF) & String(vData).toInt();
                current_adc_raw = (0x0FFF) & String(cData).toInt();
                Serial.print(F("vRaw: ")); Serial.println(voltage_adc_raw);
                Serial.print(F("cRaw: ")); Serial.println(current_adc_raw);
                dac_voltage_cntrl.setVoltage( (0x0FFF) & voltage_adc_raw, false);
                dac_current_cntrl.setVoltage( (0x0FFF) & current_adc_raw, false);
                tempData = "";
                tempData = "Charge " + String(dcCycle);
                cycleTime = now;
                tft.setTextSize(2);
                tft.setCursor(INFO_X , STATE_Y);
                tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
                tft.print(tempData + ":Finish");
                bulkTime = nowClock;
              }
            }

            //if voltage is higher than the set ammount
            //if current drops below low current threshold
            //if the skip button is pressed, the state is switched
            if (voltage > xVolts || current < CURR_LOW_THRESH || voltage < VOLT_LOW_THRESH || sSkip)
            {
              charge();
            }

            //if time exceeds the alloted charge time, cycle halts and enters an time-out idle state
            if (--stateTimer <= 0)
            {
              voltage_adc_raw = 0;
              current_adc_raw = 0;
              dac_voltage_cntrl.setVoltage( (0x0FFF) & voltage_adc_raw, false);
              dac_current_cntrl.setVoltage( (0x0FFF) & current_adc_raw, false);
              digitalWrite(CHARGE_PIN, OFF);
              digitalWrite(CHARGE_POWER_1, OFF);
              digitalWrite(CHARGE_POWER_2, OFF);
              digitalWrite(CHARGE_POWER_3, OFF);
              digitalWrite(SS_RELAY_GND, OFF);
              digitalWrite(POWER_SUPPLY_PIN, OFF);
              digitalWrite(CYC_FAN, OFF);
              digitalWrite(EXH_FAN, OFF);
              Serial.println(F("WARNING: Charge taken longer than maximum allowed time"));
              Serial.println(F("Exiting cycle..."));
              tft.setCursor(INFO_X , STATE_Y);
              tft.fillRect(INFO_X , STATE_Y , CLEAR_INFO_X, CLEAR_INFO_Y, LIGHTGREY);
              tft.setTextSize(1);
              tft.print(" Charge time-out");
              doneLbl = "Charge time-out";
              chgState = true;
              nowClock = 0;
              systemState = STATE_DONE;
            }
            break;
          }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        case STATE_CHGWAIT: //waits a set time between chargingand discharging
          {
            voltage = checkADC(ADC_VOLTAGE_CHANNEL);
            current = checkADC(ADC_CURRENT_CHANNEL);
            temperature = tc.readCelsius();
            nowClock = CWAIT_TIME - delayTimer;

            //power supply fans shuts off after a set time
            if (nowClock >= FWAIT_TIME || sSkip)
            {
              digitalWrite(CHARGE_POWER_1, OFF);
              digitalWrite(CHARGE_POWER_2, OFF);
              digitalWrite(CHARGE_POWER_3, OFF);
              digitalWrite(SS_RELAY_GND, OFF);
              digitalWrite(CYC_FAN, OFF);
              digitalWrite(EXH_FAN, OFF);
            }
            //saves to SD card once every minute
            if (nowClock % MINUTE == 0)
            {
              if (saveSD)
              {
                saveToSD("PC", voltage, current, temperature);
              }
            }
            //prints to the tft and serial screens
            Serial.println(F("State: Charge Wait"));
            ++totalTime;
            sendData("Charge Wait", voltage, current, temperature);
            printInfo(voltage, current, temperature);
            //if the alloted time has elapsed or if the skip button mhas been pressed, switch states
            if (--delayTimer <= 0 || sSkip)
            {
              chgWait();
              systemState = STATE_DISCHARGE;
            }
            break;
          }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        case STATE_DONE:
          {
            //when finished, program idles, displaying voltage, current, and time
            voltage = checkADC(ADC_VOLTAGE_CHANNEL);
            nowClock = doneTime;
            if (chgState) {
              current = checkADC(ADC_CURRENT_CHANNEL);
            } else {
              current = checkADC(ADC_CURRENT2_CHANNEL);
            }
            temperature = tc.readCelsius();
            if (nowClock % MINUTE == 0)
            {
              if (saveSD)
              {
                saveToSD(doneLbl, voltage, current, temperature);
              }
            }
            doneTime++;
            //prints to the tft and serial screens
            sendData("Done", voltage, current, temperature);
            printInfo(voltage, current, temperature);
            break;
          }
      }
    }
    //calls method to check if the skip button has been pressed
    checkSkip();
  }
}
//two call connection with external ESP32 board
void sendData(String state, float volt, float curr, float  temp) {
  String data = String(combined) + "," + String(sNumber) + "," +
                String(state) + "," + String(nowClock) + "," + String(temp) + "," +
                String(volt) + "," + String(curr) + "," + String(totalTime) + ",";
  Serial.println(F("sending data"));
  Serial2.println(data);
}

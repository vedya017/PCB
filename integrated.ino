#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LedControl.h>

LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 16 chars and 2-line
display

int MQ4 = A0; // select the analog input pin for the MQ4 sensor
int MQ7 = A1;
int MQ136 = A2;
int MQ137 = A3;
int BUZZER_pin =2;
int value;
int value1;
int value2;
int value3;
int value4;

float MQ4_R0;
float MQ7_R0;
float MQ136_R0;
float MQ137_R0;

int DIN_PIN = 13;
int CS_PIN= 12;
int CLK_PIN =11;

int clk_c=0;
int res_c=1;

const int BCD_A = 10;
const int BCD_B = 7;
const int BCD_C = 8;
const int BCD_D = 9;

// create an instance of the LedControl library
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1);

float R0_calculation(byte sens_pin)
{
float sensorValue;
for(int i=0; i<50; i++)
{
sensorValue = sensorValue + analogRead(sens_pin);
}
sensorValue = sensorValue/50.0;
float sensor_V = sensorValue*(5.0/1023.0);
float RS_air = ((5.0*10.0)/sensor_V)-10;
float R0 = RS_air/4.4;
return R0;
}

//// function to calculate PPM of each gas from the voltage readings

double gas_PPM(byte sens_pin, float R0)
{
float sensorValue = analogRead(sens_pin);
float sensor_V = sensorValue*(5.0/1023.0);
float RS_gas = ((5.0*10.0)/sensor_V)-10;
float ratio = RS_gas/R0;
double ppm_log = (log10(ratio)-1.133)/(-0.318); //Get ppm value in linear scale
according to the ratio value
double ppm = pow(10, ppm_log); //Convert ppm value to log scale
return ppm;
}

void show_number(int num)
{
switch(num)
{
case 0:
digitalWrite(BCD_A, LOW);
digitalWrite(BCD_B, LOW);
digitalWrite(BCD_C, LOW);
digitalWrite(BCD_D, LOW);
break;
case 1:
digitalWrite(BCD_A, HIGH);
digitalWrite(BCD_B, LOW);
digitalWrite(BCD_C, LOW);
digitalWrite(BCD_D, LOW);

break;
case 2:
digitalWrite(BCD_A, LOW);
digitalWrite(BCD_B, HIGH);
digitalWrite(BCD_C, LOW);
digitalWrite(BCD_D, LOW);
break;
case 3:
digitalWrite(BCD_A, HIGH);
digitalWrite(BCD_B, HIGH);
digitalWrite(BCD_C, LOW);
digitalWrite(BCD_D, LOW);
break;
case 4:
digitalWrite(BCD_A, LOW);
digitalWrite(BCD_B, LOW);
digitalWrite(BCD_C, HIGH);
digitalWrite(BCD_D, LOW);
break;
case 5:
digitalWrite(BCD_A, HIGH);
digitalWrite(BCD_B, LOW);
digitalWrite(BCD_C, HIGH);
digitalWrite(BCD_D, LOW);
break;
case 6:
digitalWrite(BCD_A, LOW);

digitalWrite(BCD_B, HIGH);
digitalWrite(BCD_C, HIGH);
digitalWrite(BCD_D, LOW);
break;
case 7:
digitalWrite(BCD_A, HIGH);
digitalWrite(BCD_B, HIGH);
digitalWrite(BCD_C, HIGH);
digitalWrite(BCD_D, LOW);
break;
case 8:
digitalWrite(BCD_A, LOW);
digitalWrite(BCD_B, LOW);
digitalWrite(BCD_C, LOW);
digitalWrite(BCD_D, HIGH);
break;
case 9:
digitalWrite(BCD_A, HIGH);
digitalWrite(BCD_B, LOW);
digitalWrite(BCD_C, LOW);
digitalWrite(BCD_D, HIGH);
break;
default:
digitalWrite(BCD_A, LOW);
digitalWrite(BCD_B, LOW);
digitalWrite(BCD_C, LOW);
digitalWrite(BCD_D, LOW);

break;
}
}

void setup ()
{

//Calcuate R0 of each sensor after power ON
MQ4_R0 = R0_calculation (MQ4);
MQ7_R0 = R0_calculation (MQ4);
MQ136_R0 = R0_calculation (MQ4);
MQ137_R0 = R0_calculation (MQ4);

lcd.init(); // initialize the LCD
lcd.backlight(); // turn on the backlight
lcd.clear();

// initialize the LED matrix
lc.shutdown(0, false); // turn on the display
lc.setIntensity(0, 8); // set the brightness (0-15)
lc.clearDisplay(0); // clear the display

pinMode(clk_c,OUTPUT);
pinMode(res_c,OUTPUT);

pinMode(BCD_A , OUTPUT);

pinMode(BCD_B , OUTPUT);
pinMode(BCD_C , OUTPUT);
pinMode(BCD_D , OUTPUT);
}

void loop () {

value1= analogRead(MQ4);
value2= analogRead(MQ7);
value3= analogRead(MQ136);
value4= analogRead(MQ137);

//calcuate PPM of each gas/sensor
double MQ4_PPM = gas_PPM(MQ4, MQ4_R0);
double MQ7_PPM = gas_PPM(MQ7, MQ7_R0);
double MQ136_PPM = gas_PPM(MQ136, MQ136_R0);
double MQ137_PPM = gas_PPM(MQ137, MQ137_R0);

lcd.setCursor(0,0); // set the cursor to the first column, first-row
lcd.print("CH4:"+String(value1)+" PPM "); // print the text
lcd.setCursor(0,1); // set the cursor to the first column, second-row
lcd.print("CO:"+String(value2)+" PPM ");
lcd.setCursor(0,2);
lcd.print("H2S:"+String(value3)+" PPM ");
lcd.setCursor(0,3);
lcd.print("NH3:"+String(value4)+" PPM ");

value= (value4*9)/220; //The value of hazardous level of ammonia gas is 220ppm
show_number(value) ;

delay(100); // wait for 1 second

//<--------------------- Buzzer linked with MQ-4---------------------------------------------------------
------->//
// BUZZER rings if gas concentration crosses 1000PPM
if(value1 > 50)
{
digitalWrite(BUZZER_pin, HIGH);
}
else{
digitalWrite(BUZZER_pin, LOW);
}

if (value3>100)
{
lc.setRow(0, 0, B11111111); // row 0, column 0-7
lc.setRow(0, 1, B11111111); // row 1, column 0-7
lc.setRow(0, 2, B11111111); // row 2, column 0-7
lc.setRow(0, 3, B00000000); // row 3, column 0-7
lc.setRow(0, 4, B00000000); // row 4, column 0-7
lc.setRow(0, 5, B00000000); // row 5, column 0-7
lc.setRow(0, 6, B00000000); // row 6, column 0-7

lc.setRow(0, 7, B00000000); // row 7, column 0-7
}
else
{
lc.setRow(0, 0, B00000000); // row 0, column 0-7
lc.setRow(0, 1, B00000000); // row 1, column 0-7
lc.setRow(0, 2, B00000000); // row 2, column 0-7
lc.setRow(0, 3, B00000000); // row 3, column 0-7
lc.setRow(0, 4, B00000000); // row 4, column 0-7
lc.setRow(0, 5, B00000000); // row 5, column 0-7
lc.setRow(0, 6, B00000000); // row 6, column 0-7
lc.setRow(0, 7, B00000000); // row 7, column 0-7
}

if(value2<=50)
{
for(int i=0;i<15;i++)
{
digitalWrite(clk_c, HIGH);
delay(200); // Approximately 10% duty cycle @ 1KHz
digitalWrite(clk_c, LOW);
}
digitalWrite(res_c,HIGH);
delay(1000);
}

else if(50<value2<220)

{
for(int j=15;j<2160;j++)
{
digitalWrite(clk_c, HIGH);
delay(200); //
digitalWrite(clk_c, LOW);
delay(800);
}
digitalWrite(res_c,HIGH);
delay(1000);
}

else
{
for(int k=0;k<1920;k++)
{
digitalWrite(clk_c, HIGH);
delay(200); // Approximately 10% duty cycle @ 1KHz
digitalWrite(clk_c, LOW);
delay(800);
}
digitalWrite(res_c,HIGH);
delay(1000);
}
}
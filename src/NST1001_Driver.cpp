/*ToDo: 
 * Add Pulldown mode and "Multicast" mode
 * Shorten the wait time during conversion and transmission. 
 */
#include "NST1001_Driver.h"


//Will declare varibles in constructor...
NST1001::NST1001(){
}

const void NST1001::init(int const Enable_Pin, int const Temp_Unit){
/*  Setup timer-counter 1/16-bit timer  
 *  Maximum number of pulses = 3201 => 150 degrees celsius
*/
  cli();
  EN_Pin = Enable_Pin;
  Unit   = Temp_Unit;

  pinMode(EN_Pin, OUTPUT);
  
//Timer 1 Normal mode with falling edge as clock source.
  TCCR1A = (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(0<<WGM11)|(0<<WGM10);
  TCCR1B = (0<<ICNC1)|(0<<ICES1)|(0<<WGM13)|(0<<WGM12)|(1<<CS12)|(1<<CS11)|(0<<CS10); 
  TIMSK1 = (0<<ICIE1)|(0<<OCIE1B)|(0<<OCIE1A)|(0<<TOIE1);
  TCNT1 = 0;

  sei();
}

const float NST1001::getTemp(){
  
  float Temp = 0;

  digitalWrite(EN_Pin, HIGH);
  
  TCNT1 = 0;                            // Reset counter value
  
  while(TCNT1 == 0);                    // First ~24ms of ADC conversion time.
  
  while(1){                             // Rest of the unknown transmission time.
    Temp = TCNT1;
    _delay_us(9);  
    if(Temp == TCNT1){
      break;
    }
  }

  digitalWrite(EN_Pin, LOW);
  Temp = ((TCNT1*0.0625) - 50.0625);    // Calculate temperature from sampled pulses
  
//Temp compensation, no compensation is needed in the interval 30 ≤ T ≤ 100 
  if(Temp < 30){
    Temp += ((Temp-30)*0.005);          // Compensatio for T < 30
  }
  else if (100 < Temp && Temp < 150){
    Temp += ((100-Temp)*0.012);         // Compensatio for 100 < T < 150
  }
  else if(Temp < -50 || Temp > 150 || Unit > 2){    // Sanity check, returns "huge" value
    Temp = 32500;
  }

//Changing uint, defaults to Celsius (0): 1 = Fahrenheit, 2 = Kelvin
  if(Unit == 1){
    Temp = ((Temp*1.8)+32);
  }
  else if(Unit == 2){
    Temp += 273.15;
  }

  return Temp;
}


/*
float Get_Temp(uint8_t const TEMP_UNIT){

  float Temp = 0;

  if(NST_Mode == 0){                    // Pullup mode
    NST_EN_PORT = (1 << NST_EN_PIN);    // Enable pin high
  }
  else if(NST_Mode == 1){               // Pulldown mode
    NST_EN_PORT = (0 << NST_EN_PIN);    // Enable pin low
  }
  
  TCNT1 = 0;                            // Reset counter value
  _delay_ms(50);                        // Conversion + transmission delay
  Temp = ((TCNT1*0.0625) - 50.0625);    // Calculate temperature from sampled pulses

  if(NST_Mode == 0){                    // Pullup mode
    NST_EN_PORT = (0 << NST_EN_PIN);    // Enable pin low, Turning the sensor off
  }
  else if(NST_Mode == 1){               // Pulldown mode
    NST_EN_PORT = (1 << NST_EN_PIN);    // Enable pin High, Turning the sensor off 
  }
  
//Temp compensation, no compensation is needed in the interval 30 ≤ T ≤ 100 
  if(Temp < 30){
    Temp += ((Temp-30)*0.005);          // Compensatio for T < 30
  }
  else if (100 < Temp && Temp < 150){
    Temp += ((100-Temp)*0.012);         // Compensatio for 100 < T < 150
  }
  else if(Temp < -50 || Temp > 150 || TEMP_UNIT > 2){    // Sanity check, returns "huge" value
    Temp = 32500;
  }

//Changing uint, defaults to Celsius: 1 = Fahrenheit, 2 = Kelvin
  if(TEMP_UNIT == 1){
    Temp = ((Temp*1.8)+32);
  }
  else if(TEMP_UNIT == 2){
    Temp += 273.15;
  }
  return Temp;
}*/

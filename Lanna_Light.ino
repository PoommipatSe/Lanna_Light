 /*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 16 servos, one after the other

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface. For Arduino UNOs, thats SCL -> Analog 5, SDA -> Analog 4

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
// you can also call it with a different address you want
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  34 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  4095// this is the 'maximum' pulse length count (out of 4096)

//#define delayTime 1/120
//#define lightStep 4096/120

#define in1 D5
#define in2 D6
#define analog1 A0
int sensorMain; 
int mainMode = 0;
bool start_flag = true ;
const int MAX_COUNTER = 1000;

void timer0_ISR (void) {
  
  timer0_write(ESP.getCycleCount() + 20000000L); // 80MHz == 1sec
  sensorMain = analogRead(analog1);
  
}

void setup() {
  Serial.begin(115200);

  pwm.begin();
  pwm.setPWMFreq(120);  // Analog servos run at ~60 Hz updates
  
  pwm2.begin();
  pwm2.setPWMFreq(120);
  yield();

  pinMode(in1,INPUT);
  pinMode(in2,INPUT);
  pinMode(analog1, INPUT);  //microphone analog input

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 20000000L); // 80MHz == 1sec
  interrupts();
    

}


// you can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

void setOFF(){
  //turn all 24 leds completely off
  for(int lednum=0;lednum<12;lednum++){
        pwm.setPWM(lednum,0,4096);
        pwm2.setPWM(lednum,0,4096);
    }
  }

void setON(){
  //turn all 24 leds completely on
  for(int lednum=0;lednum<12;lednum++){
        pwm.setPWM(lednum,0,4095);
        pwm2.setPWM(lednum,0,4095);
    }  
  }

void setWaveUp2(){
  //this is a LED pattern
  for(int row=0;row<3; row++){
    for(int col=0;col<8; col++){
      if(col < 4) {
        for(int16_t pulselen=SERVOMIN;pulselen<SERVOMAX;pulselen+=34){
          pwm.setPWM(col+(4*row),0,pulselen);
          delay(2);
        }
        
      }
      else {
        for(int16_t pulselen=SERVOMIN;pulselen<SERVOMAX;pulselen+=34){
          pwm2.setPWM(col+(4*row)-4,0,pulselen);
          delay(2);
        }
        
      } 
      }
    }

  for(int row=0;row<3; row++){
    for(int col=0;col<8; col++){
      if(col < 4) {
        for(int16_t pulselen=SERVOMAX;pulselen>SERVOMIN;pulselen-=34){
          pwm.setPWM(col+(4*row),0,pulselen);
          delay(2);
        }
          pwm.setPWM(col+(4*row),0,4096);
      }
      else {
        for(int16_t pulselen=SERVOMAX;pulselen>SERVOMIN;pulselen-=34){
          pwm2.setPWM(col+(4*row)-4,0,pulselen);
          delay(2);
        }
          pwm2.setPWM(col+(4*row)-4,0,4096);
        
      }
      }
    }  
  }

void setWaveDown2(){
  //this is a LED pattern
  for(int row=2;row>=0; row--){
    for(int col=7;col>=0; col--){
      if(col < 4) {
        for(int16_t pulselen=SERVOMIN;pulselen<SERVOMAX;pulselen+=34){
          pwm.setPWM(col+(4*row),0,pulselen);
          delay(2);
        }
        
      }
      else {
        for(int16_t pulselen=SERVOMIN;pulselen<SERVOMAX;pulselen+=34){
          pwm2.setPWM(col+(4*row)-4,0,pulselen);
          delay(2);
        }
        
      }
      }
    }

  for(int row=0;row<3; row++){
    for(int col=0;col<8; col++){
      if(col < 4) {
        for(int16_t pulselen=SERVOMAX;pulselen>SERVOMIN;pulselen-=34){
          pwm.setPWM(col+(4*row),0,pulselen);
          delay(2);
        }
          pwm.setPWM(col+(4*row),0,4096);
        
      }
      else {
        for(int16_t pulselen=SERVOMAX;pulselen>SERVOMIN;pulselen-=34){
          pwm2.setPWM(col+(4*row)-4,0,pulselen);
          delay(2);
        }
          pwm2.setPWM(col+(4*row)-4,0,4096);
        
      }
      }
    }  
  }

void setWaveUp(){
  //this is a LED pattern

  int16_t lvlstep = 60;
  int delaytime = 2;
  int16_t pulselen;
  for(int lednum = 0;lednum<12;lednum++){
    for(pulselen=SERVOMIN;pulselen<SERVOMAX;pulselen+=lvlstep){
        pwm.setPWM(lednum,0,pulselen);
        delay(delaytime);    
      }
    }
  for(int lednum = 0;lednum<12;lednum++){
    for(pulselen=SERVOMIN;pulselen<SERVOMAX;pulselen+=lvlstep){
        pwm2.setPWM(lednum,0,pulselen);
        delay(delaytime);    
      }
    }
  for(int lednum = 0;lednum<12;lednum++){
    for(pulselen=SERVOMAX;pulselen>SERVOMIN;pulselen-=lvlstep){
        pwm.setPWM(lednum,0,pulselen);
        delay(delaytime);    
      }
      pwm.setPWM(lednum,0,4096);
    }
  for(int lednum = 0;lednum<12;lednum++){
    for(pulselen=SERVOMAX;pulselen>SERVOMIN;pulselen-=lvlstep){
        pwm2.setPWM(lednum,0,pulselen);
        delay(delaytime);    
      }
      pwm2.setPWM(lednum,0,4096);
    }
  }

void setWaveDown(){
  //this is a LED pattern

  int16_t lvlstep = 60;
  int delaytime = 2;
  int16_t pulselen;
  for(int lednum = 11;lednum>=0;lednum--){
    for(pulselen=SERVOMAX;pulselen>SERVOMIN;pulselen-=lvlstep){
        pwm2.setPWM(lednum,0,pulselen);
        delay(delaytime);    
      }
      pwm2.setPWM(lednum,0,4096);
    }
  for(int lednum = 11;lednum>=0;lednum--){
    for(pulselen=SERVOMAX;pulselen>SERVOMIN;pulselen-=lvlstep){
        pwm.setPWM(lednum,0,pulselen);
        delay(delaytime);    
      }
      pwm.setPWM(lednum,0,4096);
    }
  
  }

void ledBreatheUp(int lednum, int delayTime){
  //fade in
  for(int16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen+=34){
    tunePWM(lednum,pulselen);

    if(sensorMain > 500) { 
        rapidBlink();
        mainMode = 1;
        break;
      
    }
    delay(delayTime);
    }

  }

void ledBreatheDown(int lednum, int delayTime){
  
  //fade out
  for(int16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen-=34){
    tunePWM(lednum,pulselen);
    
    if(sensorMain > 500) {       
        rapidBlink();
        mainMode = 1;
        break;  
   
    }
    delay(delayTime);
    }
  tunePWM(lednum,4096);//kill
  }
    
void tunePWM(int lednum, int16_t pulselen){
  if(lednum < 12)
    pwm.setPWM(lednum,0,pulselen);
  else
    pwm2.setPWM(lednum-12,0,pulselen);
  }


void randomFade(){

  setOFF();
  int delayTime = 8;
  int randLed[20];
  int stateLed[20];
  int16_t valsLed[20];
 
  
  for(int i=0; i <20; i++){
    int j = 0;
    int temp = random(0,24);

    while(j<=i){

      if(temp == randLed[j]){
        temp = random(0,24);
        continue;
        }
      else if(j==i){
        randLed[i] = temp;
        }
        j++;  
      }
    stateLed[i]=0;  
  }
  
  for(int i=0; i<20; i++) {
    
    if(mainMode == 1) break;
    if(i<10)
      ledBreatheUp(randLed[i], delayTime);
    else{
      ledBreatheDown(randLed[i-5], delayTime);
      ledBreatheUp(randLed[i], delayTime);
    }
  }
  for(int i=19; i>=10; i--){
    
    if(mainMode == 1) break;
    ledBreatheDown(randLed[i], delayTime);
    }
    //Serial.println("Ending RandomFade");
    
    
  }

void amplitudeShow2(){
    
  //int sensor =  analogRead(analog1);
   //15 14 13 12   3  2 1 0
   //19 18 17 16   7  6 5 4
   //23 22 21 20  11 10 9 8
  if(sensorMain <= 100) {
    setOFF();
    tunePWM(8,4095);    
  }
  else {
    if(sensorMain <200 ){
      //level1
      tunePWM(0,4095);
      tunePWM(8,4095);
          delay(50);
  
      }
    else if(sensorMain < 350){
      //level2
      tunePWM(0,4095);
      tunePWM(8,4095);
      delay(50);
      
      //tunePWM(1,4095);
      tunePWM(5,4095);
      //tunePWM(9,4095);
          delay(50);
  
      
      }
    else if(sensorMain < 400){
      //level3
      tunePWM(0,4095);
      tunePWM(8,4095);
      delay(50);
      
      //tunePWM(1,4095);
      tunePWM(5,4095);
      //tunePWM(9,4095);
      delay(50);
      
      tunePWM(2,4095);
      //tunePWM(5,4095);
      tunePWM(10,4095);
          delay(50);
  
      
      }
    else if(sensorMain < 450){
      //level4
      tunePWM(0,4095);
      tunePWM(8,4095);
      delay(50);
      
      //tunePWM(1,4095);
      tunePWM(5,4095);
      //tunePWM(9,4095);
      delay(50);
      
      tunePWM(2,4095);
      //tunePWM(5,4095);
      tunePWM(10,4095);
      delay(50);
      
      //tunePWM(3,4095);
      tunePWM(7,4095);
      //tunePWM(11,4095);
          delay(50);
  
      
      }
    else if(sensorMain < 550){
      //level5
      tunePWM(0,4095);
      tunePWM(8,4095);
      delay(50);
      
      //tunePWM(1,4095);
      tunePWM(5,4095);
      //tunePWM(9,4095);
      delay(50);
      
      tunePWM(2,4095);
      //tunePWM(5,4095);
      tunePWM(10,4095);
      delay(50);
      
      //tunePWM(3,4095);
      tunePWM(7,4095);
      //tunePWM(11,4095);
      delay(50);
      
      tunePWM(12,4095);
      //tunePWM(7,4095);
      tunePWM(20,4095);
          delay(50);
  
      
      }
    else if(sensorMain < 600){
      //level6
      tunePWM(0,4095);
      tunePWM(8,4095);
      delay(50);
      
      //tunePWM(1,4095);
      tunePWM(5,4095);
      //tunePWM(9,4095);
      delay(50);
      
      tunePWM(2,4095);
      //tunePWM(5,4095);
      tunePWM(10,4095);
      delay(50);
      
      //tunePWM(3,4095);
      tunePWM(7,4095);
      //tunePWM(11,4095);
      delay(50);
      
      tunePWM(12,4095);
      //tunePWM(7,4095);
      tunePWM(20,4095);
      delay(50);
      
      //tunePWM(13,4095);
      tunePWM(17,4095);
      //tunePWM(21,4095);
          delay(50);
  
      
      }
    else if(sensorMain < 650){
      //level7
      tunePWM(0,4095);
      tunePWM(8,4095);
      delay(50);
      
      //tunePWM(1,4095);
      tunePWM(5,4095);
      //tunePWM(9,4095);
      delay(50);
      
      tunePWM(2,4095);
      //tunePWM(5,4095);
      tunePWM(10,4095);
      delay(50);
      
      //tunePWM(3,4095);
      tunePWM(7,4095);
      //tunePWM(11,4095);
      delay(50);
      
      tunePWM(12,4095);
      //tunePWM(7,4095);
      tunePWM(20,4095);
      delay(50);
      
      //tunePWM(13,4095);
      tunePWM(17,4095);
      //tunePWM(21,4095);
      delay(50);
      
      tunePWM(14,4095);
      tunePWM(19,4095);
      tunePWM(22,4095);
          delay(50);
  
      
      }
    else if(sensorMain >= 650){
      //level8
      rapidBlink();
      delay(10);
      rapidBlink();
      delay(10);
      showEnding();         
      }
  }
}

void matrixWaveRtoLtoBright(){
   //this is a LED pattern

   //15 14 13 12   3  2 1 0
   //19 18 17 16   7  6 5 4
   //23 22 21 20  11 10 9 8
  
  for(int i=0; i<8; i++){
    for(int16_t pulselen=SERVOMIN; pulselen<SERVOMAX; pulselen+=34){
      if(i<=3){
        tunePWM(i, pulselen);
        tunePWM(i+4, pulselen);
        tunePWM(i+8, pulselen);
        
        }else{
          tunePWM(i+8, pulselen);
          tunePWM(i+12,pulselen);
          tunePWM(i+16,pulselen);
          }
       delay(8);  
     }
     if(i<=3){
        tunePWM(i, 4095);
        tunePWM(i+4, 4095);
        tunePWM(i+8, 4095);
        
        }else{
          tunePWM(i+8, 4095);
          tunePWM(i+12,4095);
          tunePWM(i+16,4095);
          }
    }
  }

void matrixWaveRtoLtoDark(){
   //this is a LED pattern

   //15 14 13 12   3  2 1 0
   //19 18 17 16   7  6 5 4
   //23 22 21 20  11 10 9 8
  
  for(int i=0; i<8; i++){
    for(int16_t pulselen=SERVOMAX; pulselen>SERVOMIN; pulselen-=34){
      if(i<=3){
        tunePWM(i, pulselen);
        tunePWM(i+4, pulselen);
        tunePWM(i+8, pulselen);
        
        }else{
          tunePWM(i+8, pulselen);
          tunePWM(i+12,pulselen);
          tunePWM(i+16,pulselen);
          }
       delay(8);  
     }
     if(i<=3){
        tunePWM(i, 4096);
        tunePWM(i+4, 4096);
        tunePWM(i+8, 4096);
        
        }else{
          tunePWM(i+8, 4096);
          tunePWM(i+12,4096);
          tunePWM(i+16,4096);
          }
    }
  }
     
void matrixWaveLtoRtoBright(){
   //this is a LED pattern

   //15 14 13 12   3  2 1 0
   //19 18 17 16   7  6 5 4
   //23 22 21 20  11 10 9 8
  
  for(int i=7; i>=0; i--){
    for(int16_t pulselen=SERVOMIN; pulselen<SERVOMAX; pulselen+=34){
    if(i>3){
      tunePWM(i+8,pulselen);
      tunePWM(i+12,pulselen);
      tunePWM(i+16,pulselen);
      }else{
        tunePWM(i,pulselen);
        tunePWM(i+4,pulselen);
        tunePWM(i+8,pulselen);
        }
      delay(8);
    }
    if(i<=3){
        tunePWM(i, 4095);
        tunePWM(i+4, 4095);
        tunePWM(i+8, 4095);
        
        }else{
          tunePWM(i+8, 4095);
          tunePWM(i+12,4095);
          tunePWM(i+16,4095);
          }
  }
}

void matrixWaveLtoRtoDark(){
   //this is a LED pattern

   //15 14 13 12   3  2 1 0
   //19 18 17 16   7  6 5 4
   //23 22 21 20  11 10 9 8
  
  for(int i=7; i>=0; i--){
    for(int16_t pulselen=SERVOMAX; pulselen>SERVOMIN; pulselen-=34){
    if(i>3){
      tunePWM(i+8,pulselen);
      tunePWM(i+12,pulselen);
      tunePWM(i+16,pulselen);
      }else{
        tunePWM(i,pulselen);
        tunePWM(i+4,pulselen);
        tunePWM(i+8,pulselen);
        }
      delay(8);
    }
    if(i<=3){
        tunePWM(i, 4096);
        tunePWM(i+4, 4096);
        tunePWM(i+8, 4096);
        
        }else{
          tunePWM(i+8, 4096);
          tunePWM(i+12,4096);
          tunePWM(i+16,4096);
          }
  }
}


void showEnding(){
   //this is a LED pattern

   //Serial.println("Ending Game");
   matrixWaveRtoLtoBright();
   //Serial.println("x1");
   matrixWaveRtoLtoDark();
   //Serial.println("x2");
   setOFF();
   //Serial.println("3");
   matrixWaveLtoRtoBright();
   //Serial.println("x4");
   matrixWaveLtoRtoDark();
   //Serial.println("x5");
   setWaveUp2();
   //Serial.println("x6 M?");
   setWaveDown2();

   setOFF();
   mainMode = 0;
  }

  
void rapidBlink(){
   //this is a LED pattern

    for(int i=0; i<4; i++){
      setOFF();
      delay(150);
      setON();
      delay(150);
    }
    setOFF();
    delay(1000);
  }

int ampShow_counter = 0;

void loop() {
  //initialize first start
  if (start_flag == true){
     delay(750);
     setOFF();
     matrixWaveRtoLtoBright();
     matrixWaveLtoRtoDark();
     setOFF();
     start_flag = false;
    }
  
  if(mainMode == 0){
    randomFade();
    delay(500);
    }

  else if(mainMode == 1){
    ampShow_counter++;
   
    if(ampShow_counter >= MAX_COUNTER){
      ampShow_counter = 0;
      rapidBlink();
      delay(10);
      rapidBlink();
      delay(10);
      showEnding();
    
    }
    amplitudeShow2();
    }  

    
}

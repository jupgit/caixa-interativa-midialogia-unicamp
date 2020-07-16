unsigned long tempoAnterior;
unsigned long tempoAtual;


int estado = 0;  // 0 = REPOUSO | 1 = APROXIMAÇÃO | 2 = DISPARO
int estadoAnterior = 9;
// ---------------------------------------------
// SERVO MOTOR
    #include <Servo.h>
    Servo motor;

    int anguloMin=90;
    int anguloMax=200;
    int angulo = 90;

// --------------------------------------------
// DISPLAY LCD 16X2 VIA PROTOCOLO I2C
    // INCLUSÃO DE BIBLIOTECAS
    #include <Wire.h>
    #include <LiquidCrystal_I2C.h>
    
    // DEFINIÇÕES
    #define endereco  0x27 // Endereços comuns: 0x27, 0x3F
    #define colunas   16
    #define linhas    2
    
    // INSTANCIANDO OBJETOS
    LiquidCrystal_I2C lcd(endereco, colunas, linhas);

// ---------------------------------------------
// MP3 PLAYER
  #include "Arduino.h"
  #include "SoftwareSerial.h"
  #include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);


// ---------------------------------------------
////////////////////
// SENSOR SETTINGS
///////////////////

// ST_HW_HC_SR04 v2.0 library
#include <ST_HW_HC_SR04.h>

// SENSOR PINS SETUP
ST_HW_HC_SR04 ultrasonicSensor(2, 3); // ST_HW_HC_SR04(TRIG, ECHO)

int sensor_mapped=50;
int sensor_mapped_smooth=50;
int sensor_read = 0 ;


/// MAXIMUM DISTANCE LIMIT (UP TO 4m)
int max_cm = 200;  // SENSOR MAX READING IN CM, TO BE MAPPED AS 100%

// ---------------------------------------------
// SMOOTHER
//  http://www.arduino.cc/en/Tutorial/Smoothing
// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


// ---------------------------------------------
// FITA LED RGB

#include <FastLED.h>

#define LED_PIN     7
//#define CLOCK_PIN 4 
#define NUM_LEDS    18
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  90

CRGB leds[NUM_LEDS];






// ---------------------------------------------

// S E T U P

// ---------------------------------------------

void setup() {

delay(1000); /// Necessário no Arduino Nano para ela dar o reset corretamente ao ligar

// ---------------------------------------------
// INICIALIZA COMUNICAÇÃO SERIAL
    Serial.begin(9600);
    Serial.println("Hello");


// ---------------------------------------------
// MP3 Player
  mySoftwareSerial.begin(9600);

  while(!myDFPlayer.begin(mySoftwareSerial))
  {
      mySoftwareSerial.begin(9600);
      delay(500);
  }

 myDFPlayer.volume(10);  //Set volume value. From 0 to 30   /// NOT WORNKING....

Serial.println(F("DFPlayer Mini online."));


// ---------------------------------------------
// CONFIGURACAO DO SERVO MOTOR
    pinMode(5, OUTPUT); 
    motor.attach(5);

    // MOVIMENTOS INICIAIS
    motor.write(90);  // reset do motor
      //myDFPlayer.play(17);  //Play mp3
      delay(1000);
    motor.write(140);  // reset do motor
     myDFPlayer.play(7);  //Play mp3
     delay(1000);
    motor.write(95);  // reset do motor
      myDFPlayer.play(8);  //Play mp3
      delay(1000);


// ---------------------------------------------
// FITA LED RGB
    LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

    // ANIMACAO INICIAL    
    for(int i=0;i<NUM_LEDS;i=i+1){  
       leds[i].setRGB(255,0,0); // COR
       FastLED.show();
       delay(25);
     }
     for(int i=0;i<NUM_LEDS;i=i+1){  
       leds[i].setRGB(0,255,0); // COR
       FastLED.show();
       delay(25);
     }
     for(int i=0;i<NUM_LEDS;i=i+1){  
       leds[i].setRGB(0,0,255); // COR
       FastLED.show();
       delay(25);
     }



// ---------------------------------------------
// DISPLAY LCD
  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY


  // TEXTO INICIAL + MP3 + PISCA PISCA
  lcd.print("Programacao &");
  lcd.setCursor(0, 1); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 2
  lcd.print("Arte (2020)");

      myDFPlayer.play(9);  //Play mp3

  for (int i=1; i<=5; i++)   {
      lcd.noBacklight(); // DESLIGA A ILUMINAÇÃO DO DISPLAY
      delay(200); // DELAY DE 2 SEGUNDOS
      lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
      delay(200); // DELAY DE 2 SEGUNDOS
    }
  delay(2000); 
  lcd.clear(); // LIMPA O DISPLAY

   FastLED.clear();  
   FastLED.show();


// ---------------------------------------------
// SMOOTHER RESET (SENSOR)
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
///



// ---------------------------------------------
// RESETA OS TEMPOS  
tempoAtual = 0;
tempoAnterior = millis();  
  
}






// ---------------------------------------------

// L O O P

// ---------------------------------------------


void loop()
{

// ---------------------------------------------
/// SMOOTHER

    if (sensor_read >= 5){  // CONSIDER AS READING ONLY ABOVE THRESHOLD  
      // subtract the last reading:
      total = total - readings[readIndex];
    }


// ---------------------------------------------
/// SENSOR READING


    int hitTime = ultrasonicSensor.getHitTime(); // In microseconds
    int cm = hitTime/29;

    sensor_read = abs(cm);

    if (sensor_read >= 5){  // CONSIDER AS READING ONLY ABOVE THRESHOLD  
    readings[readIndex] = sensor_read;  // SMOOTHER
    }

     /// PING LIBRARY
     String message = "";

     if((hitTime == 0) && (ultrasonicSensor.getTimeout() == 5000)) {
        ultrasonicSensor.setTimeout(23200);

        message += "[!] Timeout increased from 5000 to 23200\n";
        message += "[!] If the hit time is still 0 after this message, "
            "please check your wiring!\n";
    }

    /////////////////////////////// END SENSOR READING 


if (sensor_read > 5){  // DO NOT CONSIDER AS READING BELLOW 5

// ---------------------------------------------
/// SMOOTHER BACK AGAIN...
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;   ///// SMOOTHED !!!
/// SMOOTHER END
    
    }

/// MAPPING AND CONSTRAINING
//// define sensor mapped smooth (1-100)
sensor_mapped_smooth = map(average, 0, max_cm, 1, 100);
sensor_mapped_smooth = constrain (sensor_mapped_smooth, 1, 100);

/*
// ---------------------------------------------
lcd.setCursor(0, 1); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 2
lcd.print(sensor_mapped_smooth);
lcd.print("%  ");
*/

// REPOUSO
if (sensor_mapped_smooth > 30 && sensor_mapped_smooth <= 100) {  
  estado = 0;  
  }   

// APROXIMACAO
if (sensor_mapped_smooth > 10 && sensor_mapped_smooth <= 30) {   
  estado = 1;    
  }   

// DISPARO  
if (sensor_mapped_smooth > 0 && sensor_mapped_smooth <= 10) { 
  estado = 2;    
  }   

if (estado != estadoAnterior) {
  tempoAnterior = millis();
  tempoAtual = 0;

  estadoAnterior = estado;

}

// --------------------------------------------- 
// ATUALIZA OS TEMPOS
 
 tempoAtual = millis() - tempoAnterior;
  
  Serial.print(tempoAnterior);
  Serial.print(">");
  Serial.println(tempoAtual);







  
////////////////////////////////////
/////  R E P O U S O 
////////////////////////////////////
  
  
  if (estado == 0 ) {      

    if (horaDoEvento(0))       myDFPlayer.play(6);           //Play mp3
    if (horaDoEvento(250))     lcd.backlight();               // LIGA A ILUMINAÇÃO DO DISPLAY
    if (horaDoEvento(500))     textoLCD("ZzZzZzZzZzZzZzZz","zZzZzZzZzZzZzZzZ");
    if (horaDoEvento(750))     fadeInFitaLED(8,8,64);        // FADE IN DA FITA LED

    if (horaDoEvento(1500))    abreTampa(30, 1000) ;          // abre 30% da tampa em 1000ms
    

    if (horaDoEvento(12000))   abreTampa(40,1000);            // treme a tampa 5 vezes com 150ms delay
    if (horaDoEvento(15000))   fechaTampa(2000);            // treme a tampa 5 vezes com 150ms delay


    if (horaDoEvento(18000))   abreTampa(40,1000);
    if (horaDoEvento(20000))   fechaTampa(2000); 
      
    if (horaDoEvento(23000))   abreTampa(40,1000);
    if (horaDoEvento(25000))   fechaTampa(2000); 
  
    if (horaDoEvento(29000))   abreTampa(60, 500); 
    if (horaDoEvento(31000))   fechaTampa(2000); 


    if (horaDoEvento(45000)) {
      Serial.println("------------------------------------------------------->> FIM DO TIMELINE");
    
    tempoAnterior = millis();
    tempoAtual = 0;
        }
    
  }  //// FIM DO ESTADO DE REPOUSO
    
    
  

  
//////////////////////////////////////////
/////  A  P  R  O  X  I  M  A  C  A  O  
//////////////////////////////////////////
 
  
   if (estado == 1 ) {  
  
 if (horaDoEvento(0))   myDFPlayer.play(1);           //Play mp3
    if (horaDoEvento(250))    abreTampa(40, 300) ;          // abre 100% da tampa em 500ms
    if (horaDoEvento(700))   tremeTampa(3, 200);   

    if (horaDoEvento(1500))     lcd.backlight();               // LIGA A ILUMINAÇÃO DO DISPLAY
    if (horaDoEvento(1750))     textoLCD("Mumble Mumble   ","   Mumble Mumble");
    if (horaDoEvento(2000))     acendeFitaLED(32,32,255);        // FADE IN DA FITA LED
    if (horaDoEvento(3000))     tremeTampa(2, 300);
    if (horaDoEvento(3750))     textoLCD("   Mumble Mumble","Mumble Mumble   ");
    if (horaDoEvento(4000))     tremeTampa(3, 300);
    if (horaDoEvento(5000))     textoLCD("Mumble Mumble   ","   Mumble Mumble");
    if (horaDoEvento(5250))     tremeTampa(4, 300);  
    if (horaDoEvento(6500))     textoLCD("Hmmm mmmm mmmmmm","MnhamMnhamMhham ");
    if (horaDoEvento(7000))     tremeTampa(3, 300); 
    if (horaDoEvento(8000))     textoLCD("   Mumble Mumble","Mumble Mumble   ");    
    if (horaDoEvento(8250))     tremeTampa(4, 300); 
    if (horaDoEvento(9750))     textoLCD("Hmmm mmmm mmmmmm","MnhamMnhamMhham ");
    if (horaDoEvento(9750))     textoLCD("~ ~ ~ ~ ~ ~ ~ ~ ~ ","~ ~ ~ ~ ~ ~ ~ ~ ~ ");
    if (horaDoEvento(10000))    fechaTampa(2000); 
    if (horaDoEvento(11000))     textoLCD(" ~ ~ ~ ~ ~ ~ ~ ~ ~","~ ~ ~ ~ ~ ~ ~ ~ ~ ");
    if (horaDoEvento(12000))     textoLCD("~ ~ ~ ~ ~ ~ ~ ~ ~ "," ~ ~ ~ ~ ~ ~ ~ ~ ~");
    if (horaDoEvento(13000))     textoLCD(" ~ ~ ~ ~ ~ ~ ~ ~ ~","~ ~ ~ ~ ~ ~ ~ ~ ~ ");
    if (horaDoEvento(14000))     textoLCD("~ ~ ~ ~ ~ ~ ~ ~ ~ "," ~ ~ ~ ~ ~ ~ ~ ~ ~");
    if (horaDoEvento(15000))     textoLCD(" ~ ~ ~ ~ ~ ~ ~ ~ ~","~ ~ ~ ~ ~ ~ ~ ~ ~ ");
    if (horaDoEvento(16000))     textoLCD("~ ~ ~ ~ ~ ~ ~ ~ ~ "," ~ ~ ~ ~ ~ ~ ~ ~ ~");
    if (horaDoEvento(17000))     textoLCD(" ~ ~ ~ ~ ~ ~ ~ ~ ~","~ ~ ~ ~ ~ ~ ~ ~ ~ ");
    if (horaDoEvento(18000))     textoLCD("~ ~ ~ ~ ~ ~ ~ ~ ~ "," ~ ~ ~ ~ ~ ~ ~ ~ ~");
    if (horaDoEvento(19000))     textoLCD(" ~ ~ ~ ~ ~ ~ ~ ~ ~","~ ~ ~ ~ ~ ~ ~ ~ ~ ");

    if (horaDoEvento(20000)) {
      Serial.println("------------------------------------------------------->> FIM DO TIMELINE");
    
    tempoAnterior = millis();
    tempoAtual = 0;
        }

     
     
   }
  






  
//////////////////////////////////////////
/////  D I S P A R O 
//////////////////////////////////////////
 
  
   if (estado == 2 ) {  


    if (horaDoEvento(0))        myDFPlayer.play(2);           //Play mp3
    if (horaDoEvento(250))     acendeFitaLED(255,0,0);        // FADE IN DA FITA LED

    if (horaDoEvento(500))    abreTampa(80, 300) ;          // abre 100% da tampa em 500ms
    if (horaDoEvento(700))   tremeTampa(5, 100);   

    if (horaDoEvento(1500))     lcd.backlight();               // LIGA A ILUMINAÇÃO DO DISPLAY
    if (horaDoEvento(1750))     textoLCD("SAI DESGRACA!!!!","*@#@!&*&$#*#%$@!!!");
    if (horaDoEvento(2000))   fechaTampa(300); 
    if (horaDoEvento(3000))   abreTampa(80, 300) ;  
    if (horaDoEvento(4000))   fechaTampa(300);  
    if (horaDoEvento(5000))   abreTampa(70, 300) ; 
    if (horaDoEvento(6000))   fechaTampa(200);
    if (horaDoEvento(7000))   abreTampa(70, 300) ; 
    if (horaDoEvento(8000))   fechaTampa(300);   
    if (horaDoEvento(9000))   abreTampa(80, 300) ;  
    if (horaDoEvento(10000))   fechaTampa(300);  
    if (horaDoEvento(11000))   abreTampa(70, 300) ; 
    if (horaDoEvento(12000))   fechaTampa(200);

    if (horaDoEvento(20000)) {
      Serial.println("------------------------------------------------------->> FIM DO TIMELINE");
    
    tempoAnterior = millis();
    tempoAtual = 0;
        }
  
  
   delay(30);
  
  
}



}
     



////////////////////////////////////
/////  F U N C O E S
////////////////////////////////////




boolean horaDoEvento(long tempo) {
  
  if (tempoAtual > tempo && tempoAtual < tempo+50 ) {
      return true;
    } else {
       return false;
  }
  
}





// ---------------------------------------------

void tocaMP3(String faixa) {
  
  Serial.print(">>>> TOCA MP3 / Faixa: ");
    Serial.println(faixa);
  
}


// ---------------------------------------------
void abreTampa(int porcentagem, int duracao) {
  
  Serial.print(">>>> ABRE a tampa de 0 a ");
    Serial.print(porcentagem);
  Serial.print("% em ");
    Serial.print(duracao); 
  Serial.println("ms");

  angulo = map(porcentagem, 0, 100, anguloMin, anguloMax);
  
  int passo = (angulo - anguloMin) / 10;    // dividi o ângulo de acao em 10 passos

  for (int a=anguloMin; a<=angulo; a=a+passo) {
    motor.write(a);
      Serial.println(a);

    delay(duracao/10);
  }
 
}


// ---------------------------------------------
void fechaTampa(int duracao) {
  
  Serial.print(">>>> FECHA a tampa em ");
  Serial.print(duracao); 
  Serial.println("ms");

  int passo = (angulo - anguloMin) / 10;

  for (int a=angulo; a>=anguloMin; a=a-passo) {

    motor.write(a);
    Serial.println(a);

    delay(duracao/10);
    
  }

  angulo = anguloMin;
  
}


// ---------------------------------------------
void tremeTampa(int vezes, int delayTreme) {
  
  Serial.print(">>>> TREME a tampa por ");
    Serial.print(vezes); 
  Serial.println(" vezes.");

  for (int i=1; i<=vezes; i++) {
    Serial.println(angulo);
    motor.write(angulo-20);
    delay(delayTreme/2);
    motor.write(angulo+20);
    delay(delayTreme/2);    
  }

    motor.write(angulo-20);   ///???
    
}


// ---------------------------------------------
void textoLCD(String linha1, String linha2) {
  
    Serial.print(">>>> Imprime  <");
    Serial.print(linha1); 
  Serial.print("> e <");
    Serial.print(linha2);
  Serial.println("> no LCD."); 


  lcd.setCursor(0, 0); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 1
  lcd.print(linha1);
  lcd.setCursor(0, 1); // POSICIONA O CURSOR NA PRIMEIRA COLUNA DA LINHA 2
  lcd.print(linha2);
  
}



// ---------------------------------------------
void acendeFitaLED(int corR, int corG, int corB) {


  for (int i=0; i<NUM_LEDS; i++) {
       leds[i].setRGB(corR,corG,corB);      // acende cada pixel com a cor recebida
  }

  FastLED.show();

}


// ---------------------------------------------
void acendeParteFitaLED(int corR, int corG, int corB, int pixelInicial, int pixelFinal ) {



  for (int i=pixelInicial; i<pixelFinal; i++) {
       leds[i].setRGB(corR,corG,corB);      // acende cada pixel com a cor recebida
  }

  FastLED.show();

}



// ---------------------------------------------
void apagaFitaLED() {


  for (int i=0; i<NUM_LEDS; i++) {
       leds[i].setRGB(0,0,0);      // acende cada pixel com a cor recebida
  }

  FastLED.show();

}


// ---------------------------------------------
void fadeInFitaLED(int corR, int corG, int corB) {


Serial.println(">>>> Fade in Fita de LED");

  
  for (int i=0; i<NUM_LEDS; i++) {
       leds[i].setRGB(corR,corG,corB);      // acende todos pixels com a cor recebida
  }

  for (int i=0; i<=BRIGHTNESS; i=i+10) {
          FastLED.setBrightness(i);
          FastLED.show();
          delay(50);

  }

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();

}


// ---------------------------------------------
void fadeOutFitaLED() {


Serial.println(">>>> Fade OUT Fita de LED");


  for (int i=BRIGHTNESS; i>=0; i=i-10) {
          FastLED.setBrightness(i);
          FastLED.show();
          delay(50);

  }

  apagaFitaLED();
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();

}



// ---------------------------------------------

/// Usado no setup do MP3 Player
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

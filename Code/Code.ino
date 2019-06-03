#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#define NEOPIXELS_PIN               12
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
#define BLUEFRUIT_HWSERIAL_NAME           Serial1

Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

Adafruit_NeoPixel pixels(60, NEOPIXELSPIN, NEO_GRB + NEO_KHZ800);

/*=========================================================================*/
// Public Variables
int Modus_Auswahl = 0;
int Brems_auswahl = false;

int Sound_Lautstärke = 0;

int SleepModus_Bool = false;
/*=========================================================================*/


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/*=========================================================================*/



/*=========================================================================*/

void setup(void)
{
  attachInterrupt(CircuitPlayground.slideSwitch(), SleepModus, RISING);
  
  attachInterrupt(BlIncoming, Bluetooth, RISING)


  attachInterrupt(dCircuitPlayground.leftButton(), Moduswechsel, RISING);
  //Bei Bremsen auslösen
  attachInterrupt(BREMSE,Brems_Interrupt, RISING);
  
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command <-> Data Mode Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
}

/*=========================================================================*/

void SleepModus(){
    if (SleepModus_Bool){
        deattachInterrupt(BlIncoming, Bluetooth, RISING)
        deattachInterrupt(dCircuitPlayground.leftButton(), Moduswechsel, RISING);
        deattachInterrupt(BREMSE,Brems_Interrupt, RISING);

        Modus_Auswahl = 0;
        set_sleep_Mode(SLEEP_MODE_PWR_DOWN);
        sleep_Cpu();
        }
    else {
        sleep_disable();
        setup() 
    } 
}



void Moduswechsel(){
    pixels.clearPixels();
    Modus_Auswahl++;
    if(Modus_Auswahl == 5){
        Modus_Auswahl = 0;
    }
}

void Brems_Interrupt(){
    //
    Brems_auswahl = true;
    Brems_Zeit = NOW;
    Bluetooth_out("Gebremst");
}

void LICHTER_AN(int [][] LICHTER){
    for (int i = 0; i < LICHTER.length; i++){
        pixels.setPixelColor(LICHTER[i][0], LICHTER[i][1], LICHTER[i][2], LICHTER[i][3])
    }
}

void LICHTER_AUS(){
    pixels.clear();
}




/*=========================================================================*/

//Auswerten von Signalen vom Handy
void Bluetooth_Input(){
    
    char[] Input, inputs[BUFSIZE+1];
    Input = Serial.readBytes(inputs, BUFSIZE);
    
    Modus_Auswahl = 0;
    pixels.clearPixels;

    //TODO: Farbe decoden
    uint32_t FARBE = Input[1];
    if (Input.count == 2){
        switch case Input[0]
          case 1:   pixels.fill(FARBE, 0, 10);
          case 2:   pixels.fill(FARBE, 10, 10);
          case 3:   pixels.fill(Farbe, 20, 10);
    }
    else {
        if Input[0] = -1
            clearPixels;
        else if Input[0] = -2
            Bluetooth_out(Modus_Auswahl);
        else if Input[0] = -3
            Vibrieren();
        else
            Modus_Auswahl = Input[0];
        }
}

/*=========================================================================*/
//Sende Daten ans Handy
void Bluetooth_out(char [] output){
    Serial.print(output);
}


void Vibrieren(){
   //VIBRIERE
}
/*=========================================================================*/

void loop(void)
  {
    switch Modus_Auswahl{
    //Standart
      case 0:

     //Bremse
      case 1:
          LICHTER_AN(Array lichter Seite)
          Ist Brems_Zeit < 5 sec => Brems_auswahl = false

          if Brems_auswahl 
              LICHTER_AN()
          else LICHTER_AUS
      
      // Heiligenschein
      case 2:
          pixels.fill(rgb(224, 253, 255));
      
      //Party
      case 3:
          LICHTER_AN()
          LICHTER_AUS()
      
      //Sound
      case 4:
          pixels.clearPixels;
          Sound_Lautstärke = CircuitPlayground.mic.soundPressureLevel(time) / 20;
          LICHTER_AN(Array 1-5)
  }
}
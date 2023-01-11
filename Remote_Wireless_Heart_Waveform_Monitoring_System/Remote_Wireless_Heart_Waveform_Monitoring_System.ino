#include <WiFi.h>
#include <Wire.h>
#include "heltec.h"
#include "arduinoFFT.h"
#include "ThingSpeak.h"
#include <math.h>
// connect the signal of pulse sensor to the pin 13 of heltec LoRa ESP32

arduinoFFT fft = arduinoFFT();
WiFiClient client_1;


// ************* Variables and WiFi SSID/PWD setup
const char* SSID_1 = "ENTER_SSID"; 
const char* PWD_1 = "SSID_PSWD";
 
unsigned long myChannelNumber2 =   ENTER CHANNEL 2 NUMNER;         //Collects Amplitude
const char * myWriteAPIKey2 = "API_KEY2";

unsigned long myChannelNumber3 = ENTER CHANNEL 3 NUMNER;           //Collects Frequency
const char * myWriteAPIKey3 = "API_KEY2";

unsigned long myChannelNumber4 = ENTER CHANNEL 4 NUMNER;           // Collects Phase
const char * myWriteAPIKey4 = "API_KEY2";

int status = WL_IDLE_STATUS;

const int FFT_size = 256;
const int Sampling_freq = 20; 
const int Heart_Read = 13;
const int N = 6;

float Signal;
double vReal[FFT_size];
double vImag[FFT_size];
double vPhase[FFT_size];
double x = 0;
double v = 0;
double peaks[N];
double phases[N];
double freq[N];
// *******************************************************************
void setupOLED()                        //Turns on OLED when power is turned on to ESP32
{
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, LOW);        // turn D16 low to reset OLED
  delay(50);
  digitalWrite(RST_OLED, HIGH);       // while OLED is running, must set D16 in high
  Heltec.display->init();
  Heltec.display->flipScreenVertically();           
  Heltec.display->setFont(ArialMT_Plain_10);         
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT); 
}

//********************************************************************

void setupWIFI()
{                                           //ESP32 looks and connects to the wifi given in variables
  WiFi.disconnect(true);
  delay(1000);
  
  WiFi.mode(WIFI_STA);  
  WiFi.setAutoConnect(true);      
  WiFi.setAutoReconnect(true);    
  WiFi.begin(SSID_1, PWD_1);
  //ThingSpeak.begin(client_1);
/*
  Heltec.display->clear();
  if(WiFi.status() == WL_CONNECTED)
    Heltec.display->drawString(0, 0, "WiFi Connected"); 
  else
    Heltec.display->drawString(0, 0, "WiFi Connection Failed");
  Heltec.display->display();
*/  
}

//*****************************************************

void setup() 
{                                         //Allows use of serial plotter to see recorded heaertbeat of sensor
  Serial.begin(115200);
  ThingSpeak.begin(client_1);
    
  Heltec.begin(true, false, true);
  pinMode(25, OUTPUT);
  digitalWrite(25,HIGH);
  pinMode(Heart_Read, INPUT);

  Heltec.display->init();
  Heltec.display->flipScreenVertically();           
  //Heltec.display->setFont(ArialMT_Plain_10);         
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);  
}

// ******************************************************
void loop() 
{   
  delay(5000);
  for (int k = 0; k < FFT_size; k++)
     {
      vReal[k] = (double)analogRead(Heart_Read);
      //Serial.println(vReal[k]);      
      vImag[k] = 0;
      delay(50);    //Delay = 1000/Sampling_freq
     }
  delay(100);

  fft.Windowing(vReal, FFT_size, FFT_WIN_TYP_RECTANGLE, FFT_FORWARD);
  fft.Compute(vReal, vImag, FFT_size, FFT_FORWARD);
  for (int k = 0; k < FFT_size; k++)
     vPhase[k] = atan2(vImag[k], vReal[k]);
  fft.ComplexToMagnitude(vReal, vImag, FFT_size);
  fft.MajorPeak(vReal, FFT_size, Sampling_freq,&x,&v);
  int Heart_rate = (int) (x * 60);
  //Serial.println(x);
  //Serial.println(v);
  int n_peak = (int)((double)x * (double)FFT_size/(double)Sampling_freq);
  //Serial.println(n_peak);
  delay(1000);
  
  for (int cnt = 0; cnt < FFT_size/2; cnt++)
     {
      if (vReal[cnt] < 0.1 * v)
        vReal[cnt] = 0;  
     }
  
  int knt,cnt = 0;
  double temp;
  for(knt = 1; knt <= FFT_size/2; knt++)
    {
     temp = vReal[knt];
     if ((temp > vReal[knt+1]) && (temp > vReal[knt-1]) && (cnt < N))
       {
        peaks[cnt] = (double) vReal[knt]/(double)FFT_size;
        phases[cnt] = (double) vPhase[knt]; // * 180.0 / 3.141592;
        freq[cnt] = (double)(knt * Sampling_freq)/(double)FFT_size;
        cnt++;
       }
    }     
/*    
  for (cnt = 0; cnt < N; cnt++)
     {     
      Serial.print(freq[cnt]);
      Serial.print("     ");
      Serial.print(phases[cnt]);        
      Serial.print("     ");
      Serial.println(peaks[cnt]);
     } 
*/  
  delay(100);
  setupOLED();
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);
  String BPM = String("BPM: " +String(Heart_rate));
  Heltec.display->drawString(0,10,BPM);
  Heltec.display->display();
  delay(5000);
  Heltec.display->clear();
  Heltec.display->display(); 
  digitalWrite(RST_OLED, LOW);        // turn D16 low to reset OLED
  delay(50);
  digitalWrite(RST_OLED, HIGH);       // while OLED is running, must set D16 in high
    
  delay(100);
  setupWIFI();
  
  for(cnt = 0; cnt < N; cnt++)
     {
      ThingSpeak.setField(cnt+1, (float)peaks[cnt]);
      ThingSpeak.writeFields(myChannelNumber2, myWriteAPIKey2);   
      delay(15000);  
     }
     
  ThingSpeak.setField(7, Heart_rate);
  ThingSpeak.writeFields(myChannelNumber2, myWriteAPIKey2);   
  Serial.println("The heart-rate was sent to ThingSpeak");
  delay(15000); 
       
  for(cnt = 0; cnt < N; cnt++)
     {
      ThingSpeak.setField(cnt+1, (float)freq[cnt]);
      ThingSpeak.writeFields(myChannelNumber3, myWriteAPIKey3);   
      delay(15000);  
     }    
  for(cnt = 0; cnt < N; cnt++)
     {
      ThingSpeak.setField(cnt+1, (float)phases[cnt]);
      ThingSpeak.writeFields(myChannelNumber4, myWriteAPIKey4);   
      delay(15000);  
     } 

  delay(500);
  delay(300000);
  
  //WiFi.disconnect();
  WiFi.mode( WIFI_MODE_NULL );
  
  //while(1);  
}

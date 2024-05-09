#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <arduinoFFT.h>

//LED to GPIO 17
#define LED_PIN GPIO_NUM_17
#define DEBUG_PIN GPIO_NUM_16

#define SAMPLE_BUFFER_SIZE 512  // has to be power of 2

//Create FFT object
ArduinoFFT<double> FFT = ArduinoFFT<double>();
//Computational domain of FFT:
double vReal[SAMPLE_BUFFER_SIZE];
double vImag[SAMPLE_BUFFER_SIZE];


// raw waveform data:
#ifdef USE_32BIT_SAMPLING
#define MIC_BITS_PER_SAMPLE  I2S_BITS_PER_SAMPLE_32BIT
int32_t wave[SAMPLE_BUFFER_SIZE];
#else
#define MIC_BITS_PER_SAMPLE  I2S_BITS_PER_SAMPLE_16BIT
int16_t wave[SAMPLE_BUFFER_SIZE];
#endif
double maxWave = 0.0;  // to compute scaling factor for wave display

void setup()
{
  pinMode(DEBUG_PIN, OUTPUT);   // for execution time measurement
  pinMode(LED_PIN, OUTPUT);     

  Serial.begin(115200);

  initMicrophone();

  initDisplay();

  delay(1000);
}

void loop()
{
  // Read waveform
  digitalWrite(DEBUG_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);         // LED lights during sampling

  if (getWave() != ESP_OK)
  {
    Serial.println("Error: i2s_read() failed");
    displayError("i2s_read() failed");
    delay(100);
    return;
  }

  digitalWrite(LED_PIN, LOW);
  digitalWrite(DEBUG_PIN, LOW);
 
  performFFT();
 
  displayAll(); 
} 

void performFFT()
  {
    for(int i=0;i<SAMPLE_BUFFER_SIZE;i++){
      vReal[i] = wave[i] * 3.3 / 4096.0; //convert to voltage
      vImag[i] = 0;
    }
    //Calculate FFT
    FFT.windowing(vReal, SAMPLE_BUFFER_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);

    FFT.compute(vReal, vImag, SAMPLE_BUFFER_SIZE, FFT_FORWARD);

    FFT.complexToMagnitude(vReal, vImag, SAMPLE_BUFFER_SIZE); //Calculate absolute value
    
  }







// (Loop execution time: 82ms)

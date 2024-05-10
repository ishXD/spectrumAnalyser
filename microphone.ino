#include <driver/i2s.h>

// connect your INMP441 pins as following, with L/R - to VCC 3.3V:
#define I2S_WS GPIO_NUM_15
#define I2S_SD GPIO_NUM_13
#define I2S_SCK GPIO_NUM_2
#define I2S_PORT I2S_NUM_0

#define SAMPLE_RATE 8000

// for microphone left channel set the L/R pin high:
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT

void initMicrophone() 
{
  Serial.println("Setup I2S Microphone...");

  delay(1000);
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(500);
}

bool getWave() 
{
  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &wave, sizeof(wave), &bytesIn, portMAX_DELAY); 
  if (result == ESP_OK)
  {
    maxWave = 0.0;
    int tmp;
    int samples_read = bytesIn / sizeof(wave[0]);
    if (samples_read > 0) {
      //Serial.println(samples_read);
      for (int i = 0; i < samples_read; ++i) {
        //wave[i] /= 4096;
        wave[i] >>= 16;
        if((tmp=abs(wave[i])) > maxWave)
        {
          maxWave = tmp;
        }
        //Serial.println(wave[i]);
      }
    }
  }
  return result;
}
void i2s_install() {
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = MIC_BITS_PER_SAMPLE, // I2S_BITS_PER_SAMPLE_16BIT or I2S_BITS_PER_SAMPLE_32BIT
    .channel_format = I2S_MIC_CHANNEL,      // L/R to high - left, L/R to ground - right channel
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0, // default 
    //.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}

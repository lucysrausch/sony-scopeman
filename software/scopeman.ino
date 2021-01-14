/*
  Driving deflection coils with an external DAC using ESP32
  
  Copyright (C) 2020 Niklas Fauth
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BluetoothA2DPSink.h"
#include <Arduino.h>
#include "driver/i2s.h"

BluetoothA2DPSink a2dp_sink;

#define CODEC_EN   13
#define SPEAKER_EN 18
#define FLYBACK    19
#define TUNE       34
#define SWITCH     22 // This changes the mode of operation. 1 = BT Audio; 0 = Lorentz Attractor Simulation

#define BUFFER_LEN 400

uint32_t buf[BUFFER_LEN];

int16_t x_buf[BUFFER_LEN];
int16_t y_buf[BUFFER_LEN];
int16_t z_buf[BUFFER_LEN];

int i2s_num = 0;

i2s_config_t i2s_config = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 48000,
    .bits_per_sample = (i2s_bits_per_sample_t)16,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 32,
    .dma_buf_len = 256,
    .use_apll = false
};

static const i2s_pin_config_t pin_config = {
    .bck_io_num = 12,
    .ws_io_num = 27,
    .data_out_num = 14,
    .data_in_num = I2S_PIN_NO_CHANGE
};

uint32_t calc_buf_pos(int16_t x, int16_t y) { // +/- 1024
  uint16_t x_i = x*2;
  uint16_t y_i = y*2;
  uint32_t buf;

  buf = x_i | y_i << 16;
  return buf;
}

float x=0.01;
float y,z;

float a = 10.0;//10.;
float b = 28.0;//28.;
float c = 2.667;//2.66;
float dt = 0.015;
float dx, dy, dz;

void fillBuffer(uint16_t sz, float rot) {
  
  dx = (a * (y -x)) * dt;
  x = x + dx;

  dy = (x*(b-z)-y)  * dt;
  y = y + dy;

  dz = ((x*y)-(c*z))* dt;
  z = z + dz;

  float scale = 1.3;

  x_buf[sz/2] = (x*64) * scale;
  y_buf[sz/2] = (y*64) * scale;
  z_buf[sz/2] = (z*64 - 1200) * scale;
  
  //Serial.println(x_buf[sz/2]);
  
  for(uint16_t i = 0; i < sz/2; i++) {
    x_buf[i] = x_buf[i+1];
    y_buf[i] = y_buf[i+1];
    z_buf[i] = z_buf[i+1];
  }

  for(uint16_t i = sz/2+1; i < sz; i++) {
    x_buf[i] = x_buf[sz-i];
    y_buf[i] = y_buf[sz-i];
    z_buf[i] = z_buf[sz-i];
  }

  float sin_z = sin(rot);
  float cos_z = cos(rot);

  float sin_x = sin(rot * 0.5);
  float cos_x = cos(rot * 0.5);

  for(uint16_t i = 0; i < sz; i++) {

    // rotate Z axis
    int new_x = x_buf[i] * cos_z - y_buf[i] * sin_z;
    int new_y = x_buf[i] * sin_z + y_buf[i] * cos_z;

    // rotate ZY axis
    new_x = z_buf[i] * cos_x - new_y * sin_x;
    new_y = z_buf[i] * sin_x + new_y * cos_x;

    buf[i] = calc_buf_pos(new_x, new_y - 150);
  }
}


void setup() {
  pinMode(CODEC_EN, OUTPUT);
  digitalWrite(CODEC_EN, HIGH);

  pinMode(SPEAKER_EN, OUTPUT);
  digitalWrite(SPEAKER_EN, HIGH);

  ledcSetup(0, 16000, 8);
  ledcAttachPin(FLYBACK, 0);
  ledcWrite(0, 100);

  Serial.begin(115200); 
}
 
   
float rotation = 0;
uint8_t sw_mode = 1;

void loop() {
  if (digitalRead(SWITCH) == 0) {
    if (sw_mode == 0) { // Switch to Video Mode
      i2s_driver_uninstall((i2s_port_t)i2s_num);
      btStop();
      //initialize i2s with configurations above
      i2s_config.dma_buf_count = 2;
      i2s_config.dma_buf_len = BUFFER_LEN/2;
      i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
      i2s_set_pin((i2s_port_t)i2s_num, &pin_config);

      sw_mode = 1;
    }
  } else {
    if (sw_mode == 1) { // Switch to BT Audio Mode
      i2s_config.dma_buf_count = 32;
      i2s_config.dma_buf_len = 256;
      i2s_driver_uninstall((i2s_port_t)i2s_num);
      a2dp_sink.set_pin_config(pin_config); 
      a2dp_sink.set_i2s_config(i2s_config); 
      a2dp_sink.start("Sony Scopeman");
      
      sw_mode = 0;
    }
  }

  if (sw_mode == 1) { // Video Mode
    fillBuffer(BUFFER_LEN, rotation);
    i2s_write_bytes((i2s_port_t)i2s_num, (const char *)&buf, i2s_config.dma_buf_len * 8, 100);
    dt = analogRead(TUNE) / 100000.0f;
  
    rotation = rotation + 0.001;
  
    if (rotation > M_PI_2 * 720.0) {
      rotation = 0;
    }
  } 
}

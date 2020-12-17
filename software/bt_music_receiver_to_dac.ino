/*
  Streaming data from Bluetooth to internal DAC of ESP32
  
  Copyright (C) 2020 Phil Schatzmann
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

BluetoothA2DPSink a2dp_sink;

#define CODEC_EN   13
#define SPEAKER_EN 18
#define FLYBACK    19

void setup() {
  static const i2s_config_t i2s_config = {
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

  a2dp_sink.set_pin_config(pin_config); 
  a2dp_sink.set_i2s_config(i2s_config); 
  a2dp_sink.start("OtterA2DP"); 

  pinMode(CODEC_EN, OUTPUT);
  digitalWrite(CODEC_EN, HIGH);

  pinMode(SPEAKER_EN, OUTPUT);
  digitalWrite(SPEAKER_EN, HIGH);

  ledcSetup(0, 16000, 8);
  ledcAttachPin(FLYBACK, 0);
  ledcWrite(0, 100);
}

void loop() {
}

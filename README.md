# pingpong_stepper

TODO: UART から受け取った値の位置にステッピングモータを動かす

多分リミットスイッチは Arduino 側に刺すことになる


## 配線
Arduino の RX と Raspberry Pi Pico の 1 番ピン ( GPIO0 ) を接続

** A4988 との配線も記述する **

## 環境
UART 速度 115200

Arduino Micro を想定しているけど多分他の Arduino でも動くはず


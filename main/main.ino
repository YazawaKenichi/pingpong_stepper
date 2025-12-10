void setup() {
  Serial.begin(115200);    // PC との USB シリアル
  Serial1.begin(115200);   // ピン0(RX1), 1(TX1)
  while (!Serial) {
    ; // USB シリアルの接続待ち（必要なら）
  }
  Serial.println("Start");
}

void loop() {
  if (Serial1.available() > 0) {
    int c = Serial1.read();
    Serial.print("got: 0x");
    Serial.print(c, HEX);
    Serial.print(" '");
    if (c >= 32 && c <= 126) {
      Serial.write(c);
    } else {
      Serial.print(".");
    }
    Serial.println("'");
  }
}


#include "A4988.h"

// リミットスイッチ付き 1軸ステージ
// UART(Serial1) から「目標位置 [m]」を受け取り、そこに移動する

// ----- A4988 の DIR/STEP ピン -----
const int DIR_PIN  = 8;
const int STEP_PIN = 9;
// 必要なら enable ピンも
const int ENABLE_PIN = 7;   // 使っていなければ消してOK

// ----- リミットスイッチ -----
// プルアップ入力で、押されたとき LOW になる前提
const int LIMIT_MIN_PIN = 2;  // 0[m] 側
const int LIMIT_MAX_PIN = 3;  // 2[m] 側

// ----- モータとメカの条件 -----
// モータ: 1回転 200ステップ
const int MOTOR_STEPS_PER_REV = 200;
// ドライバのマイクロステップ設定 (基板のジャンパに合わせる)
const int MICROSTEPS           = 8;
const int STEPS_PER_REV        = MOTOR_STEPS_PER_REV * MICROSTEPS;

// 軸の直径: 21 cm → 0.21 m
const float SHAFT_DIAMETER_M      = 0.21f;
const float SHAFT_CIRCUMFERENCE_M = PI * SHAFT_DIAMETER_M;

// リミットスイッチ間の距離 [m]
const float AXIS_LENGTH_M = 2.0f;

// ステップパルスの長さ (速度調整用)
const int STEP_PULSE_US = 800; // 遅ければ大きく, 速くしたければ小さく

// ----- 内部状態 -----
float current_pos_m = 0.0f; // 起動時に 0[m] 付近に置いておく前提
String rxLine;

// STEP パルス 1発
void stepOneRaw()
{
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(STEP_PULSE_US);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(STEP_PULSE_US);
}

// dirPositive=true で +方向 (2m 側), false で -方向 (0m 側)
void moveSteps(bool dirPositive, long steps, bool useLimit)
{
#if 0
    digitalWrite(DIR_PIN, dirPositive ? HIGH : LOW);
#else
    current_angle = current_pos_m * (360 / 0.21 * 3.14159265);
    stepper.rotate(current_angle)

    for (long i = 0; i < steps; ++i)
    {
        if (useLimit)
        {
            // +方向のときは +側リミット、-方向のときは -側リミットだけを見る
            if (dirPositive && digitalRead(LIMIT_MAX_PIN) == LOW)
            {
                // +側リミットに到達
                current_pos_m = AXIS_LENGTH_M;
                break;
            }
            if (!dirPositive && digitalRead(LIMIT_MIN_PIN) == LOW)
            {
                // -側リミットに到達
                current_pos_m = 0.0f;
                break;
            }
        }

        stepOneRaw();
    }
#endif
}

// 目標位置[m]へ移動
void moveToPosition(float target_m)
{
    // 範囲制限
    if (target_m < 0.0f)           target_m = 0.0f;
    if (target_m > AXIS_LENGTH_M)  target_m = AXIS_LENGTH_M;

    float delta = target_m - current_pos_m;
    if (fabs(delta) < 1e-6) return; // ほぼ同じ位置なら何もしない

    const float stepsPerMeter = (float)STEPS_PER_REV / SHAFT_CIRCUMFERENCE_M;
    long steps = lround(fabs(delta) * stepsPerMeter);

    bool dirPositive = (delta > 0.0f);

    moveSteps(dirPositive, steps, true);

    // 実際にリミットに当たっていれば、その座標にスナップ
    if (digitalRead(LIMIT_MIN_PIN) == LOW)
        current_pos_m = 0.0f;
    else if (digitalRead(LIMIT_MAX_PIN) == LOW)
        current_pos_m = AXIS_LENGTH_M;
    else
        current_pos_m = target_m;

    Serial.print("current_pos_m = ");
    Serial.println(current_pos_m, 4);
}

const int MOTOR_STEPS = 200;
const int DIR = 8;
const int STEP = 9;
float rpm = 10;
int microsteps = 30;
A4988 stepper(MOTOR_STEPS, DIR, STEP);

void setup()
{
    Serial.begin(115200);   // デバッグ出力
    Serial1.begin(115200);  // UART から位置[m]を受け取る

    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, LOW); // A4988 有効 (基板によっては HIGH が有効のこともある)

    pinMode(LIMIT_MIN_PIN, INPUT_PULLUP);
    pinMode(LIMIT_MAX_PIN, INPUT_PULLUP);

    digitalWrite(DIR_PIN, LOW);
    digitalWrite(STEP_PIN, LOW);

    Serial.println("ready. send target position [m] to Serial1, e.g. \"0.5\\n\"");
    Serial.println("※ 起動時は 0[m] 側のリミット近くに置いておく想定");
    // 必要ならここで「原点出し」(リミットに当たるまでゆっくり動かす)を足してもOK
    stepper.begin(rpm, microsteps);
}

void loop()
{
    // UART(Serial1) から 1 行読み込んで float に変換
    while (Serial1.available())
    {
        char c = Serial1.read();

        if (c == '\r') continue; // 無視

        if (c == '\n')
        {
            if (rxLine.length() > 0)
            {
                float target = rxLine.toFloat();  // 単位: m
                Serial.print("target(m) = ");
                Serial.println(target, 4);

                moveToPosition(target);
                rxLine = "";
            }
        }
        else
        {
            // 数字・小数点・符号を素直に蓄積 (簡単のため何でも受ける)
            rxLine += c;
        }
    }

    // 安全のため、常にリミット状態を見て座標を補正
    if (digitalRead(LIMIT_MIN_PIN) == LOW)
        current_pos_m = 0.0f;
    else if (digitalRead(LIMIT_MAX_PIN) == LOW)
        current_pos_m = AXIS_LENGTH_M;
}

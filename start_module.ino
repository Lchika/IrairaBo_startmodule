/**@file start_module.ino */
/* スタートモジュール  */

#include "start_module.h"
#include "define_comm.h"
#include "dsub_master_communicator.hpp"
#include "debug.h"
#include <Arduino.h>
#include <Servo.h>
#include <DFRobotDFPlayerMini.h>
#include <ArduinoSTL.h>
#include <map>

/* クラス・変数宣言 */
unsigned char slave_num;    // スレーブの数(ゴールモジュールを含む)
DsubMasterCommunicator *dsubMasterCommunicator = NULL;    //  Dsub関係管理用
SerialCommunicator *serialCommunicator = NULL;            //  シリアル通信管理用
char dprint_buff[128];                                    //  デバッグ出力用バッファ
Servo myservo;                                            //  多回転サーボ動作用
SoftwareSerial softwareSerial(PIN_SOFT_RX1, PIN_SOFT_TX1);    // RX, TX
DFRobotDFPlayerMini dFPlayer;

void setup(void) {
  BeginDebugPrint();
  DebugPrint("HardwareSerial ready");

  //  シリアル通信管理クラスのインスタンスを生成
  serialCommunicator = new SerialCommunicator(PIN_RX, PIN_TX, PC_SSERIAL_BAUDRATE);
  softwareSerial.begin(9600);
  DebugPrint("SoftwareSerial ready");

  pinMode(PIN_GOAL_START, INPUT);   //  ゴール判定ピン(スタート)を入力に設定
  pinMode(PIN_HIT_START, INPUT);    //  当たった判定ピン(スタートモジュール)を入力に設定
  pinMode(PIN_LED, OUTPUT);         //  LEDピンを出力に設定
  digitalWrite(PIN_LED, LOW);       //  LED OFF
  pinMode(PIN_VMOTOR, OUTPUT);
  pinMode(PIN_LED_STICK, OUTPUT);
  
  /* ディップスイッチを入力として設定 */
  pinMode(PIN_DIP_0, INPUT);
  pinMode(PIN_DIP_1, INPUT);
  pinMode(PIN_DIP_2, INPUT);
  pinMode(PIN_DIP_3, INPUT);
  DebugPrint("pinMode set end");

  //  接続スレーブ数を確認
  slave_num = ReadDipSwitch();
  sprintf(dprint_buff, "slave num = %d", slave_num);
  DebugPrint(dprint_buff);

  std::map<String, void(*)()> call_backs;
  call_backs["onHit"] = func_on_hit;
  //  Dsub関係管理クラスのインスタンスを生成
  dsubMasterCommunicator = new DsubMasterCommunicator(slave_num,
                            serialCommunicator, INTERVAL_DSUB_COMM_MS, call_backs);
  DebugPrint("created dsubMasterCommunicator");
  
  //  スレーブモジュールとの疎通確認
  //  全スレーブとの通信が成功するまでこのループは抜けられない
  while(!dsubMasterCommunicator->confirm_connect()){
    //  I2C接続に異常があるスレーブモジュールが存在する場合
    //  スタート地点のLEDを点滅させる
    blink_led(PIN_LED, LED_ERROR_INTERVAL_MS, LED_ERROR_BLINK_COUNT);
    delay(1000);
  }

  //  DFPlayer起動準備
  softwareSerial.listen();
  if (!dFPlayer.begin(softwareSerial)) {  //Use softwareSerial to communicate with mp3.
    DebugPrint("Unable to begin:");
    DebugPrint("1.Please recheck the connection!");
    DebugPrint("2.Please insert the SD card!");
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  DebugPrint("DFPlayer Mini online.");
  //  DFPlayerの音量設定
  softwareSerial.listen();
  dFPlayer.volume(5);
  dFPlayer.disableLoopAll();

  myservo.attach(PIN_SERVO);  
  myservo.write(90);  //サーボ停止
  DebugPrint("setup end");
  return;
}

/**
 * @brief ループ処理
 * @param None
 * @return None
 * 
 * @details
 * 状態遷移
 * - 開始待ち状態→スタートモジュール通過中状態→スレーブモジュール通過中状態
 * 各状態に応じた処理を行う
 */
void loop(void) {
  /* ループ処理内で使用する変数 */
  static int state = STATE_WAIT_START;  //  初期状態は開始待ち状態
  static unsigned long last_hit_time = millis();

  switch(state) {
    /* PCへSTARTメッセージを送り、返答を受け取ったらスタートモジュール通過中状態に遷移 */
    case STATE_WAIT_START:
      //DebugPrint("STATE: START");

      if(digitalRead(PIN_START_SW) == LOW){
        serialCommunicator->send(SERIAL_START);
        state = STATE_IN_START_M;
        DebugPrint("STATE: RUNNING");
      }
      break;

    /* スタートモジュール通過判定を検知したらスレーブモジュール通過中状態へ遷移 */
    case STATE_IN_START_M:
      //  スタートモジュールの当たり判定処理
      if(digitalRead(PIN_HIT_START) == LOW) {  //  HIT判定があるかどうか
        //  現在時刻取得
        unsigned long now_time = millis();
        //  最終接触検知時刻と現在時刻との差が閾値以上であればPCに接触通知
        if((now_time - last_hit_time) > INTERVAL_DETECT_HIT_MS){
          //  最終接触時刻を更新
          last_hit_time = now_time;
          //  PCにコース接触を通知
          serialCommunicator->send(SERIAL_HIT);
          //  振動モータを作動
          func_on_hit();
          //  LEDを点滅させる
          blink_led(PIN_LED, LED_HIT_INTERVAL_MS, LED_HIT_BLINK_COUNT);
        }
      }

      myservo.write(80);  //一方方向へ回転

      //  スタートモジュールゴール判定処理
      if(digitalRead(PIN_GOAL_START) == LOW) { //  通過したかどうか
        //  PCにモジュール通過を通知
        serialCommunicator->send(SERIAL_THROUGH);
        //  D-sub通信を開始する(スレーブアドレス0x01指定)
        dsubMasterCommunicator->active(0x01);
        //  スレーブモジュール通過中状態に遷移
        state = STATE_IN_SLAVE_M;
        myservo.write(90);  //サーボ停止
      }
      break;
    
    /* スレーブモジュールとの通信を定期的に行う */
    case STATE_IN_SLAVE_M:
      //  Dsub関係イベント処理(これは定期的に呼ぶ必要がある)
      dsubMasterCommunicator->handle_dsub_event();

      //  Dsub通信が終了していた場合
      if(!dsubMasterCommunicator->get_active()){
        //  すべてのモジュールを通過し終えているのでゲーム終了
        //  スタート待機状態に戻る
        state = STATE_WAIT_START;
      }
      break;

    default:
      /* ここには来ない */
      DebugPrint("!!STATE BUG!!");
      break;
  }
  return;
}

/* ディップスイッチの値を10進数で読み取る */
unsigned char ReadDipSwitch(void) {
  unsigned char value = 0;

  if(digitalRead(PIN_DIP_0) == HIGH) {
    value += 1;
  }
  if(digitalRead(PIN_DIP_1) == HIGH) {
    value += 2;
  }
  if(digitalRead(PIN_DIP_2) == HIGH) {
    value += 4;
  }
  if(digitalRead(PIN_DIP_3) == HIGH) {
    value += 8;
  }

  return value;
}

/**
 * @brief LED点滅処理
 * @param[in] pin         LED出力ピン
 * @param[in] blink_time  点滅間隔[ms]
 * @param[in] blink_count 点滅回数
 * @return None
 */
void blink_led(int pin, int blink_time, int blink_count){
  for(int i = 0; i < blink_count; i++){
    digitalWrite(pin, HIGH);
    delay(blink_time);
    digitalWrite(pin, LOW);
    delay(blink_time);
  }
  return;
}

/**
 * @brief 振動モータ作動処理
 * @param[in] drive_time  作動時間[ms]
 * @return None
 */
static void drive_vmotor(int drive_time){
  analogWrite(PIN_VMOTOR, POWER_VMOTER_MOVE);
  delay(drive_time);
  analogWrite(PIN_VMOTOR, 0);
}

/**
 * @brief コース接触時処理
 * @return None
 */
static void func_on_hit(void){
  //  振動モータを作動
  drive_vmotor(TIME_VMOTOR_MOVE_MS);
  //  LEDを点滅
  blink_led(PIN_LED_STICK, LED_HIT_S_INTERVAL_MS, LED_HIT_S_BLINK_COUNT);
  //  HIT音再生
  softwareSerial.listen();
  dFPlayer.play(1);         //Play the first mp3
}
/**@file start_module.ino */
/* スタートモジュール  */

#include "start_module.h"
#include "define_comm.h"
#include "dsub_master_communicator.hpp"
#include "debug.h"
#include <Arduino.h>
#include <Servo.h>

/* クラス・変数宣言 */
unsigned char slave_num;    // スレーブの数(ゴールモジュールを含む)
DsubMasterCommunicator *dsubMasterCommunicator = NULL;    //  Dsub関係管理用
SerialCommunicator *serialCommunicator = NULL;            //  シリアル通信管理用
char dprint_buff[128];                                    //  デバッグ出力用バッファ
Servo myservo;                                            //  多回転サーボ動作用

void setup(void) {
  BeginDebugPrint();
  DebugPrint("HardwareSerial ready");

  //  シリアル通信管理クラスのインスタンスを生成
  serialCommunicator = new SerialCommunicator(PIN_RX, PIN_TX, PC_SSERIAL_BAUDRATE);
  DebugPrint("SoftwareSerial ready");

  pinMode(PIN_GOAL_START, INPUT);   //  ゴール判定ピン(スタート)を入力に設定
  pinMode(PIN_HIT_START, INPUT);    //  当たった判定ピン(スタートモジュール)を入力に設定
  
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

  //  Dsub関係管理クラスのインスタンスを生成
  dsubMasterCommunicator = new DsubMasterCommunicator(slave_num,
                            serialCommunicator, INTERVAL_DSUB_COMM_MS);
  
  //  スレーブモジュールとの疎通確認
  if(!dsubMasterCommunicator->confirm_connect()){
    //  I2C接続に異常があるスレーブモジュールが存在する場合
    //  今は特に処理内容を決めていないが、できればLED等を使って通知できるようにしたい
    //  LEDの点滅回数で番号を通知?
  }

  DebugPrint("created dsubMasterCommunicator");
  myservo.attach(PIN_SERVO);  
  myservo.write(90);  //サーボ停止
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
        }
      }

      myservo.write(60);  //一方方向へ回転

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

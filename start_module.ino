/* start_module.ino */
/* スタートモジュール  */

#include "start_module.h"
#include "define_comm.h"
#include "dsub_master_communicator.hpp"
#include "debug.h"
#include <Arduino.h>

/* クラス・変数宣言 */
unsigned char slave_num;    // スレーブの数(ゴールモジュールを含む)
DsubMasterCommunicator *dsubMasterCommunicator = NULL;    //  Dsub関係管理用
SerialCommunicator *serialCommunicator = NULL;            //  シリアル通信管理用

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

  //  Dsub関係管理クラスのインスタンスを生成
  dsubMasterCommunicator = new DsubMasterCommunicator(slave_num,
                            serialCommunicator, INTERVAL_DSUB_COMM_MS);
  DebugPrint("created dsubMasterCommunicator");
}

/*
/* 状態遷移：開始待ち状態→ゲーム進行状態→終了状態
/* 開始待ち状態→ゲーム進行状態：PCとのシリアル通信が成功したとき遷移
/* ゲーム進行状態→終了状態：GOALに到達した(orX回当たったorタイムアップ等)とき遷移
 */
void loop(void) {
  /* ループ処理内で使用する変数 */
  static char state = STATE_START;  //  初期状態は開始待ち状態

  switch(state) {
    /* PCへSTARTメッセージを送り、返答を受け取ったらゲーム進行状態に遷移 */
    case STATE_START:
      DebugPrint("STATE: START");

      serialCommunicator->send(SERIAL_START);
      state = STATE_RUNNING;
      DebugPrint("STATE: RUNNING");
      break;

    /* 終了条件を満たしたら終了状態へ遷移 */
    case STATE_RUNNING:
      //  スタートモジュールの当たり判定処理
      if(digitalRead(PIN_HIT_START) == HIGH) {  //  HIT判定があるかどうか
        serialCommunicator->send(SERIAL_HIT);
      }

      //  スタートモジュールゴール判定処理
      if(digitalRead(PIN_GOAL_START) == HIGH) { //  通過したかどうか
        serialCommunicator->send(SERIAL_THROUGH);
        dsubMasterCommunicator->active(0x01);
      }

      //  Dsub関係イベント処理(これは定期的に呼ぶ必要がある)
      dsubMasterCommunicator->handle_dsub_event();

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

/* start_module.ino */
/* スタートモジュール  */

#include <Wire.h>
#include <SoftwareSerial.h>
#include "start_define.h"

/* プロトタイプ宣言 */
void Send2pc(char);
void SetSLV(void);

/* クラス・変数宣言 */
SoftwareSerial swSerial(PIN_RX, PIN_TX);
unsigned char slvNum = 2;          //スレーブの数(ゴールモジュールを除く) TODO:動的に変更できないか？
unsigned char comSlv = SLAVE_1;    //通過したかどうかを確認すべきスレーブ
char state = STATE_START; //初期状態は開始待ち状態
char through_startmodule_f = 0; //スタートモジュールを通過したフラグ

void setup(void) {
  Serial.begin(57600); //ハードウェアシリアル(デバッグ用)開始
  Serial.println("HardwareSerial ready");

  swSerial.begin(4800); //ソフトウェアシリアル(PCとの通信用)開始
  Serial.println("SoftwareSerial ready");

  Wire.begin(); //マスタとしてI2C開始
  Serial.println("I2C ready");

  pinMode(PIN_GOAL, INPUT); //ゴール判定ピンを入力に設定
  pinMode(PIN_HIT, INPUT); //当たった判定ピンを入力に設定
}

/*
/* 状態遷移：開始待ち状態→ゲーム進行状態→終了状態
/* 開始待ち状態→ゲーム進行状態：PCとのシリアル通信が成功したとき遷移
/* ゲーム進行状態→終了状態：GOALに到達した(orX回当たったorタイムアップ等)とき遷移
 */
void loop(void) {
  /* ループ処理内で使用する変数 */
  
  switch(state) {
    /* PCへSTARTメッセージを送り、返答を受け取ったらゲーム進行状態に遷移 */
    case STATE_START:
      Serial.println("STATE: START");

      Send2pc(START);

      state = STATE_RUNNING;
      Serial.println("STATE: RUNNING");

      break;

    /* 終了条件を満たしたら終了状態へ遷移 */
    case STATE_RUNNING:
      if(digitalRead(PIN_HIT) == HIGH) { //HIT判定があるかどうか
        Send2pc(HIT); //PCへHITを送信
        Serial.print("BEGIN TRANSMISSION TO SLAVE");
        Serial.println(comSlv);
        Wire.beginTransmission(comSlv); //スレーブとの通信を開始
        Wire.write(MASTER_DETECT_HIT); //HITを検知したことを通知
        Wire.endTransmission(); //通信終了
        delay(100); //念の為
        Serial.print("END TRANSMISSION TO SLAVE");
        Serial.println(comSlv);
      }

      if(digitalRead(PIN_GOAL) == HIGH) { //通過したかどうか
        if(through_startmodule_f == 0) { //スタートモジュールを通過したとき
          Send2pc(THROUGH); //PCへTHROUGHを送信
          through_startmodule_f = 1;
        } else {
          if(comSlv != SLAVE_GOAL) { //ゴール以外のモジュール
            Send2pc(THROUGH); //PCへTHROUGHを送信

            Serial.print("BEGIN TRANSMISSION TO SLAVE");
            Serial.println(comSlv);
            Wire.beginTransmission(comSlv); //スレーブとの通信を開始
            Wire.write(MASTER_DETECT_GOAL); //通過を確認すべきモジュールが次のモジュールへ遷移したことを通知
            Wire.endTransmission(); //通信終了
            delay(100); //念の為
            Serial.print("END TRANSMISSION TO SLAVE");
            Serial.println(comSlv);

            SetSlv(); //通過したか確認すべきスモジュールを次のモジュールに変更
          } else { //ゴールモジュール
            state = STATE_FINISH;
            Serial.println("STATE: FINISH");
          }
        }
      }

      break;

    /* リセットされるまで待機 */
    case STATE_FINISH:
      Send2pc(FINISH);
      break;

    default:
      /* ここには来ない */
      Serial.println("state BUG");
      break;
  }
}

/* PCにシリアル通信でメッセージを送信し、返答があるまで待機 */
void Send2pc(char state) {
  switch(state) {
    case START:
      swSerial.write(SERIAL_START);
      Serial.println("SEND2PC: START");
      while(swSerial.read() != SERIAL_START) {
        //PCからSTARTの返答があるまで待機
      }
      Serial.println("RECEIVE FROM PC: START");
      break;

    case HIT:
      swSerial.write(SERIAL_HIT);
      Serial.println("SEND2PC: HIT");
      while(swSerial.read() != SERIAL_HIT) {
        //PCからHITの返答があるまで待機
      }
      Serial.println("RECEIVE FROM PC: HIT");
      break;

    case THROUGH:
      swSerial.write(SERIAL_THROUGH);
      Serial.println("SEND2PC: THROUGH");
      while(swSerial.read() != SERIAL_THROUGH) {
        //PCからTHROUGHの返答があるまで待機
      }
      Serial.println("RECEIVE FROM PC: THROUGH");
      break;

    case POINT:
      swSerial.write(SERIAL_POINT);
      Serial.println("SEND2PC: POINT");
      while(swSerial.read() != SERIAL_POINT) {
        //PCからPOINTの返答があるまで待機
      }
      Serial.println("RECEIVE FROM PC: POINT");
      break;

    case FINISH:
      swSerial.write(SERIAL_FINISH);
      Serial.println("SEND2PC: FINISH");
      while(swSerial.read() != SERIAL_FINISH) {
        //PCからFINISH返答があるまで待機
      }
      Serial.println("RECEIVE FROM PC: FINISH");
      break;

    default:
      /* ここには来ない */
      Serial.println("send BUG");
      break;
  }
}

/* 通過したことを確認すべきモジュールを設定する */
void SetSlv(void) {
  if(comSlv < slvNum) { //前回確認したモジュールがゴールを除いた最後のモジュールでないとき
    comSlv++; //次のモジュールをセット
  } else if(comSlv == slvNum) { //前回確認したモジュールがゴールを除いた最後のモジュールのとき
    comSlv = SLAVE_GOAL;  //ゴールモジュールをセット
  } else {
    /* ここには来ない */
    Serial.println("!BUG!");
  }
  Serial.print("BEGIN TRANSMISSION TO SLAVE");
  Serial.println(comSlv);
  Wire.beginTransmission(comSlv); //スレーブとの通信を開始
  Wire.write(MASTER_BEGIN_TRANS); //通過を確認すべきモジュールが自分であることを通知
  Wire.endTransmission(); //通信終了
  delay(100); //念の為
  Serial.print("END TRANSMISSION TO SLAVE");
  Serial.println(comSlv);
}

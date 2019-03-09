/* start_module.ino */
/* スタートモジュール  */

#include <Wire.h>
#include <SoftwareSerial.h>
#include "start_define.h"

/* プロトタイプ宣言 */
void Send2pc(char);
void SetSLV(void);
static void start_i2c_with_slave(unsigned char slave_address);

/* クラス・変数宣言 */
SoftwareSerial swSerial(PIN_RX, PIN_TX);
unsigned char slvNum;          //スレーブの数(ゴールモジュールを除く)
unsigned char comSlv;    //通過したかどうかを確認すべきスレーブ

void setup(void) {
  Serial.begin(DEBUG_HSERIAL_BAUDRATE); //ハードウェアシリアル(デバッグ用)開始
  Serial.println("HardwareSerial ready");

  swSerial.begin(DEBUG_HSERIAL_BAUDRATE); //ソフトウェアシリアル(PCとの通信用)開始
  Serial.println("SoftwareSerial ready");

  Wire.begin(); //マスタとしてI2C開始
  Serial.println("I2C ready");

  pinMode(PIN_GOAL_SLAVE, INPUT);   //ゴール判定ピン(スレーブ)を入力に設定
  pinMode(PIN_GOAL_START, INPUT);   //ゴール判定ピン(スタート)を入力に設定
  pinMode(PIN_HIT_SLAVE, INPUT); //当たった判定ピン(スレーブ)を入力に設定
  pinMode(PIN_HIT_START, INPUT);  //当たった判定ピン(スタートモジュール)を入力に設定
  
  /* ディップスイッチを入力として設定 */
  pinMode(PIN_DIP_0, INPUT);
  pinMode(PIN_DIP_1, INPUT);
  pinMode(PIN_DIP_2, INPUT);
  pinMode(PIN_DIP_3, INPUT);

  slvNum = ReadDipSwitch();
  SetSlvAddressAtStart();
}

/*
/* 状態遷移：開始待ち状態→ゲーム進行状態→終了状態
/* 開始待ち状態→ゲーム進行状態：PCとのシリアル通信が成功したとき遷移
/* ゲーム進行状態→終了状態：GOALに到達した(orX回当たったorタイムアップ等)とき遷移
 */
void loop(void) {
  /* ループ処理内で使用する変数 */
  static char state = STATE_START; //初期状態は開始待ち状態
  //static char through_startmodule_f = 0; //スタートモジュールを通過したフラグ
  static unsigned char pin_goal = PIN_GOAL_START;

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
      //スタートモジュールの当たり判定処理
      if(digitalRead(PIN_HIT_START) == HIGH) {  //HIT判定があるかどうか
        Send2pc(HIT);  //PCへHITを送信
      }
      
      //スレーブの当たり判定処理
      if(digitalRead(PIN_HIT_SLAVE) == HIGH) { //HIT判定があるかどうか
        Send2pc(HIT); //PCへHITを送信
        Serial.print("<HIT>BEGIN TRANSMISSION TO SLAVE");
        Serial.println(comSlv);
        Wire.beginTransmission(comSlv); //スレーブとの通信を開始
        Wire.write(MASTER_DETECT_HIT); //HITを検知したことを通知
        Wire.endTransmission(); //通信終了
        delay(100); //念の為
        Serial.print("END TRANSMISSION TO SLAVE");
        Serial.println(comSlv);
      }

      if(digitalRead(pin_goal) == HIGH) { //通過したかどうか
        if(pin_goal == PIN_GOAL_START) { //スタートモジュールを通過したとき
          Send2pc(THROUGH); //PCへTHROUGHを送信
          pin_goal = PIN_GOAL_SLAVE;  //確認対象ピンをスレーブからのゴール通知ピンに設定
          //through_startmodule_f = 1;
          start_i2c_with_slave(comSlv);
        } else {
          if(comSlv != SLAVE_GOAL) { //ゴール以外のモジュール
            Send2pc(THROUGH); //PCへTHROUGHを送信

            Serial.print("<THROUGH>BEGIN TRANSMISSION TO SLAVE");
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
      Serial.println("!!STATE BUG!!");
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

unsigned char SetSlvAddressAtStart(void) {
  if(slvNum == 0) {
    comSlv = SLAVE_GOAL;
  } else {
    comSlv = SLAVE_1;
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
      Serial.println("!!SEND2PC BUG!!");
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
    Serial.println("!!SETSLV BUG!!");
  }
  Serial.print("<SETSLV>BEGIN TRANSMISSION TO SLAVE");
  Serial.println(comSlv);
  start_i2c_with_slave(comSlv);
  delay(100); //念の為
  Serial.print("END TRANSMISSION TO SLAVE");
  Serial.println(comSlv);
}

/* 引数で受け取ったスレーブアドレスのデバイスとI2C通信を始める */
static void start_i2c_with_slave(unsigned char slave_address){
  Serial.println("start_i2c_with_slave(" + String(slave_address) + ")");
  Wire.beginTransmission(slave_address); //スレーブとの通信を開始
  Wire.write(MASTER_BEGIN_TRANS); //通過を確認すべきモジュールが自分であることを通知
  Wire.endTransmission(); //通信終了
  return;
}
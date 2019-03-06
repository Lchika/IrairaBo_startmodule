/* start_define.h            */
/* スタートモジュール関係のヘッダ */

#ifndef _START_DEFINE_H_
#define _START_DEFINE_H_

/* Arduino のピン定義 */
#define PIN_GOAL_SLAVE      4   //ゴール判定ピン(スレーブ)
#define PIN_GOAL_START      12  //ゴール判定ピン(スタートモジュール)
#define PIN_HIT_SLAVE       5   //当たった判定ピン(スレーブ)
#define PIN_HIT_START       3   //当たった判定ピン(スタートモジュール)
#define PIN_RX              10  //ソフトウェアシリアルRX
#define PIN_TX              11  //ソフトウェアシリアルTX
#define PIN_DIP_0           6   //DIPスイッチbit0
#define PIN_DIP_1           7   //DIPスイッチbit1
#define PIN_DIP_2           8   //DIPスイッチbit2
#define PIN_DIP_3           9   //DIPスイッチbit3

/* PC とのソフトウェアシリアル通信関係 */
/* 送信用関数への引数 */
#define START   0
#define HIT     1
#define THROUGH 2
#define POINT   3
#define FINISH  4
/* メッセージの内容 */
const char SERIAL_START   = 's'; //開始
const char SERIAL_HIT     = 'h'; //当たった
const char SERIAL_THROUGH = 't'; //モジュール通過
const char SERIAL_POINT   = 'p'; //スコアアップ
const char SERIAL_FINISH  = 'f'; //終了

/* I2C関係 */
/* スレーブのアドレス */
#define SLAVE_GOAL 0 //ゴールモジュールのスレーブアドレス
#define SLAVE_1    1 //モジュール1のスレーブアドレス
#define SLAVE_2    2 //モジュール2のスレーブアドレス
#define SLAVE_3    3 //モジュール3のスレーブアドレス
#define SLAVE_4    4 //モジュール4のスレーブアドレス
#define SLAVE_5    5 //モジュール2のスレーブアドレス
#define SLAVE_6    6 //モジュール3のスレーブアドレス
#define SLAVE_7    7 //モジュール4のスレーブアドレス
#define SLAVE_8    8 //モジュール2のスレーブアドレス
#define SLAVE_9    9 //モジュール3のスレーブアドレス
/* スレーブへ送信する情報 */
#define MASTER_BEGIN_TRANS 0 //通信を開始すること
#define MASTER_DETECT_HIT  1 //HITを受信したこと
#define MASTER_DETECT_GOAL 2 //通過/ゴールを受信したこと

/* スタートモジュールの状態 */
#define STATE_START   0 //開始待ち状態
#define STATE_RUNNING 1 //ゲーム進行状態
#define STATE_FINISH  2 //終了状態

#endif //_START_DEFINE_H_

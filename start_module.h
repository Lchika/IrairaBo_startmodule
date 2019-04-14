/* start_define.h            */
/* スタートモジュール関係のヘッダ */

#ifndef _START_MODULE_H_
#define _START_MODULE_H_

/* Arduino のピン定義 */
#define PIN_GOAL_START      2  //ゴール判定ピン(スタートモジュール)
#define PIN_HIT_START       4   //当たった判定ピン(スタートモジュール)
#define PIN_RX              11  //ソフトウェアシリアルRX
#define PIN_TX              13  //ソフトウェアシリアルTX
#define PIN_DIP_0           6   //DIPスイッチbit0
#define PIN_DIP_1           7   //DIPスイッチbit1
#define PIN_DIP_2           8   //DIPスイッチbit2
#define PIN_DIP_3           9   //DIPスイッチbit3
#define PIN_SERVO           3   //多回転サーボの信号線用ピン
#define PIN_LED             5   //スタート部分のLEDの出力用ピン

/* PC とのソフトウェアシリアル通信関係 */
#define PC_SSERIAL_BAUDRATE     9600  //ボーレート

/* D-sub通信関係 */
#define INTERVAL_DSUB_COMM_MS   500   //スレーブに対してイベント発生状況を確認する間隔[ms]

/* スタートモジュールの実行状態 */
typedef enum _RUN_STATE_E{
  STATE_WAIT_START = (0),  //開始待ち状態
  STATE_IN_START_M,        //スタートモジュール通過中状態
  STATE_IN_SLAVE_M,        //スレーブモジュール通過中状態
} RUN_STATE_E;

#endif //_START_MODULE_H_

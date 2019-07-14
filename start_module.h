/* start_define.h            */
/* スタートモジュール関係のヘッダ */

#ifndef _START_MODULE_H_
#define _START_MODULE_H_

/* Arduino のピン定義 */
#define PIN_GOAL_START      2   //ゴール判定ピン(スタートモジュール)
#define PIN_HIT_START       4   //当たった判定ピン(スタートモジュール)
#define PIN_RX              11  //ソフトウェアシリアルRX
#define PIN_TX              13  //ソフトウェアシリアルTX
#define PIN_DIP_0           10  //DIPスイッチbit0
#define PIN_DIP_1           8   //DIPスイッチbit1
#define PIN_DIP_2           9   //DIPスイッチbit2
#define PIN_DIP_3           7   //DIPスイッチbit3
#define PIN_SERVO           3   //多回転サーボの信号線用ピン
#define PIN_LED             5   //スタート部分のLEDの出力用ピン
#define PIN_START_SW        12  //スタート部分のマイクロスイッチ
#define PIN_VMOTOR          6   //振動モータ
#define PIN_LED_STICK       16  //棒部分のLED出力用ピン
#define PIN_SOFT_RX1        14  //DFPlayerMini用シリアル
#define PIN_SOFT_TX1        15  //DFPlayerMini用シリアル


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

/* モジュール動作関係 */
#define INTERVAL_DETECT_HIT_MS  1000  //  コース接触検知間隔[ms]
#define LED_ERROR_INTERVAL_MS   50    //  エラー時LED点滅間隔[ms]
#define LED_ERROR_BLINK_COUNT   30    //  エラー時LED点滅回数
#define LED_HIT_INTERVAL_MS     50    //  コース接触時LED点滅間隔[ms]
#define LED_HIT_BLINK_COUNT     5     //  コース接触時LED点滅回数
#define TIME_VMOTOR_MOVE_MS     300   //  振動モータを動かす時間[ms]
#define POWER_VMOTER_MOVE       130   //  振動モータに印加する電圧の大きさ(0-255)
#define LED_HIT_S_INTERVAL_MS   50    //  コース接触時棒部分LED点滅間隔[ms]
#define LED_HIT_S_BLINK_COUNT   3     //  コース接触時棒部分LED点滅回数

#endif //_START_MODULE_H_

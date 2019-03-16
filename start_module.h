/* start_define.h            */
/* スタートモジュール関係のヘッダ */

#ifndef _START_MODULE_H_
#define _START_MODULE_H_

/* Arduino のピン定義 */
#define PIN_GOAL_START      12  //ゴール判定ピン(スタートモジュール)
#define PIN_HIT_START       3   //当たった判定ピン(スタートモジュール)
#define PIN_RX              10  //ソフトウェアシリアルRX
#define PIN_TX              11  //ソフトウェアシリアルTX
#define PIN_DIP_0           6   //DIPスイッチbit0
#define PIN_DIP_1           7   //DIPスイッチbit1
#define PIN_DIP_2           8   //DIPスイッチbit2
#define PIN_DIP_3           9   //DIPスイッチbit3

/* PC とのソフトウェアシリアル通信関係 */
#define PC_SSERIAL_BAUDRATE     9600  //ボーレート

/* D-sub通信関係 */
#define INTERVAL_DSUB_COMM_MS   500   //スレーブに対してイベント発生状況を確認する間隔[ms]

/* スタートモジュールの状態 */
#define STATE_START   0 //開始待ち状態
#define STATE_RUNNING 1 //ゲーム進行状態
#define STATE_FINISH  2 //終了状態

#endif //_START_MODULE_H_

//  通信関係define定義

#ifndef DEFINE_COMM_H
#define DEFINE_COMM_H

//  スレーブへ送信/受信する情報
#define I2C_BEGIN_TRANS     0     //  通信を開始すること
#define I2C_DETECT_HIT      1     //  HITを検知したこと
#define I2C_DETECT_GOAL     2     //  通過/ゴールを検知したこと
#define I2C_CHECK_CONNECT   3     //  接続確認用

//  PCへ送信する情報
const char SERIAL_START   = 's';  //  開始
const char SERIAL_HIT     = 'h';  //  当たった
const char SERIAL_THROUGH = 't';  //  モジュール通過
const char SERIAL_POINT   = 'p';  //  スコアアップ
const char SERIAL_FINISH  = 'f';  //  終了

#endif //DEFINE_COMM_H
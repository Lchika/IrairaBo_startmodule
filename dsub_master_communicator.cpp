#include "dsub_master_communicator.hpp"
#include "debug.h"
#include <Wire.h>
#include <Arduino.h>

/**
 * @brief コンストラクタ
 * @param[in] slave_adress          スレーブアドレス
 * @param[in] serialComm            シリアル通信用クラス
 * @param[in] interval_comm_req_ms  i2c通信間隔[ms]
 * @return None
 */
DsubMasterCommunicator::DsubMasterCommunicator(unsigned char slave_num,
  SerialCommunicator *serialComm, unsigned int interval_comm_req_ms)
  :_slave_num(slave_num)
  ,_serialCommunicator(serialComm)
  ,_interval_comm_ms(interval_comm_req_ms)
{
  DebugPrint("start");
  //  マスタとしてI2C通信開始
  Wire.begin();
  DebugPrint("I2C ready");
  _active = false;
  DebugPrint("end");
};

/**
 * @brief 活性化処理
 * @param None
 * @return None
 */
bool DsubMasterCommunicator::active(unsigned char slave_adress)
{
  DebugPrint("start");
  _comm_slave_adress = slave_adress;
  sprintf(dprint_buff, "start i2c with SLAVE%d", slave_adress);
  DebugPrint(dprint_buff);
  Wire.beginTransmission(slave_adress);
  Wire.write(I2C_BEGIN_TRANS);
  Wire.endTransmission();
  DebugPrint("end i2c trans");
  _active = true;
  DebugPrint("end")
  return true;
}

/**
 * @brief 状態リセット処理
 * @param None
 * @return None
 */
bool DsubMasterCommunicator::dis_active(void)
{
  _active = false;
  return true;
}

/**
 * @brief 活性状態取得処理
 * @param None
 * @return None
 */
bool DsubMasterCommunicator::get_active(void)
{
  return _active;
}

/**
 * @brief D-sub関係イベント処理関数
 * @param None
 * @return bool true:エラーなし、false:エラーあり
 * 
 * @details
 * この関数はゴール・コース接触検知を行うため、定期的に呼ぶ必要がある\n
 * とりあえずはloop()内で実行しておけば大丈夫のはず\n
 * 丁寧なつくりにするならタイマを使って定期的に実行できるようにするとよい
 */
bool DsubMasterCommunicator::handle_dsub_event(void)
{
  //DebugPrint("start");
  if(!(this->_active)){
    //  不活性状態の場合は何もしない
    return true;
  }
  Wire.requestFrom(_comm_slave_adress, I2C_DATA_SIZE);
  while(Wire.available()){
    byte massage = Wire.read();
    switch(massage){
      //  コース接触通知
      case I2C_DETECT_HIT:
        DebugPrint("got HIT");
        _serialCommunicator->send(SERIAL_HIT);
        break;
      //  ゴール通知
      case I2C_DETECT_GOAL:
        DebugPrint("got GOAL");
        //  ゴールモジュールからの通知の場合
        if(_comm_slave_adress == _slave_num){
          DebugPrint("GOAL from goal_module");
          //  PCにゲーム終了を通知
          _serialCommunicator->send(SERIAL_FINISH);
          //  D-sub通信を終了
          this->dis_active();
        //  ゴール以外からの通知の場合
        }else{
          DebugPrint("GOAL from slave_module");
          //  PCにモジュール通過を通知
          _serialCommunicator->send(SERIAL_THROUGH);
        }
        break;
      default:
        //DebugPrint("got default");
        break;
    }
  }
  //DebugPrint("end");
  return true;
}
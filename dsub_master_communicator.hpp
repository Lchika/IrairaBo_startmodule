#ifndef DSUB_MASTER_COMMUNICATOR_H
#define DSUB_MASTER_COMMUNICATOR_H

#include "define_comm.h"
#include "serial_communicator.hpp"

/**
 * @class DsubMasterCommunicator
 * @brief D-subを介した通信を管理するクラス、master用
 */
class DsubMasterCommunicator {
private:
  static const unsigned char I2C_DATA_SIZE = 1;   //  I2C通信のデータサイズ[byte]
  unsigned char _comm_slave_adress;               //  通信対象のスレーブアドレス
  unsigned char _slave_num;                       //  スレーブモジュールの数(ゴール含む)
  SerialCommunicator *_serialCommunicator = NULL; //  PCとの通信用クラス
  bool _active = false;                           //  活性状態
  unsigned int _interval_comm_ms;                 //  i2cメッセージ通信間隔[ms]
  char dprint_buff[128];

public:
  DsubMasterCommunicator(unsigned char slave_num,
      SerialCommunicator *serialComm, unsigned int interval_comm_req_ms);
  bool active(unsigned char slave_adress);
  bool dis_active(void);
  bool get_active(void);
  bool handle_dsub_event(void);
};

#endif
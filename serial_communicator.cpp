#include "serial_communicator.hpp"
#include "debug.h"
#include <Arduino.h>

/**
 * @bried コンストラクタ
 * @param[in] pin_rx    RXピン
 * @param[in] pin_tx    TXピン
 * @param[in] baudrate  ボーレート
 * @return None
 */
SerialCommunicator::SerialCommunicator(int pin_rx,
  int pin_tx, int baudrate, int timeout_ms)
  :_timeout(timeout_ms)
{
  DebugPrint("start");
  serial = new SoftwareSerial(pin_rx, pin_tx);
  serial->begin(baudrate);
  sprintf(dprint_buff, "begin rx[%d],tx[%d],baudrate[%d]", pin_rx, pin_tx, baudrate);
  DebugPrint(dprint_buff);
  DebugPrint("end");
}

/**
 * @brief デストラクタ
 * @param None
 * @return None
 */
SerialCommunicator::~SerialCommunicator(void)
{
  DebugPrint("start");
  if(serial != NULL){
    delete serial;
  }
  DebugPrint("end");
}

/**
 * @brief 送信処理
 * @param[in] message 送信メッセージ
 * @return bool true:エラーなし, false:エラーあり
 */
bool SerialCommunicator::send(char message){
  DebugPrint("start");
  if(message == NULL){
    DebugPrint("<ERROR> invalid value");
    return false;
  }
  serial->listen();
  serial->write(message);
  sprintf(dprint_buff, "message = %c", message);
  DebugPrint(dprint_buff);
  DebugPrint("wait response start");
  unsigned long send_time = millis();
  while(serial->read() != message){
    if(millis() - send_time > _timeout){
      //  通信タイムアウト処理
      //  今はログを出すだけ
      DebugPrint("<ERROR> timeout");
      break;
    }
  }
  DebugPrint("got response");
  DebugPrint("end");
  return true;
}
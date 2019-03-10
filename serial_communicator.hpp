#ifndef SERIAL_COMMUNICATOR_H
#define SERIAL_COMMUNICATOR_H

#include <SoftwareSerial.h>

/**
 * @class SerialCommunicator
 * @brief シリアル通信を管理するクラス
 */
class SerialCommunicator {
private:
  SoftwareSerial *serial = NULL;
  char dprint_buff[128];

public:
  SerialCommunicator(int pin_rx, int pin_tx, int baudrate);
  ~SerialCommunicator(void);
  bool send(char massage);
};

#endif
#ifndef SERIAL_COMMUNICATOR_H
#define SERIAL_COMMUNICATOR_H

#include <SoftwareSerial.h>

/**
 * @class SerialCommunicator
 * @brief PCとのシリアル通信を管理するクラス
 */
class SerialCommunicator {
private:
  SoftwareSerial *serial = NULL;
  char dprint_buff[128];
  int _timeout;

public:
  SerialCommunicator(int pin_rx, int pin_tx, int baudrate, int timeout_ms = 3000);
  ~SerialCommunicator(void);
  bool send(char message);
};

#endif
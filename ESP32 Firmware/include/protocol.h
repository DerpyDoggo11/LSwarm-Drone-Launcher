#pragma once

#define LSWARM_FW_VERSION "0.1.0"
#define LSWARM_LINK_BAUD 115200

#define LSWARM_TELEM_MS 1000
#define LSWARM_LINE_MAX 96

#define LSWARM_VDIV_NUM 440000UL
#define LSWARM_VDIV_DEN 220000UL

#define LSWARM_BATT_EMPTY_MV 3300
#define LSWARM_BATT_FULL_MV 4200

#define CMD_PING "PING"
#define CMD_ID "ID?"
#define CMD_STATUS "STATUS?"
#define CMD_LED "LED"
#define CMD_BUZZ "BUZZ"
#define CMD_BEEP "BEEP"
#define CMD_TELEM "TELEM"

#define RSP_READY "RDY"
#define RSP_PONG "PONG"
#define RSP_ID "ID"
#define RSP_TELEM "TELEM"
#define RSP_EVENT "EVT"
#define RSP_OK "OK"
#define RSP_ERR "ERR"

#define LSWARM_DEV_NAME "LSwarm-ESP32C3"

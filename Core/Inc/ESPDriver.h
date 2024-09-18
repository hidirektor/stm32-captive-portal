/*
 * ESPDriver.h
 *
 *  Created on: Aug 10, 2024
 *      Author: hidir
 */

#ifndef INC_ESPDRIVER_H_
#define INC_ESPDRIVER_H_

#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

// AT Komutları için Define Tanımlamaları
#define AT_CMD                    "AT\r\n"
#define AT_RST_CMD                "AT+RST\r\n"
#define AT_GSLP_CMD               "AT+GSLP=%d\r\n"
#define AT_RESTORE_CMD            "AT+RESTORE\r\n"
#define AT_CIUPDATE_CMD           "AT+CIUPDATE\r\n"
#define AT_RFPOWER_CMD            "AT+RFPOWER=%d\r\n"
#define AT_CWMODE_CMD             "AT+CWMODE=%d\r\n"
#define AT_CWMODE_QUERY_CMD       "AT+CWMODE?\r\n"
#define AT_CIFSR_CMD              "AT+CIFSR\r\n"
#define AT_CWJAP_CMD              "AT+CWJAP=\"%s\",\"%s\"\r\n"
#define AT_CWQAP_CMD              "AT+CWQAP\r\n"
#define AT_CWDHCP_CMD             "AT+CWDHCP=1,%d\r\n"
#define AT_CWDHCP_QUERY_CMD       "AT+CWDHCP?\r\n"
#define AT_CIPSTA_CMD             "AT+CIPSTA=\"%s\",\"%s\",\"%s\"\r\n"
#define AT_CWSAP_CMD              "AT+CWSAP=\"%s\",\"%s\",%d,%d,%d,%d\r\n"
#define AT_CWJAP_QUERY_CMD        "AT+CWJAP?\r\n"
#define AT_CWLIF_CMD              "AT+CWLIF\r\n"
#define AT_CIPSTATUS_CMD          "AT+CIPSTATUS\r\n"
#define AT_PING_CMD               "AT+PING=\"%s\"\r\n"
#define AT_CIPMUX_CMD             "AT+CIPMUX=%d\r\n"
#define AT_CIPMUX_QUERY_CMD       "AT+CIPMUX?\r\n"
#define AT_CIPSTART_TCP_CMD       "AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n"
#define AT_CIPSTART_UDP_CMD       "AT+CIPSTART=%d,\"UDP\",\"%s\",%d,%d\r\n"
#define AT_CIPCLOSE_CMD           "AT+CIPCLOSE=%d\r\n"
#define AT_CIPSERVER_ENABLE_CMD   "AT+CIPSERVER=1,%d\r\n"
#define AT_CIPSERVER_DISABLE_CMD  "AT+CIPSERVER=0,%d\r\n"
#define AT_CIPSEND_CMD            "AT+CIPSEND=%d,%d\r\n"

// Wifi Modları
typedef enum {
    WifiMode_Station = 1,
    WifiMode_SoftAP,
    WifiMode_StationAndSoftAP
} WifiMode_t;

// Wifi Şifreleme Türleri
typedef enum {
    WifiEncryptionType_OPEN = 0,
    WifiEncryptionType_WPA_PSK,
    WifiEncryptionType_WPA2_PSK,
    WifiEncryptionType_WPA_WPA2_PSK
} WifiEncryptionType_t;

// Wifi Rx Temizleme
void Wifi_RxClear(void);

// String Gönderme
int Wifi_SendString(UART_HandleTypeDef *huart, char *data);

// String Bekleme
int Wifi_WaitForString(UART_HandleTypeDef *huart, uint32_t TimeOut_ms, uint8_t *result, uint8_t CountOfParameter, ...);

// Wifi Rx Callback
void Wifi_RxCallBack(UART_HandleTypeDef *huart);
void Wifi_ProcessReceivedData(uint8_t* buffer, uint16_t length);

// ESP8266'nın Temel Fonksiyonları
int Wifi_Init(UART_HandleTypeDef *huart);
void Wifi_Enable(void);
void Wifi_Disable(void);
int Wifi_Restart(UART_HandleTypeDef *huart);
int Wifi_DeepSleep(UART_HandleTypeDef *huart, uint16_t DelayMs);
int Wifi_FactoryReset(UART_HandleTypeDef *huart);
int Wifi_Update(UART_HandleTypeDef *huart);
int Wifi_SetRfPower(UART_HandleTypeDef *huart, uint8_t Power_0_to_82);

// ESP8266'nın Mod Ayarları: Station, SoftAP veya her ikisi
int Wifi_SetMode(UART_HandleTypeDef *huart, WifiMode_t WifiMode_);
int Wifi_GetMode(UART_HandleTypeDef *huart);
int Wifi_GetMyIp(UART_HandleTypeDef *huart);

// Station Modu İşlemleri
int Wifi_Station_ConnectToAp(UART_HandleTypeDef *huart, char *SSID, char *Pass, char *MAC);
int Wifi_Station_Disconnect(UART_HandleTypeDef *huart);
int Wifi_Station_DhcpEnable(UART_HandleTypeDef *huart, bool Enable);
int Wifi_Station_DhcpIsEnable(UART_HandleTypeDef *huart);
int Wifi_Station_SetIp(UART_HandleTypeDef *huart, char *IP, char *GateWay, char *NetMask);

// SoftAP Modu İşlemleri
int Wifi_SoftAp_Create(UART_HandleTypeDef *huart, char *SSID, char *password, uint8_t channel,
		WifiEncryptionType_t WifiEncryptionType, uint8_t MaxConnections_1_to_4,
		bool HiddenSSID);
int Wifi_GetApConnection(UART_HandleTypeDef *huart);
int Wifi_SoftAp_GetConnectedDevices(UART_HandleTypeDef *huart);

// TCP/IP İşlemleri
int Wifi_TcpIp_GetConnectionStatus(UART_HandleTypeDef *huart);
int Wifi_TcpIp_Ping(UART_HandleTypeDef *huart, char *PingTo);
int Wifi_TcpIp_SetMultiConnection(UART_HandleTypeDef *huart, bool EnableMultiConnections);
int Wifi_TcpIp_GetMultiConnection(UART_HandleTypeDef *huart);
int Wifi_TcpIp_StartTcpConnection(UART_HandleTypeDef *huart, uint8_t LinkId, char *RemoteIp, uint16_t RemotePort,
		uint16_t TimeOut_S);
int Wifi_TcpIp_StartUdpConnection(UART_HandleTypeDef *huart, uint8_t LinkId, char *RemoteIp, uint16_t RemotePort,
		uint16_t LocalPort);
int Wifi_TcpIp_Close(UART_HandleTypeDef *huart, uint8_t LinkId);
int Wifi_TcpIp_SetEnableTcpServer(UART_HandleTypeDef *huart, uint16_t PortNumber);
int Wifi_TcpIp_SetDisableTcpServer(UART_HandleTypeDef *huart, uint16_t PortNumber);
int Wifi_TcpIp_SendDataUdp(UART_HandleTypeDef *huart, uint8_t LinkId, uint16_t dataLen, uint8_t *data);
int Wifi_TcpIp_SendDataTcp(UART_HandleTypeDef *huart, uint8_t LinkId, uint16_t dataLen, uint8_t *data);


#endif /* INC_ESPDRIVER_H_ */

/*
 * ESPDriver.c
 *
 *  Created on: Aug 10, 2024
 *      Author: hidirektor
 */

#include "ESPDriver.h"
#include "WifiConfig.h"

static uint8_t RxBuffer[_WIFI_RX_SIZE];   // RX tampon boyutu WifiConfig.h dosyasından alındı
static uint8_t TxBuffer[_WIFI_TX_SIZE];   // TX tampon boyutu WifiConfig.h dosyasından alındı
static uint8_t RxBufferForData[_WIFI_RX_FOR_DATA_SIZE]; // Response için ek buffer
static volatile uint16_t RxIndex = 0;

void Wifi_RxClear(void) {
    memset(RxBuffer, 0, sizeof(RxBuffer));
    RxIndex = 0;
}

int Wifi_SendString(UART_HandleTypeDef *huart, char *data) {
    // WiFi üzerinden komut gönderme
    return HAL_UART_Transmit(huart, (uint8_t*)data, strlen(data), HAL_MAX_DELAY);
}

int Wifi_WaitForString(UART_HandleTypeDef *huart, uint32_t TimeOut_ms, uint8_t *result, uint8_t CountOfParameter, ...) {
    va_list args;
    va_start(args, CountOfParameter);

    uint32_t startTime = HAL_GetTick();
    while ((HAL_GetTick() - startTime) < TimeOut_ms) {
        for (int i = 0; i < CountOfParameter; i++) {
            char *expectedString = va_arg(args, char*);
            if (strstr((char*)RxBuffer, expectedString) != NULL) {
                *result = i;
                va_end(args);
                return 1; // Beklenen string bulundu
            }
        }
    }
    va_end(args);
    return 0; // Beklenen string bulunamadı
}

void Wifi_RxCallBack(UART_HandleTypeDef *huart) {
    uint8_t data;

    if (HAL_UART_Receive_IT(huart, &data, 1) == HAL_OK) {
        RxBuffer[RxIndex++] = data;
        if (RxIndex >= _WIFI_RX_SIZE) {
            RxIndex = 0;  // Tampon taşarsa başa dön
        }
    }
}

void Wifi_ProcessReceivedData(uint8_t* buffer, uint16_t length) {
    // Custom receive function
}

// ESP8266'nın Temel Fonksiyonları
int Wifi_Init(UART_HandleTypeDef *huart) {
    Wifi_RxClear();
    return Wifi_SendString(huart, AT_CMD); // "AT" komutu gönder
}

void Wifi_Enable(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // GPIO_PIN_SET = HIGH
}

void Wifi_Disable(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // GPIO_PIN_RESET = LOW
}

int Wifi_Restart(UART_HandleTypeDef *huart) {
    Wifi_RxClear();

    return Wifi_SendString(huart, AT_RST_CMD); // "AT+RST" komutu gönder
}

int Wifi_DeepSleep(UART_HandleTypeDef *huart, uint16_t DelayMs) {
    char cmd[32];
    sprintf(cmd, AT_GSLP_CMD, DelayMs);

    return Wifi_SendString(huart, cmd);
}

int Wifi_FactoryReset(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_RESTORE_CMD); // "AT+RESTORE" komutu
}

int Wifi_Update(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CIUPDATE_CMD); // "AT+CIUPDATE" komutu
}

int Wifi_SetRfPower(UART_HandleTypeDef *huart, uint8_t Power_0_to_82) {
    char cmd[32];
    sprintf(cmd, AT_RFPOWER_CMD, Power_0_to_82);

    return Wifi_SendString(huart, cmd);
}

// ESP8266'nın Mod Ayarları
int Wifi_SetMode(UART_HandleTypeDef *huart, WifiMode_t WifiMode_) {
    char cmd[32];
    sprintf(cmd, AT_CWMODE_CMD, WifiMode_);

    return Wifi_SendString(huart, cmd);
}

int Wifi_GetMode(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CWMODE_QUERY_CMD); // "AT+CWMODE?" komutu
}

int Wifi_GetMyIp(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CIFSR_CMD); // "AT+CIFSR" komutu
}

// Station Modu İşlemleri
int Wifi_Station_ConnectToAp(UART_HandleTypeDef *huart, char *SSID, char *Pass, char *MAC) {
    char cmd[128];
    sprintf(cmd, AT_CWJAP_CMD, SSID, Pass);

    return Wifi_SendString(huart, cmd);
}

int Wifi_Station_Disconnect(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CWQAP_CMD); // "AT+CWQAP" komutu
}

int Wifi_Station_DhcpEnable(UART_HandleTypeDef *huart, bool Enable) {
    char cmd[32];
    sprintf(cmd, AT_CWDHCP_CMD, Enable ? 1 : 0);

    return Wifi_SendString(huart, cmd);
}

int Wifi_Station_DhcpIsEnable(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CWDHCP_QUERY_CMD); // "AT+CWDHCP?" komutu
}

int Wifi_Station_SetIp(UART_HandleTypeDef *huart, char *IP, char *GateWay, char *NetMask) {
    char cmd[128];
    sprintf(cmd, AT_CIPSTA_CMD, IP, GateWay, NetMask);

    return Wifi_SendString(huart, cmd);
}

// SoftAP Modu İşlemleri
int Wifi_SoftAp_Create(UART_HandleTypeDef *huart, char *SSID, char *password, uint8_t channel,
        WifiEncryptionType_t WifiEncryptionType, uint8_t MaxConnections_1_to_4, bool HiddenSSID) {

    char cmd[128];
    sprintf(cmd, AT_CWSAP_CMD, SSID, password, channel, WifiEncryptionType, MaxConnections_1_to_4, HiddenSSID ? 1 : 0);

    return Wifi_SendString(huart, cmd);
}

int Wifi_GetApConnection(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CWLIF_CMD); // "AT+CWLIF" komutu
}

int Wifi_SoftAp_GetConnectedDevices(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CWLIF_CMD); // "AT+CWLIF" komutu
}

// TCP/IP İşlemleri
int Wifi_TcpIp_GetConnectionStatus(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CIPSTATUS_CMD); // "AT+CIPSTATUS" komutu
}

int Wifi_TcpIp_Ping(UART_HandleTypeDef *huart, char *PingTo) {
    char cmd[128];
    sprintf(cmd, AT_PING_CMD, PingTo);

    return Wifi_SendString(huart, cmd);
}

int Wifi_TcpIp_SetMultiConnection(UART_HandleTypeDef *huart, bool EnableMultiConnections) {
    char cmd[32];
    sprintf(cmd, AT_CIPMUX_CMD, EnableMultiConnections ? 1 : 0);

    return Wifi_SendString(huart, cmd);
}

int Wifi_TcpIp_GetMultiConnection(UART_HandleTypeDef *huart) {
    return Wifi_SendString(huart, AT_CIPMUX_QUERY_CMD); // "AT+CIPMUX?" komutu
}

int Wifi_TcpIp_StartTcpConnection(UART_HandleTypeDef *huart, uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t TimeOut_S) {
    char cmd[128];
    sprintf(cmd, AT_CIPSTART_TCP_CMD, LinkId, RemoteIp, RemotePort, TimeOut_S);

    return Wifi_SendString(huart, cmd);
}

int Wifi_TcpIp_StartUdpConnection(UART_HandleTypeDef *huart, uint8_t LinkId, char *RemoteIp, uint16_t RemotePort, uint16_t LocalPort) {
    char cmd[128];
    sprintf(cmd, AT_CIPSTART_UDP_CMD, LinkId, RemoteIp, RemotePort, LocalPort);

    return Wifi_SendString(huart, cmd);
}

int Wifi_TcpIp_Close(UART_HandleTypeDef *huart, uint8_t LinkId) {
    char cmd[32];
    sprintf(cmd, AT_CIPCLOSE_CMD, LinkId);

    return Wifi_SendString(huart, cmd);
}

int Wifi_TcpIp_SetEnableTcpServer(UART_HandleTypeDef *huart, uint16_t PortNumber) {
    char cmd[32];
    sprintf(cmd, AT_CIPSERVER_ENABLE_CMD, PortNumber);

    return Wifi_SendString(huart, cmd);
}

int Wifi_TcpIp_SetDisableTcpServer(UART_HandleTypeDef *huart, uint16_t PortNumber) {
	char cmd[32];
	sprintf(cmd, AT_CIPSERVER_DISABLE_CMD, PortNumber);

	return Wifi_SendString(huart, cmd);
}

int Wifi_TcpIp_SendDataUdp(UART_HandleTypeDef *huart, uint8_t LinkId, uint16_t dataLen, uint8_t *data) {
	char cmd[32];
	sprintf(cmd, AT_CIPSEND_CMD, LinkId, dataLen);

	Wifi_SendString(huart, cmd);

	return HAL_UART_Transmit(huart, data, dataLen, HAL_MAX_DELAY);
}

int Wifi_TcpIp_SendDataTcp(UART_HandleTypeDef *huart, uint8_t LinkId, uint16_t dataLen, uint8_t *data) {
	char cmd[32];
	sprintf(cmd, AT_CIPSEND_CMD, LinkId, dataLen);

	Wifi_SendString(huart, cmd);

	return HAL_UART_Transmit(huart, data, dataLen, HAL_MAX_DELAY);
}

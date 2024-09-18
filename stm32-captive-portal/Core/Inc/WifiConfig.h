/*
 * WifiConfig.h
 *
 *  Created on: Sep 18, 2024
 *      Author: hidirektor
 */

#ifndef INC_WIFICONFIG_H_
#define INC_WIFICONFIG_H_

#define _WIFI_RX_SIZE 1024
#define _WIFI_TX_SIZE 1024
#define _WIFI_RX_FOR_DATA_SIZE 2048

// WiFi bağlantı bilgileri
#define WIFI_SSID       "MyWiFiSSID"
#define WIFI_PASSWORD   "MyWiFiPass"

// Access Point bilgileri
#define AP_SSID         "OnderGrup"
#define AP_PASSWORD     "111111111111"
#define AP_CHANNEL      5
#define AP_ENCRYPTION   3  // 0: Open, 2: WPA_PSK, 3: WPA2_PSK
#define AP_MAX_CONN     4  // Maksimum 4 bağlantı
#define AP_HIDDEN       0  // 0: SSID görünür, 1: SSID gizli

// Login sayfası URL'si
#define LOGIN_URL       "https://ondergrup.hidirektor.com.tr/login"

#endif /* INC_WIFICONFIG_H_ */

#pragma once

// wifi
#include <WiFi.h>
#include <WiFiMulti.h>

bool wifi_connect();

struct wificonf {
	char const * ssid;
	char const * pwd;
};

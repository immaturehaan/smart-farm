// Ref: https://github.com/espressif/arduino-esp32/issues/1100#issuecomment-392397250

#include <esp_wifi.h>
#include <string.h>
// #include <WiFi.h>
#include <Preferences.h> // Storage
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

// const char *rssiSSID; // NO MORE hard coded set AP, all SmartConfig
// const char *password;
String PrefSSID, PrefPassword; // used by preferences storage

int WFstatus;
int32_t rssi; // store WiFi signal strength here
String getSsid;
String getPass;
String MAC;
String DeviceName;

// Storage
Preferences preferences; // declare class object

// ca 
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFXTCCBEWgAwIBAgISAxvxvvJ45NQpdqrs+ZF/SIycMA0GCSqGSIb3DQEBCwUA\n" \
"MEoxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MSMwIQYDVQQD\n" \
"ExpMZXQncyBFbmNyeXB0IEF1dGhvcml0eSBYMzAeFw0xOTA2MDcxMjI0MTBaFw0x\n" \
"OTA5MDUxMjI0MTBaMB0xGzAZBgNVBAMTEnd3dy5zbWFydGZhcm1pby5tbDCCASIw\n" \
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK3NrYE54dlxHQnAOykNZb1dhof7\n" \
"kTYJxqqVDiEZPtdUKCq/Gi8e7VcCU/qPbnSanlGZMKuugm4cg/CvHSnwnddY7Oy1\n" \
"czRw5k+6rygU9s75zIFTtbuKiXdp9PYeebAe0KJ12pNZLqX3fGV+i1wMF67QmKim\n" \
"N2oXW1kpqOlwc66jdencLBHs5ZI5BRLgj1pDwMl+aUY6FyGk7kuhRGaDvp6FdtOg\n" \
"oiNYzTIXgvrpU9k0AwQ+lIOZwoNCpAx3U/f7ZIR/Ek0pAo8nKBlIPQa9RyiHsX/U\n" \
"HfCerCKL5+L6fIjsit6yg00G2dHQ4+bMo0yAqANnV6uHpSn+HxF0fG80FOcCAwEA\n" \
"AaOCAmgwggJkMA4GA1UdDwEB/wQEAwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYI\n" \
"KwYBBQUHAwIwDAYDVR0TAQH/BAIwADAdBgNVHQ4EFgQURpIk723x6ea0vm8Vyg2o\n" \
"4HtFUjYwHwYDVR0jBBgwFoAUqEpqYwR93brm0Tm3pkVl7/Oo7KEwbwYIKwYBBQUH\n" \
"AQEEYzBhMC4GCCsGAQUFBzABhiJodHRwOi8vb2NzcC5pbnQteDMubGV0c2VuY3J5\n" \
"cHQub3JnMC8GCCsGAQUFBzAChiNodHRwOi8vY2VydC5pbnQteDMubGV0c2VuY3J5\n" \
"cHQub3JnLzAdBgNVHREEFjAUghJ3d3cuc21hcnRmYXJtaW8ubWwwTAYDVR0gBEUw\n" \
"QzAIBgZngQwBAgEwNwYLKwYBBAGC3xMBAQEwKDAmBggrBgEFBQcCARYaaHR0cDov\n" \
"L2Nwcy5sZXRzZW5jcnlwdC5vcmcwggEFBgorBgEEAdZ5AgQCBIH2BIHzAPEAdgBj\n" \
"8tvN6DvMLM8LcoQnV2szpI1hd4+9daY4scdoVEvYjQAAAWsyHGhVAAAEAwBHMEUC\n" \
"IHJcl4aN7TWn4sHxezvj1dpU7WFx1+KXK4azUh8a+y65AiEAhCBCwX1Tx0BFSB69\n" \
"FZyG4i3/U86R5QQA9iFW3cOWZocAdwB0ftqDMa0zEJEhnM4lT0Jwwr/9XkIgCMY3\n" \
"NXnmEHvMVgAAAWsyHGonAAAEAwBIMEYCIQDZRWMH95oAXVeLKfHlVwDPcozv5X5B\n" \
"RroMb4LmBLBZogIhALei2/Jc4UlgQPGE2C46vpURzPEtbWOJfdrbsWgQ4xkfMA0G\n" \
"CSqGSIb3DQEBCwUAA4IBAQBQzfKNA4wt1NSk402drzOxoTnAFZuW1/e7rBNNOV50\n" \
"wCVJ3u+0x75hkW5GkxoarFqHhEnf7tOs8EYMFTHhXfNIbOz7o/tsEOhBidRo597s\n" \
"2K+WYds3Nv1HtnPRHFW8kfTFfrBqUhOaUOer8fsct3AZzqJBEcJDisFYMYjRO3sI\n" \
"UAv6pDKY0YZDFRbGHSi/y9NpzD3YcRBRt4csiznw36y1Mgq892IlumAP25KFEHjd\n" \
"q4cAGJA9/3DorBHuRh5ixaniXczd+rOy/G1w5igWNgi4FbeS9kM2X8qp/18eIU+g\n" \
"c1ZSuiqMYlxvoe4F5etM5chjQt3i6SBDlUmg99S4HkQx\n" \
"-----END CERTIFICATE-----\n";

// Time
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
int curTimeStamp = 0;
unsigned long previousMillis = 0;
unsigned long resetInterval = 50*1000;

// PIN
int const I_PIN_SOIL = 34;
int const I_PIN_TEMP = 35;
int const I_PIN_RESET_WIFI = 4;

int const LED_STATUS = 22;
int const LED_RED = 13;
int const LED_GREEN = 2;
int const LED_BLUE = 0;

int const O_PUMP = 17;

// Global Const
int const GLOBAL_DELAY = 2000;
int const SHORT_DELAY = 200;
int const SAMPLE_DURATION = 10;
int const SAMPLE_TIME = 1;
int const H_PUMP_LEVEL = 70;
int const L_PUMP_LEVEL = 40;
int const SENSOR_RES = 4096;

// Global var
String deviceName;
bool IsAutoPump = false;
bool IsPump = false;
int HighPumpLevel = H_PUMP_LEVEL;
int LowPumpLevel = L_PUMP_LEVEL;
int CurSoil = -1;
int CurTemp = -1;
bool ledOn = true;

int const UPDATE_TIME = 500;
const String HOST = "https://www.smartfarmio.ml/pots";
// const String HOST = "http://192.168.8.28:3000/pots";
const String POST_HOST = HOST + "/update";
const String GET_HOST = HOST + "/info?iot=true&id=";
const bool sslActive = true;
int GlobalCount = 0;

// Function declare
void updateLed(int state);
void setupPin();
void wifiInit();
void wifiDown();
bool checkPrefsStore();
void initSmartConfig();
void IP_info();
int getWifiStatus(int WiFiStatus);
String getMacAddress(void);
String getSsidPass(String s);
void syncTime();
int getTimeStamp();
void loadData();
void syncInfo();
void sendInfo();
void getInfo();
void getSoilValue();
void getTempValue();
void getSensorInfo();
void autoPump();
void pump();

void setup() {
  Serial.begin(115200);

  setupPin();

  updateLed(0);

  wifiInit(); // get WiFi connected
  IP_info();
  MAC = getMacAddress();

  syncTime();

  loadData();

  delay(SHORT_DELAY);
}

void loop() {
  getSensorInfo();
  if (WiFi.status() == WL_CONNECTED) {
    // Handle server config
    syncInfo();
  } else { 
    wifiDown();
  }
  if (digitalRead(I_PIN_RESET_WIFI)) {
    initSmartConfig();
  }

  if (IsAutoPump) {
    autoPump();
  } else {
    pump();
  }
  delay(GLOBAL_DELAY);
}

void updateLed(int state) {
  if (digitalRead(I_PIN_RESET_WIFI)) {
    initSmartConfig();
  }
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  if (state == 0) {
    digitalWrite(LED_STATUS, HIGH);
    digitalWrite(LED_RED, HIGH);
  }
  if (state == 1) { // blink
    if (ledOn)
    {
      digitalWrite(LED_STATUS, LOW);
    } else {
      digitalWrite(LED_STATUS, HIGH);
    }
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
    ledOn = !ledOn;
  }
  if (state == 2) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_STATUS, LOW);
  }
}

void setupPin() {
  pinMode(I_PIN_RESET_WIFI, INPUT);

  pinMode(LED_STATUS, OUTPUT);
  pinMode(O_PUMP, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(15, LOW); //GND for led
}

void wifiInit() {
  Serial.printf("\tWiFi Setup -- \n");

  WiFi.mode(WIFI_AP_STA); // required to read NVR before WiFi.begin()

  // load credentials from NVR, a little RTOS code here
  wifi_config_t conf;
  esp_wifi_get_config(WIFI_IF_STA, &conf); // load wifi settings to struct comf
  // rssiSSID = reinterpret_cast<const char *>(conf.sta.ssid);
  // password = reinterpret_cast<const char *>(conf.sta.password);

  // Open Preferences with "wifi" namespace. Namespace is limited to 15 chars
  preferences.begin("wifi", false);
  PrefSSID = preferences.getString("ssid", "none");         //NVS key ssid
  PrefPassword = preferences.getString("password", "none"); //NVS key password
  preferences.end();
  
  // keep from rewriting flash if not needed
  if (!checkPrefsStore() && PrefSSID == "none")   // see is NV and Prefs are the same
  {                         // not the same, setup with SmartConfig
    initSmartConfig();
  }

  // // I flash LEDs while connecting here

  WiFi.begin(PrefSSID.c_str(), PrefPassword.c_str());

  int WLcount = 0;
  while (WiFi.status() != WL_CONNECTED && WLcount < 100) // can take > 200 loops depending on router settings
  {
    delay(SHORT_DELAY);
    Serial.printf(".");
    updateLed(1);
    ++WLcount;
  }
  delay(GLOBAL_DELAY);
  updateLed(2);
  //  stop the led flasher here

}

void wifiDown() {
  //  wifi down start LED flasher here
    WFstatus = getWifiStatus(WFstatus);

    WiFi.begin(PrefSSID.c_str(), PrefPassword.c_str());
    int WLcount = 0;
    while (WiFi.status() != WL_CONNECTED && WLcount < 100)
    {
      delay(SHORT_DELAY);
      Serial.printf(".");
      updateLed(1);
      ++WLcount;
    }

    if (getWifiStatus(WFstatus) == 3) //wifi returns
    {
      // stop LED flasher, wifi going up
      updateLed(2);
    }
    delay(GLOBAL_DELAY);
}

// match WiFi IDs in NVS to Pref store,  assumes WiFi.mode(WIFI_AP_STA);  was executed
bool checkPrefsStore() {
  bool val = false;
  String NVssid, NVpass, prefssid, prefpass;

  NVssid = getSsidPass("ssid");
  NVpass = getSsidPass("pass");

  // Open Preferences with my-app namespace. Namespace name is limited to 15 chars
  preferences.begin("wifi", false);
  prefssid = preferences.getString("ssid", "none"); //NVS key ssid
  prefpass = preferences.getString("password", "none"); //NVS key password
  preferences.end();

  if (NVssid.equals(prefssid) && NVpass.equals(prefpass))
  {
    val = true;
  }

  return val;
}

// optionally call this function any way you want in your own code
// to remap WiFi to another AP using SmartConfig mode.   Button, condition etc..
void initSmartConfig() {
  preferences.begin("wifi", false); // put it in storage
  PrefSSID = preferences.putString("ssid", "none");
  preferences.end();
  WiFi.mode(WIFI_AP_STA); //Init WiFi, start SmartConfig
  Serial.printf("Entering SmartConfig\n");

  WiFi.beginSmartConfig();

  while (!WiFi.smartConfigDone())
  {
    // flash led to indicate not configured
    updateLed(1);
    Serial.printf(".");
    delay(SHORT_DELAY);
  }

  // stopped flasher here

  Serial.printf("\nSmartConfig received.\n Waiting for WiFi\n\n");
  delay(GLOBAL_DELAY);

  while (WiFi.status() != WL_CONNECTED) // check till connected
  {
    updateLed(1);
    delay(SHORT_DELAY);
  }
  IP_info(); // connected lets see IP info

  preferences.begin("wifi", false); // put it in storage
  preferences.putString("ssid", getSsid);
  preferences.putString("password", getPass);
  preferences.end();

  delay(SHORT_DELAY);
  ESP.restart(); // reboot with wifi configured
}

void IP_info() {
  getSsid = WiFi.SSID();
  getPass = WiFi.psk();
  WFstatus = getWifiStatus(WFstatus);
  MAC = getMacAddress();

  Serial.printf("\n\n\tSSID\t%s\n, ", getSsid.c_str());
  Serial.printf("\tPass:\t %s\n", getPass.c_str());
  Serial.print("\n\n\tIP address:\t");
  Serial.print(WiFi.localIP());
  Serial.print(" / ");
  Serial.println(WiFi.subnetMask());
  Serial.print("\tGateway IP:\t");
  Serial.println(WiFi.gatewayIP());
  Serial.print("\t1st DNS:\t");
  Serial.println(WiFi.dnsIP());
  Serial.printf("\tMAC:\t\t%s\n", MAC.c_str());
}

int getWifiStatus(int WiFiStatus) {
  WiFiStatus = WiFi.status();
  Serial.printf("\n\tStatus %d", WiFiStatus);
  switch (WiFiStatus)
  {
  case WL_IDLE_STATUS: // WL_IDLE_STATUS     = 0,
    Serial.printf(", WiFi IDLE \n");
    break;
  case WL_NO_SSID_AVAIL: // WL_NO_SSID_AVAIL   = 1,
    Serial.printf(", NO SSID AVAIL \n");
    break;
  case WL_SCAN_COMPLETED: // WL_SCAN_COMPLETED  = 2,
    Serial.printf(", WiFi SCAN_COMPLETED \n");
    break;
  case WL_CONNECTED: // WL_CONNECTED       = 3,
    Serial.printf(", WiFi CONNECTED \n");
    break;
  case WL_CONNECT_FAILED: // WL_CONNECT_FAILED  = 4,
    Serial.printf(", WiFi WL_CONNECT FAILED\n");
    break;
  case WL_CONNECTION_LOST: // WL_CONNECTION_LOST = 5,
    Serial.printf(", WiFi CONNECTION LOST\n");
    WiFi.persistent(false); // don't write FLASH
    break;
  case WL_DISCONNECTED: // WL_DISCONNECTED    = 6
    Serial.printf(", WiFi DISCONNECTED ==\n");
    WiFi.persistent(false); // don't write FLASH when reconnecting
    break;
  }
  return WiFiStatus;
}

// Get the station interface MAC address.
// @return String MAC
String getMacAddress(void) {
  WiFi.mode(WIFI_AP_STA); // required to read NVR before WiFi.begin()
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA); // Get MAC address for WiFi station
  char macStr[18] = {0};
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(macStr);
}

// Returns String NONE, ssid or pass arcording to request
// ie String var = getSsidPass( "pass" );
String getSsidPass(String s) {
  String val = "NONE"; // return "NONE" if wrong key sent
  s.toUpperCase();
  if (s.compareTo("SSID") == 0)
  {
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    val = String(reinterpret_cast<const char *>(conf.sta.ssid));
  }
  if (s.compareTo("PASS") == 0)
  {
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    val = String(reinterpret_cast<const char *>(conf.sta.password));
  }
  return val;
}

void syncTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  curTimeStamp = mktime(&timeinfo);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

int getTimeStamp() {
  // TODO: When over 50 days millis will reset
  int res = millis()/1000;
  return res + curTimeStamp;
}

void loadData() {
  Serial.println("Start loading data");
  preferences.begin("pot", false);
  // Serial.println(preferences.freeEntries());

  DeviceName = preferences.getString("dvName", MAC);
  IsAutoPump = preferences.getBool("autoPump", false);
  IsPump = preferences.getBool("isPump", false);
  HighPumpLevel = preferences.getInt("hPLevel", H_PUMP_LEVEL);
  LowPumpLevel = preferences.getInt("lPLevel", L_PUMP_LEVEL);

  preferences.end();
}

void saveData(StaticJsonDocument<800> doc) {
  preferences.begin("pot", false);

  if (doc.containsKey("DeviceName"))
  {
    DeviceName = doc["DeviceName"].as<String>();
    preferences.putString("dvName", DeviceName);
  }

  if (doc.containsKey("IsAutoPump"))
  {
    IsAutoPump = doc["IsAutoPump"].as<bool>();
    preferences.putBool("autoPump", IsAutoPump);
  }

  if (!IsAutoPump && doc.containsKey("IsPump"))
  {
    IsPump = doc["IsPump"].as<bool>();
    preferences.putBool("isPump", IsPump);
  }

  if (doc.containsKey("HighPumpLevel"))
  {
    HighPumpLevel = doc["HighPumpLevel"].as<int>();
    preferences.putInt("hPLevel", HighPumpLevel);
  }

  if (doc.containsKey("LowPumpLevel"))
  {
    LowPumpLevel = doc["LowPumpLevel"].as<int>();
    preferences.putInt("lPLevel", LowPumpLevel);
  }

  preferences.end();

  if (doc.containsKey("ResetWifi")) {
    preferences.begin("wifi", false);
    int resetWifi = preferences.getInt("resetWifi", -1);
    if (doc["ResetWifi"] != resetWifi)
    {
      PrefSSID = preferences.putString("ssid", "none");
      preferences.putInt("resetWifi", doc["ResetWifi"]);
      preferences.end();
      ESP.restart();
    }
    preferences.end();
  }
}

void syncInfo() {
  getInfo();
  sendInfo();
}

void sendInfo() {
  HTTPClient http;
  if (sslActive)
  {
    http.begin(POST_HOST, root_ca);
  } 
  else {
    http.begin(POST_HOST);
  }
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> jsonData;

  jsonData["ID"] = MAC;
  jsonData["DvName"] = DeviceName;
  jsonData["IsAutoPump"] = IsAutoPump;
  jsonData["IsPump"] = IsPump;
  jsonData["HighPumpLevel"] = HighPumpLevel;
  jsonData["LowPumpLevel"] = LowPumpLevel;
  jsonData["CurSoil"] = CurSoil;
  jsonData["CurTemp"] = CurTemp;
  jsonData["UpdateAt"] = getTimeStamp();

  char buffer[200];

  serializeJson(jsonData, buffer);

  int httpCode = http.POST(buffer);
  Serial.println(httpCode);

  if (httpCode > 0) {
    updateLed(2);
    String response = http.getString();
    StaticJsonDocument<450> doc;
    deserializeJson(doc, response);
  }

  else {
    updateLed(0);
    Serial.println("Error on HTTP request");
  }

  http.end(); //Free the resources
}

void getInfo() {
  HTTPClient http;
  if (sslActive)
  {
    http.begin(GET_HOST + MAC, root_ca);
  } 
  else {
    http.begin(GET_HOST + MAC);
  }

  int httpCode = http.GET();

  Serial.println(httpCode);

  if (httpCode > 0) {
    updateLed(2);
    String response = http.getString();
    StaticJsonDocument<800> doc;
    deserializeJson(doc, response);
    saveData(doc);
  }

  else {
    updateLed(0);
    Serial.println("Error on HTTP request");
  }

  http.end(); //Free the resources
}

void getSoilValue() {
  int t = SAMPLE_DURATION / SAMPLE_TIME;
  int total = 0;
  for (int i = 0; i < SAMPLE_TIME; i++)
  {
    total += analogRead(I_PIN_SOIL);
    delay(t);
  }
  CurSoil = map(total / SAMPLE_TIME, 0, SENSOR_RES, 100, 0);
  Serial.println(CurSoil);
}

void getTempValue() {
  int t = SAMPLE_DURATION / SAMPLE_TIME;
  int total = 0;
  for (int i = 0; i < SAMPLE_TIME; i++)
  {
    total += analogRead(I_PIN_TEMP);
    delay(t);
  }
  CurTemp = total * 5.0 * 100 / SAMPLE_TIME / SENSOR_RES;
  Serial.println(CurTemp);
}

void getSensorInfo() {
  getSoilValue();
  getTempValue();
}

void autoPump() {
  if (CurSoil < LowPumpLevel) {
    IsPump = true;
  }
  if (CurSoil > HighPumpLevel) {
    IsPump = false;
  } 
  pump();
}

void pump() {
  if (IsPump) {
    Serial.println("Pump is working");
    digitalWrite(O_PUMP, HIGH);
  } else {
    Serial.println("Pump is stoped working");
    digitalWrite(O_PUMP, LOW);
  }
}

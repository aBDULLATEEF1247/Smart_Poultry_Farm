#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "camera_pins.h"
#include <FirebaseESP32.h>
#include <HTTPClient.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include "DHT.h"
#include "url.h"

DHT dht(13,DHT11);
WebServer server(80);
#define CAMERA_MODEL_AI_THINKER
#define API_KEY "ru8rOqeGAxARK61mrsY2MxdeGNWCdIIceIURjmbB"
#define DATABASE_URL "https://smartpoultry-4324d-default-rtdb.firebaseio.com" 

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configure;

const char* ssid = "Abdullateef";
const char* password = "omolara01";
const int pir =12;
const int ldr = 14;
const int lightRelay = 15;
const int fanRelay = 2;
unsigned long lastMillis;
unsigned long sendDataPrevMillis;
unsigned long ReadDataPrevMillis;

int pirValue = 0;
int ldrValue; 
float t,h;
String Intruder = "False";
String light = "Off";
String fan = "Off";
String ipAddress = "";

void startCameraServer();
void setupLedFlash(int pin);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_SXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  

  if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif


   pinMode(pir,INPUT);
   pinMode(ldr,INPUT);
   pinMode(lightRelay,OUTPUT);
   //pinMode(fanRelay,OUTPUT);
   dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.print(WiFi.localIP());

   configure.api_key = API_KEY;
   configure.database_url = DATABASE_URL;
   Firebase.begin(DATABASE_URL, API_KEY);
  // Firebase.setDoubleDigits(7);
 
}

  void loop() {
    //readData();
     ldrValue = analogRead(ldr);
    t = dht.readTemperature();
    h = dht.readHumidity();
  if (ldrValue >= 800)
  {
    digitalWrite(lightRelay,HIGH);
    light = "ON";
    delay(10);
  }
  else if (ldrValue <= 700)
  {
    digitalWrite(lightRelay,LOW);
    light = "Off";
  }
  if (t >= 35){
    digitalWrite(fanRelay,HIGH);
    fan = "ON";
    delay(10);
  }
  else if (t <= 29){
    digitalWrite(fanRelay,LOW);
    fan = "Off";
  }
  
  Serial.print("Temp: "); Serial.print(t); Serial.print("    "); Serial.print("Hum: "); Serial.println(h);
  Serial.print("Light: "); Serial.print(ldrValue); Serial.print("   "); Serial.print("Pir: "); Serial.println(pirValue);
  writeData();
 }


void writeData()
{
   if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    Firebase.RTDB.setInt(&fbdo,    "FarmData/Temperature",t);
    Firebase.RTDB.setInt(&fbdo,    "FarmData/Humidity",h);
    Firebase.RTDB.setString(&fbdo, "FarmData/lightStatus",light);
    Firebase.RTDB.setString(&fbdo, "FarmData/FanStatus",fan);
    Firebase.RTDB.setString(&fbdo, "FarmData/Intruder",Intruder);
    Firebase.RTDB.setString(&fbdo, "FarmData/IP_Address",ipAddress);
   }
}

void readData()
{
  if (Firebase.ready() && (millis() - ReadDataPrevMillis > 1000 || ReadDataPrevMillis == 0)) {
    ReadDataPrevMillis = millis();
  if ((Firebase.RTDB.getInt(&fbdo, "/FarmWorker/Worker1")) && (Firebase.RTDB.getInt(&fbdo, "/FarmWorker/Worker2")) && (Firebase.RTDB.getInt(&fbdo, "/FarmWorker/Worker3"))) {
      if (fbdo.dataType() == "int") {
        int clock = fbdo.intData();
        Serial.println(clock);
        pirValue = digitalRead(pir);
        switch (clock){
          case 0:
            if (pirValue == 1){ Intruder = "True"; ipAddress = "https://" + String(WiFi.localIP()) + ":81/stream"; }
            else{Intruder = "False"; ipAddress = "null";}
            break;
          case 1:
            Intruder = "False";
            ipAddress = "null";
            break;
        }
        delay(10);

        
      }
    }
  }
}

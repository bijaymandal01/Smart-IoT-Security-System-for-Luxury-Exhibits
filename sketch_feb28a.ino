//final code
// WiFi Credentials
const char* ssid     = "SSID";   // WiFi SSID
const char* password = "password";   // WiFi Password
String token = "8135804780:AAEfDcQ753fxtgh9s523ECg8BffOrZbJCn8";  // Telegram Bot Token
String chat_id = "6587145346";  // Telegram Chat ID

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"

// HC-SR04 Ultrasonic Sensor Pins
#define TRIG_OBJECT  12  // First sensor (Object detection)
#define ECHO_OBJECT  14
#define TRIG_PERSON  15  // Second sensor (Person distance)
#define ECHO_PERSON  13

// Buzzer Pin
#define BUZZER_PIN 4  // Change this if needed

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(115200);
  delay(10);

  // Set up the Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // Ensure buzzer is OFF initially

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  
  long int startTime = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if ((startTime + 15000) < millis()) {  // Timeout after 15 sec
      Serial.println("WiFi Connection Failed!");
      return;
    }
  } 

  Serial.println("WiFi Connected! IP Address: " + WiFi.localIP().toString());

  // Initialize HC-SR04 sensors
  pinMode(TRIG_OBJECT, OUTPUT);
  pinMode(ECHO_OBJECT, INPUT);
  pinMode(TRIG_PERSON, OUTPUT);
  pinMode(ECHO_PERSON, INPUT);

  // Initialize Camera
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000; // Lower frequency for stability
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  Serial.println("Camera Initialized Successfully!");
}

void loop() {
  long objectDistance = getDistance(TRIG_OBJECT, ECHO_OBJECT);
  delay(200); 
  long personDistance = getDistance(TRIG_PERSON, ECHO_PERSON);

  Serial.print("Object Distance: ");
  Serial.print(objectDistance);
  Serial.println(" cm");

  Serial.print("Person Distance: ");
  Serial.print(personDistance);
  Serial.println(" cm");

  // Check object distance (Security System)
 if (objectDistance > 10) {  
    Serial.println("Object moved! Capturing evidence...");
    digitalWrite(BUZZER_PIN, HIGH);  // Turn buzzer ON immediately

    sendTelegramAlert(token, chat_id);

    // Keep buzzer ON for 5 seconds but don't delay image capture
    unsigned long buzzerStart = millis();
    while (millis() - buzzerStart < 5000) {  
      digitalWrite(BUZZER_PIN, HIGH);
    }
    digitalWrite(BUZZER_PIN, LOW);  
  }

  // Check person distance (Proximity Warning)
  if (personDistance > 10 && personDistance <= 50) {  // If person is too close
    Serial.println("Person too close! Beeping buzzer...");
    for (int i = 0; i < 5; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      delay(200);
    }
  }
  
  delay(1000);  
}

// Function to measure distance using HC-SR04
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout after 30ms
  long distance = duration * 0.034 / 2;

  if (distance < 2 || distance > 300) return -1;  // Ignore invalid values
  return distance;
}


// Function to send an image to Telegram
void sendTelegramAlert(String token, String chat_id) {
  Serial.println("Capturing Image...");
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  Serial.println("Image Captured! Sending to Telegram...");

  // Telegram API Endpoint
  String url = "https://api.telegram.org/bot" + token + "/sendPhoto";
  
  WiFiClientSecure client;
  client.setInsecure();  // Skip certificate validation
  
  if (!client.connect("api.telegram.org", 443)) {
    Serial.println("Connection to Telegram failed!");	
    esp_camera_fb_return(fb);
    return;
  }

  // Prepare the HTTP Request
  String boundary = "----ESP32Boundary";
  String head = "--" + boundary + "\r\n" +
                "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n" +
                chat_id + "\r\n" +
                "--" + boundary + "\r\n" +
                "Content-Disposition: form-data; name=\"photo\"; filename=\"image.jpg\"\r\n" +
                "Content-Type: image/jpeg\r\n\r\n";

  String tail = "\r\n--" + boundary + "--\r\n";

  client.print("POST " + url + " HTTP/1.1\r\n");
  client.print("Host: api.telegram.org\r\n");
  client.print("Content-Type: multipart/form-data; boundary=" + boundary + "\r\n");
  client.print("Content-Length: " + String(head.length() + fb->len + tail.length()) + "\r\n");
  client.print("\r\n");

  client.print(head);
  client.write(fb->buf, fb->len);  // Send image buffer
  client.print(tail);

  Serial.println("Image sent to Telegram!");
  esp_camera_fb_return(fb);  // Free memory
}

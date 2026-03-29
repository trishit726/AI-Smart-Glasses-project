#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_camera.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ======================= CONFIGURATION =======================
const char* ssid = "YOUR_WIFI_SSID";         // <-- INSERT YOUR WIFI SSID HERE
const char* password = "YOUR_WIFI_PASSWORD"; // <-- INSERT YOUR WIFI PASSWORD HERE
String serverUrl = "http://YOUR_SERVER_IP:8000/vision"; // <-- INSERT YOUR SERVER IP HERE (e.g. 192.168.1.xxx)

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C

// I2C Pins for XIAO ESP32S3
#define I2C_SDA 5 // D4
#define I2C_SCL 6 // D5

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button settings
#define BUTTON_PIN 2 // D1
bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Double tap detection variables
unsigned long lastTapTime = 0;
const unsigned long doubleTapDelay = 400; // time in ms to wait for the second tap
int tapCount = 0;

// Camera Pins for Seeed Studio XIAO ESP32S3 Sense (OV2640)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39
#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13
// =============================================================

void displayMessage(String msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  // println handles wrapping nicely as long as words are shorter than screen width,
  // but Adafruit_GFX automatically wraps characters by default.
  display.println(msg);
  display.display();
}

void setupCamera() {
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
  config.xclk_freq_hz = 20000000;
  
  // Set resolution. VGA works well without blowing up PSRAM
  config.frame_size = FRAMESIZE_VGA; 
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12; // 0-63, lower means better quality
  config.fb_count = 1;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    displayMessage("Camera Init Failed");
    return;
  }
}

void setup() {
  Serial.begin(115200);

  // Setup I2C & OLED
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  displayMessage("Booting...");

  // Setup Button with internal pull up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Setup ESP32 Camera
  setupCamera();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  displayMessage("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  
  displayMessage("Ready!\nDouble Tap to snap.");
}

void captureAndSend() {
  displayMessage("Capturing image...");
  
  // Capture an image
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    displayMessage("Camera capture failed");
    return;
  }

  displayMessage("Sending to AI...");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    
    // We send raw JPEG data directly
    http.addHeader("Content-Type", "image/jpeg");

    // Execute POST Request
    int httpResponseCode = http.POST(fb->buf, fb->len);

    if (httpResponseCode > 0) {
      // Receive AI description
      String response = http.getString();
      Serial.print("HTTP Code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + response);
      
      // Display the response from FastAPI directly on the OLED
      // Adafruit_GFX handles text wrapping
      displayMessage(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
      displayMessage("Server Error /\nConnection Failed");
    }
    http.end();
  } else {
    displayMessage("WiFi Disconnected. Rechecking...");
  }

  // Very important to free memory back to ESP32 core
  esp_camera_fb_return(fb); 
}

void loop() {
  // Read button
  int reading = digitalRead(BUTTON_PIN);
  static int lastButtonState = HIGH;
  static int buttonState = HIGH;

  // Debouncing logic
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      
      // If button is depressed (LOW because of INPUT_PULLUP)
      if (buttonState == LOW) {
        unsigned long currentTime = millis();
        // Check if double tap threshold is met
        if (currentTime - lastTapTime < doubleTapDelay && tapCount == 1) {
          Serial.println("Double Tap Detected!");
          captureAndSend();
          tapCount = 0; // Reset tap counting
        } else {
          // It's the first tap
          tapCount = 1;
          lastTapTime = currentTime;
        }
      }
    }
  }

  // Timeout logic to reset tap count if second press takes too long
  if (tapCount == 1 && (millis() - lastTapTime) > doubleTapDelay) {
    // Single tap timeout
    tapCount = 0;
  }

  lastButtonState = reading;
}

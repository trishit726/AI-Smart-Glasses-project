# AI Smart Glasses Project

This project contains two parts:
1. **Arduino Firmware (`firmware/smart_glasses/smart_glasses.ino`)** - C++ code for the Seeed Studio XIAO ESP32S3 Sense.
2. **Python Backend (`backend/server.py`)** - A FastAPI server to process images with OpenAI GPT-4o Vision.

## 1. Setting up the ESP32 (Arduino)

### Hardware Wiring:
- **Seeed Studio XIAO ESP32S3 Sense** with the OV2640 camera attached.
- **OLED Display (0.96 inch SSD1306):**
  - **SDA** -> XIAO D4 / GPIO 5
  - **SCL** -> XIAO D5 / GPIO 6
  - **VCC** -> 3.3V
  - **GND** -> GND
- **Push Button:**
  - One leg -> XIAO D1 / GPIO 2
  - Other leg -> GND (Uses internal pullup resistor).

### Software Setup:
1. Open the project in the standard Arduino IDE.
2. Install the necessary libraries from the Arduino Library Manager:
   - `Adafruit GFX Library`
   - `Adafruit SSD1306`
3. Make sure to install the esp32 boards manager package (by Espressif). Select "XIAO_ESP32S3" as your board and enable PSRAM to `OPI PSRAM` in the Tools menu (Camera requires PSRAM).
4. **CREDENTIAL INJECTION:**
   - Open `firmware/smart_glasses/smart_glasses.ino`.
   - Edit `ssid`, `password`, and `serverUrl` with your Wi-Fi details and local computer IP address respectively!

## 2. Setting up the Python Backend

We use FastAPI and the official OpenAI Python package.

### Installation:
1. Open up a terminal in the `backend/` directory.
2. (Optional) Create a virtual environment: `python -m venv venv` and activate it.
3. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```
4. **API KEY INJECTION:**
   - Open `backend/server.py`.
   - Set `OPENAI_API_KEY` to your real OpenAI secret key.

### Running the Server:
Run the server with Python directly:
```bash
python server.py
```
*Note: Make sure your ESP32 and Computer are on the exact same Wi-Fi network.*

## Usage:
1. Power on the ESP32. It should show a Booting/Wi-Fi connection screen on the OLED.
2. Look at the OLED, it will say "Ready! Double Tap to snap."
3. **Double-tap the push button.** 
4. The ESP32 captures an image, posts it to the server. The Python server queries GPT-4o Vision, which returns text.
5. The OLED displays the output sentence!

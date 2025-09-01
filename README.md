# ESP32 AlertSign

A WiFi-enabled ESP32 project that creates a visual alert system using WS2812 LED strips. The device can display different LED animations based on status signals received via HTTP requests, making it perfect for monitoring systems, status indicators, or ambient lighting.

## Features

### 🎨 LED Animations
- **Rainbow Animation**: Default colorful rainbow pattern
- **Urgent Status**: Red/green alternating lights for urgent notifications
- **Alert Status**: Blue/purple reverse animation for alerts
- **Good Status**: Chasing blue dot animation
- **Vibe Mode**: Dual colored dots moving around the strip
- **Setup Animation**: Green blinking sequence on startup

### 🌐 Web Interface
- **Web Server**: Built-in HTTP server for remote control
- **Status Control**: Change LED patterns via HTTP GET/POST requests
- **OTA Updates**: Over-the-air firmware updates via web interface
- **Uptime Kuma Integration**: Automatic status detection from Uptime Kuma webhooks
- **mDNS Support**: Accessible via `http://esp32.local`

### ⚡ Multi-Core Processing
- **Core 0**: Dedicated to LED animation control
- **Core 1**: Handles WiFi and web server operations
- **Inter-Core Communication**: Queue-based messaging between cores

## Hardware Requirements

- **ESP32 Development Board** (ESP32 DevKit recommended)
- **WS2812 LED Strip** (35 LEDs configured, customizable)
- **Power Supply**: 5V, 500mA minimum (regulated power supply recommended)
- **Connections**:
  - LED Data Pin: GPIO 13
  - Built-in LED: GPIO 2 (status indicator)

## Software Dependencies

- **PlatformIO** (recommended) or Arduino IDE
- **FastLED Library** (v3.8.0+)
- **ESP32 Arduino Framework**

## Installation & Setup

### 1. Clone the Repository
```bash
git clone <repository-url>
cd ESP32_AlertSign
```

### 2. Configure WiFi Settings

#### Option A: Using Environment Variables (Recommended)
1. Copy the example environment file:
   ```bash
   cp env.example .env
   ```
2. Edit `.env` and add your WiFi credentials:
   ```bash
   WIFI_SSID=your_wifi_ssid_here
   WIFI_PASSWORD=your_wifi_password_here
   HOST_NAME=esp32
   ```
3. PlatformIO will automatically use these environment variables during build.

#### Option B: Direct Configuration (Legacy)
Edit `include/ota_control.h` and update your WiFi credentials:
```cpp
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";
```

### 3. Build and Upload
Using PlatformIO:
```bash
pio run --target upload
```

Or using Arduino IDE:
1. Open `src/main.cpp` in Arduino IDE
2. Install required libraries (FastLED, ESP32 board support)
3. Select your ESP32 board and port
4. Upload the code

### 4. Access the Device
After upload, the device will:
1. Connect to WiFi automatically
2. Display a green setup animation
3. Be accessible via:
   - `http://esp32.local` (mDNS)
   - `http://[ESP32_IP_ADDRESS]` (check Serial Monitor for IP)

## Usage

### Web Interface
1. Navigate to the device's IP address
2. Login with username: `admin`, password: `admin`
3. Use the web interface to control LED patterns

### HTTP API Endpoints

#### Set Status
```bash
# GET request
curl "http://esp32.local/set?status=urgent"

# POST request
curl -X POST "http://esp32.local/set" -d "status=alert"
```

#### Available Status Values
- `none` (0): Rainbow animation (default)
- `urgent` (1): Red/green alternating lights
- `alert` (2): Blue/purple reverse animation
- `good` (3): Chasing blue dot
- `vibe` (4): Dual colored dots

#### Uptime Kuma Integration
```bash
curl -X POST "http://esp32.local/kuma" -d "status=your_kuma_status"
```
Automatically converts Uptime Kuma status to LED patterns:
- "Up" status → `none` (rainbow)
- "Down" status → `urgent` (red/green)

#### Check Current Status
```bash
curl "http://esp32.local/status"
```

### OTA Updates
1. Access the web interface
2. Navigate to the update page
3. Select and upload new firmware binary
4. Device will automatically restart with new firmware

## Configuration

### Environment Variables
The project supports environment variables for secure credential management:

| Variable | Description | Default |
|----------|-------------|---------|
| `WIFI_SSID` | Your WiFi network name | `your_wifi_ssid` |
| `WIFI_PASSWORD` | Your WiFi password | `your_wifi_password` |
| `HOST_NAME` | mDNS hostname for the device | `esp32` |

**Security Note**: Never commit your `.env` file to version control. The `.env` file is already in `.gitignore`.

### LED Strip Configuration
Edit `include/led_control.h`:
```cpp
#define NUM_LEDS 35        // Number of LEDs in your strip
#define DATA_PIN 13        // GPIO pin connected to LED data line
uint8_t max_brightness = 50; // LED brightness (0-255)
```

### Power Management
The device includes FastLED power management:
```cpp
FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); // 5V, 500mA limit
```

### Partition Table
The project includes a custom partition table (`partitions_two_ota.csv`) for OTA updates:
- Factory partition: 1MB
- OTA partitions: 2x 1MB (for dual OTA support)

## Troubleshooting

### Common Issues

1. **LEDs not lighting up**
   - Check power supply (5V, sufficient current)
   - Verify DATA_PIN connection (GPIO 13)
   - Ensure correct LED strip type (WS2812)

2. **WiFi connection issues**
   - Verify SSID and password in `ota_control.h`
   - Check Serial Monitor for connection status
   - Ensure 2.4GHz WiFi network (ESP32 doesn't support 5GHz)

3. **Web interface not accessible**
   - Check Serial Monitor for IP address
   - Try accessing via `http://esp32.local`
   - Verify mDNS is working on your network

4. **OTA update fails**
   - Ensure sufficient flash space
   - Check partition table configuration
   - Verify firmware binary is compatible

### Serial Debug Output
The device provides debug information via Serial Monitor (9600 baud):
- WiFi connection status
- IP address assignment
- mDNS setup status
- OTA update progress

## Project Structure

```
ESP32_AlertSign/
├── src/
│   └── main.cpp              # Main application entry point
├── include/
│   ├── led_control.h         # LED animation and control functions
│   ├── ota_control.h         # WiFi, web server, and OTA functionality
│   ├── login_page.h          # Web interface login page
│   └── update_page.h         # OTA update interface
├── lib/
│   └── ota_control/          # Library dependencies
├── platformio.ini            # PlatformIO configuration
├── partitions_two_ota.csv    # Custom partition table
└── README.md                 # This file
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is licensed under the terms specified in the LICENSE file.

## Acknowledgments

- FastLED library for LED control
- ESP32 Arduino framework
- PlatformIO for development environment


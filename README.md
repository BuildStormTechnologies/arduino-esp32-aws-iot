# Arduino ESP32 AWS IoT
The Buildstorm platform streamlines the incorporation of AWS IoT core features onto ESP32 Hardware, covering everything from basic IoT functionalities to advanced features like OTA updates and provisioning. This creates a solid foundation for your next IoT project, giving you a three-month head start in product development.

Supported features:
1. MQTT pub/sub
2. Shadow updates
3. Jobs
4. Web OTA using AWS jobs and S3 bucket
5. Local websever OTA
6. Provision by claim
7. BLE Device Provisioning


## Refrences
1. [ESP32 Aws IoT Platform](https://buildstorm.com/solutions/esp32-on-aws-iot-platform/)
2. [Mobile App: User Guide](https://buildstorm.com/blog/mobile-app-user-manual/)


---
## Example Setup
1. Copy the thing/claim certificates to `Aws_IoT\examples\example\certificates.c` file
2. Update the following WiFi and AWS parameters in `app_config.h` of the example
3. This configuration will be same accross all examples

```
#define TEST_WIFI_SSID     "YOUR WIFI SSID"
#define TEST_WIFI_PASSWORD "YOUR WIFI PWD"

#define AWS_IOT_MQTT_HOST "YOUR AWS HOST"
#define AWS_IOT_MQTT_PORT  8883
```

---
## SoC Compatibility

| Name            | BLE           | OTA           |
|-----------------|---------------|---------------|
| ESP32           | Not Supported | Not Supported |
| ESP32 S3        | Supported     | Supported     |

---
## Arduino setting
1. ESP32 settings

    ![esp32 arduino settings](<images/esp32 arduino settings.png>)
<br />
<br />
<br />

2. ESP32S3 settings

   ![esp32s3 arduino settings](<images/esp32s3 arduino settings.png>)
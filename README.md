# Arduino ESP32 AWS IoT

The Buildstorm platform simplifies the integration of AWS IoT core features onto ESP32 Hardware, spanning from basic IoT functionalities to advanced features like OTA updates and provisioning.

The architecture is based on the core IDF APIs, complemented by a C++ wrapper tailored for application use, guaranteeing non-blocking operation of all APIs. Each user action, including HTTP requests, MQTT publishing, Shadow updates, and OTA, is logged as an event and seamlessly managed in the background. To maintain this seamless operation, the platform effectively runs a system task.

This establishes a robust foundation for your upcoming IoT project.

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
2. [Mobile App: User Guide](https://buildstorm.com/2024/01/25/aws-iot-mobile-app-user-manual/)


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

While our Arduino IoT platform currently supports BLE and OTA solely on ESP32S3, these capabilities are accessible on our ESP-IDF platform for all ESP32 SoCs. These functionalities will soon be integrated into Arduino. Until then, you can explore the IDF version of our platform [ESP32-idf AWS IoT Platform](https://github.com/BuildStormTechnologies/esp32_aws_iot).

---
## Arduino setting
1. ESP32 settings

    ![esp32 arduino settings](<images/esp32 arduino settings.png>)
<br />
<br />
<br />

2. ESP32S3 settings

   ![esp32s3 arduino settings](<images/esp32s3 arduino settings.png>)

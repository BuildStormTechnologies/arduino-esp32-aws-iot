#include "app_config.h"
#include "LibSystem.h"
#include "LibAws.h"
#include "LibBle.h"
//#include <Arduino_FreeRTOS.h>
#include <TFT_eSPI.h>
#include "Free_Fonts.h"
#include "bt_icon.h"
#include "wifi_icon.h"
#include "cloudCon_icon.h"
#include "btDiscon_icon.h"
#include "wifiDiscon_icon.h"
#include "cloudDiscon_icon.h"
TFT_eSPI tft = TFT_eSPI();


#include <TFT_eWidget.h>               // Widget library
#include <OneWire.h>
#include <DallasTemperature.h>


// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);


// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);



GraphWidget gr = GraphWidget(&tft);    // Graph widget gr instance with pointer to tft
TraceWidget tr = TraceWidget(&gr);     // Graph trace tr with pointer to gr
typedef uint16_t color_t;


typedef enum
{
  DISPLAY_STATUS_OFF,
  DISPLAY_STATUS_NO_CHANGE,
  DISPLAY_STATUS_TOGGLE,
  DISPLAY_STATUS_ON,
} displayStatusCodes_et;


static uint8_t eventWIFIState = DISPLAY_STATUS_OFF;
static uint8_t eventBLEState = DISPLAY_STATUS_OFF;
static uint8_t eventMQTTState = DISPLAY_STATUS_OFF;
bool printStatus = false;


const float gxLow  = 0.0;
const float gxHigh = 255.0;
const float gyLow  = 0.0;
const float gyHigh = 255.0;


const unsigned char ecgRaw[] ={
0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x20, 0x1a, 0x20, 
0x20, 0x1a, 0x1a, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x1a, 0x1a, 0x1a, 0x20, 0x27, 0x2d, 0x27, 0x20, 
0x20, 0x1a, 0x1a, 0x1a, 0x20, 0x20, 0x1a, 0x1a, 
0x20, 0x1a, 0x20, 0x1a, 0x20, 0x27, 0x13, 0x13, 
0x83, 0xff, 0xc4, 0x2d, 0x00, 0x13, 0x0d, 0x13, 
0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 
0x13, 0x13, 0x13, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 
0x1a, 0x1a, 0x20, 0x20, 0x27, 0x27, 0x34, 0x34, 
0x34, 0x34, 0x3a, 0x3a, 0x3a, 0x41, 0x3a, 0x34, 
0x3a, 0x34, 0x2d, 0x2d, 0x27, 0x27, 0x20, 0x20, 
0x1a, 0x13, 0x13, 0x13, 0x13, 0x0d, 0x0d, 0x13, 
0x0d, 0x0d, 0x13, 0x13
};



#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define GREY        0x2108
#define SCALE0      0xC655                                                               // accent color for unused scale segments                                   
#define SCALE1      0x5DEE                                                               // accent color for unused scale segments
#define TEXT_COLOR  0xFFFF
#define gray 0x6B6D                                     // is currently white 


// rainbow scale ring meter color scheme
#define RED2RED     0
#define GREEN2GREEN 1
#define BLUE2BLUE   2
#define BLUE2RED    3
#define GREEN2RED   4
#define RED2GREEN   5


extern const char aws_root_ca_pem[];
extern const char certificate_pem_crt[];
extern const char private_pem_key[];


LibSystem SYS;
LibBle BLE;
Aws AWS;


void app_init();
void app_loop();


void setup()
{
  Serial.begin(115200);
    display_init();
  get_temp();
  app_init();


  AWS.subscribe(TEST_AWS_TOPIC_SUBSCRIBE, 0);


}


void app_task(void *param)
{
  while (1)
  {
    traceGraph();
      if (printStatus == true)
  {
    icon_status();
    printStatus = false;
  }
    delay(5);
  }
}



void app_init()
{
  systemInitConfig_st sysConfig = {0};


  sysConfig.systemEventCallbackHandler = app_eventsCallBackHandler;
  sysConfig.pAppVersionStr = (char *)APP_VERSION;


  sysConfig.s_mqttClientConfig.maxPubMsgToStore_u8 = 5;
  sysConfig.s_mqttClientConfig.maxSubMsgToStore_u8 = 6;
  sysConfig.s_mqttClientConfig.maxSubscribeTopics_u8 = 5;
  sysConfig.s_mqttClientConfig.pRootCaStr = (char *)aws_root_ca_pem;



  if (SYS.init(&sysConfig))
  {
     BLE.start();
    SYS.start();
  }
  else
  {
    while (1)
    {
      printf("\n System Init failed, verify the init config ....");
      delay(5000);
    }
  }


  uint8_t err = xTaskCreate(&app_task, "app_task", TASK_APP_STACK_SIZE, NULL, TASK_APP_PRIORITY, NULL);


  if (pdPASS != err)
  {
    printf("\n Error 0x%X in creating app_task \n restarting system\n\r\n\r", err);
  }
}


void loop()
{
  app_loop();


  delay(1000);
}


void app_loop()
{
  static uint32_t nextPubTime = 0;
  static uint32_t count_u32 = 1;


  switch (SYS.getMode())
  {
    case SYSTEM_MODE_DEVICE_CONFIG:
      if (millis() > nextPubTime)
      {
        nextPubTime = millis() + 5000;


        if (SYS.isDeviceRegistered())
        {
          printf("\nDevice successfully provisioned");
        }
        else
        {
          printf("\nDevice is not provisioned");
        }
      }
      break;


    case SYSTEM_MODE_NORMAL:
      if (AWS.isConnected())
      {
        mqttMsg_st s_mqttMsg = {0};
        if (AWS.read(&s_mqttMsg))
        {
          printf("\n Received: [%s]:%s", s_mqttMsg.topicStr, s_mqttMsg.payloadStr);
        }
        if ((millis() > nextPubTime))
        {
          uint8_t temp = get_temp();
          s_mqttMsg.payloadLen_u16 = sprintf(s_mqttMsg.payloadStr, "{\"temp\": %d}", temp);


          strcpy(s_mqttMsg.topicStr, TEST_AWS_TOPIC_PUBLISH);
          s_mqttMsg.qos_e = QOS0_AT_MOST_ONCE;
          // AWS.publish(&s_mqttMsg);
          nextPubTime = millis() + 5000;
          printf("\n %s\n", s_mqttMsg.payloadStr);
        }
      }
      break;


    case SYSTEM_MODE_OTA:
      break;


    default:
      break;
  }
}


void app_eventsCallBackHandler(systemEvents_et event_e)
{
  switch (event_e)
  {
    case EVENT_WIFI_CONNECTED:
      eventWIFIState = DISPLAY_STATUS_ON;
      printStatus = true;
      printf("\nEVENT_WIFI_CONNECTED");
      break;
    case EVENT_WIFI_DISCONNECTED:
      eventWIFIState = DISPLAY_STATUS_OFF;
      printStatus = true;
      printf("\nEVENT_WIFI_DISCONNECTED");
      break;


    case EVENT_MQTT_CONNECTED:
      eventMQTTState = DISPLAY_STATUS_ON;
      printStatus = true;
      printf("\nEVENT_AWS_CONNECTED");
      break;
    case EVENT_MQTT_DISCONNECTED:
      eventMQTTState = DISPLAY_STATUS_OFF;
      printStatus = true;
      printf("\nEVENT_AWS_DISCONNECTED");
      break;


    case EVENT_BLE_CONNECTED:
      eventBLEState = DISPLAY_STATUS_ON;
      printStatus = true;
      printf("\nEVENT_BLE_CONNECTED");
      break;
    case EVENT_BLE_AUTHENTICATED:
      printf("\nEVENT_BLE_AUTHENTICATED");
      break;
    case EVENT_BLE_DISCONNECTED:
      eventBLEState = DISPLAY_STATUS_OFF;
      printStatus = true;
      printf("\nEVENT_BLE_DISCONNECTED");
      break;


    default:
      break;
  }
}




void display_init()
{
  // Start the DS18B20 sensor
  sensors.begin();
  tft.setSwapBytes(true);
  tft.init ();
  tft.setRotation (3);
  tft.fillScreen (TFT_BLACK);
  tft.setFreeFont(FF1);


  drawHeaderFooter();
  circles();
  writeCircles();
  icon_status();
  //  drawIcons();
  consGraph();
  //  tft.pushImage(360, 2, 25, 25, azureDiscon_icon);
}





void drawHeaderFooter()
{
  //  tft.drawRect  ( 0,   0, 479, 28, TFT_WHITE);
  tft.setTextColor (TFT_WHITE);
  tft.setFreeFont(FF6);
  tft.setTextSize (1);
  tft.setCursor (180, 20);
  tft.print ("AWS IoT");
  tft.drawRect  (  0, 287, 479, 28, TFT_WHITE);
  tft.setCursor (80, 307);
  tft.print ("BuildStorm Technologies");
}



void icon_status()
{


  if (eventBLEState == DISPLAY_STATUS_ON)
  {
    tft.pushImage(395, 2, 25, 25, bt_icon);
  }
  else
  {
    tft.pushImage(395, 2, 25, 25, btDiscon_icon);
  }
  if (eventWIFIState == DISPLAY_STATUS_ON)
  {
    tft.pushImage(455, 2, 25, 25, wifi_icon);
  }
  else
  {
    tft.pushImage(455, 2, 25, 25, wifiDiscon_icon);
  }
  if (eventMQTTState == DISPLAY_STATUS_ON)
  {
    tft.pushImage(425, 4, 25, 25, cloudCon_icon);
  }
  else
  {
    tft.pushImage(425, 4, 25, 25, cloudDiscon_icon);
  }
}


void circles()
{
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FF0);
  tft.setTextSize (0);
  char degree = 176;


  tft.drawCircle (60, 238,  40, TFT_MAGENTA);
  tft.setCursor (55, 207);
  tft.print ("HR");
  tft.setCursor (55, 267);
  tft.print ("bpm");
  tft.drawCircle (180, 238,  40, TFT_YELLOW);
  tft.setCursor (170, 207);
  tft.print ("SpO2");
  tft.setCursor (180, 267);
  tft.print ("%");
  tft.drawCircle (300, 238,  40, TFT_BLUE);
  tft.setCursor (290, 207);
  tft.print ("Resp");
  tft.setCursor (295, 267);
  tft.print ("bpm");
  tft.drawCircle (420, 238,  40, TFT_GREEN);
  tft.setCursor (410, 207);
  tft.print ("Temp");
  tft.setCursor (420, 267);
  tft.print ("F");
  //\xC2
}


void writeCircles()
{
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FF7);
  tft.setTextSize (1);
  tft.setCursor (40, 250);
  tft.setTextColor(TFT_MAGENTA);
  tft.print ("72");
  tft.setCursor (160, 250);
  tft.setTextColor(TFT_YELLOW);
  tft.print ("98");
  tft.setCursor (280, 250);
  tft.setTextColor(TFT_BLUE);
  tft.print ("60");


}


int get_temp()
{
  static uint32_t tempTime = millis();
  static uint8_t prevTempF = 0;
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  uint8_t temperatureF = sensors.getTempFByIndex(0);


  Serial.println("Temperature in ºC   ");
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.println("Temperature in ºF    ");
  Serial.print(temperatureF);
  Serial.println("ºF");
  if (temperatureF > prevTempF || temperatureF < prevTempF)
  {
    tft.fillRect(400, 220, 40, 47, TFT_BLACK);
    tft.setCursor(400, 250);
    tft.setFreeFont(FF7);
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN);
    tft.print(temperatureF);
    prevTempF = temperatureF;
  }
  return temperatureF;
}



void consGraph()
{
  gr.createGraph(478, 150, TFT_BLACK);



  // x scale units is from 0 to 100, y scale units is -512 to 512
  gr.setGraphScale(gxLow, gxHigh, gyLow, gyHigh);


  // X grid starts at 0 with lines every 20 x-scale units
  // Y grid starts at -512 with lines every 64 y-scale units
  // blue grid
  gr.setGraphGrid(gxLow, 20.0, gyLow, 40.0, TFT_BLACK);


  // Draw empty graph, top left corner at pixel coordinate 40,10 on TFT
  gr.drawGraph(2, 35);
  //  tft.drawRect(1, 65, 479, 118, TFT_WHITE);
  tft.drawLine(1, 35, 1, 186, gray);
  tft.drawLine(1, 186, 480, 186, gray);

  // Start a new trace with using white
  tr.startTrace(TFT_RED);
}


void traceGraph()
{
  static uint32_t nextPlotTime = millis()+500;
  static float gx = 0.0, gy = 50.0;
  static float delta = 0.1;
  static float amplitude = 20.0;
  static uint16_t ecgIndex = 0;
  float heartbeat=1.0;
  // Create a new plot point every 50ms (increased speed)
  if ((millis() > nextPlotTime)) {
    nextPlotTime = millis()+5;


    // Calculate the y-coordinate using a combination of a sine function and random variation
    // float heartbeat = amplitude * (0.5 * sin(gx * 0.1) + 0.5 * random(-10, 10));
    
    heartbeat = ecgRaw[ecgIndex++];
    if(ecgIndex>=99)
    {
      ecgIndex = 0;
    }
    // Add a plot point
    tr.addPoint(gx, heartbeat);
    gx += 1.0;


    // If the end of the graph x-axis is reached, start a new trace at 0.0, 0.0
    if (gx > gxHigh) {
      gx = 0.0;
      // gy = 50.0;


      // Draw an empty graph at 40,10 on the display to clear the old one
      gr.drawGraph(2, 35);
      //      tft.drawRect(1, 65, 479, 118, TFT_WHITE);
      // Start a new trace with a green color
      tr.startTrace(TFT_RED);
    }
  }
}

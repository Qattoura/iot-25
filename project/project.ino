////includes
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <NTPClient.h>
#include "ESPAsyncWebServer.h"
#include "DHT.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>
////defines
#define DHTTYPE DHT11 // DHT 11
#define SD_CS 5
#define LED_PIN 2
#define LED_EXTRNL 14
#define LIGHT_SENSOR_PIN 34
#define SMOKE_PIN 32
#define warning_PIN 25
uint8_t DHTPin = 4;



#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "******@gmail.com"
#define AUTHOR_PASSWORD "**** **** **** ****"
#define RECIPIENT_EMAIL "******@gmail.com"

float Temperature;
float Humidity;
String formattedDate;
String dayStamp;
String timeStamp;
bool warning = true;
bool threshould = true;
// Replace with your network credentials
const char* ssid = "ssid";
const char* password = "passowrd";
//////////////
// Functions declearation
void sd_reading_setup();
void Read_TempHum();
void logSDCard();
void getTimeStamp();
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
String processor(const String& var);
String readDHTTemperature();
String readDHTHumidity();
void web_server_setup();
void warning_mail();
void smtpCallback(SMTP_Status status);
//////////////////
AsyncWebServer server(80);
String dataMessage;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

Servo window;
SMTPSession smtp;
DHT dht(DHTPin, DHTTYPE);



// WebPage (HTML , CSS & JS)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
      font-family: Arial;
      display: inline-block;
      margin: 0px auto;
      text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align: middle;
      padding-bottom: 15px;
    }
    .alert {
      color: white;
      background-color: red;
      padding: 10px;
      font-size: 1.5rem;
      display: none;
      margin-top: 20px;
    }
  </style>
</head>
<body>
  <h2 style="color: rgb(55, 129, 189);"><span style="color: rgb(194, 70, 138);">Flona</span> 1.0.3 Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature: </span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i>
    <span class="dht-labels">Humidity: </span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <div class="alert" id="alert">
    WARNING: Critical Alert Detected (Fiire)!
  </div>
  <div class="alert" id="hot-alert">
    WARNING: The weather is too hot!
  </div>
</body>
<script>
setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      let warningState = this.responseText;
      if (warningState === "false") {
        document.getElementById("alert").style.display = "block";
      } else {
        document.getElementById("alert").style.display = "none";
      }
    }
  };
  xhttp.open("GET", "/warning", true);
  xhttp.send();
}, 2000);

setInterval(function () {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      let threshouldState = this.responseText;
      if (threshouldState === "false") {
        document.getElementById("hot-alert").style.display = "block";
      } else {
        document.getElementById("hot-alert").style.display = "none";
      }
    }
  };
  xhttp.open("GET", "/threshould", true);
  xhttp.send();
}, 2000);
</script>
</html>)rawliteral";




void setup() {
///////////////////////
// Serial & pinModes
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DHTPin, INPUT);
  pinMode(LIGHT_SENSOR_PIN,INPUT);
  pinMode(SMOKE_PIN,INPUT);
  // pinMode(warning_PIN,INPUT);
  pinMode(LED_EXTRNL,OUTPUT);
/////////////////////////////////
  dht.begin();
  sd_reading_setup();
  web_server_setup();
  // Servo
  window.attach(26);
  window.write(180);

}
void loop() {

  digitalWrite(LED_PIN, HIGH);

  Read_TempHum();
  getTimeStamp();
  logSDCard();
  digitalWrite(LED_PIN, LOW);


  delay(5000); //Wait for 5 seconds before writing the next data 

// Light Sensor  
  int adc_value = analogRead(LIGHT_SENSOR_PIN);
  if (adc_value < 2600)
  {
    digitalWrite(LED_EXTRNL, LOW);
  }
  else{
    digitalWrite(LED_EXTRNL, HIGH);
  }
  Serial.println(adc_value);

// Smoke Sensor
  int smoke_value = analogRead(SMOKE_PIN);

  Serial.println(smoke_value);

  if (smoke_value > 700 && warning )
  {
    warning = false;
    // digitalWrite(warning_PIN, HIGH);
    warning_mail();
    Serial.println("llllllllllllllllllllllllllllllll");
  }


  if (Temperature > 24) {
    threshould = false;
      window.write(90);
    Serial.println("22222222222");
  } else {
      window.write(180);
    threshould = true;
  }


}
// Function to get temperature
void Read_TempHum(){
  Temperature = dht.readTemperature(); 
  Humidity = dht.readHumidity(); 
  Serial.print("Temperature = ");
  Serial.println(Temperature);

}
// Function to setup web server
void web_server_setup() {
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });

  server.on("/warning", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", warning ? "true" : "false");
  });

  server.on("/threshould", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send_P(200, "text/plain", threshould ? "true" : "false");
  });

  // Start server
  server.begin();
}







void smtpCallback(SMTP_Status status) {

  if (status.success()) {
    smtp.sendingResult.clear();
  }
}

// Function to setup sd card
void sd_reading_setup()
{


  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(7200);
  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    return;
  }
  if (!SD.begin(SD_CS)) {
    return;    // init failed
  }
  File file = SD.open("/data.txt");
  if(!file) {
    writeFile(SD, "/data.txt", "Date, Time, Temperature, Humidity \r\n");
  }
  else {
     Serial.println("the file is already created");
  }
  file.close();
}

// Function to get date and time from NTPClient
void getTimeStamp() {

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
}
// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage =  String(dayStamp) + "," + String(timeStamp) + "," + 
                String(Temperature) + "," + String(Humidity)+ "\r\n";
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    return;
  }
  if(file.print(message)) {
  } else {
  }
  file.close();
}
// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY"){
    return readDHTHumidity();
  }
  return String();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    return;
  }
  if(file.print(message)) {
  } else {
  }
  file.close();
}

String readDHTTemperature() {

  float t = dht.readTemperature();

  if (isnan(t)) {
    return "--";
  }
  else {
    return String(t);
  }
}

String readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h)) {
    return "--";
  }
  else {
    return String(h);
  }
}
// Function to send Mail
void warning_mail(){
  ////////////////////////////////////////
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(300);
  }
  smtp.debug(1);
  smtp.callback(smtpCallback);
  Session_Config config;
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 7200;
  SMTP_Message message;
  message.sender.name = F("Flona");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("Flona Alert Email");
  message.addRecipient(F("Recipient"), RECIPIENT_EMAIL);
  message.text.content = "FIIRE in HOME!!! - Sent from Flona 1.0.3";
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  if (!smtp.connect(&config)) {
  
  return;
  }
  if (!MailClient.sendMail(&smtp, &message))
  {
    Serial.println(" Send mail faild");
  }


}
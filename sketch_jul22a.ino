#define BLINKER_WIFI
// #define BLINKER_WITHOUT_SSL // WiFi without ssl 主要用于堆栈不足的设备使用非加密方式接入

#include <Blinker.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <IRremote.hpp>


// ------------------- Macro definitions ----------------------------------------------------

#define BAUD_RATE 115200
#define PORT_RELAY D1
#define IRpin D2
#define PORT_L_SERVO D7
#define PORT_R_SERVO D8
#define Reset D3


// ----------------- Global variables ------------------------------------------------------------

void TurnOnLeftLight();
void TurnOffLeftLight();
void TurnOnRightLight();
void TurnOffRightLight();

// ESP8266WiFiMulti wifiMulti;
ESP8266WebServer esp8266_server(80);
Servo l_servo, r_servo;

char auth[] = "71ce3cf0e713";   // blinker密钥
char ssid[] = "sdu_no_net";
char pswd[] = "xiaomi.com";


BlinkerButton Light_all("btn-light");
BlinkerButton Light_left("btn-light-left");
BlinkerButton Light_right("btn-light-right");

BlinkerButton AC1("btn-AC1");
BlinkerButton AC2("btn-AC2");
BlinkerButton AC3("btn-AC3");
BlinkerButton AC4("btn-AC4");
BlinkerButton ACOFF("btn-ACOFF");
BlinkerButton ACNum("btn-ACNum");
BlinkerButton MCU_Reset("btn-reset");


bool left_light_on = true, right_light_on = true;



// ----------------- IR code for air conditioner --------------------------------

// 关机
uint16_t turnOff_AC[343] = {9030,4470, 580,1670, 530,1670, 580,470, 630,470, 580,520, 580,520, 580,520, 580,1670, 580,470, 630,1670, 530,1670, 580,470, 630,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,1670, 580,520, 580,470, 630,470, 630,470, 630,470, 580,520, 580,1670, 580,520, 580,520, 580,520, 580,1670, 530,1670, 580,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,7920, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 530,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1620, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 580,1620, 580,520, 580,520, 580,520, 580,1670, 530,1670, 580,7970, 530,520, 580,570, 530,570, 530,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 530,570, 530,1670, 530,570, 530,570, 530,1720, 530,520, 580,520, 580,520, 530,570, 580,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,520, 580,520, 580,520, 580,1670, 530,570, 530,570, 530,1720, 480,570, 530,1720, 530,520, 580,1670, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,1720, 480,570, 530,570, 530,570, 530};  // Protocol=UNKNOWN Hash=0xCB072BC6 172 bits (incl. gap and start) received
// 开机 模式：制冷 温度：24°C 风速：自动
uint16_t turnUp_24C_cold[343] = {9030,4470, 580,1670, 530,1670, 580,470, 630,470, 630,470, 580,520, 580,520, 580,1670, 580,520, 580,1670, 530,1670, 580,520, 580,470, 630,470, 630,470, 630,470, 580,520, 580,520, 580,1670, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,1620, 580,520, 580,520, 580,520, 580,1670, 530,1670, 580,520, 580,470, 630,470, 630,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,7920, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1620, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 530,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,570, 530,570, 530,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 530,570, 530,570, 530,570, 530,570, 530,570, 530,520, 530,570, 580,520, 580,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,1670, 580,1670, 530,570, 530,570, 530,570, 530,1670, 530,1720, 530,7970, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,1670, 530,570, 530,1720, 530,570, 530,570, 480,1720, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 480,620, 480,620, 480,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 480,620, 480,620, 480,1770, 480,1720, 480,620, 480,620, 480,1770, 480,620, 480,1720, 480,620, 480,1770, 430,620, 480,620, 480,620, 480,620, 480,620, 480,620, 480,620, 480,620, 480,620, 480,620, 480,620, 480};  // Protocol=UNKNOWN Hash=0x1D7DD34C 172 bits (incl. gap and start) received
// 数显 模式：制冷 温度：24°C 风速：自动
uint16_t LED_switch[343] = {9030,4470, 580,1670, 530,1670, 580,520, 580,520, 580,470, 630,470, 630,470, 630,1620, 580,520, 580,1670, 530,1670, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 580,470, 630,470, 630,470, 580,1670, 580,1670, 530,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 630,7920, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,1620, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 530,1670, 580,520, 530,570, 530,570, 530,570, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,1670, 530,570, 530,570, 530,570, 530,570, 530,1720, 530,1670, 530,8020, 480,570, 530,1720, 530,520, 580,520, 580,570, 530,570, 530,570, 530,570, 480,620, 480,1720, 530,1720, 480,1720, 530,570, 530,570, 530,570, 530,570, 530,570, 480,620, 480,620, 480,620, 480,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 480,620, 480,620, 480,620, 480,620, 480,620, 480,620, 480,1770, 480,1720, 480,620, 480,620, 480,1720, 480,620, 480,1770, 480,620, 480,1720, 480,620, 480,620, 480,620, 480,1770, 430,620, 480,620, 480,1770, 480,620, 480,1720, 480,620, 480,620, 480};  // Protocol=UNKNOWN Hash=0xF131B3DE 172 bits (incl. gap and start) received
// 开机 模式：制冷 温度：23°C 风速：自动
uint16_t turnUp_23C_cold[343] = {9030,4470, 580,1670, 530,1670, 580,520, 580,520, 580,520, 580,520, 580,470, 630,1620, 580,520, 580,1670, 530,1670, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 580,470, 630,470, 630,1620, 580,470, 630,1670, 530,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 580,7970, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,1620, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 580,1670, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 530,520, 580,520, 580,1670, 580,1620, 580,1670, 530,1670, 580,7970, 530,570, 530,570, 530,570, 530,570, 530,520, 580,520, 530,570, 580,520, 530,570, 530,1720, 530,570, 530,570, 530,570, 530,570, 530,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,520, 580,520, 580,520, 580,1670, 530,1720, 530,520, 580,520, 530,1720, 530,570, 530,1670, 530,570, 530,1720, 530,570, 530,570, 530,570, 480,1720, 530,1720, 480,1720, 530,570, 530,570, 530,1670, 530,570, 530,570, 530};  // Protocol=UNKNOWN Hash=0x579A3FD4 172 bits (incl. gap and start) received
// 开机 模式：制热 温度：27°C 风速：自动
uint16_t turnUp_27C_hot[343] = {9030,4470, 580,1670, 530,1670, 580,520, 530,570, 530,570, 530,570, 530,570, 530,1720, 530,570, 530,1670, 530,1720, 530,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,520, 580,570, 530,570, 530,570, 530,570, 530,570, 530,1670, 530,570, 530,570, 530,1720, 530,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,7970, 580,520, 580,520, 580,470, 630,470, 630,470, 630,470, 580,520, 580,1670, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 580,520, 580,520, 580,1670, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 480,1720, 530,570, 530,570, 530,7970, 580,520, 580,520, 580,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,1670, 580,520, 580,520, 580,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,520, 580,570, 530,520, 580,570, 530,570, 530,570, 530,570, 480,620, 480,1720, 530,1670, 530,570, 530,570, 530,1720, 530,570, 530,1670, 530,570, 530,1720, 480,570, 530,570, 530,570, 530,1720, 530,1670, 530,1720, 530,570, 480,620, 480,1720, 530,570, 530,570, 480};  // Protocol=UNKNOWN Hash=0xE06ECD33 172 bits (incl. gap and start) received
// 开机 模式：制热 温度：28°C 风速：自动
uint16_t turnUp_28C_hot[343] = {9030,4470, 580,1620, 580,1670, 530,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 580,520, 580,1670, 530,1670, 580,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 530,520, 580,1670, 580,470, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,7970, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,1670, 530,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,470, 630,470, 630,470, 630,470, 630,470, 580,520, 580,520, 630,470, 630,470, 630,470, 580,520, 580,520, 580,520, 580,520, 580,1620, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,7970, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,1670, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 480,620, 480,570, 580,520, 530,570, 530,570, 580,520, 580,520, 580,520, 530,570, 530,570, 530,570, 530,570, 530,570, 530,570, 530,1720, 480,1720, 530,570, 530,570, 530,1720, 480,620, 480,1720, 530,570, 530,1670, 530,570, 530,570, 530,570, 530,1720, 480,1720, 530,1720, 480,620, 480,570, 530,1720, 530,570, 530,570, 530};  // Protocol=UNKNOWN Hash=0xB771F341 172 bits (incl. gap and start) received



// ------------------------ set up ----------------------------------------------------------------

void setup(void)
{
    Serial.begin(BAUD_RATE);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PORT_RELAY, OUTPUT);
    pinMode(Reset, OUTPUT);
    digitalWrite(PORT_RELAY, HIGH);

    l_servo.attach(PORT_L_SERVO);
    r_servo.attach(PORT_R_SERVO);
    l_servo.write(90);
    r_servo.write(90);
    wifiMulti.addAP(ssid, pswd);
    Serial.println("Connecting ...");

    int i = 0;
    while (wifiMulti.run() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(i++);
        Serial.print(' ');
    }
    Serial.println('\n');

    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    esp8266_server.begin();
    esp8266_server.on("/", HTTP_GET, handleRoot);
    //  esp8266_server.on("/LeftLightOn", HTTP_POST, LightOnLeft);
    //  esp8266_server.on("/LeftLightOff", HTTP_POST, LightOffLeft);
    //  esp8266_server.on("/RightLightOn", HTTP_POST, LightOnRight);
    //  esp8266_server.on("/RightLightOn", HTTP_POST, LightOffRight);
    esp8266_server.on("/TurnOnLight", HTTP_POST, TurnOnLight);
    esp8266_server.on("/TurnOffLight", HTTP_POST, TurnOffLight);
    esp8266_server.on("/TurnOnLight", HTTP_GET, TurnOnLight);
    esp8266_server.on("/TurnOffLight", HTTP_GET, TurnOffLight);
    esp8266_server.onNotFound(handleNotFound);
    Serial.println("HTTP esp8266_server started");

    IrSender.begin(IRpin, DISABLE_LED_FEEDBACK);


    // ------------------------------ 初始化blinker ------------------------------ 
    Blinker.begin(auth, ssid, pswd);
    Serial.println("Blinker started");


    Light_all.attach(Light_all_callback);
    Light_left.attach(Light_left_callback);
    Light_right.attach(Light_right_callback);
    
    AC1.attach(AC1_callback);
    AC2.attach(AC2_callback);
    AC3.attach(AC3_callback);
    AC4.attach(AC4_callback);
    ACOFF.attach(ACOFF_callback);
    ACNum.attach(ACNum_callback);
    MCU_Reset.attach(Reset_callback);


    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
    }

    digitalWrite(PORT_RELAY, LOW);
}

// ------------------------------------------- loop --------------------------------------------

void loop(void)
{
    esp8266_server.handleClient();
    Blinker.run();
}

// ------------------------------------------ web server --------------------------------------------

void handleRoot()
{
    // String content = "<form action=\"/Light\" method=\"POST\"><input type=\"submit\" value=\"Toggle Light\"></form>";
    // content += "<form action=\"/Light\" method=\"POST\"><input type=\"submit\" value=\"Toggle Light\"></form>";
    String str = "<form action=\"/TurnOnLight\" method=\"POST\"><input type=\"submit\" value=\"Turn On\" style=\"width: 40%; height: 20%\"></form>";
    str += "<form action=\"/TurnOffLight\" method=\"POST\"><input type=\"submit\" value=\"Turn Off\" style=\"width: 40%; height: 20%\"></form>";
    esp8266_server.send(200, "text/html", str);
}

void TurnOnLeftLight()
{
    digitalWrite(PORT_RELAY, HIGH);
    l_servo.write(120);
    delay(300);
    l_servo.write(90);
    delay(300);
    
    left_light_on = true;
}

void TurnOffLeftLight()
{
    digitalWrite(PORT_RELAY, HIGH);
    l_servo.write(57);
    delay(300);
    l_servo.write(90);
    delay(300);
    digitalWrite(PORT_RELAY, LOW);

    left_light_on = false;
}

void TurnOnRightLight()
{
    digitalWrite(PORT_RELAY, HIGH);
    r_servo.write(60);
    delay(300);
    r_servo.write(90);
    delay(300);
    digitalWrite(PORT_RELAY, LOW);

    right_light_on = true;
}

void TrunOffRightLight()
{
    digitalWrite(PORT_RELAY, HIGH);
    r_servo.write(130);
    delay(300);
    r_servo.write(90);
    delay(300);
    digitalWrite(PORT_RELAY, LOW);

    right_light_on = false;
}

void TurnOnLight()
{
    TurnOnLeftLight();
    TurnOnRightLight();

    esp8266_server.sendHeader("Location", "/");
    esp8266_server.send(303);
}

void TurnOffLight()
{
    TurnOffLeftLight();
    TrunOffRightLight();

    esp8266_server.sendHeader("Location", "/");
    esp8266_server.send(303);
}

// 404
void handleNotFound()
{
    esp8266_server.send(404, "text/plain", "404: Not found");
}

void TurnOnAC_Cold24()
{
    IrSender.sendRaw(turnUp_24C_cold, 343, 38);
}

void TurnOnAC_Cold23()
{
    IrSender.sendRaw(turnUp_23C_cold, 343, 38);
}

void TurnOnAC_Hot27()
{
    IrSender.sendRaw(turnUp_27C_hot, 343, 38);
}

void TurnOnAC_Hot28()
{
    IrSender.sendRaw(turnUp_28C_hot, 343, 38);
}

void TurnOffAC()
{
    IrSender.sendRaw(turnOff_AC, 343, 38);
}

void SwitchACNum()
{
    IrSender.sendRaw(LED_switch, 343, 38);
}

// -------------------------------------------------- blinker ---------------------------------------------

void Reset_callback(const String &state)
{
    digitalWrite(Reset, HIGH);
}

void Light_all_callback(const String &state)
{
    Blinker.vibrate();
    if (left_light_on)  TurnOffLeftLight();
    else    TurnOnLeftLight();

    if(right_light_on)  TrunOffRightLight();
    else    TurnOnRightLight();

    BLINKER_LOG("Here is a log.");
}

void Light_left_callback(const String &state)
{
    Blinker.vibrate();
    if (left_light_on)  TurnOffLeftLight();
    else    TurnOnLeftLight();
}

void Light_right_callback(const String &state)
{
    Blinker.vibrate();
    if(right_light_on)  TrunOffRightLight();
    else    TurnOnRightLight();
}

void AC1_callback(const String &state)
{
    TurnOnAC_Cold24();
    Blinker.vibrate();
}

void AC2_callback(const String &state)
{
    TurnOnAC_Cold23();
    Blinker.vibrate();
}

void AC3_callback(const String &state)
{
    TurnOnAC_Hot27();
    Blinker.vibrate();
}

void AC4_callback(const String &state)
{
    TurnOnAC_Hot28();
    Blinker.vibrate();
}

void ACOFF_callback(const String &state)
{
    TurnOffAC();
    Blinker.vibrate();
}

void ACNum_callback(const String &state)
{
    Blinker.vibrate();
    SwitchACNum();
}

// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String &data)
{
    BLINKER_LOG("Blinker readString: ", data);
}

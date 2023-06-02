#include "time.h"
#include <WiFi.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 23
#define DHTTYPE DHT11
#define TEMP 27

#define BOTON_MINUTOS 34
#define BOTON_HORAS 35

#define SET 0
#define FIN 1

#define PANTALLA1 0
#define DEFINIR_HORA 1
#define DEFINIR_TIEMPO_REG 2
DHT dht(DHTPIN, DHTTYPE);

float temperaturaActual;
int funcionamiento;
float RELOJ;
float temperaturaAntes;

int lecturaBOTON_MINUTOS;
int lecturaBOTON_HORAS;

int estadoBoton;
int estadoBOTON_MINUTOS;
int estadoBOTON_HORAS;
int cambioEstados;
int definirRELOJ;
int flagTemperatura;


int ms;
int seg;
int MINUTOS;
int HORAS;

int reg = 0;
int riego = reg;
int hora, minuto ;

#define LedPin 25
int Regcuento = 0;
int Regcuento2 = 0;




#include <ESP32Time.h>
//**********************************************************//
//* DECLARO DE FUN                                        **//
//**********************************************************//
void pedir_lahora(void);
void setup_rtc_ntp(void);
void hora_real(void);
void led_riego(void);

struct tm timeinfo;
ESP32Time rtc;


/// time
long unsigned int timestamp; // hora
const char *ntpServer = "south-america.pool.ntp.org";
const long gmtOffset_sec = -10800;
const int daylightOffset_sec = 0;

const char* ssid = "ORT-IoT";
const char* password = "OrtIOTnew22$2";















void setup() {

  initWiFi();
  setup_rtc_ntp();


  pinMode(BOTON_MINUTOS, INPUT_PULLUP);
  pinMode(BOTON_HORAS, INPUT_PULLUP);
  pinMode(LedPin, OUTPUT);
  pinMode(TEMP, INPUT);

  Serial.begin(115200);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  ms = millis();
}

void loop() {
  hora_real();
  led_riego();
  temperaturaAntes = temperaturaActual;
  temperaturaActual = dht.readTemperature();

  if (flagTemperatura == 0) {
    temperaturaAntes = temperaturaActual;
    flagTemperatura = 1;
  }

  lecturaBOTON_MINUTOS = digitalRead(BOTON_MINUTOS);
  lecturaBOTON_HORAS = digitalRead(BOTON_HORAS);

  switch (funcionamiento) {

    case PANTALLA1:

      if (temperaturaAntes != temperaturaActual) {
        display.clearDisplay();
      }

      display.setCursor(0, 25);
      display.print("Temp actual: ");
      display.println(temperaturaActual);
      display.display();

      /*  if (millis() - ms  >= 1000) {
          seg = seg + 1;
          ms = millis();
        }

        if (seg >= 60) {
          display.clearDisplay();
          MINUTOS = MINUTOS + 1;
          seg = 0;
        }

        if (MINUTOS >= 60) {
          display.clearDisplay();
          HORAS = HORAS + 1;
          MINUTOS = 0;
        }

        if (HORAS >= 24) {
          HORAS = 0;
        }
      */
      if (definirRELOJ == SET) {
        display.setCursor(0, 16);
        display.print("CICLO: ");
        display.print(HORAS);
        display.print(":");
        display.print(MINUTOS);
        display.setCursor(0, 45);
        display.println("tiempo riego: ");
        display.print(riego);
        display.display();
      }

      else {
        display.setCursor(0, 16);
        display.print("CICLO: ");
        display.print(HORAS);
        display.print(":");
        display.print(MINUTOS);
        display.setCursor(0, 45);
        display.print("tiempo riego: ");
        display.print(riego);
        display.display();
      }

      if (lecturaBOTON_MINUTOS == LOW && lecturaBOTON_HORAS == LOW ) {
        cambioEstados = HIGH;
      }

      if (lecturaBOTON_MINUTOS == HIGH && lecturaBOTON_HORAS == HIGH && cambioEstados == HIGH) {
        cambioEstados = LOW;
        funcionamiento = 1;
      }

      break;

    case DEFINIR_HORA:

      definirRELOJ = FIN;

      display.clearDisplay();

      display.setCursor(0, 25);
      display.println("CICLO ACTUAL: ");
      display.print(HORAS);
      display.print(":");
      display.print(MINUTOS);
      display.display();


      if (lecturaBOTON_MINUTOS == LOW && lecturaBOTON_HORAS == LOW ) {
        cambioEstados = HIGH;
      }

      if (lecturaBOTON_MINUTOS == HIGH && lecturaBOTON_HORAS == HIGH && cambioEstados == HIGH) {
        cambioEstados = LOW;
        funcionamiento = 2;
        display.clearDisplay();
        break;

      }

      if (lecturaBOTON_MINUTOS == LOW && lecturaBOTON_HORAS == HIGH && cambioEstados == LOW) {
        estadoBOTON_MINUTOS = HIGH;
      }

      if (lecturaBOTON_MINUTOS == HIGH && estadoBOTON_MINUTOS == HIGH && cambioEstados == LOW) {
        MINUTOS += 1;
        estadoBOTON_MINUTOS = LOW;
      }

      if (lecturaBOTON_HORAS == LOW && lecturaBOTON_MINUTOS == HIGH && cambioEstados == LOW) {
        estadoBOTON_HORAS = HIGH;
      }

      if (lecturaBOTON_HORAS == HIGH && estadoBOTON_HORAS == HIGH && cambioEstados == LOW) {
        HORAS += 1;
        estadoBOTON_HORAS = LOW;
      }

      if (HORAS == 24) {
        HORAS = 0;
      }

      if (MINUTOS == 60) {
        MINUTOS = 0;
        HORAS = HORAS + 1;
      }


      break;









    case DEFINIR_TIEMPO_REG:
      display.setCursor(0, 25);
      display.println("tiempo reg: ");
      display.display();

      if (lecturaBOTON_MINUTOS == LOW && lecturaBOTON_HORAS == LOW ) {
        cambioEstados = HIGH;
      }

      if (lecturaBOTON_MINUTOS == HIGH && lecturaBOTON_HORAS == HIGH && cambioEstados == HIGH) {
        cambioEstados = LOW;
        funcionamiento = 0;
        display.clearDisplay();
        break;

      }


      if (lecturaBOTON_MINUTOS == LOW && lecturaBOTON_HORAS == HIGH && cambioEstados == LOW) {
        estadoBOTON_MINUTOS = HIGH;
      }

      if (lecturaBOTON_MINUTOS == HIGH && estadoBOTON_MINUTOS == HIGH && cambioEstados == LOW) {
        reg = reg + 30;
        estadoBOTON_MINUTOS = LOW;
      }

      if (lecturaBOTON_HORAS == LOW && lecturaBOTON_MINUTOS == HIGH && cambioEstados == LOW) {
        estadoBOTON_HORAS = HIGH;
      }

      if (lecturaBOTON_HORAS == HIGH && estadoBOTON_HORAS == HIGH && cambioEstados == LOW) {
        reg = reg - 30;
        estadoBOTON_HORAS = LOW;
      }
      if (reg < 0)
      {
        reg = 0;
      }
      riego = reg;

      display.clearDisplay();
      display.setCursor(0, 25);
      display.println("tiempo reg: ");
      display.print(reg);
      display.display();
      seg = 0;




      break;
  }
}




























////////////////////////////////////////////
// Initialize WiFi
void initWiFi() {
  WiFi.begin(ssid , password );
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}


//**********************************************************//
//* SETUP  rtc                                           **//
//**********************************************************//
void setup_rtc_ntp(void)
{
  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  timestamp = time(NULL);
  rtc.setTime(timestamp + gmtOffset_sec);
}

//**********************************************************//
//* PIDE LA HORA AL SERVER O AL ESP32                     **//
//**********************************************************//
void pedir_lahora(void)
{
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("veo la hora del rtc interno ");
    timestamp = rtc.getEpoch() - gmtOffset_sec;
    timeinfo = rtc.getTimeStruct();
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
  else
  {
    Serial.print("NTP Time:");
    timestamp = time(NULL);
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }

  return;
}


void hora_real()
{
  pedir_lahora();


  hora = timeinfo.tm_hour;
  minuto = timeinfo.tm_min;

}

void led_riego()
{

  if (HORAS == hora && MINUTOS == minuto)
  {

    digitalWrite(LedPin, HIGH);

    Regcuento = 1;
  }

  if (Regcuento == 1)
  {
    display.clearDisplay();
    riego = riego - seg;
    seg = 0;

    Regcuento2 = 1;

  }


  if ( Regcuento2 == 1 && millis() - ms  >= 1000) {

    seg = seg + 1;
    ms = millis();


  }

  if (riego <= 0) {
    digitalWrite(LedPin, LOW);
    Regcuento = 0;
    Regcuento2 = 0;

  }

}

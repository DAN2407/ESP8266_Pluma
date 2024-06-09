#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Servo.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "CLARO_531CE"
#define WLAN_PASS       "5348PCPWFC"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Dan240701"
#define AIO_KEY         "aio_LINh19XoUwcJDzo0Rg88NfFWVyfz"

// Cliente WiFi
WiFiClient client;

// Cliente MQTT
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Feeds de Adafruit IO
Adafruit_MQTT_Subscribe controlPlumaFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

// Servo
Servo servoMotor;
const int servoPin = D1; // Pin donde está conectado el servo
const int openPosition = 90; // Ángulo para abrir la pluma
const int closedPosition = 0; // Ángulo para cerrar la pluma

void setup() {
  Serial.begin(115200);
  servoMotor.attach(servoPin);
  
  // Conexión a WiFi
  connectToWiFi();
  
  // Configuración de MQTT
  mqtt.subscribe(&controlPlumaFeed);
  
  // Inicialmente, cerramos la pluma
  servoMotor.write(closedPosition);
}

void loop() {
  // Conectar a MQTT
  MQTT_connect();
  
  // Verificar mensajes de Adafruit IO
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &controlPlumaFeed) {
      Serial.print(F("Recepción del feed control_pluma: "));
      Serial.println((char *)controlPlumaFeed.lastread);
      int command = atoi((char *)controlPlumaFeed.lastread);
      if (command == 1) {
        abrirPluma();
        delay(5000); // Mantener la pluma abierta por 5 segundos
        cerrarPluma();
      }
    }
  }
}

void abrirPluma() {
  Serial.println("Abriendo pluma...");
  servoMotor.write(openPosition);
}

void cerrarPluma() {
  Serial.println("Cerrando pluma...");
  servoMotor.write(closedPosition);
}

void MQTT_connect() {
int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;}
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0) {       // basically die and wait for WDT to reset me
         while (1);}}
  Serial.println("MQTT Connected!");
}

void connectToWiFi() {
 Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

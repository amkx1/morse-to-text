#include <WiFi.h>
#include <HTTPClient.h>

#define BUTTON_PIN 18
#define LED_PIN 2

const char *ssid = "";
const char *password = "";

const char *serverURL = "http://192.168.0.181:5000/morse";
const char *clearURL = "http://192.168.0.181:5000/clear";

const unsigned long DASH_THRESHOLD = 400;
const unsigned long LETTER_GAP = 1000;
const unsigned long WORD_GAP = 2500;

bool lastButtonState = HIGH;

unsigned long pressStart = 0;
unsigned long lastRelease = 0;

String currentMorse = "";

bool letterFinalized = false;

// Send Input to server

void sendMorse(String morse, String type)
{

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wi-Fi disconnected!");
    return;
  }

  HTTPClient http;

  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");

  String json = "{\"morse\":\"" + morse +
                "\",\"type\":\"" + type + "\"}";

  int responseCode = http.POST(json);

  Serial.print("Sent: ");
  Serial.print(morse);
  Serial.print(" | Server: ");
  Serial.println(responseCode);

  http.end();
}

// clear
void clearMessage()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    return;
  }

  HTTPClient http;
  http.begin(clearURL);
  int responseCode = http.POST("");

  Serial.print("Clear response: ");
  Serial.println(responseCode);

  http.end();
}

void setup()
{

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

  Serial.println();
  Serial.println("Connecting to Wi-Fi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  Serial.println("Morse input ready!");
}

void loop()
{
  bool buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && buttonState == LOW)
  {
    pressStart = millis();
    digitalWrite(LED_PIN, HIGH);
  }

  if (lastButtonState == LOW && buttonState == HIGH)
  {

    unsigned long pressDuration = millis() - pressStart;
    digitalWrite(LED_PIN, LOW);

    if (pressDuration < DASH_THRESHOLD)
    {
      currentMorse += ".";
      Serial.print(".");
    }
    else
    {
      currentMorse += "-";
      Serial.print("-");
    }

    lastRelease = millis();
    letterFinalized = false;
  }

  // gap detection
  if (buttonState == HIGH && lastRelease > 0)
  {
    unsigned long gap = millis() - lastRelease;

    // Word Gap
    if (gap >= WORD_GAP)
    {

      if (letterFinalized)
      {
        Serial.println("  [WORD SPACE]");
        sendMorse("", "space");
        letterFinalized = false;
        lastRelease = 0;
      }
    }

    // Letter gap

    else if (gap >= LETTER_GAP && !letterFinalized)
    {
      if (currentMorse.length() > 0)
      {
        Serial.print("  [LETTER] ");
        sendMorse(currentMorse, "letter");
        currentMorse = "";
        letterFinalized = true;
      }
    }
  }

  lastButtonState = buttonState;

  delay(10);
}
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

WebServer server(80);

const char* ssid = "Topcon Laser";
const char* password = "12345678";

// Global transition counters — updated after each capture
int transitions_UP4 = 0;
int transitions_UP3 = 0;
int transitions_UP2 = 0;
int transitions_UP1 = 0;
int transitions_CEN = 0;
int transitions_DN1 = 0;
int transitions_DN2 = 0;
int transitions_DN3 = 0;
int transitions_DN4 = 0;
int transitions_ARR = 0;

// ─────────────────────────────────────────────
// Pin definitions
// ─────────────────────────────────────────────
constexpr int UP_BAR_4    = A2;
constexpr int UP_BAR_3    = A3;
constexpr int UP_BAR_2    = A1;
constexpr int UP_BAR_1    = A4;
constexpr int CENTER_BAR  = A5;
constexpr int DOWN_BAR_1  = A6;
constexpr int DOWN_BAR_2  = A7;
constexpr int DOWN_BAR_3  =  2;
constexpr int DOWN_BAR_4  =  3;
constexpr int TOP_ARROW   = A0;

constexpr int CAPTURE_DURATION_MS = 30;
constexpr int SAMPLE_INTERVAL_US  = 20;
constexpr int MAX_SAMPLES = 4000;

// One array per channel
uint8_t s_UP4[MAX_SAMPLES];
uint8_t s_UP3[MAX_SAMPLES];
uint8_t s_UP2[MAX_SAMPLES];
uint8_t s_UP1[MAX_SAMPLES];
uint8_t s_CEN[MAX_SAMPLES];
uint8_t s_DN1[MAX_SAMPLES];
uint8_t s_DN2[MAX_SAMPLES];
uint8_t s_DN3[MAX_SAMPLES];
uint8_t s_DN4[MAX_SAMPLES];
uint8_t s_ARR[MAX_SAMPLES];

// ─────────────────────────────────────────────
// Count number of transitions (0→1 or 1→0)
// ─────────────────────────────────────────────
int countTransitions(const uint8_t data[], uint32_t len) {
  if (len < 2) return 0;

  int flips = 0;
  uint8_t prev = data[0];

  for (uint32_t i = 1; i < len; i++) {
    if (data[i] != prev) {
      flips++;
      prev = data[i];
    }
  }

  return flips;
}

// ─────────────────────────────────────────────
// HTML: 9 horizontal bars stacked vertically (UP4 top → DN4 bottom)
// No labels, grey by default, green when active (transitions > 4)
// Updates every 200 ms
// ─────────────────────────────────────────────
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Laser Receiver</title>
  <style>
    body { margin:0; height:100vh; background:#111; display:flex; justify-content:center; align-items:center; }
    .container { height:80vh; width:300px; display:flex; flex-direction:column; gap:4px; }
    .bar { flex:1; background:#444; border-radius:6px; transition:background 0.1s; }
    .bar.active { background:#3eff51; }
  </style>
</head>
<body>
  <div class="container">
    <div class="bar" id="bar-UP4"></div>
    <div class="bar" id="bar-UP3"></div>
    <div class="bar" id="bar-UP2"></div>
    <div class="bar" id="bar-UP1"></div>
    <div class="bar" id="bar-CEN" style="border: 2px solid red;"></div>
    <div class="bar" id="bar-DN1"></div>
    <div class="bar" id="bar-DN2"></div>
    <div class="bar" id="bar-DN3"></div>
    <div class="bar" id="bar-DN4"></div>
  </div>

  <script>
    const bars = {
      "UP4": document.getElementById("bar-UP4"),
      "UP3": document.getElementById("bar-UP3"),
      "UP2": document.getElementById("bar-UP2"),
      "UP1": document.getElementById("bar-UP1"),
      "CEN": document.getElementById("bar-CEN"),
      "DN1": document.getElementById("bar-DN1"),
      "DN2": document.getElementById("bar-DN2"),
      "DN3": document.getElementById("bar-DN3"),
      "DN4": document.getElementById("bar-DN4")
    };

    function update() {
      fetch('/data')
        .then(r => r.json())
        .then(data => {
          Object.keys(data).forEach(key => {
            if (bars[key]) {
              if (data[key] > 4) {
                bars[key].classList.add('active');
              } else {
                bars[key].classList.remove('active');
              }
            }
          });
        })
        .catch(e => console.error(e));
    }

    update();
    setInterval(update, 50);
  </script>
</body>
</html>
)rawliteral";

// ─────────────────────────────────────────────
void setup() {
  Serial.begin(2000000);
  delay(200);

  pinMode(UP_BAR_4,   INPUT);
  pinMode(UP_BAR_3,   INPUT);
  pinMode(UP_BAR_2,   INPUT);
  pinMode(UP_BAR_1,   INPUT);
  pinMode(CENTER_BAR, INPUT);
  pinMode(DOWN_BAR_1, INPUT);
  pinMode(DOWN_BAR_2, INPUT);
  pinMode(DOWN_BAR_3, INPUT);
  pinMode(DOWN_BAR_4, INPUT);
  pinMode(TOP_ARROW,  INPUT);

  WiFi.softAP(ssid, password);
  Serial.print("Hotspot: ");
  Serial.print(ssid);
  Serial.print(" | IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() {
    server.send_P(200, "text/html", html);
  });

  server.on("/data", []() {
    JsonDocument doc;
    doc["UP4"] = transitions_UP4;
    doc["UP3"] = transitions_UP3;
    doc["UP2"] = transitions_UP2;
    doc["UP1"] = transitions_UP1;
    doc["CEN"] = transitions_CEN;
    doc["DN1"] = transitions_DN1;
    doc["DN2"] = transitions_DN2;
    doc["DN3"] = transitions_DN3;
    doc["DN4"] = transitions_DN4;

    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
  });

  server.begin();
}

// ─────────────────────────────────────────────
void loop() {
  server.handleClient();

  static uint32_t lastStart = 0;
  if (millis() - lastStart < 100) return;  // capture every 0.1 second
  lastStart = millis();

  uint32_t tStart = micros();
  uint32_t tEnd   = tStart + CAPTURE_DURATION_MS * 1000UL;

  uint32_t count = 0;
  uint32_t nextSampleTime = tStart;

  while (micros() < tEnd && count < MAX_SAMPLES) {
    while (micros() < nextSampleTime) {}  // wait

    s_UP4[count] = digitalRead(UP_BAR_4);
    s_UP3[count] = digitalRead(UP_BAR_3);
    s_UP2[count] = digitalRead(UP_BAR_2);
    s_UP1[count] = digitalRead(UP_BAR_1);
    s_CEN[count] = digitalRead(CENTER_BAR);
    s_DN1[count] = digitalRead(DOWN_BAR_1);
    s_DN2[count] = digitalRead(DOWN_BAR_2);
    s_DN3[count] = digitalRead(DOWN_BAR_3);
    s_DN4[count] = digitalRead(DOWN_BAR_4);
    s_ARR[count] = digitalRead(TOP_ARROW);

    count++;
    nextSampleTime += SAMPLE_INTERVAL_US;
  }

  // Update global transition counts
  transitions_UP4 = countTransitions(s_UP4, count);
  transitions_UP3 = countTransitions(s_UP3, count);
  transitions_UP2 = countTransitions(s_UP2, count);
  transitions_UP1 = countTransitions(s_UP1, count);
  transitions_CEN = countTransitions(s_CEN, count);
  transitions_DN1 = countTransitions(s_DN1, count);
  transitions_DN2 = countTransitions(s_DN2, count);
  transitions_DN3 = countTransitions(s_DN3, count);
  transitions_DN4 = countTransitions(s_DN4, count);
  transitions_ARR = countTransitions(s_ARR, count);
}

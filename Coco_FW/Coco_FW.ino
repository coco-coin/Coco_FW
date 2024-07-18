#include <WiFi.h>
#include <ArduinoJson.h>  // JSON 라이브러리 포함

const char *ssid = "wi_dje21"; 
const char *password = "Djedsmhspw2015!";
const char *host = "192.168.0.80";
const int port = 5000; 

#define TRIGPIN 2 // 기본 Trigger핀
#define ECHOPIN 3 // 기본 Echo핀

long distance[4]; // 각 동전에서의 거리(cm) (4개의 센서를 사용한다고 가정)
int Count[4]; // 각 동전별 카운트 (0 : 10 won, 1 : 50 won, 2 : 100 won, 3 : 500 won)
bool Trig[4]; // 동전이 발겨됬는가?

WiFiClient client; // WiFiClient 객체 생성

// 거리 측정을 위한 함수
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  
  return distance;
}

void setup() {
  // 시리얼 통신 시작
  Serial.begin(9600);
  Serial.println();
  Serial.println("******************************************************");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // 핀 모드 설정
  for (int i = 0; i < 4; i++) { // 4개의 센서를 사용한다고 가정
    pinMode(TRIGPIN + i * 2, OUTPUT);
    pinMode(ECHOPIN + i * 2, INPUT);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  for (int i = 0; i < 4; i++) { // 4개의 센서를 사용한다고 가정
    distance[i] = measureDistance(TRIGPIN + i * 2, ECHOPIN + i * 2);  // 각 센서로부터 거리 측정
    if (distance[i] <= 10) {
      if (Trig[i]) {
        Count[i]++;
        Trig[i] = false;
        if (client.connect(host, port)) {
          StaticJsonDocument<200> doc;
          if (i == 0) {
            doc["sensor1"] = distance[i];
          } else if (i == 1) {
            doc["sensor2"] = distance[i];
          } else if (i == 2) {
            doc["sensor3"] = distance[i];
          } else {
            doc["sensor4"] = distance[i];
          }
          // JSON 데이터 전송
          char buffer[256];
          size_t n = serializeJson(doc, buffer);
          client.write((const uint8_t *)buffer, n);
          client.stop();
        }
      }
    } else {
      Trig[i] = true;
    }
  }

  /*// 측정 결과 출력
  Serial.print("Sensor 1: ");
  Serial.print(distance[0]);
  Serial.println(" cm");
  Serial.print("10 won: ");
  Serial.println(Count[0]);
  
  Serial.print("Sensor 2: ");
  Serial.print(distance[1]);
  Serial.println(" cm");
  Serial.print("50 won: ");
  Serial.println(Count[1]);
  
  Serial.print("Sensor 3: ");
  Serial.print(distance[2]);
  Serial.println(" cm");
  Serial.print("100 won : ");
  Serial.println(Count[2]);
  
  Serial.print("Sensor 4: ");
  Serial.print(distance[3]);
  Serial.println(" cm");
  Serial.print("500 won : ");
  Serial.println(Count[3]);*/
}

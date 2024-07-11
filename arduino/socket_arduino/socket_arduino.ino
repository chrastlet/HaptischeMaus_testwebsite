#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"

const char* ssid = "Hotspot-ESP32";
const char* password = "password";

const int VIBRATORS[5] = {21, 17, 16, 19, 18};
const int unten = 16;
const int rechts = 18;
const int links = 19;
const int mitte = 17;
const int oben = 21;

uint8_t pwmValue = 250; // 50% duty cycle


int sliderValue = 0;  // Variable to store the slider value


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");




void setup() {
    Serial.begin(9600);  // Initialize serial communication
    // Initialize all vibrator pins as outputs and set them to LOW
    for(uint8_t i = 0; i < 5; i++) {
              Serial.println(VIBRATORS[i]);

        pinMode(VIBRATORS[i], OUTPUT);
    }

  //submitPattern();
    connectToWifi();
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.begin();

}


void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}


// Funktion zur Verarbeitung der WebSocket-Nachricht
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    // Konvertiere die empfangenen Daten in eine Zeichenkette
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;  // Null-terminate the string
        String message = (char*)data;

        // JSON-Dokument erstellen
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, message);

        // Überprüfen, ob das Parsen erfolgreich war
        if (error) {
            Serial.printf("Fehler beim Parsen der JSON-Nachricht: %s\n", error.c_str());
            return;
        }

        // JSON-Nachricht verarbeiten
        const char* msg = doc["message"];

        if (strcmp(msg, "startdrag") == 0) {
            // Code für startdrag
            startDrag();
        } else if (strcmp(msg, "dragover") == 0) {
            JsonArray dataArray = doc["data"];
            int data[4];
            for (size_t i = 0; i < dataArray.size() && i < 4; ++i) {
                data[i] = dataArray[i];
            }
            dragOver(data[0], data[1], data[2], data[3]);
        } else if (strcmp(msg, "drop") == 0) {
            drop();
        } else if (strcmp(msg, "button") == 0) {
            clickableButtonPattern();
        } else if (strcmp(msg, "link") == 0) {
            clickableLinkPattern();
        } else if (strcmp(msg, "text-input") == 0) {
            textInputPattern();
        } else if (strcmp(msg, "textarea") == 0) {
            textInputPattern();
        } else if (strcmp(msg, "checkbox") == 0) {
            checkboxPattern();
        } else if (strcmp(msg, "radio") == 0) {
            radioButtonPattern();
        } else if (strcmp(msg, "select_click") == 0) {
            selectClickPattern();
        } else if (strcmp(msg, "select_change") == 0) {
            JsonArray dataArray = doc["data"];
            int data[2];
            for (size_t i = 0; i < dataArray.size() && i < 2; ++i) {
                data[i] = dataArray[i];
            }
            selectChangePattern(data[0], data[1]);
        } else if (strcmp(msg, "range") == 0) {
            int data = doc["data"];
            sliderPattern(data);
        } else if (strcmp(msg, "submit") == 0) {
            submitPattern();
        } else if (strcmp(msg, "endVertical") == 0) {
          
        } else if (strcmp(msg, "endHorizontal") == 0) {
            // Code für endHorizontal
        } else {
            // Code für den Fall, dass kein anderer Fall zutrifft
        }

        // Variablen ausgeben
        Serial.printf("WebSocket JSON message received: message=%s\n", msg);
    }
}



    void startDrag(){
        analogWrite(oben, )
    }

    void drop(){

    }

   void selectClickPattern(){

   }

  void selectChangePattern(int cur, int max){

  }


void smoothTransition(int motor1, int motor2, int dauer) {
  for (int i = 0; i <= 255; i++) {
    analogWrite(motor1, 255 - i);  // Motor1 verringern
    analogWrite(motor2, i);        // Motor2 erhöhen
    delay(dauer / 255);            // Wartezeit, um die Dauer gleichmäßig zu verteilen
  }
}


// Pattern for Clickable Button: Light vibration on all four motors
void clickableButtonPattern() {
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
    }
    delay(100); // Buzz for 100 ms
    for (uint8_t i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 0); // Turn off vibration
    }
}

// Pattern for Clickable Link: Movement from bottom to top
void clickableLinkPattern() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(50);
        analogWrite(VIBRATORS[i], 0);
    }
}


// Pattern for Text Input Field & Text Area: Continuous light vibration while focused
void textInputPattern() {
    for (int i = 0; i < 3; i++) {
        analogWrite(mitte, pwmValue); // Mid vibrator (port 15)
        delay(30);
        analogWrite(mitte, 0);
        delay(30);
    }
}
void Waitcursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
    // Set the vibration intensity using analogWrite
    analogWrite(VIBRATORS[i], 200); // Adjust the value to control the intensity
    delay(100); // Vibrate for 100 milliseconds
    analogWrite(VIBRATORS[i], 0); // Turn off the motor
  }
}
void PointerCursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        // Set the vibration intensity using analogWrite
        analogWrite(VIBRATORS[i], 150); // Adjust the value to control the intensity
        delay(100); // Vibrate for 100 milliseconds
        analogWrite(VIBRATORS[i], 0); // Turn off the motor
    }
}

void CrosshairCursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        // Set the vibration intensity using analogWrite
        analogWrite(VIBRATORS[i], 100); // Adjust the value to control the intensity
        delay(100); // Vibrate for 100 milliseconds
        analogWrite(VIBRATORS[i], 0); // Turn off the motor
    }
}

void TextCursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        // Set the vibration intensity using analogWrite
        analogWrite(VIBRATORS[i], 50); // Adjust the value to control the intensity
        delay(100); // Vibrate for 100 milliseconds
        analogWrite(VIBRATORS[i], 0); // Turn off the motor
    }
}

void MoveCursor() {
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        // Set the vibration intensity using analogWrite
        analogWrite(VIBRATORS[i], 255); // Adjust the value to control the intensity
        delay(100); // Vibrate for 100 milliseconds
        analogWrite(VIBRATORS[i], 0); // Turn off the motor
    }
}

// Pattern for Checkbox: Cross vibration or check vibration
void checkboxPattern() {
  //von links nach unten smooth
    // Übergang von links zu unten
  smoothTransition(links, unten, 500); // Sanfter Übergang von links nach unten über 1000 ms

  // Übergang von unten zu rechts
  smoothTransition(unten, rechts, 500); // Sanfter Übergang von unten nach rechts über 1000 ms

  // Übergang von rechts zu links (zurücksetzen)
  smoothTransition(rechts, links, 500); // Sanfter Übergang von rechts nach links über 1000 ms
  
  delay(2000); // Wartezeit vor der nächsten Bewegung

}

void SliderFeedback(int value) {
    int leftIntensity = 0;
    int midIntensity = 0;
    int rightIntensity = 0;

    if (value <= 50) { // Left to mid transition
        leftIntensity = map(value, 0, 50, 255, 0); // Full intensity on the left to off
        midIntensity = map(value, 0, 50, 0, 255);  // Off to full intensity on the mid
    } else { // Mid to right transition
        midIntensity = map(value, 50, 100, 255, 0); // Full intensity on the mid to off
        rightIntensity = map(value, 50, 100, 0, 255); // Off to full intensity on the right
    }

    analogWrite(VIBRATORS[0], leftIntensity);
    analogWrite(VIBRATORS[1], midIntensity);
    analogWrite(VIBRATORS[2], rightIntensity);
}

void DragVibration() {
    // Vibrate all motors with low intensity while dragging
    for (int i = 0; i < NUM_VIBRATORS; i++) {
        analogWrite(VIBRATORS[i], 100); // Low intensity
    }
    delay(100); // Adjust delay as needed to control vibration frequency
    for (int i = 0; i < NUM_VIBRATORS; i++) {
        analogWrite(VIBRATORS[i], 0); // Turn off motors
    }
    delay(100); // Adjust delay as needed to control vibration frequency
}

void DropVibration() {
    // Single intense vibration when dropping
    for (int i = 0; i < NUM_VIBRATORS; i++) {
        analogWrite(VIBRATORS[i], 255); // Maximum intensity
    }
    delay(200); // Intense vibration duration
    for (int i = 0; i < NUM_VIBRATORS; i++) {
        analogWrite(VIBRATORS[i], 0); // Turn off motors
    }
}

void ScrollMovement() {
    // Imitate scrolling movement with motors
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], 100); // Start vibration with low intensity
        delay(50); // Short delay to simulate movement
        analogWrite(VIBRATORS[i], 0); // Turn off the motor
        delay(50); // Short delay before the next motor vibrates
    }
    for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
        analogWrite(VIBRATORS[i], 100); // Start vibration with low intensity
        delay(50); // Short delay to simulate movement
        analogWrite(VIBRATORS[i], 0); // Turn off the motor
        delay(50); // Short delay before the next motor vibrates
    }
}

void StrongVibration(int direction) {
    // Strong vibration in the scroll direction
    if (direction == 1) { // Scroll down or right
        for (int i = 0; i < sizeof(VIBRATORS); i++) {
            analogWrite(VIBRATORS[i], 255); // Maximum intensity
            delay(100); // Vibrate for 100 milliseconds
            analogWrite(VIBRATORS[i], 0); // Turn off the motor
        }
    } else if (direction == -1) { // Scroll up or left
        for (int i = sizeof(VIBRATORS) - 1; i >= 0; i--) {
            analogWrite(VIBRATORS[i], 255); // Maximum intensity
            delay(100); // Vibrate for 100 milliseconds
            analogWrite(VIBRATORS[i], 0); // Turn off the motor
        }
    }
}
// Pattern for Radio Button: Circular movement across all vibrators
void radioButtonPattern() {
  smoothTransition(links, oben, 100); 

  smoothTransition(oben, rechts, 100); 

  smoothTransition(rechts, unten, 100); 

  smoothTransition(unten, links, 100); 

}

// Pattern for Select Menu Dropdown: From top to bottom
void dropdownPattern() {
    for (int i = 0; i < sizeof(VIBRATORS); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(50);
        analogWrite(VIBRATORS[i], 0);
    }
}


// Pattern for Slider: Position indicated between the 3 middle motors based on percentage
void sliderPattern(int value) {
    int position = map(value, 0, 100, 1, 3);
    analogWrite(VIBRATORS[position], pwmValue);
    delay(100);
    analogWrite(VIBRATORS[position], 0);
}
// Pattern for Submit: Two vibrations, the second one is longer
void submitPattern() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < sizeof(VIBRATORS); j++) {
            analogWrite(VIBRATORS[j], pwmValue);
        }
        delay(i == 0 ? 100 : 300);
        for (int j = 0; j < sizeof(VIBRATORS); j++) {
            analogWrite(VIBRATORS[j], 0);
        }
        delay(100);
    }
}

// Pattern for Scroll: Mimic scroll movement with motors
void scrollPattern() {
    for (int i = 0; i < sizeof(VIBRATORS) / sizeof(VIBRATORS[0]); i++) {
        analogWrite(VIBRATORS[i], pwmValue);
        delay(50);
        analogWrite(VIBRATORS[i], 0);
    }
}

// Pattern for End of Scroll: Strong vibration in scroll direction
void endScrollPattern() {
    analogWrite(VIBRATORS[4], pwmValue); // Bottom vibrator (port 33)
    delay(300);
    analogWrite(VIBRATORS[4], 0);
}

//Pattern for context menu
void contextMenuPattern(){
    

    analogWrite(links,200);
    delay(500);
    analogWrite(links,0);
    delay(500);

    analogWrite(mitte,200);
    delay(500);
    analogWrite(mitte,0);
    delay(500);

    analogWrite(rechts,200);
    delay(500);
    analogWrite(mitte,0);
    delay(500);
}

void tooltipPattern() {
  int obenIntensity = 255;
  int transitionThreshold = 120;
  
  while (obenIntensity >= 0) {
    // Oben von 255 auf 0 abnehmen
    analogWrite(oben, obenIntensity);
    
    if (obenIntensity <= transitionThreshold) {
      // Mitte, rechts, links ab 255 auf 0 abnehmen
      int sidesIntensity = 255 - obenIntensity; // Intensität für Mitte, rechts, links berechnen
      analogWrite(mitte, sidesIntensity);
      analogWrite(links, sidesIntensity);
      analogWrite(rechts, sidesIntensity);
      
      // Unten von 255 auf 0 abnehmen, wenn obenIntensity unterhalb der Schwelle ist
      analogWrite(unten, sidesIntensity);
    }
    
    obenIntensity--;
    delay(5);  // Kleine Verzögerung für sanfte Bewegung
  }
  
  // Am Ende alle Vibrationsmotoren ausschalten
  analogWrite(oben, 0);
  analogWrite(mitte, 0);
  analogWrite(links, 0);
  analogWrite(rechts, 0);
  analogWrite(unten, 0);
}

//Pattern fordropzone
void dragOver(int x, int y, int max_x, int max_y){
//position darstellen
  float l = 255;
  float o = 255;
  float u = 255;
  float r = 255;
  float m = 255;


  float norm_x = (float)x / max_x;
  float norm_y = (float)y / max_y;

  //center
  float center_x = 0.5;
  float center_y = 0.5;

  // Calculate the distance to the center
  float dist_to_center = sqrt(pow(norm_x - center_x, 2) + pow(norm_y - center_y, 2));

  // Normalize the distance to be between 0 (center) and 1 (edges)
  float max_dist = sqrt(0.5 * 0.5 + 0.5 * 0.5); // Maximum possible distance in a normalized 1x1 grid
  float normalized_dist = dist_to_center / max_dist;

  // Invert the distance to make the center 1 and edges near 255
  float inverted_value = (1 - normalized_dist);

  // Scale the center multiplier
  m = m * inverted_value;

  // Scale the edge multipliers
  l = l * norm_x;
  r = r * (1 - norm_x);
  o = o * norm_y;
  u = u * (1 - norm_y);

  analogWrite(links, l);
  analogWrite(rechts, r);
  analogWrite(oben, o);
  analogWrite(unten, u);
  analogWrite(mitte, m);
  delay(200);

  // Print the results (for debugging)
  Serial.print("Left: ");
  Serial.print(l);
  Serial.print(", Right: ");
  Serial.print(r);
  Serial.print(", Top: ");
  Serial.print(o);
  Serial.print(", Bottom: ");
  Serial.print(u);
  Serial.print(", Middle: ");
  Serial.println(m);


}

//Pattern for hover
void hoverPattern(){
//mini vibration
  analogWrite(links, 100);
  analogWrite(rechts, 100);
  analogWrite(oben, 100);
  analogWrite(unten, 100);
  analogWrite(mitte, 100);
  delay(200);
    analogWrite(links, 0);
  analogWrite(rechts, 0);
  analogWrite(oben, 0);
  analogWrite(unten, 0);
  analogWrite(mitte, 0);
}


void grabPattern() {
  int stepDelay = 5;  // Zeitverzögerung in Millisekunden für jede Stufe
  int intensity=1;
  int innen =255;
  int außen =100;
  
  while(innen>0 && außen<255){
      innen-=intensity;
      außen += intensity;
      analogWrite(oben, außen);
      analogWrite(unten, außen);
      analogWrite(links, außen);
      analogWrite(rechts, außen);
      analogWrite(mitte, innen);
      delay(stepDelay);
  } 
      analogWrite(oben, 0);
      analogWrite(unten, 0);
      analogWrite(links, 0);
      analogWrite(rechts, 0);
}


//Pattern for no drop
void noDropPattern(){
  //leicht vibration durchgehend
    analogWrite(oben, 150);
    analogWrite(unten, 150);
    analogWrite(links, 150);
    analogWrite(rechts, 150);
    analogWrite(mitte, 150);

    delay(1000);
    
    analogWrite(oben, 0);
    analogWrite(unten, 0);
    analogWrite(links, 0);
    analogWrite(rechts, 0);

    // Mitte erhöhen
    analogWrite(mitte, 0);

    // Mitte erhöhen

}
// Pattern for zoom in
void zoomInPattern() {
  int stepDelay = 10;  // Zeitverzögerung in Millisekunden für jede Stufe
  int intensity=1;
  int innen =255;
  int außen =100;
  
  while(innen>0 && außen<255){
      innen-=intensity;
      außen += intensity;
      analogWrite(oben, außen);
      analogWrite(unten, außen);
      analogWrite(links, außen);
      analogWrite(rechts, außen);
      analogWrite(mitte, innen);
      delay(stepDelay);
  } 
      analogWrite(oben, 0);
      analogWrite(unten, 0);
      analogWrite(links, 0);
      analogWrite(rechts, 0);
}

void zoomOutPattern() {
  int stepDelay = 10;  // Zeitverzögerung in Millisekunden für jede Stufe
  int intensity=1;
  int innen= 150;
  int außen =255;
  
  while(innen<255 && außen>0){
      innen+=intensity;
      außen -= intensity;
      analogWrite(oben, außen);
      analogWrite(unten, außen);
      analogWrite(links, außen);
      analogWrite(rechts, außen);
      analogWrite(mitte, innen);
      delay(stepDelay);
  } 
      analogWrite(oben, 0);
      analogWrite(unten, 0);
      analogWrite(links, 0);
      analogWrite(rechts, 0);
}




void waitPattern() {
  int stepDelay = 100;  // Zeitverzögerung in Millisekunden für jede Stufe
  int maxIntensity = 255;
  int minIntensity = 0;
  int steps = 20;

  // Kreisbewegungen von außen nach innen
  for (int i = 0; i < 3; i++) { // Wiederhole die Kreisbewegung 3 mal
    for (int j = 0; j < steps; j++) {
      int intensity = (maxIntensity * j) / steps;

      // Oben
      analogWrite(oben, intensity);
      delay(stepDelay);
      analogWrite(oben, minIntensity);

      // Rechts
      analogWrite(rechts, intensity);
      delay(stepDelay);
      analogWrite(rechts, minIntensity);

      // Unten
      analogWrite(unten, intensity);
      delay(stepDelay);
      analogWrite(unten, minIntensity);

      // Links
      analogWrite(links, intensity);
      delay(stepDelay);
      analogWrite(links, minIntensity);
    }
  }
}

// Pattern for not allowed
void notAllowedPattern() {
  int pulseDuration = 500;  // Dauer der starken Vibration in Millisekunden
  int intensity = 255;      // Maximale Intensität der Vibration

  // Starke Vibration für alle Motoren
  analogWrite(oben, intensity);
  analogWrite(unten, intensity);
  analogWrite(links, intensity);
  analogWrite(rechts, intensity);
  analogWrite(mitte, intensity);

  // Warte für die festgelegte Dauer
  delay(pulseDuration);

  // Alle Motoren ausschaldten
  analogWrite(oben, 0);
  analogWrite(unten, 0);
  analogWrite(links, 0);
  analogWrite(rechts, 0);
  analogWrite(mitte, 0);
}


//
//
//
//
//




void loop() {
  // Main code here

  //zu stark


  /*
  tooltipPattern();
  delay(2000);
    
  
  hoverPattern();
  delay(2000);
  
  grabPattern();
  delay(2000);

  noDropPattern();
  delay(2000);

  zoomOutPattern();
  delay(2000);

  zoomOutPattern();
  delay(2000);

  //klappt net
  waitPattern();
  delay(2000);

  notAllowedPattern();
  delay(2000);
*/

}







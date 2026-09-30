#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

/*Put your SSID & Password*/
const char* ssid = "YourNetworkName";   // Enter SSID here
const char* password = "YourPassword";  //Enter Password here

Servo myservo;  // create servo object to control a servo

// Define servo pin
const int servoPin = 13;  // Change this to your actual servo pin

WebServer server(80);

void setup() {
  // Allow allocation of all timers for servo library
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Set servo PWM frequency to 50Hz
  myservo.setPeriodHertz(50);

  // Attach to servo and define minimum and maximum positions
  myservo.attach(servoPin, 500, 2400);

  // Start serial
  Serial.begin(115200);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  int servoPos;

  // Check if value parameter exists
  if (server.hasArg("value")) {
    String valueString = server.arg("value");
    servoPos = valueString.toInt();

    // Send a simple response for AJAX requests
    server.send(200, "text/plain", "OK");
  } else {
    // Move servo to 90 deg initially
    servoPos = 90;

    // Send the main webpage
    server.send(200, "text/html", createHTML());
  }

  // Move servo into position
  myservo.write(servoPos);

  // Print value to serial monitor
  Serial.print("Servo position: ");
  Serial.println(servoPos);
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String createHTML() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <style>
          html {font-family: sans-serif; text-align: center;}
          h1 {margin: 80px auto 40px; font-weight: 300; font-size: 2.5em;}
          .slider-container {padding-top: 30px; margin: 40px auto; max-width: 500px;}
          .field {display: flex; justify-content: space-between; align-items: center; gap: 20px; margin: 20px 0;}
          .field .value {font-size: 18px; width: 50px;}
          .slider {appearance: none; width: 100%; height: 8px; border-radius: 25px; background: rgba(0, 0, 0, 0.1); outline: none; position: relative; cursor: pointer;}
          .slider-input {position:relative; width: 100%; height: 18px;}
          .slider::-webkit-slider-thumb {appearance: none; width: 28px; height: 28px; border-radius: 50%; background: #fff; cursor: pointer; box-shadow: 0 4px 15px rgba(0, 0, 0, 0.3), 0 0 0 3px rgba(255, 255, 255, 0.2); transition: all 0.2s ease; position: relative;}
          .slider::-webkit-slider-thumb:hover {transform: scale(1.1);}
          .position-display {padding: 20px;}
          .position-value {font-size: 2.5em;}
          .slider-fill {position: absolute; height: 8px; background: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%); border-radius: 25px; pointer-events: none; top: 7px; margin: 0 2px;}
      </style>
  </head>
  <body>
      <h1>Servo Control</h1>
      
      <div class="slider-container">
          <div class="field">
              <div class="value">0°</div>
              <div class="slider-input">
                  <div class="slider-fill" id="sliderFill"></div>
                  <input type="range" min="0" max="180" class="slider" id="servoSlider" onchange="servo(this.value)" value="90"/>
              </div>
              <div class="value">180°</div>
          </div>
          
          <div class="position-display">
              <div>Current Position</div>
              <div class="position-value"><span id="servoPos">90</span>°</div>
          </div>
      </div>
      
      <script>
          // Get DOM elements
          var slider = document.getElementById("servoSlider");
          var servoP = document.getElementById("servoPos");
          var sliderFill = document.getElementById("sliderFill");
          
          // Initialize display
          servoP.innerHTML = slider.value;
          updateSliderFill();
          
          // Update display when slider moves
          slider.oninput = function() {
              servoP.innerHTML = this.value;
              updateSliderFill();
          }
          
          // Function to update the visual fill of the slider
          function updateSliderFill() {
              var percentage = (slider.value / 180) * 100;
              sliderFill.style.width = percentage + '%';
          }
          
          // Function to send servo position to ESP32
          function servo(pos) {
              // Create XMLHttpRequest object
              var xhr = new XMLHttpRequest();
              
              // Set timeout
              xhr.timeout = 1000;
              
              // Handle timeout
              xhr.ontimeout = function() {
                  console.log('Request timed out');
              };
              
              // Handle errors
              xhr.onerror = function() {
                  console.log('Request failed');
              };
              
              // Send GET request to ESP32
              xhr.open('GET', '/?value=' + pos, true);
              xhr.send();
          }
      </script>
  </body>
  </html>
)rawliteral";

  return html;
}

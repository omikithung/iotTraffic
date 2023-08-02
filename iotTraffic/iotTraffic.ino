//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
/*
esp board 2.0.9 version has problems

ESP32 WROOM DA
*/

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include  <Ticker.h>
#include  <WebSocketsServer.h>

AsyncWebServer server(80);
WebSocketsServer websocket(81);

Ticker timer;
#define USE_SERIAL Serial1

const char* ssid = "Magnifico";
const char* password = "dragonLore";

// bool direction[itemsNo] = {directionBoth, directionRtoL, directionLtoR};
bool direction[3] = {true, false, false};

//this synax treat as tring literal value
//PROGMEM is the flash
char homePage[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Path Allotment</title>
</head>

<style>
    .div1 {
        height: 100vh;
        width:100%;
        /* background-color: aquamarine; */
        display: flex;
        align-items: center;
    }
    .contain {
        display: flex;
        flex-direction: column;
        margin: auto;
        box-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
    }
    .control {
        display: flex;
        justify-content: space-around;
    }
    button {
        /*removing default styling*/
        background-color: transparent;
        border: none;
        outline: none;
        cursor: pointer;
    }
    .control button {
        align-self: center;
        padding: 20px 0 20px 0;
        border-radius: 2px;
        width: 100%;
        font-weight: bold;
        color: rgb(211, 226, 236);
    }
    /* CSS class doesn'tseem to work when class is added on styled css using child selectors */
    .controlBtnLR {
        background-color: green;
    }
    .controlBtnSelf {
        background-color: rgb(54, 53, 53);
    }
    .controlBtn {
        position: relative;
        top: -10px;
        cursor: not-allowed;
        background-color: rgba(61, 60, 60, 0.918);
        font-style: italic;
    }

    .hold {
        width: 640px;
        height: 180px;
        display: flex;
        justify-content: space-around;
        margin: auto;
        margin-top: auto;
        padding-bottom: 8px;
    }
    .pathContainer {
        display: flex;
        flex-direction: column;
        flex: 1;
    }
   .path{
        position: relative;
        height: 100%;
        width: 100%;
        display: flex;
        justify-content: center;
        background-color: rgba(0, 0, 0, 0.74);
    }
    .pathMark {
        border: 1px dashed rgb(131, 130, 130);
        width: 100%;
        align-self: center;
        margin: 0 30px;
    }
    .gate {
        display: flex;
        flex-direction: column;
        /* border: 1px solid red; */
        height: 100%;
    }
    .gateDecorate {
        display: flex;
        flex-direction: column;
        padding: 0 10px;
        /* border: 1px solid red; */
        height: 100%;
        justify-content: center;
        background-color: rgb(151, 63, 104);
        color: antiquewhite;
    }
    .gateStatus {
        height: 62px;
        display: flex;
        /* border: 1px solid red; */
        flex-direction: column;
        align-items: center;
        justify-content: space-around;
    }
    .moveTowardsLeft {
        position: absolute;
        top: 50%;
        left: 100%;
        transform: translate(-45%,-45%);
        z-index:1;
        visibility: hidden;
    }
    .moveTowardsRight {
        position: absolute;
        top: 50%;
        left: 0;
        transform: translate(-45%,-45%);
        z-index:1;
        visibility: hidden;
    }
    @keyframes moveTowardsLeft {
        from {
            left:100%;
            opacity: 1;
        }
        to {
            left:-100%;
            visibility: visible;
            opacity: 0;
        }
    }
    @keyframes moveTowardsRight {
        from {
            left:0;
            opacity: 1;
        }
        to {
            left:200%;
            visibility: visible; 
            opacity: 0;
        }
    }
    .moveTowardsLeftAnimate {
        animation: moveTowardsLeft 2s ease infinite;
    }
    .moveTowardsRightAnimate {
        animation: moveTowardsRight 2s ease infinite;
    }

    .head {
        margin: auto;
        background-color: rgba(0, 0, 0, 0.74);
        position: relative; 
        top: -20px;
        border-radius: 10px;
        padding: 2px;
    }
    .head > div {
        padding: 5px 12px 2px 12px;
        border-radius: 50%;
        border: 1px solid rgba(112, 109, 109, 0.712);
    }
    /* for Light Indication */
    .lightIndicate {
        width: 30px;
        height: 30px;
        border-radius: 100%;
        margin: 0;
        cursor: not-allowed;
        background-color: rgba(0, 0, 0, 0.74);
    }
    .lightColorRed {     
        background-color: red;
        cursor: pointer;
    }
    .lightColorGreen {
        background-color: green;
        cursor: pointer;
    }
    </style>

<body>
    <div class="div1" >
        <div class="contain">
           <div class="hold">
                <!-- path 1 -->
               <div class="pathContainer">
                   <div class="head">
                       <div>
                           <button id="leftRed" class="lightIndicate"></button>
                           <button id="leftGreen" class="lightIndicate"></button>
                       </div>
                   </div>
                   <div class="path">
                        <!-- for animated object car -->
                        <div class="moveTowardsRight"><span>&#x1F89A</span></div>
                        <!-- for road middle mark -->
                        <div class="pathMark"></div>
                   </div>
               </div>
               <!-- Gate/Traffic point -->
               <div class="gate">
                <div class="gateStatus">
                    <div class="directPoint" id="pointLeft">&#x2192;</div>
                    <div class="directPoint" id="pointRight">&#x2190;</div>
                </div>
                   <div class="gateDecorate">
                        <span>G</span><span>A</span><span>T</span><span>E</span>
                    </div>
               </div>
               <!-- path 2 -->
               <div class="pathContainer">
                   <div class="head">
                       <div>
                           <button id="rightRed" class="lightIndicate"></button>
                           <button id="rightGreen" class="lightIndicate"></button>
                       </div>
                   </div>
                    <div class="path">
                        <!-- for animated object car -->
                        <div class="moveTowardsLeft"><span>&#x1F898;</span></div>        
                        <!-- for road middle mark -->
                        <div class="pathMark"></div>
                    </div>

               </div>
           </div>

            <!-- for control -->
           <div class="control">
               <button onclick="setTravelDirect('leftToRight')" id="lTorBtn" class="controlBtnLR">Left Entry</button>
               <button onclick="setTravelDirect('selfTrafficFlow')" id="selfTrafficFlowBtn" class="controlBtnSelf">Self traffic flow</button>
               <button onclick="setTravelDirect('rightToLeft')" id="rTolBtn" class="controlBtnLR">Right Entry</button>
           </div>
        </div>
        
    </div>

    <script>
        
        var serverCon = new WebSocket('ws://'+location.hostname+':81/');
        serverCon.onmessage = function(event) {
            var data = event.data;
            adjustUI(data);
        }
        let leftRed = document.querySelector('#leftRed');
        let leftGreen = document.querySelector('#leftGreen');
        let rightRed = document.querySelector('#rightRed');
        let rightGreen = document.querySelector('#rightGreen');
        //button
        let rightToLeftB = document.querySelector('#rTolBtn');
        let selfTrafficFlowB = document.querySelector('#selfTrafficFlowBtn');
        let leftToRightB = document.querySelector('#lTorBtn');

        let moveTowardsLeft = document.querySelector('.moveTowardsLeft');
        let moveTowardsRight = document.querySelector('.moveTowardsRight');
        let value = ""
        function setTravelDirect(value="selfTrafficFlow") {
            if(value==="leftToRight") {
                serverCon.send("leftToRight");
            }
            else if(value==="rightToLeft") {
                serverCon.send("rightToLeft");
            }
            else if(value==="selfTrafficFlow") {
                serverCon.send("selfTrafficFlow");
            }
            
        }

        function adjustUI(value) {
            if(value==="leftToRight") {
                //for lights
                leftRed.classList.remove("lightColorRed");
                leftGreen.classList.add("lightColorGreen");
                rightRed.classList.add("lightColorRed");
                rightGreen.classList.remove("lightColorGreen");
                //for animation arrow
                moveTowardsLeft.classList.remove("moveTowardsLeftAnimate"); 
                moveTowardsRight.classList.add("moveTowardsRightAnimate");
                //button
                leftToRightB.classList.add("controlBtn");
                rightToLeftB.classList.remove("controlBtn");
                selfTrafficFlowB.classList.remove("controlBtn");
                leftToRightB.disabled = true;
                rightToLeftB.disabled = false;
                selfTrafficFlowB.disabled = false;
            }
            else if(value==="rightToLeft") {
                //for lights
                leftRed.classList.add("lightColorRed");
                leftGreen.classList.remove("lightColorGreen");
                rightRed.classList.remove("lightColorRed");
                rightGreen.classList.add("lightColorGreen");
                //for animation arrow
                moveTowardsLeft.classList.add("moveTowardsLeftAnimate");
                moveTowardsRight.classList.remove("moveTowardsRightAnimate");
                //button
                leftToRightB.classList.remove("controlBtn");
                rightToLeftB.classList.add("controlBtn");
                selfTrafficFlowB.classList.remove("controlBtn");
                leftToRightB.disabled = false;
                rightToLeftB.disabled = true;
                selfTrafficFlowB.disabled = false;
            }
            else if(value==="selfTrafficFlow") {
                //for lights
                leftRed.classList.remove("lightColorRed");
                leftGreen.classList.remove("lightColorGreen");
                rightRed.classList.remove("lightColorRed");
                rightGreen.classList.remove("lightColorGreen");
                //for animation arrow
                moveTowardsLeft.classList.remove("moveTowardsLeftAnimate");
                moveTowardsRight.classList.remove("moveTowardsRightAnimate");
                //button
                leftToRightB.classList.remove("controlBtn");
                rightToLeftB.classList.remove("controlBtn");
                selfTrafficFlowB.classList.add("controlBtn");
                leftToRightB.disabled = false;
                rightToLeftB.disabled = false;
                selfTrafficFlowB.disabled = true;
            }
            
        }
    </script>
 
</body>
</html>

)=====";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[%u] Disconnected!\n", num);
    break;
    
    case WStype_CONNECTED:{
      IPAddress ip = websocket.remoteIP(num);
      USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      
      Serial.printf("[%u] connected from...", num);
      Serial.println("http://"+ip.toString());

			// send message to client
			websocket.sendTXT(num, "Connected to websocket :)");

    }
    break;
    
    case WStype_TEXT:
      USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
      // send message to client
      // webSocket.sendTXT(num, "message here");
      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      String dataReceived = String( (char*) (payload) );
      Serial.println(dataReceived);

      if(dataReceived=="selfTrafficFlow") {
        direction[0] = true;
        direction[1]=false;
        direction[2]=false;
      }
      else if(dataReceived=="rightToLeft") {
        direction[0] = false;
        direction[1]=true;
        direction[2]=false;
      }
      else if(dataReceived=="leftToRight") {
        direction[0] = false;
        direction[1]=false;
        direction[2]=true;
      }
    break;
  }

}

void entry() {

  if(direction[0] == true) {
    websocket.broadcastTXT("selfTrafficFlow");
  }
  else if (direction[1]==true) {
    websocket.broadcastTXT("rightToLeft");
  }
  else if(direction[2]==true) {
    websocket.broadcastTXT("leftToRight");
  }

}

void setup() {

    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", homePage); 
    });

    server.onNotFound(notFound);

    server.begin();
    websocket.begin();
    websocket.onEvent(webSocketEvent);
    timer.attach(0.1,entry);
}

void loop() {
  websocket.loop();
}
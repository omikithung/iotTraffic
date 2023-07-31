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

#define USE_SERIAL Serial1

const char* ssid = "Magnifico";
const char* password = "dragonLore";

bool directionBoth = true, directionRtoL=false, directionLtoR=false;

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
    .hold > div {
        display: flex;
        flex-direction: column;
    }
    .hold > div:first-child {
        flex: 1;
    }
    .hold > div:nth-child(2) > div:first-child {
        height: 40px;
        background-color: rgba(0, 0, 0, 0.842);
    }
    .hold > div:nth-child(2) > div:nth-child(2) {
        background-color: rgba(75, 73, 73, 0.719);
        height: 100%;
        margin-top: 10px;
        display: flex;
        flex-direction: column;
        vertical-align: middle;
        justify-content: center;
    }
    .hold > div:nth-child(3) {
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
    span {
        font-size: 60px;
        color: antiquewhite;
        transform: rotateY(45deg);
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
               <div>
                   <div class="head">
                       <div>
                           <button id="leftRed" class="lightIndicate"></button>
                           <button id="leftGreen" class="lightIndicate"></button>
                       </div>
                   </div>
                   <div class="path">
                        <div class="moveTowardsRight"><span>&#x1F89A;</span></div>
                   </div>
               </div>
               <div>
                   <div>
                       <div class="directPoint" id="pointLeft">&#x2192;</div>
                       <div class="directPoint" id="pointRight">&#x2190;</div>
                   </div>
                   <div>
                       <div class="directPoint" id="pointLeft">&#x2192;</div>
                       <div class="directPoint" id="pointRight">&#x2190;</div>
                   </div>
               </div>
               <div>
                   <div class="head">
                       <div>
                           <button id="rightRed" class="lightIndicate"></button>
                           <button id="rightGreen" class="lightIndicate"></button>
                       </div>
                   </div>
                    <div class="path">
                        <div class="moveTowardsLeft"><span>&#x1F898;</span></div>        
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
            var fullData = event.data;
            console.log(fullData);
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
                serverCon.send("leftToRight");
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
                serverCon.send("rightToLeft");
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
                serverCon.send("selfTrafficFlow");

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

      //on changes in event
      entry();

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
        directionBoth = true;
        directionRtoL=false;
        directionLtoR=false;
      }
      else if(dataReceived=="rightToLeft") {
        directionBoth = false;
        directionRtoL=true;
        directionLtoR=false;
      }
      else if(dataReceived=="leftToRight") {
        directionBoth = false;
        directionRtoL=false;
        directionLtoR=true;
      }
    break;
  }

}

void entry() {

  if(directionBoth == true) {
    websocket.broadcastTXT("selfTrafficFlow");
  }
  else if (directionRtoL==true) {
    websocket.broadcastTXT("rightToLeft");
  }
  else if(directionLtoR==false) {
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
}

void loop() {
  websocket.loop();
}
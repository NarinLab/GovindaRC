void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            // Decrease client number flag
            _CLIENT_NUM--;
            safeme();
            // Dim light
            for (int i = 0; i < 3; i++){
                light(1, 500);
                delay(500);
            }
            break;
        case WStype_CONNECTED:
            {
                // Increase client number flag
                _CLIENT_NUM++;
                // Set head
                look(90, 90);
                //IPAddress ip = webSocket.remoteIP(num);
                // Dim light
                light(1, 100);
            }
            break;
        case WStype_TEXT:
            {
                // Command router
                StaticJsonBuffer<PAYLOAD_SIZE> msg;
                JsonObject& data = msg.parseObject(payload);

                StaticJsonBuffer<PAYLOAD_SIZE> jsonBuffer;
                JsonObject& root = jsonBuffer.createObject();
                char payload[PAYLOAD_SIZE];
                // Check message format
                if(!data.success()){
                    root["msgId"] = "-";
                    root["status"] = 400;
                    root["type"] = "debug";
                    root["data"] = "-";
                    root["desc"] = "Bad Request";
                    root.printTo(payload, sizeof(payload));
                    webSocket.sendTXT(num, payload);
                    return;
                }
                // movement section
                if(data["cmd"] == "move"){
                    if(data["arg"] >= 0 && data["arg"] <= 4){
                        navigate(data["arg"]);
                        root["msgId"] = data["msgId"];
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = data["arg"];
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                    else{
                        root["msgId"] = data["msgId"];
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "move[arg: (int) 0-4]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                }
                // head direction section
                else if(data["cmd"] == "look"){
                    if(data["arg"]["x"] >= 0 && data["arg"]["x"] <= 180 && 
                        data["arg"]["y"] >= 0 && data["arg"]["y"] <= 180){
                        look(data["arg"]["x"], data["arg"]["y"]);
                        root["msgId"] = data["msgId"];
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = data["arg"];
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                    else{
                        root["msgId"] = data["msgId"];
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "look[ arg: (arr) {x: 0-180, y: 0-180} ]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                }
                // Ultrasonic pinger section
                else if(data["cmd"] == "scan"){
                    if(data["arg"] > 0){
                        float cm = scan(data["arg"]);
                        root["msgId"] = data["msgId"];
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = cm;
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                    else{
                        root["msgId"] = data["msgId"];
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "scan[ arg: (int) 1-65535 ]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                }
                // Light lamp section
                else if(data["cmd"] == "light"){
                    if(data["arg"] >= 0 && data["arg"] <= 1){
                        light(data["arg"], 0);
                        root["msgId"] = data["msgId"];
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = data["arg"];
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                    else{
                        root["msgId"] = data["msgId"];
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "light[ arg: (int) 0-1 ]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                }  
                // speed section
                else if(data["cmd"] == "speed"){
                    if(data["arg"]["a"] >= 0 && data["arg"]["a"] <= 1023 && 
                        data["arg"]["b"] >= 0 && data["arg"]["b"] <= 1023){
                        speed(data["arg"]["a"], data["arg"]["b"]);
                        root["msgId"] = data["msgId"];
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = data["arg"];
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                    else{
                        root["msgId"] = data["msgId"];
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "look[ arg: (arr) {a: 0-1023, b: 0-1023} ]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                }
                // Not implemented
                else{
                    root["msgId"] = data["msgId"];
                    root["status"] = 501;
                    root["type"] = data["cmd"];
                    root["data"] = data["arg"];
                    root["desc"] = "Not Implemented";
                    root.printTo(payload, sizeof(payload));
                    webSocket.sendTXT(num, payload);
                }  

                //Serial.printf("[%u] get Text: %s\n", num, payload);

                // send message to client
                // webSocket.sendTXT(num, payload);

                // send data to all connected clients
                // webSocket.broadcastTXT("message here");
            }
            break;
        case WStype_BIN:
            {
            //Serial.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
            }
            break;
    }

}
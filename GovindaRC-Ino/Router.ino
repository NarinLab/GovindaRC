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
                    root["status"] = 400;
                    root["type"] = "debug";
                    root["data"] = "-";
                    root["desc"] = "Bad Request";
                    root.printTo(payload, sizeof(payload));
                    webSocket.sendTXT(num, payload);
                    return;
                }
                if (data["cmd"] == "P"){
                    webSocket.sendTXT(num, "GR");
                }
                // movement section
                else if(data["cmd"] == "move"){
                    if(data["arg"] >= 0 && data["arg"] <= 4){
                        navigate(data["arg"]);
                        /**
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = data["arg"];
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);**/
                    }
                    else{
                        /**
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "move[arg: (int) 0-4]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);**/
                    }
                }
                // Ultrasonic pinger section
                else if(data["cmd"] == "scan"){
                    if(data["arg"] > 0){
                        float cm = scan(data["arg"]);
                        
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = cm;
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                    else if(data["arg"] == 0){
                        _FLAG_SCAN = !_FLAG_SCAN;
                    }
                    else{
                        /**
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "scan[ arg: (int) 1-65535 ]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);**/
                    }
                }
                // Light lamp section
                else if(data["cmd"] == "light"){
                    if(data["arg"] >= 0 && data["arg"] <= 1){
                        light(data["arg"], 0);
                        
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = data["arg"];
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                    else{
                        /**
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "light[ arg: (int) 0-1 ]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);**/
                    }
                }  
                // speed section
                else if(data["cmd"] == "speed"){
                    if(data["arg"]["a"] >= 0 && data["arg"]["a"] <= 1023 && 
                        data["arg"]["b"] >= 0 && data["arg"]["b"] <= 1023){
                        speed(data["arg"]["a"], data["arg"]["b"]);
                        /**
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = data["arg"];
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);**/
                    }
                    else{
                        /**
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "speed[ arg: (arr) {a: 0-1023, b: 0-1023} ]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);**/
                    }
                }
                // wifi section
                else if(data["cmd"] == "set_wifi"){
                    if(((sizeof(data["arg"]["ssid"]) >= 8) && (sizeof(data["arg"]["pass"]) >= 8)) && ((sizeof(data["arg"]["ssid"]) <= 30) && (sizeof(data["arg"]["pass"]) <= 30))){
                        String ssid = data["arg"]["ssid"];
                        String password = data["arg"]["pass"];
                        ssid.toCharArray(WIFI_SSID, sizeof(WIFI_SSID) - 1);
                        password.toCharArray(WIFI_PASSWORD, sizeof(WIFI_PASSWORD) - 1);
                        saveCredentials();

                        
                        root["status"] = 200;
                        root["type"] = data["cmd"];
                        root["data"] = data["arg"];
                        root["desc"] = "OK";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);
                    }
                    else{
                        /**
                        root["status"] = 400;
                        root["type"] = "debug";
                        root["data"] = "speed[ arg: (arr) {a: 0-1023, b: 0-1023} ]";
                        root["desc"] = "Bad Request";
                        root.printTo(payload, sizeof(payload));
                        webSocket.sendTXT(num, payload);**/
                    }
                }
                else if(data["cmd"] == "get_wifi"){
                    
                    root["status"] = 200;
                    root["type"] = data["cmd"];
                    JsonObject& data = root.createNestedObject("data");
                    data["ssid"] = WIFI_SSID;
                    data["pass"] = WIFI_PASSWORD;
                    root["desc"] = "OK";
                    root.printTo(payload, sizeof(payload));
                    webSocket.sendTXT(num, payload);
                }
                else if(data["cmd"] == "get_version"){
                    
                    root["status"] = 200;
                    root["type"] = data["cmd"];
                    root["data"] = VERSION;
                    root["desc"] = "OK";
                    root.printTo(payload, sizeof(payload));
                    webSocket.sendTXT(num, payload);
                }
                else if(data["cmd"] == "engine"){
                    if(data["arg"] >= 0 && data["arg"] <= 1){
                        engine_switch(data["arg"]);
                    }
                }
                // Not implemented
                else{
                    
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
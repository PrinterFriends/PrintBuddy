#include "JsonRequestClient.h"

StaticJsonDocument<JSON_MAX_BUFFER> JsonRequestClient::lastJsonDocument;

JsonRequestClient::JsonRequestClient(DebugController *debugController) {
    this->debugController = debugController;
}

WiFiClient JsonRequestClient::requestWifiClient(
    int requestType,
    String server,
    int port,
    String encodedAuth,
    String httpPath,
    String apiPostBody
) {
    WiFiClient requestClient;
    requestClient.setTimeout(5000);
    httpPath = (requestType == PRINTER_REQUEST_POST ? "POST " : "GET ") + httpPath + " HTTP/1.1";
    String fullTarget = server + ":" + String(port) + " | " + httpPath;

    this->debugController->print("Request data from ");
    this->debugController->print(httpPath);
    if (requestType == PRINTER_REQUEST_POST) {
        this->debugController->print(" | " + apiPostBody);
    }
    this->debugController->printLn("");

    if (requestClient.connect(server, port)) {  //starts client connection, checks for connection
        requestClient.println(httpPath);
        requestClient.println("Host: " + server + ":" + String(port));
        if (encodedAuth != "") {
            requestClient.print("Authorization: ");
            requestClient.println("Basic " + encodedAuth);
        }
        requestClient.println("User-Agent: ArduinoWiFi/1.1");
        requestClient.println("Connection: close");
        if (requestType == PRINTER_REQUEST_POST) {
            requestClient.println("Content-Type: application/json");
            requestClient.print("Content-Length: ");
            requestClient.println(apiPostBody.length());
            requestClient.println();
            requestClient.println(apiPostBody);
        }

        if (requestClient.println() == 0) {
            this->debugController->printLn("SOCKET: Connection to " + fullTarget + " failed.");
            this->debugController->printLn("");
            this->lastError = "SOCKET: Connection to " + fullTarget + " failed.";
            return requestClient;
        }
    } 
    else {
        // error message if no client connect
        this->debugController->printLn("SOCKET: Connection failed: " + fullTarget);
        this->debugController->printLn("");
        this->lastError = "SOCKET: Connection failed: " + fullTarget;
        return requestClient;
    }

    while(requestClient.connected() && !requestClient.available()) delay(1); 

    // Did we have header data in response?
    char statusPeek[32] = {0};
    requestClient.peekBytes(statusPeek, sizeof(statusPeek));
    if (String(statusPeek).indexOf("HTTP/") < 0) {
        this->debugController->print("No HTTP Header: ");
        this->debugController->printLn(statusPeek);
        return requestClient;
    }

    // Check HTTP status
    char status[32] = {0};
    requestClient.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
        this->debugController->print("Unexpected response: ");
        this->debugController->printLn(status);
        this->lastError = "SOCKET: Response: " + String(status);
        return requestClient;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!requestClient.find(endOfHeaders)) {
        this->debugController->printLn("Invalid response");
        this->lastError = "SOCKET: Invalid response from " + fullTarget;
    }

    return requestClient;
}

JsonDocument *JsonRequestClient::requestJson(
    int requestType,
    String server,
    int port,
    String encodedAuth,
    String httpPath,
    String apiPostBody,
    bool withResponse = false
) {
    // Request data
    this->resetLastError();
    WiFiClient reqClient = this->requestWifiClient(requestType, server, port, encodedAuth, httpPath, apiPostBody);
    if ((this->lastError != "") || !withResponse) {
        reqClient.stop();
        return NULL;
    }
    
    // Parse JSON object
    DeserializationError error = deserializeJson(JsonRequestClient::lastJsonDocument, reqClient);
    if (error) {
        this->debugController->printLn("Data Parsing failed: " + server + ":" + String(port) + "[" + error.c_str() + "]");
        this->lastError = "PARSER: Data Parsing failed: " + server + ":" + String(port);
        reqClient.stop();
        return NULL;
    }
    reqClient.stop();
    return &JsonRequestClient::lastJsonDocument;
}

String JsonRequestClient::getLastError() {
    return this->lastError;
}

void JsonRequestClient::resetLastError() {
    this->lastError = "";
}

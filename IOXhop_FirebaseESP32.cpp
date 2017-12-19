#ifndef _IOXhop_FirebaseESP32_CPP_INCLUDED
#define _IOXhop_FirebaseESP32_CPP_INCLUDED

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "IOXhop_FirebaseESP32.h"

FirebaseESP32::FirebaseESP32() {
	// not use
}

void FirebaseESP32::begin(String host) {
	begin(host, "");
}

void FirebaseESP32::begin(String host, String auth) {
	host.replace("https:", "");
	host.replace("/", "");
	_host = host;
	_auth = auth;
}

// Start of get
int FirebaseESP32::getInt(String path) {
	return _http(path, "GET").toInt();
}

float FirebaseESP32::getFloat(String path) {
	return _http(path, "GET").toFloat();
}

String FirebaseESP32::getString(String path) {
	String value = _http(path, "GET");
	return value.substring(1, value.length() - 1);
}

bool FirebaseESP32::getBool(String path) {
	return _http(path, "GET").indexOf("true") >= 0;
}

void FirebaseESP32::get(String path, int &value) {
	value = getInt(path);
}

void FirebaseESP32::get(String path, float &value) {
	value = getFloat(path);
}

void FirebaseESP32::get(String path, String &value) {
	value = getString(path);
}

void FirebaseESP32::get(String path, bool &value) {
	value = getBool(path);
}

JsonVariant FirebaseESP32::get(String path) {
	return StaticJsonBuffer<FIREBASE_JSON_VALUE_BUFFER_SIZE>().parseObject(_http(path, "GET"));
}
// END of get

// Start of set
void FirebaseESP32::setInt(String path, int value) {
	set(path, (int)value);
}

void FirebaseESP32::setFloat(String path, float value, int point) {
	set(path, (float)value);
}

void FirebaseESP32::setString(String path, String value) {
	set(path, (String)value);
}

void FirebaseESP32::setBool(String path, bool value) {
	set(path, (bool)value);
}

void FirebaseESP32::set(String path, int value) {
	_http(path, "PUT", String(value));
}

void FirebaseESP32::set(String path, float value, int point) {
	_http(path, "PUT", String(value, point));
}

void FirebaseESP32::set(String path, String value) {
	String buf = "";
	JsonVariant(value.c_str()).printTo(buf);
	_http(path, "PUT", buf);
}

void FirebaseESP32::set(String path, bool value) {
	_http(path, "PUT", value ? "true" : "false");
}

void FirebaseESP32::set(String path, JsonVariant value) {
	String bufferJson = "";
	JsonObject& data = value;
	data.printTo(bufferJson);
	_http(path, "PUT", bufferJson);
}
// END of set

// Start of push
String FirebaseESP32::pushInt(String path, int value) {
	return push(path, value);
}

String FirebaseESP32::pushFloat(String path, float value, int point) {
	return push(path, value);
}

String FirebaseESP32::pushBool(String path, bool value) {
	return push(path, value);
}

String FirebaseESP32::pushString(String path, String value) {
	return push(path, value);
}

String FirebaseESP32::push(String path, int value) {
	return _pushValue(path, String(value));
}

String FirebaseESP32::push(String path, float value, int point) {
	return _pushValue(path, String(value, point));
}

String FirebaseESP32::push(String path, String value) {
	String buf;
	JsonVariant(value.c_str()).printTo(buf);
	return _pushValue(path, buf);
}

String FirebaseESP32::push(String path, bool value) {
	return _pushValue(path, value ? "true" : "false");
}

String FirebaseESP32::push(String path, JsonVariant value) {
	String bufferJson = "";
	JsonObject& data = value;
	data.printTo(bufferJson);
	return _pushValue(path, bufferJson);
}

String FirebaseESP32::_pushValue(String path, String data) {
	String rosJson = _http(path, "POST", data);
	if (failed()) return String();
	
	StaticJsonBuffer<FIREBASE_RETUEN_JSON_BUFFER_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(rosJson);
	if (!root.success()) {
		_errCode = 1;
		_errMsg = "firebase not respond json formant, parseObject() failed";
		return String();
	}
	
	if (!root.containsKey("name")) {
		_errCode = 2;
		_errMsg = "firebase not respond 'name' for object";
		return String();
	}
	
	return root["name"].as<String>();
}
// END of push

// Start of stream
void FirebaseESP32::stream(String path, StreamHandlerCallback callback) {
	FirebaseStreamCallback = callback;
	FirebaseStreamURL = genURL(path);
	
	xTaskCreatePinnedToCore([](void* param) {
		for (;;) {
			delay(5); // Disable WDT
				
			if (!FirebaseStreamHttp.connected()) {
				FirebaseStreamHttp.end();
				FirebaseStreamHttp.begin(FirebaseStreamURL);
				FirebaseStreamHttp.setTimeout(5000);
				FirebaseStreamHttp.addHeader("Accept", "text/event-stream");
				int httpCode = FirebaseStreamHttp.GET();
				if (httpCode != HTTP_CODE_OK) {
					Serial.println("Error !, Firebase stream fail: " + String(httpCode));
					continue;
				}
				FirebaseStreamSocket = FirebaseStreamHttp.getStreamPtr();
			}
			
			if (!FirebaseStreamSocket) continue;
			
			if (FirebaseStreamSocket->available()) {
				String line = FirebaseStreamSocket->readStringUntil(0x0A);
				if (line.startsWith("event:")) {
					FirebaseStreamEvent = line.substring(7, line.length());
					FirebaseStreamEvent.trim();
				} else if (line.startsWith("data:")) {
					FirebaseStreamDataJson = line.substring(6, line.length());
					FirebaseStreamDataJson.trim();
				} else if (line.length() == 0) {
					if (FirebaseStreamCallback) FirebaseStreamCallback(FirebaseStream(FirebaseStreamEvent, FirebaseStreamDataJson));
				}
			}
		}
		vTaskDelete(NULL);
		FirebaseStreamTaskHandle = NULL;
	}, "FirebaseStream_Task", FIREBASE_STREAM_TASK_BUFFER_SIZE, NULL, 3, &FirebaseStreamTaskHandle, ARDUINO_RUNNING_CORE);
	return;
}

void FirebaseESP32::stopStream() {
	if (FirebaseStreamTaskHandle) vTaskDelete(FirebaseStreamTaskHandle);
	if (FirebaseStreamHttp.connected()) {
		FirebaseStreamHttp.end();
	}
	FirebaseStreamCallback = NULL;
	FirebaseStreamEvent = "";
	FirebaseStreamDataJson = "";
}
// END of stream

// Start of remove
void FirebaseESP32::remove(String path) {
	_http(path, "DELETE");
}
// END of remove

int FirebaseESP32::failed() {
	return _errCode;
}

String FirebaseESP32::error() {
	return _errMsg;
}

String FirebaseESP32::_http(String path, String type, String payload) {
	HTTPClient http;
	http.begin(genURL(path));
	int httpCode = http.sendRequest(type.c_str(), payload);
	if (httpCode == HTTP_CODE_OK) {
		_errCode = 0;
		_errMsg = "";
	} else {
		_errCode = httpCode;
		_errMsg = http.errorToString(httpCode);
	}
	String ros = http.getString();
	return ros == "null" ? "" : ros;
}

String FirebaseESP32::genURL(String path) {
	String url = "https://" + _host + "/" + path + ".json";
	if (_auth.length() > 0) url += "?auth=" + _auth;
	return url;
}

FirebaseESP32 Firebase = FirebaseESP32();

#endif
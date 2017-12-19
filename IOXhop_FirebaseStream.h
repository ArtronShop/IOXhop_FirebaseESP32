#ifndef _IOXhop_FirebaseStream_H_INCLUDED
#define _IOXhop_FirebaseStream_H_INCLUDED

#include <Arduino.h>
#include <ArduinoJson.h>

#define STREAM_JSON_BUFFER_SIZE 1024
#define STREAM_JSON_DATA_BUFFER_SIZE 1024

class FirebaseStream {
	public:
		FirebaseStream(String event, String data) {
			_event = event;
			StaticJsonBuffer<STREAM_JSON_BUFFER_SIZE> jsonBuffer;
			JsonObject &root = jsonBuffer.parseObject(data);
			if (root.success()) {
				if (root.containsKey("path") && root.containsKey("data")) {
					_path = root["path"].as<String>();
					_data = root["data"].as<String>();
				}
			}
		}
		
		String getEvent() {
			return _event;
		}
		
		String getPath() {
			return _path;
		}
		
		int getDataInt() {
			return _data.toInt();
		}
		
		float getDataFloat() {
			return _data.toFloat();
		}
		
		String getDataString() {
			return _data;
		}
		
		float getDataBool() {
			return _data.indexOf("true") >= 0;
		}
		
		void getData(int &value) {
			value = getDataInt();
		}
		
		void getData(float &value) {
			value = getDataFloat();
		}
		
		void getData(String &value) {
			value = getDataString();
		}
		
		void getData(bool &value) {
			value = getDataBool();
		}
		
		JsonVariant getData() {
			return StaticJsonBuffer<STREAM_JSON_DATA_BUFFER_SIZE>().parseObject(_data);
		}
		
	private:
		String _event, _path, _data = "";
		bool _dataError = false, _begin = false;

}
;

#endif
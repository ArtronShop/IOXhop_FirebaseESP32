# IOXhop_FirebaseESP32
Get Set Push Stream value on Firebase via ESP32. the function compatible with [firebase-arduino](https://github.com/firebase/firebase-arduino) except stream feature and now Cloud Messaging not support.

## Device Support

This list device support and tested then worked.

 * NodeMCU-32S

## Needs
 * [ArduinoJson V5.13.3](https://github.com/bblanchon/ArduinoJson/releases/tag/v5.13.3)

## Features

### Get

Read value from object in firebase. And support data type `int` `float` `String` `bool` and `JsonObject` via function `type get[type](String path)` or `void get(String path, [type] &value)`

```c++
int getInt(String path) ;
float getFloat(String path) ;
String getString(String path) ;
bool getBool(String path) ;
void get(String path, int &value) ;
void get(String path, float &value) ;
void get(String path, String &value) ;
void get(String path, bool &value) ;
JsonVariant get(String path) ;
```

**Example**

```c++
digitalWrite(LED_PIN, Firebase.getInt("/led"));
```

### Set

Set value in object. Support data type `int` `float` `String` `bool` and `JsonObject` via function `void set[type](String path, [type] value)` or `void set(String path, [type] value)`

```c++
void setInt(String path, int value) ;
void setFloat(String path, float value, int point = 2) ;
void setString(String path, String value) ;
void setBool(String path, bool value) ;
void set(String path, int value) ;
void set(String path, float value, int point = 2) ;
void set(String path, String value) ;
void set(String path, bool value) ;
void set(String path, JsonVariant value) ;
```

**Example**

```c++
Firebase.set("/led", digitalRead(BTN1));
```

### Push

If you use push. The value add to child node in object. Support data type `int` `float` `String` `bool` and `JsonObject` via function `void push[type](String path, [type] value)` or `void push(String path, [type] value)`

```c++
String pushInt(String path, int value) ;
String pushFloat(String path, float value, int point = 2) ;
String pushBool(String path, bool value) ;
String pushString(String path, String value) ;
String push(String path, int value) ;
String push(String path, float value, int point = 2) ;
String push(String path, String value) ;
String push(String path, bool value) ;
String push(String path, JsonVariant value) ;
```

**Example**

```c++
Firebase.push("/log/temp", dht11.read());
```

### Stream

Real time get event and data update in some object. In esp32 support multitasking via FreeRTOS so in `void loop()` not have `Firebase.available()`. If new event or data update the **callback function** auto cell and you can get detail via FirebaseStream class

```c++
void stream(String path, StreamHandlerCallback callback) ;
void stopStream() ;
```

**FirebaseStream class**

```c++
String getEvent() ;
String getPath() ;
int getDataInt() ;
float getDataFloat() ;
String getDataString() ;
float getDataBool() ;
void getData(int &value) ;
void getData(float &value) ;
void getData(String &value) ;
void getData(bool &value) ;
JsonVariant getData() ;
```

**Example**

```c++
Firebase.stream("", [](FirebaseStream stream) {
  if (stream.getEvent() == "put" && stream.getPath() == "/led") {
    digitalWrite(LED_PIN, stream.getDataInt());
  }
});
```

## Full Example

* [FirebaseDemo_ESP32](https://github.com/ioxhop/IOXhop_FirebaseESP32/blob/master/examples/FirebaseDemo_ESP32/FirebaseDemo_ESP32.ino)
* [FirebaseNeoPixel_ESP32](https://github.com/ioxhop/IOXhop_FirebaseESP32/blob/master/examples/FirebaseNeoPixel_ESP32/FirebaseNeoPixel_ESP32.ino)
* [FirebaseRoom_ESP32](https://github.com/ioxhop/IOXhop_FirebaseESP32/blob/master/examples/FirebaseRoom_ESP32/FirebaseRoom_ESP32.ino)
* [FirebaseStream_ESP32](https://github.com/ioxhop/IOXhop_FirebaseESP32/blob/master/examples/FirebaseStream_ESP32/FirebaseStream_ESP32.ino)

## License
This library open source. Copied, distributed for free

 * [IOXhop.com](http://www.ioxhop.com/)

#include <SoftwareSerial.h>
#include <stdlib.h>
#include <SoftwareSerial.h>
#include "DHT.h" //cargamos la librería DHT
#define DHTPIN 2 //Seleccionamos el pin en el que se //conectará el sensor
#define DHTTYPE DHT11 //Se selecciona el DHT11 (hay //otros DHT)
DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que será usada por Arduino para comunicarse con el sensor

#define SSID "HITRON-A540"
#define PASS "0CFJUE6O659M"
//#define IP "184.106.153.149" // thingspeak.com
String apiKey = "5KUQKCVAS5JDS5KU";  //thingspeak API key
String GET = "GET /update?key=5KUQKCVAS5JDS5KU&field1=";

SoftwareSerial mySerial(10, 11); // RX, TX
String command = ""; // Stores response of the HC-06 Bluetooth device
String getstr;
String data; // the data of the smart band + linking device to be sent to the cloud

void setup()
{
  Serial.begin(9600);
  Serial.println("AT");
  
  // The HC-06 defaults to 9600 according to the datasheet.
  mySerial.begin(9600);
 
  if(Serial.find("OK")){
    connectWiFi();
  }
}

void loop(){
  
//******************************* Reading smart band data if is available.
  if (mySerial.available()) {
    while(mySerial.available()) { // While there is more to be read, keep reading.
      command = (char)mySerial.read();
    }
    
    //Serial.print(command);
    data= String(command);
    data+= getstr;
    Serial.print(data);
    command = ""; // No repeats
  }

    
  // Read user input if available.
  /*if (Serial.available()){
    delay(10); // The delay is necessary to get this working!
    mySerial.write(Serial.read());
  }*/
  
//************************************************** DHT11 sensor reading
  float h = dht.readHumidity(); //Se lee la humedad
  float t = dht.readTemperature(); //Se lee la temperatura

//*************************************************sensor readings convert to string
  char buf[16];  // convert to string
 
  String strTemp = dtostrf(t, 4, 1, buf);
  String strHum = dtostrf(h, 4, 1, buf);
  //updateTemp(strTemp);
  //************************************************ TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  //cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "192.168.0.13"; // moonki
  //cmd += "\",80";
  cmd += "\",5000";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }

// two methods to upload through URL          https://api.thingspeak.com/update.json?api_key=5KUQKCVAS5JDS5KU&field1=58&field2=23            https://api.thingspeak.com/update?api_key=5KUQKCVAS5JDS5KU&field1=0&field2=0
  //******************************************************* prepare GET string
  
  String getStr = "GET /test?api_key=5KUQKCVAS5JDS5KU";
 // getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strTemp);
  getStr +="&field2=";
  getStr += String(strHum);
  getStr += "\r\n\r\n";

  // send data length
 cmd = "AT+CIPSEND=";
 cmd += String(getStr.length());
 //cmd += getStr;                 //****************  able this line if you are not using ESP8266 to transmit, this option transmit cmd without the need of receiving <
  Serial.println(cmd);
  delay(2000);

//Serial.print(getStr);
delay(2000);
 if(Serial.find(">")){
    Serial.print(getStr);
  }
 else{
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
  
  delay(16000);  // thingspeak needs 15 sec delay between updates
}

 
boolean connectWiFi(){
  Serial.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  delay(5000);
  if(Serial.find("OK")){
    return true;
  }else{
    return false;
  }
}

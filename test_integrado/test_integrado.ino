#include <SoftwareSerial.h>
#include <stdlib.h>
#include <SoftwareSerial.h>
#include "DHT.h" //cargamos la librería DHT
#define DHTPIN 2 //Seleccionamos el pin en el que se //conectará el sensor
#define DHTTYPE DHT11 //Se selecciona el DHT11 (hay //otros DHT)
DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que será usada por Arduino para comunicarse con el sensor

#define SSID "Casa7"
#define PASS "Kishi12345"
//String SERVER = "192.168.1.91";
String SERVER = "moonki.herokuapp.com";
String GET = "GET /api/save?";

SoftwareSerial mySerial(10, 11); // RX, TX
String command = ""; // Stores response of the HC-06 Bluetooth device
String getstr;
String data; // the data of the smart band + linking device to be sent to the cloud

void setup()
{
  Serial.begin(115200);
  Serial.println("AT");
  
  // The HC-06 defaults to 9600 according to the datasheet.
  mySerial.begin(9600);
 
  if(Serial.find("OK")){
    connectWiFi();
  }
}

void loop(){
  command = "";
  data= "";
//******************************* Reading smart band data if is available.
  if (mySerial.available()) {
    while(mySerial.available()) { // While there is more to be read, keep reading.
      command += mySerial.read();
    }
    
    //Serial.print(command);
    //data= command;
    //data+= getstr;
    //Serial.print(data);
    //command = ""; // No repeats
  }
  else {
    Serial.print("Puerto no disponible");
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

  //************************************************ TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\""; 
  cmd += SERVER;
  cmd += "\",80";

  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }

  //**************************************************** Waiting for Band data
while(!mySerial.available()){}

while(mySerial.available()) { // While there is more to be read, keep reading.
  data += mySerial.read();
}



  //******************************************************* prepare GET string
  
  String getStr = GET;
  getStr +="temperatura=";
  getStr += String(strTemp);
  getStr +="&humedad=";
  getStr += String(strHum);
  getStr +="&banda=";
  getStr += String(data);
  getStr += "\r\n\r\n";

 Serial.print(getStr);

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

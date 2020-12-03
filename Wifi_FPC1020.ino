#include <WiFi.h>
#include <SPI.h>
#include "FPC1020.h"

const char* ssid     = "ESP_TEST";
const char* password = "12345678";
const char *server = "113.161.152.35";

String endpoint = "/api/User/fingerimage";
String data_image = "{\n";
FPC1020 fpc;
extern unsigned char rBuf[36864];
String Take_image();
void Post_data(String data_post);

void setup()
{
  Serial.begin(500000);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  fpc.reset();
  fpc.init();
  delay(10);
  Serial.println("FPC1020 test");
  fpc.setup();
  Serial.println();  
}

void loop()
{
  Take_image();
  
//  if( c!="")
//  {
////    Serial.println(c);
//    ESP.getFreeHeap();
//    Post_data(c);
//    }

//  if( c!="")
//  {
//    ESP.getFreeHeap();
//
//  data_image = "{";
//  data_image += "\"ImageText\" : \"";
//  data_image += c;
//  data_image += "\""; 
//  data_image += ("}\n"); 
//  c="";
////   Serial.println(data_image);
//   
//WiFiClient client;
//String PostData = String("{\"ImageText\":\"test_choi\"}");
//if (client.connect(server, 2030)) 
//  {
//    Serial.println("Connected to server");
//    // Make a HTTP request
////    Serial.println(data_image);
//    client.println("POST " + endpoint +" HTTP/1.1");
//    client.println("Host: " + (String)server);
//    client.println("Connection: close"); 
//    client.println("Content-Type: application/json");
//    client.println("User-Agent: Arduino/1.0");
//    client.print("Content-Length: ");
//    client.println(data_image.length());
//    client.println();
//    client.println(data_image);
//    delay(500); 
//    }
// else
// {
//  ESP.restart();
// }
//    
//  // Read all the lines of the reply from server and print them to Serial
//  while (client.available()) {
//    String line = client.readStringUntil('\r');
//    Serial.print(line);
//  }
//
//  Serial.println();
//  Serial.println("closing connection");
//  client.stop();
// }
}

String Take_image()
{
    String a = "";
    String b = "";
//    Serial.printf("Hardware ID: 0x%04X\n", fpc.hardware_id());
    fpc.command(FPC102X_REG_FINGER_PRESENT_QUERY); //0X20
    fpc.command(FPC102X_REG_WAIT_FOR_FINGER);//0X24
    delay(10);

    uint8_t interrupt = fpc.interrupt(true);

//    Serial.printf("Interrupt status: 0x%02X\n", interrupt);
    int thoi_gian = millis();
    if (interrupt == 0x81)
    {
            fpc.command(FPC1020X_REG_CAPTURE_IMAGE); //0X20
            delay(20);
            uint8_t interrupt = fpc.interrupt(true);
            Serial.printf("Interrupt status: 0x%02X\n", interrupt);
            delay(20); 
            
            if (interrupt == 0x20)
            { 
              fpc.capture_image();
                
              Serial.print("thoi gian chay for: ");
              Serial.println(millis()-thoi_gian);   
              Serial.println("DONE!!");
               for(word i=1; i<10001; i++)
              {
//                a+=rBuf[i];
                if(i<1000)
                {a+=rBuf[i];}
                else if(i>=1000){
                  {b+=rBuf[i];}
                  }
              }
              memset(rBuf, 0, sizeof(rBuf));
              Serial.print("getFreeHeap:");
              Serial.println(ESP.getFreeHeap());
              Post_data(a);
              a="";
              Post_data(b);
              b="";
            }     
     }
     
     delay(10);
//     return a;
  }

void Post_data(String data_post)
{
  data_image = "{";
  data_image += "\"ImageText\" : \"";
  data_image += data_post;
  data_image += "\""; 
  data_image += ("}\n"); 
  data_post = "";
  int data_length = data_image.length();
  Serial.print("data_length:");
  Serial.println(data_length);
  WiFiClient client;
if (client.connect(server, 2030)) 
  {
  Serial.println(F("Connected to server"));
    // Make a HTTP request
//    Serial.println(data_image);
    client.println("POST " + endpoint +" HTTP/1.1");
    client.println("Host: " + (String)server);
    client.println("Connection: close"); 
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(data_length);
    client.println();
    client.println(data_image.c_str());
    delay(500);
  }   
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  client.stop();
} 

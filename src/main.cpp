#include <WiFi.h>
#include <EEPROM.h>
#include <PubSubClient.h>

String ssid;
String pwd;
String ms;
String topic;
int t;



WiFiClient espClient;
PubSubClient client(espClient);

struct change {
  String E_ssid, E_pwd, E_ip, E_topic;
};

change acc;

String line() 
{
  String str;
  while (1) {
    if (Serial.available()) 
    {
      str = Serial.readString();
      break;
    }
  }
  return str;
}

void arrange()
{
  EEPROM.get(0,acc);
  ms=acc.E_ip;
  pwd=acc.E_pwd;
  topic=acc.E_topic;
  ssid=acc.E_ssid;
}


void set() {
  acc.E_ssid = ssid;
  acc.E_pwd = pwd;
  acc.E_ip = ms;
  acc.E_topic = topic;
  EEPROM.put(0, acc);
  EEPROM.commit();
  arrange();
}

void server() {
  Serial.print("Enter Your Broker IP : ");
  ms = line();
  Serial.println(ms);
  Serial.print("Enter Topic : ");
  topic = line();
  Serial.println(topic);
}


void wifi() 
{
  WiFi.disconnect(true);
  Serial.print("Enter Your Hotspot name : ");
  ssid = line();
  Serial.println(ssid);
  Serial.print("Enter You Password : ");
  pwd = line();
  Serial.println(pwd);
  WiFi.begin(ssid, pwd);
  set();
}

void value() {


   Serial.println("-----------------------------------------------------------------------------");
  if (EEPROM.read(0) == 255) {
    Serial.println("EEPROM is empty please enter the all value manually");
    wifi();
    server();
    set();
    Serial.println("-----------------------------------------------------------------------------");
  }

  EEPROM.get(0, acc);

  Serial.println("Hotspot name : " + acc.E_ssid);
  Serial.println("Password : " + acc.E_pwd);
  Serial.println("Borker IP : " + acc.E_ip);
  Serial.println("Topic name : " + acc.E_topic);
}






void pref() {
  String choice;
  Serial.println("If you want change Value Press: 1\nDon't wanna change Press any key");
  choice = line();
  if (choice == "1") 
  {
    wifi();
    server();
    set();

  } 
  else
    return;
}




void connect() {
  while (!client.connected())
    // {
    //    if (client.connect("Barani")) {
    //     Serial.println("Connected to broker");
    //     client.subscribe(topic);
    //   }


    client.connect("Barani", "bharani", "1234");
  client.subscribe(topic.c_str());
  Serial.println("Client connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  WiFi.mode(WIFI_STA);
  value();
  pref();
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(2000);
    t++;
    if (t == 5) 
    {
  
      Serial.println();
      wifi();
      t = 0;
    }
  }
  Serial.println();
  Serial.println("Connected");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
  set();
  client.setServer(ms.c_str(), 1883);
  client.setCallback(callback);
}

void loop() {

  String val;
  if (!client.connected())
    connect();
  // Serial.println("Enter the Publish string");
  if (Serial.available()) {
    val = Serial.readString();
  }
  if (val.length() != 0) {
    client.publish(topic.c_str(), val.c_str());
  }
  client.loop();

  delay(100);
}

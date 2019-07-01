// Load Wi-Fi library
#include <WiFi.h>
#include <control.cpp>
#include <Arduino.h>
#include <ESPmDNS.h>

// Replace with your network credentials
// const char* ssid     = "pendulum";
// const char* password = "cicacicacica";
const char* ssid     = "soda_and_code";
const char* password = "5GmO2vpSVau8";

const char* name     = "pen-motor";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
String strParam;

// Set your Static IP address
/*
IPAddress local_IP(192, 168, 123, 100);
IPAddress gateway(192, 168, 123, 1);
IPAddress subnet(255, 255, 255, 0);
*/

// driver parameters
const int DRVNR = 4;
int drv_period_t[DRVNR] = {0, 0, 0, 0};
int drv_pull_t[DRVNR] = {0, 0, 0, 0};
int drv_pull_f[DRVNR] = {0, 0, 0, 0};
int drv_hold_t[DRVNR] = {0, 0, 0, 0};
int drv_hold_f[DRVNR] = {0, 0, 0, 0};
int drv_rew_t[DRVNR] = {0, 0, 0, 0};
int drv_rew_f[DRVNR] = {0, 0, 0, 0};

int drv_start[DRVNR] = {0, 0, 0, 0};

Control control = Control();

void setup()
{
  // serial console
  Serial.begin(115200);

    // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // set IP address
  /*
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  */

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin(name)) {
    Serial.println("Error setting mdns responder!");
  }

  server.begin();
}

String arr_to_str(int *arr)
{
  String res = "";

  for (int i = 0; i < DRVNR; i++)
  {
    res += String(arr[i]);
    if (i < DRVNR - 1)
      res += ",";
  }

  return res;
}

void parse_params(int *arr, String param, int len)
{
  for (int i = 0; i < len; i++)
  {
    arr[i] = param.toInt();
    int idx = param.indexOf(",");
    if (idx > 0)
      param = param.substring(idx + 1);
  }
}

void set_param(int *arr, String param)
{
  parse_params(arr, param, DRVNR);
}

void set_pendulum_param(String param)
{
  // /set/pendulum/[id],[period-t],[pull-t],[pull-f],[hold-t],[hold-f],[rew-t],[rew-f],[start]
  int params[9] = {0,0,0,0,0,0,0,0,0};

  parse_params(params, param, 9);
  int pid = params[0]; // pendulum id
  if (pid >= DRVNR) return;
  drv_period_t[pid] = params[1];
  drv_pull_t[pid]   = params[2];
  drv_pull_f[pid]   = params[3];
  drv_hold_t[pid]   = params[4];
  drv_hold_f[pid]   = params[5];
  drv_rew_t[pid]    = params[6];
  drv_rew_f[pid]    = params[7];
  drv_start[pid]    = params[8];
}

String get_param_from_header(String hdr, String path)
{
  String strP = "";
  int plen = path.length();
  if (hdr.indexOf("GET " + path) >= 0)
  {
    int getIndex = hdr.indexOf("GET " + path);
    int getEndIndex = hdr.indexOf(" HTTP", getIndex);
    strP = hdr.substring(getIndex + 4 + plen, getEndIndex);
  }
  return strP;
}

void loop()
{
  WiFiClient client = server.available();

  //control.tick();

  if (client)
  {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        // Serial.write(c);
        header += c;
        if (c == '\n')
        {
          // \n\n means end of request header, send response
          if (currentLine.length() == 0)
          {
            // send headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type: application/json");
            client.println("Access-Control-Allow-Origin: *"); // CORS
            client.println("Connection: close");
            client.println();

            // set parameters
            strParam = get_param_from_header(header, "/set/period_t/");
            if (strParam != "")
              set_param(drv_period_t, strParam);
            strParam = get_param_from_header(header, "/set/pull_t/");
            if (strParam != "")
              set_param(drv_pull_t, strParam);
            strParam = get_param_from_header(header, "/set/pull_f/");
            if (strParam != "")
              set_param(drv_pull_f, strParam);
            strParam = get_param_from_header(header, "/set/hold_t/");
            if (strParam != "")
              set_param(drv_hold_t, strParam);
            strParam = get_param_from_header(header, "/set/hold_f/");
            if (strParam != "")
              set_param(drv_hold_f, strParam);
            strParam = get_param_from_header(header, "/set/rew_t/");
            if (strParam != "")
              set_param(drv_rew_t, strParam);
            strParam = get_param_from_header(header, "/set/rew_f/");
            if (strParam != "")
              set_param(drv_rew_f, strParam);
            strParam = get_param_from_header(header, "/set/start/");
            if (strParam != "")
              set_param(drv_start, strParam);
            strParam = get_param_from_header(header, "/set/pendulum/");
            if (strParam != "")
              set_pendulum_param(strParam);

            client.println("{\"params\": {");

            client.print("  \"period_t\": [");
            client.print(arr_to_str(drv_period_t));
            client.println("],");
            client.print("  \"pull_t\": [");
            client.print(arr_to_str(drv_pull_t));
            client.println("],");
            client.print("  \"pull_f\": [");
            client.print(arr_to_str(drv_pull_f));
            client.println("],");
            client.print("  \"hold_t\": [");
            client.print(arr_to_str(drv_hold_t));
            client.println("],");
            client.print("  \"hold_f\": [");
            client.print(arr_to_str(drv_hold_f));
            client.println("],");
            client.print("  \"rew_t\": [");
            client.print(arr_to_str(drv_rew_t));
            client.println("],");
            client.print("  \"rew_f\": [");
            client.print(arr_to_str(drv_rew_f));
            client.println("],");
            client.print("  \"start\": [");
            client.print(arr_to_str(drv_start));
            client.println("]");

            client.println("}}");

            control.receivedMessage(drv_period_t, drv_pull_t, drv_pull_f, drv_hold_t, drv_hold_f, drv_rew_t, drv_rew_f, drv_start);

            // \n\n end of response
            client.println();
            // exit the loop
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }

    header = "";

    // close connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

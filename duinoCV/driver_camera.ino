const char* WIFI_SSID = "len12-75";
const char* WIFI_PASS = "doc12345";

#define flash_light_pin = 4



void handleBmp()
{
  /*
  if (!esp32cam::Camera.changeResolution(vgaRes)) {
    console_print("SET-VGA-RES FAIL");
  }*/

  static auto res  = esp32cam::Resolution::find(96, 96);
  if (!esp32cam::Camera.changeResolution(res  )) {
    console_print("SET-VGA-RES FAIL");
  }
  

  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    console_print("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  console_print("CAPTURE OK " +  String(frame->getWidth()) + " " + String(frame->getHeight()) + " " +
                String(static_cast<int>(frame->size())));

  if (!frame->toBmp()) {
    console_print("CONVERT FAIL");
    server.send(503, "", "");
    return;
  }
  console_print("CONVERT OK " + String(frame->getWidth()) + " " + String(frame->getHeight()) + " " +
                String(static_cast<int>(frame->size())));

  server.setContentLength(frame->size());
  server.send(200, "image/bmp");
  WiFiClient client = server.client();
  frame->writeTo(client);
}


void serveJpg()
{
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    console_print("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  console_print("CAPTURE OK " + String(frame->getWidth()) + " " + String(frame->getHeight()) + " " +
                String(static_cast<int>(frame->size())));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgHi()
{
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    console_print("SET-HI-RES FAIL");
  }
  serveJpg();
}

void handleJpg()
{
  server.sendHeader("Location", "/cam-hi.jpg");
  server.send(302, "", "");
}

void handleMjpeg()
{
  if (!esp32cam::Camera.changeResolution(vgaRes)) {
    console_print("SET-HI-RES FAIL");
  }

  console_print("STREAM BEGIN");
  WiFiClient client = server.client();
  auto startTime = millis();
  int res = esp32cam::Camera.streamMjpeg(client);
  if (res <= 0) {
    console_print("STREAM ERROR " + String(res));
    return;
  }
  auto duration = millis() - startTime;
  console_print("STREAM END " +  String(res) + " " +  String(1000.0 * res / duration));
}

void setup_camera()
{
  Serial.begin(115200);
  {
    esp32cam::Config cfg;
    cfg.setPins(esp32cam::pins::AiThinker);
    cfg.setResolution(cvRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(cvRes_quality);

    bool ok = esp32cam::Camera.begin(cfg);
    console_print(ok ? "CAMERA OK" : "CAMERA FAIL");

    if(!ok){
      console_print("");
      console_print("Reseting...");
      delay(2000);
      ESP.restart();
    }
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  console_print("http://" + ip2Str(WiFi.localIP()));
  console_print("  /cam.bmp");
  console_print("  /cv.bmp");
  console_print("  /cv_nofilter.bmp");
  console_print("  /cam.jpg");
  console_print("  /cam.mjpeg");

  server.on("/cam.bmp",           handleBmp);
  server.on("/cv.bmp",            handleCV);
  server.on("/cv_nofilter.bmp",   handleCV_NF);
  server.on("/cam.jpg",        handleJpgHi);
  server.on("/cam.mjpeg",         handleMjpeg);

  server.begin();
}

String ip2Str(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++) {
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  }
  return s;
}

void loop_server()
{
  server.handleClient();
}


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                      GET PIXEL                                    # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                       CAPTURE                                     # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

std::unique_ptr<esp32cam::Frame> capture_CV_Frame(){
  esp32cam::Camera.changeResolution(cvRes);
  std::unique_ptr<esp32cam::Frame> frame = esp32cam::capture();  
  frame->toBmp();
  return frame;
}

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                         LOOP                                      # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/


void cv_diplay_camera_loop(){
  std::unique_ptr<esp32cam::Frame> frame = capture_CV_Frame();  
  display_drawFrame(frame->data());
}


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                     CV WEBSERVER                                  # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/


void handleCV()
{
  
  auto frame = capture_CV_Frame();
  console_print("CAPTURE OK " + String(frame->getWidth()) + " " + String(frame->getHeight()) + " " +
                String(static_cast<int>(frame->size())));

  server.setContentLength(frame->size());
  server.send(200, "image/bmp");
  WiFiClient client = server.client();

  
  //filter_contrast_blackWhite(frame->data());
  //filter_inverse(frame->data());

  frame->writeTo(client);
}

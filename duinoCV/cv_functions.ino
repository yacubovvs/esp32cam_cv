
/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                       CAPTURE                                     # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

std::unique_ptr<Frame> capture_CV_Frame(){
  std::unique_ptr<Frame> frame = capture();  
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
  std::unique_ptr<Frame> frame = capture_CV_Frame();  
  cv_applyFilters(frame->data());
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

  cv_applyFilters(frame->data());

  frame->writeTo(client);

  //delete(frame->data());
}

void handleCV_NF()
{
  
  auto frame = capture_CV_Frame();
  console_print("CAPTURE OK " + String(frame->getWidth()) + " " + String(frame->getHeight()) + " " +
                String(static_cast<int>(frame->size())));

  server.setContentLength(frame->size());
  server.send(200, "image/bmp");
  WiFiClient client = server.client();

  //cv_applyFilters(frame->data());

  frame->writeTo(client);

  //delete(frame->data());
}

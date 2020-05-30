



void cv_loop(){
  std::unique_ptr<esp32cam::Frame> frame = captureFrame();  
  frame->toBmp();
  display_drawFrame(frame->data());
}

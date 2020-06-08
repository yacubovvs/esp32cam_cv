#ifdef controls_enable

  #ifdef controls_btn1
    bool btn1_on = true;
  #endif
  #ifdef controls_btn2
    bool btn2_on = true;
  #endif
  

  void controls_setup(){
    #ifdef controls_btn1
      pinMode(controls_btn1, INPUT_PULLUP);
    #endif
    #ifdef controls_btn2
      pinMode(controls_btn2, INPUT_PULLUP);
    #endif
  }
  
  void controls_loop(){
    #ifdef controls_btn1
      if(!digitalRead(controls_btn1)){
        if(!btn1_on){
          #ifdef display_camera_enable
            if(drawFrameEnable(!drawFrameEnable())){
              console_print("Camera image is shown");
            }else{
              console_print("Camera image is hidden");
            };
          #endif
        }
        btn1_on = true;    
      }else{
        btn1_on = false;
      }
    #endif

    #ifdef controls_btn2
      if(digitalRead(controls_btn2)){
        
      }else{
        
      }
    #endif
  }
  
#endif

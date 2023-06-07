// simple anti-bounce button connected between controller input and GND
// uses internal or external PULLUP resistor

class Button{
private:
  byte pin_;
  uint32_t timer_;
  bool flag_;
public:
  Button(byte pin) : pin_(pin){
      pinMode(pin_, INPUT_PULLUP);
  }
  bool click(){
    bool btn_state = digitalRead(pin_);
    if (!flag_ && !btn_state && millis()-timer_ > 200){
      flag_ = true;
      timer_ = millis();
      return true;
    }
    if (flag_ && !btn_state && millis()-timer_ > 500){
      timer_ = millis();
      return true;
    }
    if (flag_ && btn_state){
      flag_ = false;
      timer_ = millis();
    }
    return false;
  }
};

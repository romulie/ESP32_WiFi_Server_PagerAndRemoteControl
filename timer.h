class Timer{
private:
  uint32_t period_ = 0, time_ = 0;
public:
  Timer(uint32_t period = 0) : period_(period) {};  
  void setPeriod(uint32_t period = 0) { period_ = period; }; 
  bool ready(){
    if (period_ !=0 && millis() - time_ >= period_){
        time_ = millis();
        return true;
      }
      return false;
    }
};

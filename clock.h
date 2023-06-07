
class Clock{
private:
  byte secs = 0, mins = 0, hours = 0; 
public: 
  Clock() = default;
  Clock(byte secs_, byte mins_, byte hours_) : secs(secs_), mins(mins_), hours(hours_) {};
  void tick(){
    ++secs;
    if (secs == 60)   { ++mins; secs = 0; }
    if (mins == 60)   { ++hours; mins = 0; }
    if (hours == 24)  { hours = 0; }
  }
  void reset()            { secs = 0; mins = 0; hours = 0; }
  byte getSeconds() const { return secs; }
  byte getMinutes() const { return mins; }
  byte getHours() const   { return hours; }
  
};

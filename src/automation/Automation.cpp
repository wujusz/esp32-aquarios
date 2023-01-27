#include "Automation.h"

Automation::Automation()
        {           
          _validNTP = false;
          pinMode(LED, OUTPUT);
        };

SystemRestart Automation::getSystemRestart(time_t t_now){
  struct tm *lt = localtime(&t_now);
  lt->tm_hour = 0;
  lt->tm_min = 0;
  lt->tm_sec = 0;

  time_t midNightToday = mktime(lt);
  SystemRestart restart;
  //restartTime set to one hour after midnight
  restart.restartTime = TWENTY_FOUR_HOUR_DURATION + midNightToday - t_now + ONE_HOUR_DURATION;
  restart.wekDay = lt->tm_wday;
  return(restart);
}

void Automation::staticTickerCallbackRestartSystemNow(Automation *pThis)
{
    pThis->restartSystemNow();
}

void Automation::restartSystemNow() {
    WiFi.disconnect(true);
    delay(500);
    ESP.restart();
  }

void Automation::ntpSearch(){
  Serial.println(F("START stpSearch"));
    _blinkerHeartBeat.attach(0.125, &Automation::staticTickerCallbackChangeState, this);
}

void Automation::staticTickerCallbackChangeBrightness(Automation *pThis)
{
  Serial.println(F("Automation::staticTickerCallbackChangeBrightness"));
  // pThis->changeBrightness();
}

void Automation::setupBrightness(uint32_t channelId, uint32_t controlPin)
{
  Serial.println(F("Automation::setupBrightness"));
  Serial.print("setupBrightness: ch: ");
  Serial.print(channelId);
  Serial.print(" PIN: ");
  Serial.println(controlPin);
    // Setup timer and attach timer to a led pin
  // ledcSetup(controlPin, DEFAULT_BRIGHTNESS_FREQ, DEFAULT_BRIGHTNESS_BIT);
  // ledcAttachPin(controlPin, channelId);
}

void Automation::changeBrightness(uint8_t channelId, uint32_t value, uint32_t valueMax = 255) {
  uint32_t brightness = (int)(value * 2.55);
  // calculate duty, 4095 from 2 ^ 12 - 1
  uint32_t duty = (4095 / valueMax) * min(brightness, valueMax);

  Serial.println(F("Automation::changeBrightness"));
  // write duty to LEDC
  // ledcWrite(channelId, duty);
}

void Automation::staticTickerCallbackChangeState(Automation *pThis)
{
    pThis->changeState();
}

void Automation::changeState()
{
  digitalWrite(LED, !(digitalRead(LED)));
}

void Automation::staticTickerCallbackTurnLedOff(Automation *pThis)
{
    pThis->turnLedOff();
}

void Automation::turnLedOff(){
  Serial.println(F("Automation::turnLedOff"));
  digitalWrite(LED, LED_OFF);
}

void Automation::staticTickerCallbackTurnLedOn(Automation *pThis)
{
    pThis->turnLedOn();
}

void Automation::turnLedOn(){
  Serial.println(F("AUtomation::TURNLEDON"));
  digitalWrite(LED, LED_ON);
  _blinkerHeartBeatOff.once(0.125, &Automation::staticTickerCallbackTurnLedOff, this);
}

void Automation::setSchedules(std::list<ScheduleTask>* scheduleTaskList){
    // check to see if NTP updated the local time
    if(!_validNTP){
        time_t t_now = time(nullptr);
        String dateText = ctime(&t_now); 
        int year = dateText.substring(dateText.lastIndexOf(" ")+1).toInt();
        if(year > 1970){
          _validNTP = true;
          _blinkerHeartBeat.detach();
          _blinkerHeartBeat.attach(2.0, &Automation::staticTickerCallbackTurnLedOn, this);    
          for(std::list<ScheduleTask>::iterator i = scheduleTaskList->begin(); i != scheduleTaskList->end();)
            {
              i->channelTaskScheduler->setSchedule();
              i->channelTaskScheduler->setToggleSwitch(
                i->bToggleSwitch,
                i->toggleReadPin,
                i->blinkLed,
                i->ledOn);
              i++;
            }
          SystemRestart restart = getSystemRestart(t_now);
          if(restart.wekDay == 0 && restart.restartTime > 0) {
            //schedule reset the system one hour after midnight every sunday
            _restartTicker.once(restart.restartTime , &Automation::staticTickerCallbackRestartSystemNow, this);
          }
        }
    }
}

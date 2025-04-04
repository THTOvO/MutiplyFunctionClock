
#include"Includes.h"



void timeTick();
void TimeTask(void *w){
  timeTick();
  vTaskDelete(NULL);
}
TaskHandle_t handle=NULL;
TaskHandle_t InputHandle=NULL;
//主函数
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();
  pinMode(Button1,INPUT_PULLUP);
  pinMode(Button2,INPUT_PULLUP);
  pinMode(Button3,INPUT_PULLUP);
  pinMode(Button4,INPUT_PULLUP);
  pinMode(JoyStick_X,INPUT);
  pinMode(JoyStick_Y,INPUT);
  xTaskCreate(TimeTask,"Time",10000,NULL,1,&handle);
  xTaskCreate(inputHandle,"Input",10000,NULL,2,&InputHandle);
  Displays.init();
  Displays.setRotation(0);
  Displays.fillScreen(TFT_BLACK);
  Displays.setTextColor(TFT_WHITE);
  Displays.setTextSize(1);
  Displays.setCursor(0,0);     

  CurrentAPP=new WelcomeAPP();
  CurrentAPP->Enter();
}

void loop() {
  // put your main code here, to run repeatedly:
  CurrentAPP->RunLogic();
  if(CurrentAPP->Code=='w' && CurrentAPP->QuitToAPP==NULL){
    ((WelcomeAPP*)CurrentAPP)->QuitToAPP=new SelectionAPP();
  }
  else if(CurrentAPP->Code=='g' && CurrentAPP->QuitToAPP==NULL){
    ((SnakeAPP*)CurrentAPP)->QuitToAPP=new SelectionAPP();
  }
  else if(CurrentAPP->Code=='s' && CurrentAPP->QuitToAPP==NULL){
    ((SettingAPP*)CurrentAPP)->QuitToAPP=new SelectionAPP();
  }else if(CurrentAPP->Code='C' && CurrentAPP->QuitToAPP==NULL){
    ((CountTime*)CurrentAPP)->QuitToAPP=new SelectionAPP();
  }
  if(SwitchAPP!=NULL){
    CurrentAPP->Quit();
    delete CurrentAPP;
    SwitchAPP->Enter();
    CurrentAPP=SwitchAPP;
    SwitchAPP=NULL;
  }
}

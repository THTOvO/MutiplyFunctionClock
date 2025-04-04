#include<DHT.h>
#include<string>
#include"TFT_eSPI.h"
#include"SPI.h"
#include <Arduino.h>
#include<cstdio>
using namespace std;
#define DisplayWidth 128
#define DisplayHeight 160
//显示相关
//int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst
//#define CS 15
///#define DC 2
//#define MOSI 23   //SDA接口相当于
//#define SCLK 18
//#define RST 4
TFT_eSPI Displays= TFT_eSPI(DisplayWidth,DisplayHeight);
TFT_eSprite Avater=TFT_eSprite(&Displays);
//时间处理相关（我不理解，单独time头文件不能使用，放到别的地方使用就可以使用FreeRtos）
int Seconds=0;
int Minutes=0;
int Hours=12;
float Humidity=0;
float Temperature=0; // Removed duplicate definition
DHT dht(5,DHT11);
void timeTick(){
    while(true){
        Seconds+=1;
        if(Seconds>=60){
            Seconds=0;
            Minutes+=1;
        }
        Humidity=
        dht.readHumidity();
        Temperature=
        dht.readTemperature();
        if(Minutes>=60){
            Minutes=0;
            Hours+=1;
        }
        if(Hours>24){
            Hours=0;
        }
        delay(999);
        Serial.print(Hours);
        Serial.print(":");
        Serial.print(Minutes);
        Serial.print(":");
        Serial.println(Seconds);
    }
}
//输入处理相关

#define JoyStick_X 12
#define JoyStick_Y 13
#define Button1 32
#define Button2 33
#define Button3 25
#define Button4 26
bool Button1State=false;
bool Button2State=false;
bool Button3State=false;
bool Button4State=false;
bool PressedLock1=false;
bool PressedLock2=false;
bool PressedLock3=false;
bool PressedLock4=false;
enum X_JoyStick{
    XJoyStickLeft
    ,XJoyStickRight,
    XJoyStickMiddle
};
enum Y_JoyStick{
    YJoyStickUp
    ,YJoyStickDown,
    YJoyStickMiddle
};
X_JoyStick JoyStick_X_State=X_JoyStick::XJoyStickMiddle;
Y_JoyStick JoyStick_Y_State=Y_JoyStick::YJoyStickMiddle;
void inputHandle(void * pvParameters){
    while(true){
        //614
        if(analogRead(JoyStick_X)>2688){
            JoyStick_X_State=X_JoyStick::XJoyStickRight;
        }
        else if(analogRead(JoyStick_X)<1508){
            JoyStick_X_State=X_JoyStick::XJoyStickLeft;
        }
        else{
            JoyStick_X_State=X_JoyStick::XJoyStickMiddle;
        }
        if(analogRead(JoyStick_Y)>2688){
            JoyStick_Y_State=Y_JoyStick::YJoyStickDown;
        }
        else if(analogRead(JoyStick_Y)<1508){
            JoyStick_Y_State=Y_JoyStick::YJoyStickUp;
        }
        else{
            JoyStick_Y_State=Y_JoyStick::YJoyStickMiddle;
        }

        if(digitalRead(Button1)==LOW){
            delay(10); //防抖动
            if(digitalRead(Button1)==LOW){
                Button1State=true;
            }
        }
        else if(digitalRead(Button1)==HIGH){
            Button1State=false;
            PressedLock1=false;
        }
        if(digitalRead(Button2)==LOW){
            delay(10); //防抖动
            if(digitalRead(Button2)==LOW){
                Button2State=true;
            }
        }
        else if(digitalRead(Button2)==HIGH){
            Button2State=false;
            PressedLock2=false;
        }
        if(digitalRead(Button3)==LOW){
            delay(10); //防抖动
            if(digitalRead(Button3)==LOW){
                Button3State=true;
            }
        }
        else if(digitalRead(Button3)==HIGH){
            Button3State=false;
            PressedLock3=false;
        }
        if(digitalRead(Button4)==LOW){
            delay(10); //防抖动
            if(digitalRead(Button4)==LOW){
                Button4State=true;
            }
        }
        else if(digitalRead(Button4)==HIGH){
            Button4State=false;
            PressedLock4=false;
        }
        vTaskDelay(1);
    }
}
const String AppNameList[]={"Snake(Debuging)","Setting","Count"};
const int AppCount=3;
//APP相关
int FrameRate=60;
struct Vector2{
    int x;
    int y;
    bool operator!=(const struct Vector2&a)const
    {
        return x!=a.x && y!=a.y;
    }
};
Vector2 ReturnUp(Vector2 Position){
    Position.y+=1;
    return Position;
}
Vector2 ReturnDown(Vector2 Position){
    Position.y-=1;
}
Vector2 ReturnLeft(Vector2 Position){
    Position.x-=1;
    return Position;
}
Vector2 ReturnRight(Vector2 Position){
    Position.x+=1;
    return Position;
}
class App{
    private:
    public:

    App *QuitToAPP;
    static const int Appid;
    static const char AppName;
    char Code;
    virtual void Enter(){
    }
    virtual void RunLogic(){

    }
    virtual void Quit(){

    }
};
App *CurrentAPP;
App *SwitchAPP;
/*地图就以屏幕为坐标系建系，左上角为原点，x轴向右，y轴向下
单位以4*4的像素为单位
所以总共地图是32*40的大小
*/
#define GameMapX 32
#define GameMapY 40
enum Direction{
    Up,
    Down,
    Left,
    Right
};
class GameObject{
    protected:
    int CountFrame=0;
    int JudgeFrame=60;
    public:
    Vector2 Position;
    String Name;
    virtual void Update(){

    }
    virtual void Restart(){

    }
    virtual void Start(){
        Restart();
    }

    virtual void Destroy(){

    }
};
class Snake:public GameObject{
    public:
    Vector2 PreviousPosition;
    Direction MoveToward;
};


class SnakeBody:public Snake{
    private:
    Snake *FollowBody;
    public:
    void Setting(Snake *snake){
        FollowBody=snake;
    }
    void Update(){
        PreviousPosition=Position;
        Position=FollowBody->PreviousPosition;
    }
    void Start(){
        Name="SnakeBody";
    }
    void Destroy(){

    }
};
class Candy:public GameObject{
    public:
    void Start(){
        Name="Candy";
    }

    void UpdatePosition(){
        randomSeed(millis()+Humidity+Temperature+Seconds+Hours);
        Position.x=random(0,DisplayWidth/4);
        Position.y=random(0,DisplayHeight/4);
    }
};
class SnakeTop:public Snake{
    private:
    SnakeBody *Body;
    int Speed;

    public:
    void Input(){
        if(Body!=NULL){
            if(JoyStick_X_State==X_JoyStick::XJoyStickLeft && Body->Position!=ReturnLeft(Position)){
                MoveToward=Direction::Left;
            }
            if(JoyStick_X_State==X_JoyStick::XJoyStickRight && Body->Position!=ReturnRight(Position)){
                MoveToward=Direction::Right;
            }
            if(JoyStick_Y_State==Y_JoyStick::YJoyStickUp && Body->Position!=ReturnUp(Position)){
                MoveToward=Direction::Up;
            }
            if(JoyStick_Y_State==Y_JoyStick::YJoyStickDown && Body->Position!=ReturnDown(Position)){
                MoveToward=Direction::Down;
            }
        }else{
            if(JoyStick_X_State==X_JoyStick::XJoyStickLeft){
                MoveToward=Direction::Left;
            }
            if(JoyStick_X_State==X_JoyStick::XJoyStickRight){
                MoveToward=Direction::Right;
            }
            if(JoyStick_Y_State==Y_JoyStick::YJoyStickUp){
                MoveToward=Direction::Up;
            }
            if(JoyStick_Y_State==Y_JoyStick::YJoyStickDown){
                MoveToward=Direction::Down;
            }
        }
        if(Button1State && Speed!=0){
            Speed=10;
        }
        else{
            Speed=8;
        }
        if(Button2State && Speed!=0 && !PressedLock2){
            PressedLock2=true;
            Speed=0;
        }
        else if(Button2State && Speed==0 && !PressedLock2){
            PressedLock2=true;
            Speed=8;
        }
    }
    void Update(){
        Input();

        if(Speed!=0){
        if(JudgeFrame/Speed==CountFrame){
            PreviousPosition=Position;
        switch(MoveToward){
                case Direction::Up:
                    if(Position.y-1<0){
                        Position.y=GameMapY;
                    }else{
                        Position.y-=1;
                    }
                    break;
                case Direction::Down:
                    if(Position.y+1>GameMapY){
                        Position.y=0;
                    }else{
                        Position.y+=1;
                    }
                    break;
                case Direction::Right:
                    if(Position.x+1>GameMapX){
                        Position.x=0;
                    }else{
                        Position.x+=1;
                    }
                    break;
                case Direction::Left:
                    if(Position.x-1<0){
                        Position.x=GameMapX;
                    }else{
                        Position.x-=1;
                    }
                    break;
            }
        }
        }
        else{
            Serial.println("pause");
        }
        CountFrame+=1;
        if(CountFrame==JudgeFrame+1){
            CountFrame=0;
        }
    }
    void BackBodyGet(SnakeBody *Get){
        Body=Get;
    }
    void Start(){
        Name="SnakeTop";
        Speed=8;
    }
    void Destroy(){

    }

};
class SnakeAPP:public App{
    private:
    SnakeTop *Top;
    SnakeBody *Body[100];
    int BodyCount=0;
    Candy *candys;
    bool IsFail=false;
    public:
    static const int Appid=2;
    int Score=0;
    void Enter(){
        Restart();
        Top=new SnakeTop();
        Top->Start();
        Vector2 spawnPoint;
        spawnPoint.x=GameMapX/2;
        spawnPoint.y=GameMapY/2;
        Top->Position=spawnPoint;
        candys=new Candy();
        candys->Start();
        candys->UpdatePosition();
        Code='g';
    }
    void RestartGame(){
        Score=0;
        Enter();
    }
    void Input(){
        if(Button4State && SwitchAPP==NULL && !PressedLock4){
            SwitchAPP=QuitToAPP;
            PressedLock4=true;
        }
        if(!IsFail){

        }else{
            if(Button1State && !PressedLock1){
                RestartGame();
                PressedLock1=true;
            }
        }
    }
    void FailGame(){
        Displays.fillScreen(TFT_BLACK);
        Displays.setTextSize(2);
        Displays.setCursor(DisplayWidth/2,DisplayHeight/2);
        Displays.print("Game Over");
        IsFail=true;
        Top=NULL;
        for(int i=0;i<BodyCount;i++){
            Body[i]=NULL;
        }
        candys=NULL;
    }
    void Restart(){
        
    }
    void Render(){
        if(!IsFail){
            Displays.fillScreen(TFT_BLACK);
            Displays.fillRect(Top->Position.x*4,Top->Position.y*4,4,4,TFT_RED);
            for(int i=0;i<BodyCount;i++){
                Displays.fillRect(Body[i]->Position.x*4,Body[i]->Position.y*4,4,4,TFT_GREEN);
            }
            Displays.fillRect(candys->Position.x*4,candys->Position.y*4,4,4,TFT_SKYBLUE);
            Displays.setCursor(0,0);
            Displays.setTextSize(1);
            Displays.setTextColor(TFT_WHITE);
            Displays.print("Score:");
            Displays.print(Score);
        }
    }
    void Collision() {
        for (int i = 0; i < BodyCount; i++) {
            if (Top->Position.x == Body[i]->Position.x && Top->Position.y == Body[i]->Position.y) {
                FailGame();
            }
        }
        if (Top->Position.x == candys->Position.x && Top->Position.y == candys->Position.y) {
            // 创建新的蛇身
            SnakeBody* newBody = new SnakeBody();
            newBody->Start();
    
            // 设置新蛇身的跟随目标
            if (BodyCount >= 1) {
                newBody->Setting(Body[BodyCount - 1]);
                newBody->Position = Body[BodyCount - 1]->PreviousPosition;
            } else {
                newBody->Setting(Top);
                newBody->Position = Top->PreviousPosition;
            }
    
            // 确保新蛇身的位置不与蛇头重叠
            if (newBody->Position.x == Top->Position.x && newBody->Position.y == Top->Position.y) {
                newBody->Position = Top->PreviousPosition;
            }
    
            // 将新蛇身添加到蛇身数组
            Body[BodyCount] = newBody;
            BodyCount++;
    
            // 更新糖果位置，确保糖果不与蛇头或蛇身重叠
            do {
                candys->UpdatePosition();
            } while (IsCandyOverlapping());
    
            Score++;
        }
    }
    
    // 检查糖果是否与蛇头或蛇身重叠
    bool IsCandyOverlapping() {
        if (candys->Position.x == Top->Position.x && candys->Position.y == Top->Position.y) {
            return true;
        }
        for (int i = 0; i < BodyCount; i++) {
            if (candys->Position.x == Body[i]->Position.x && candys->Position.y == Body[i]->Position.y) {
                return true;
            }
        }
        return false;
    }
    void RunLogic(){
        delay(16);
        Input();
        if(!IsFail){
            Top->Update();
            for(int i=0;i<BodyCount;i++){
                Body[i]->Update();
            }
        
            Collision();
            Render();
        }
        Displays.setTextSize(1);
        Displays.setTextColor(TFT_WHITE);
        Displays.setCursor(0,6);
        Displays.print("InDebugging!!!!Warning！");
    }
    void Quit(){

    }
};
String SettingNameList[]={"TimeSetting","About"};
const int SettingCount=2;
enum SettingMode{
    None,
    TimeSetting,
    About
};
class CountTime:public App{
    private:
    int Seconds=0;
    int Minutes=0;
    int Hours=0;
    bool IsCount=false;
    int CountFrame=0;
    public:
    void Enter(){
        Code='C';
    }
    void Render(){
        Displays.fillScreen(TFT_BLACK);
        Displays.setCursor(DisplayWidth/2-30,DisplayHeight/2);
        Displays.setTextColor(TFT_WHITE);
        Displays.setTextSize(2);
        Displays.print(Hours);
        Displays.print(":");
        Displays.print(Minutes);
        Displays.print(":");
        Displays.print(Seconds);
        Displays.setTextSize(1);
        Displays.setCursor(DisplayWidth/2-10,DisplayHeight/2+20);
        if(!IsCount){
            Displays.print("Start");
        }else{
            Displays.print("Pause");
        }
    }
    void RunLogic(){
        delay(20);
        Render();
        if(Button1State && !PressedLock1){
            IsCount=!IsCount;
            PressedLock1=true;
            if(IsCount){
                Seconds=0;
                Minutes=0;
                Hours=0;
            }
        }
        if(Button4State && !PressedLock4){
            PressedLock4=true;
            SwitchAPP=QuitToAPP;
        }
        if(IsCount){
            CountFrame+=1;
            if(CountFrame%50==0){
                Seconds+=1;
            }
            if(Seconds>=60){
                Minutes+=1;
                Seconds=0;
            }
            if(Minutes>=60){
                Hours+=1;
                Minutes=0;
            }
        }
    }
};
class SettingAPP:public App{
    private:
    int SettingIndex=0;
    int TimeIndexSetting=0;
    SettingMode Mode=SettingMode::None;
    public:
    static const int Appid=3;
    void Restart(){
        TimeIndexSetting=0;
    }
    void Enter(){
        Restart();
        Code='s';
    }
    void SelectMode(){
        if(Button1State && !PressedLock1){
            PressedLock1=true;
            switch(SettingIndex){
                case 0:
                    Mode=SettingMode::TimeSetting;
                    break;
                case 1:
                    Mode=SettingMode::About;
                    break;
                default:
                    break;
            }
        }
        if(Button2State && !PressedLock2){
            PressedLock2=true;
            SettingIndex+=1;
            if(SettingIndex>=SettingCount){
                SettingIndex=0;
            }
        }
        if(Button3State && !PressedLock3){
            PressedLock3=true;
            SettingIndex-=1;
            if(SettingIndex<0){
                SettingIndex=SettingCount-1;
            }
        }
        if(Button4State && SwitchAPP==NULL && !PressedLock4){
            SwitchAPP=QuitToAPP;
            PressedLock4=true;
        }
    }
    void TimeSetting(){
        Displays.setCursor(DisplayHeight/2-40,DisplayWidth/2-60);
        Displays.setTextColor(TFT_WHITE);
        Displays.setTextSize(2);
        Displays.print(Hours);
        Displays.print(":");
        Displays.print(Minutes);
        Displays.print(":");
        Displays.print(Seconds);
        Displays.setCursor(0,150);
        Displays.print(TimeIndexSetting);
        Displays.print("Index");
        //写关于设置相关的控制代码
        //退出
        switch(TimeIndexSetting){
            case 0:
                if(Button1State && !PressedLock1){
                    TimeIndexSetting+=1;
                    PressedLock1=true;
                }
                if(Button2State && !PressedLock2){
                    Hours+=1;
                    if(Hours>24){
                        Hours=0;
                    }
                    else if(Hours<0){
                        Hours=24;
                    }
                    PressedLock2=true;
                }
                if(Button3State && !PressedLock3){
                    Hours-=1;
                    PressedLock3=true;
                }
                break;
            case 1:
                if(Button1State && !PressedLock1){
                    TimeIndexSetting+=1;
                    PressedLock1=true;
                }
                if(Button2State && !PressedLock2){
                    Minutes+=1;
                    PressedLock2=true;
                }
                if(Button3State && !PressedLock3){
                    Minutes-=1;
                    PressedLock3=true;
                }
                break;
            case 2:
                if(Button1State && !PressedLock1){
                    Mode=SettingMode::None;
                    PressedLock1=true;
                    TimeIndexSetting=0;
                }
                if(Button2State && !PressedLock2){
                    Seconds+=1;
                    PressedLock2=true;
                }
                if(Button3State && !PressedLock3){
                    Seconds-=1;
                    PressedLock3=true;
                }
                break;
        }
        if(Button4State && !PressedLock4){
            Mode=SettingMode::None;
            PressedLock4=true;
        }
    }
    int AboutIndex=0;
    void About(){
        switch (AboutIndex)
        {
        case 0:
            Displays.setCursor(2,2);
            Displays.setTextColor(TFT_WHITE);
            Displays.setTextSize(2);
            Displays.println("Program design and Circuit:THT");
            Displays.println("OWO");
            Displays.setTextSize(1);
            Displays.println("There are many improve work I didn't finish.Because the task is huge for me to complete as I need to study in daytime.The other reason is darkest dungeon.I addicted it rencently QWQ");
            if(Button1State && !PressedLock1){
                AboutIndex=1;
                PressedLock1=true;
            }
            break;
        case 1:
            Displays.fillTriangle(DisplayWidth/2-10,3,DisplayWidth/2,0,DisplayWidth/2+10,3,TFT_WHITE);
            Displays.setTextColor(TFT_WHITE);
            Displays.setTextSize(1);
            Displays.setCursor(0,10);
            Displays.println("Maybe I'll open source after event.");
            Displays.println("In the next stage I would make a game demo in Godot Engine");
            Displays.println("Feeling so RELIEF!After long time dev....");
            if(Button1State && !PressedLock1){
                AboutIndex=0;
                PressedLock1=true;
            }

            break;
        }        
        if(Button4State && !PressedLock4){
            Mode=SettingMode::None;
            PressedLock4=true;
        }
    }
    void RunLogic(){
        delay(8);
        Displays.fillScreen(TFT_BLACK);
        Displays.setTextColor(TFT_WHITE);
        Displays.setTextSize(1);
        switch(Mode){
            case SettingMode::None:
                for(int i=0;i<2;i++){
                    Displays.drawString(SettingNameList[i],2,10*i+10);
                }
                Displays.drawTriangle(30,10*SettingIndex+20,40,10*SettingIndex+5,40,10*SettingIndex+15,TFT_WHITE);
                SelectMode();
                break;
            case SettingMode::TimeSetting:
                TimeSetting();
                break;
            case SettingMode::About:
                About();
                break;
        }
    }
    void Quit(){
        Serial.println("Debuging:Setting quit");
    }
};
class WelcomeAPP:public App{
    private:

    public:
    static const int Appid=0;
    void Restart(){
        
    }
    void Enter(){
        Restart();
        //Welcome类的标识码初始化
        Code='w';
    }
    void RunLogic(){
        delay(8);
        
        Displays.fillScreen(TFT_BLACK);
        Displays.setTextColor(TFT_WHITE);
        Displays.setCursor(0,0);
        Displays.setTextSize(1);
        Displays.print("Alpha 0.12");
        Displays.setTextSize(1);
        Displays.setCursor(DisplayWidth/2-30,20);
        Displays.setTextColor(TFT_SILVER);
        Displays.print("Welcome!");
        Displays.setTextSize(1);
        Displays.setTextColor(TFT_WHITE);
        Displays.setCursor(DisplayWidth/2-30,40);
        Displays.print(Hours);
        Displays.print(":");
        Displays.print(Minutes);
        Displays.print(":");
        Displays.print(Seconds);
        Displays.setCursor(DisplayWidth/2-30,80);
        Displays.setTextSize(1);
        Displays.print("Temp:");
        Displays.print(Temperature);
        Displays.print("C");
        Displays.setCursor(DisplayWidth/2-30,100);
        Displays.print("Humidity:");
        Displays.print(Humidity);
        Displays.print("%");
        if(Button1State && SwitchAPP==NULL && !PressedLock1){
            SwitchAPP=QuitToAPP;
            PressedLock1=true;
        }
    }
    void Quit(){
        Serial.println("Debuging:Welcome ui quit");
    }
};
class SelectionAPP:public App{
    private:
    int AppIndex=0;
    public:
    static const int Appid=1;
    void Enter(){
        Code='S';
    }
    void SelectInput(){
        if(Button3State&&!PressedLock3){
            PressedLock3=true;
            AppIndex-=1;
            if(AppIndex<0){
                AppIndex=AppCount-1;
            }
        }
        if(Button2State&&!PressedLock2){
            PressedLock2=true;
            AppIndex+=1;
            if(AppIndex>=AppCount){
                AppIndex=0;
            }
        }
        if(Button1State && SwitchAPP==NULL && !PressedLock1){
            switch(AppIndex){
                case 0:
                    SwitchAPP=new SnakeAPP();
                    break;
                case 1:
                    SwitchAPP=new SettingAPP();
                    break;
                case 2:
                    SwitchAPP=new CountTime();
                default:
                    break;
            }
            PressedLock1=true;
        }
        if(Button4State && SwitchAPP==NULL && !PressedLock4){
            SwitchAPP=new WelcomeAPP();
            PressedLock4=true;
        }
    }
    void RunLogic(){
        delay(16);
        SelectInput();
        //test代码显示选择界面
        //切换图形数据参数(22,56)(11,73)(18,88)   (106,56)(117,73)(110,88) 
        Displays.fillScreen(TFT_BLACK);
        Displays.fillTriangle(22,56,11,73,18,88,TFT_WHITE);
        Displays.fillTriangle(106,56,117,73,110,88,TFT_WHITE);
        Displays.drawString(AppNameList[AppIndex],DisplayWidth/2-16,DisplayHeight/2-10);
    }
    void Quit(){
        Serial.println("Debuging:Selection quit");
    }
};
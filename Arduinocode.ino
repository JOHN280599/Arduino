#include <SimpleKalmanFilter.h>
SimpleKalmanFilter temp1(2, 2, 0.1);
SimpleKalmanFilter temp2(2, 2, 0.1);
SimpleKalmanFilter temp3(2, 2, 0.1);
SimpleKalmanFilter temp4(2, 2, 0.1);
SimpleKalmanFilter temp5(2, 2, 0.1);
SimpleKalmanFilter temp6(2, 2, 0.1);
SimpleKalmanFilter temp7(2, 2, 0.1);
SimpleKalmanFilter temp8(2, 2, 0.1);
SimpleKalmanFilter humi1(2, 2, 0.1);
SimpleKalmanFilter humi2(2, 2, 0.1);
SimpleKalmanFilter humi3(2, 2, 0.1);
SimpleKalmanFilter humi4(2, 2, 0.1);
#include <EasyNextionLibrary.h>
EasyNex myNex(Serial2);
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(4, 6);
//5RX-6TX


float temp[8];
float humi[4];
int analog[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const float VCC = 5;      //điện áp đọc
const float R = 10000;    //R=10kΩ
const float B = 3435;     //hệ số
const float RT0 = 10000;  //RT0=10kΩ
float n = 0;
unsigned long timer = 0;//timer cho web
unsigned long timer2 = 0;//timer cho web lần 2
unsigned long timer3 = 0;//timer cho web lần 3(cái này gửi thời gian chạy)
int dd, hh, mm, dds, hhs, mms;
// khai báo điều khiển đầu ra
String ST = "Stop";
float TempSet = 0;
int number = 1;
unsigned long timerDK = 0;
unsigned long timerDK2 = 0;
int timeOn = 0;
int timeOff = 0;
const int Output = 9;
const int Output1 = 12;
const int Output2 = 11;
const int Output3 = 10;
int COutput1 = 0;
int COutput2 = 0;
int COutput3 = 0;
int OPcheck = 0;
int change = 0;
int GIOreal = 0;
int Phutreal = 0;
int GIO = 0;
int Phut = 0;
String Pass, Name;
int wifiConnect = 0;
//---------------------------

void setup() {
  myNex.begin(9600);
  Serial1.begin(9600);
  Serial.begin(9600);
  delay(1000);
  timer = millis();
  timer2 = millis();
  timerDK = millis();
  timerDK2 = millis();
  //  Serial.println("Program started");
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
  // khai báo điều khiển đầu ra
  pinMode(Output, OUTPUT);
  digitalWrite(Output, LOW);
  //--------------------
  //lấy các giá trị từ HMI
  dds = myNex.readNumber("setting2.dd.val");
  hhs = myNex.readNumber("setting2.hh.val");
  ST = myNex.readStr("home.t3.txt");
  if (ST == "ON")
  {
    ST = "Start";
  }
  else if (ST == "OFF")
  {
    ST = "Stop";
  }
  TempSet = myNex.readNumber("home.n0.val");
  number = myNex.readNumber("home.n1.val");
  COutput3 = myNex.readNumber("manual.c2.val");
  COutput2 = myNex.readNumber("manual.c1.val");
  COutput1 = myNex.readNumber("manual.c0.val");
  GIOreal = myNex.readNumber("home.ddd.val");
  Phutreal = myNex.readNumber("home.hhh.val");     
  if (COutput1 == 1)
  {
    myNex.writeStr("manual.t3.txt", "ON");
    digitalWrite(Output1, HIGH);
  }
  else if (COutput1 == 0)
  {
    myNex.writeStr("manual.t3.txt", "OFF");
    digitalWrite(Output1, LOW);
  }
  if (COutput2 == 1)
  {
    digitalWrite(Output2, HIGH);
    myNex.writeStr("manual.t4.txt", "ON");
  }
  else if (COutput2 == 0)
  {
    digitalWrite(Output2, LOW);
    myNex.writeStr("manual.t4.txt", "OFF");
  }
  if (COutput3 == 1)
  {
    digitalWrite(Output3, HIGH);
    myNex.writeStr("manual.t5.txt", "ON");
  }
  else if (COutput3 == 0)
  {
    digitalWrite(Output3, LOW);
    myNex.writeStr("manual.t5.txt", "OFF");
  }
  // kết nối tự động wifi
  wifiConnect = myNex.readNumber("setting2.c0.val");
  Pass = myNex.readStr("setting2.t5.txt");
  Name = myNex.readStr("setting2.t6.txt");
  if (wifiConnect == 1)// conncect wifi
  {
    StaticJsonDocument<1000> doc2;
    doc2["x"] = 3;
    doc2["N"] = Name;
    doc2["P"] = Pass;
    serializeJson(doc2, Serial1);
  }
  else if (wifiConnect == 0)// disconnect wifi
  {
    StaticJsonDocument<1000> doc2;
    doc2["x"] = 4;
    serializeJson(doc2, Serial1);
  }
  //-------------------------------------------
  //  Pass = myNex.readStr("setting2.t5.txt");
  //  Name = myNex.readStr("setting2.t6.txt");
  //  StaticJsonDocument<1000> doc2;
  //  doc2["x"] = 3;
  //  doc2["N"] = Name;
  //  doc2["P"] = Pass;
  //  serializeJson(doc2, Serial1);
  //    serializeJson(doc2, Serial);
}

void loop() {
  StaticJsonDocument<1000> doc;
  myNex.NextionListen();
  DeserializationError error = deserializeJson(doc, Serial1);
  int W = doc["W"];
  if (W == 1)
  {
    number = doc["VT"];
    if (number == 1)
    {
      myNex.writeNum("setting1.c0.val", 1);
      myNex.writeNum("setting1.c1.val", 0);
      myNex.writeNum("setting1.c2.val", 0);
      myNex.writeNum("setting1.c3.val", 0);
    }
    else if (number == 2)
    {
      myNex.writeNum("setting1.c1.val", 1);
      myNex.writeNum("setting1.c0.val", 0);
      myNex.writeNum("setting1.c2.val", 0);
      myNex.writeNum("setting1.c3.val", 0);
    }
    else if (number == 3)
    {
      myNex.writeNum("setting1.c2.val", 1);
      myNex.writeNum("setting1.c0.val", 0);
      myNex.writeNum("setting1.c1.val", 0);
      myNex.writeNum("setting1.c3.val", 0);
    }
    else if (number == 4)
    {
      myNex.writeNum("setting1.c3.val", 1);
      myNex.writeNum("setting1.c0.val", 0);
      myNex.writeNum("setting1.c1.val", 0);
      myNex.writeNum("setting1.c3.val", 0);
    }
    int STW = doc["DK"];
    if (STW == 1 ) {
      if (ST == "Stop")
      {
        ST = "Start";
        //  rtc2=0
        //  rtc3=0
        //  rtc4=0
        //  rtc5=0
        myNex.writeNum("rtc0", 0);
        myNex.writeNum("rtc1", 0);
        myNex.writeNum("rtc2", 0);
        myNex.writeNum("rtc3", 0);
        myNex.writeNum("rtc4", 0);
        myNex.writeNum("rtc5", 0);
        myNex.writeNum("rtc6", 0);
      }
    }
    else if (STW == 0)ST = "Stop";
    TempSet = doc["GT"];
    dds = doc["H"];
    hhs = doc["M"];
    myNex.writeNum("setting2.n0.val", TempSet);
    myNex.writeNum("setting2.dd.val", dds);
    myNex.writeNum("setting2.hh.val", hhs);
  }
  else if (W == 2)
  {
    OPcheck = doc["op"];
    if (OPcheck == 1)
    {
      if (digitalRead(Output1) == HIGH)
      {
        digitalWrite(Output1, LOW);
        myNex.writeStr("manual.t3.txt", "OFF");
        myNex.writeNum("manual.c0.val", 0);
        COutput1 = 0;
      }
      else if (digitalRead(Output1) == LOW)
      {
        digitalWrite(Output1, HIGH);
        myNex.writeStr("manual.t3.txt", "ON");
        myNex.writeNum("manual.c0.val", 1);
        COutput1 = 1;
      }
    }
    else if (OPcheck == 2)
    {
      if (digitalRead(Output2) == HIGH)
      {
        digitalWrite(Output2, LOW);
        myNex.writeStr("manual.t4.txt", "OFF");
        myNex.writeNum("manual.c1.val", 0);
        COutput2 = 0;
      }
      else if (digitalRead(Output2) == LOW)
      {
        digitalWrite(Output2, HIGH);
        myNex.writeStr("manual.t4.txt", "ON");
        myNex.writeNum("manual.c1.val", 1);
        COutput2 = 1;
      }
    }
    else if (OPcheck == 3)
    {
      if (digitalRead(Output3) == HIGH)
      {
        digitalWrite(Output3, LOW);
        myNex.writeStr("manual.t5.txt", "OFF");
        myNex.writeNum("manual.c2.val", 0);
        COutput3 = 0;
      }
      else if (digitalRead(Output3) == LOW)
      {
        digitalWrite(Output3, HIGH);
        myNex.writeStr("manual.t5.txt", "ON");
        myNex.writeNum("manual.c2.val", 1);
        COutput3 = 1;
      }
    }
  }
  //  Serial.println(ND);
  //  serializeJson(doc, Serial);
  // đọc giá trị cảm biến rồi gửi lên mqtt------------------------------------------
  if ((unsigned long)(millis() - timer) > 1000) {

    for (int i = 0; i <= 3; i++) {
      temp[i] = Nhietdo(analog[i]);
      temp[i + 4] = Nhietdo(analog[i + 4]);
      humi[i] = Doam(i, temp[i], temp[i + 4]);
      //      humi[i] = random(50, 100);

    }
    StaticJsonDocument<1000> doc2;
    doc2["x"] = 1;
    doc2["t1"] = temp[0];
    doc2["t2"] = temp[1];
    doc2["t3"] = temp[2];
    doc2["t4"] = temp[3];
    doc2["h1"] = humi[0];
    doc2["h2"] = humi[1];
    doc2["h3"] = humi[2];
    doc2["h4"] = humi[3];
    if (wifiConnect == 1)
    {
      serializeJson(doc2, Serial1);
    }
    timer = millis();
  }
  //--------------------------------------------------------------------------------
  //gửi giá trị cảm biến cho HMI----------------------------------------------------
  if ((unsigned long)(millis() - timerDK2) > 300)
  {
    myNex.writeNum("setting1.x0.val", int(temp[0] * 10));
    myNex.writeNum("setting1.x1.val", int(temp[1] * 10));
    myNex.writeNum("setting1.x2.val", int(temp[2] * 10));
    myNex.writeNum("setting1.x3.val", int(temp[3] * 10));
    myNex.writeNum("setting1.x4.val", int(humi[0] * 10));
    myNex.writeNum("setting1.x5.val", int(humi[1] * 10));
    myNex.writeNum("setting1.x6.val", int(humi[2] * 10));
    myNex.writeNum("setting1.x7.val", int(humi[3] * 10));
    timerDK2 = millis();
  }
  //--------------------------------------------------------------------------------
  //hiển thị trạng thái hoạt động lên HMI-------------------------------------------
  if ((unsigned long)(millis() - timerDK) > 500)
  {
    if (ST == "Stop")
    {
      digitalWrite(Output, LOW);
      myNex.writeStr("home.t3.txt", "OFF");
      myNex.writeStr("home.t7.txt", "OFF");
      timeOn = 0;
    }
    else if (ST == "Start")
    {
      if (temp[number - 1] < TempSet)
      {
        if (timeOn >= 20)
        {
          digitalWrite(Output, HIGH);
          myNex.writeStr("home.t7.txt", "ON");
        }
        if (timeOn < 20)
        {
          timeOn++;
        }

      }
      else if (temp[number - 1] >= TempSet)
      {
        digitalWrite(Output, LOW);
        myNex.writeStr("home.t7.txt", "OFF");
        timeOn = 0;
      }
      myNex.writeStr("home.t3.txt", "ON");
    }
    timerDK = millis();
  }
  //--------------------------------------------------------------------------------
  //hiển thị trạng thái hoạt động lên web
  if ((unsigned long)(millis() - timer2) > 200);
  {
    StaticJsonDocument<1000> doc2;
    if (change==0)
    {
    doc2["x"] = 2;
    if (ST == "Start")doc2["t1"] = 1;
    else if (ST == "Stop")doc2["t1"] = 0;
    doc2["t2"] = TempSet;
    doc2["t3"] = number;
    if (digitalRead(Output) == 1)doc2["t4"] = 1;
    else if (digitalRead(Output) == 0)doc2["t4"] = 0;
    doc2["h1"] = dds;
    doc2["h2"] = hhs;
    change=1;
    }
    else if (change==1)
    {

        doc2["x"] = 5;
        doc2["op1"] = COutput1;
        doc2["op2"] = COutput2;
        doc2["op3"] = COutput3;
        GIOreal = myNex.readNumber("home.ddd.val");
        Phutreal = myNex.readNumber("home.hhh.val");        
        doc2["h"] = GIOreal;
        doc2["p"] = Phutreal;
        change=0;
        //serializeJson(doc2, Serial1);
    }
    if (wifiConnect == 1)
    {
      serializeJson(doc2, Serial1);
    }
    //    Serial.println(dds);
    timer2 = millis();
  }
  //------------------------------------------------------------------------------------
  //gửi thời gian điều khiển------------------------------------------------------------
  //  if ((unsigned long)(millis()-timer3)>1000)
  //  {
  //
  //  dd = myNex.readNumber("home.ddd.val");
  //  hh = myNex.readNumber("home.hhh.val");
  //  mm = myNex.readNumber("home.mmm.val");
  //
  //  StaticJsonDocument<1000> doc2;
  //  doc2["x"] = 3;
  //  doc2["t1"]=dds;
  //  doc2["t2"]=hhs;
  //  doc2["t3"]=mms;
  //  doc2["t4"]=dd;
  //  doc2["h1"]=hh;
  //  doc2["h2"]=mm;
  //  serializeJson(doc2, Serial1);
  //  }
}

float Nhietdo(int X) {
  float VRT = 0;
  if (X == A0)
  {
    VRT = temp1.updateEstimate(analogRead(X)); //đọc giá trị analog
  }
  else if (X == A1)
  {
    VRT = temp2.updateEstimate(analogRead(X));
  }
  else if (X == A2)
  {
    VRT = temp3.updateEstimate(analogRead(X));
  }
  else if (X == A3)
  {
    VRT = temp4.updateEstimate(analogRead(X));
  }
  else if (X == A4)
  {
    VRT = temp5.updateEstimate(analogRead(X));
  }
  else if (X == A5)
  {
    VRT = temp6.updateEstimate(analogRead(X));
  }
  else if (X == A6)
  {
    VRT = temp7.updateEstimate(analogRead(X));
  }
  else if (X == A7)
  {
    VRT = temp8.updateEstimate(analogRead(X));
  }
  float TX = 0;
  if (VRT < 1000) {
    VRT = (5.00 / 1023.00) * VRT;  //chuyển từ analog sang điện áp
    float VR = VCC - VRT;          //tính ra điện áp của trở 10k
    float RT = VRT / (VR / R);     //tính ra điện trở cảm biến
    float T0 = 25 + 273.15;
    float ln = log(RT / RT0);          // tính .... chịu
    TX = (1 / ((ln / B) + (1 / T0)));  //tính ra nhiệt độ K
    TX = TX - 273.15;
  }  //chuyển sang độ C

  return TX;
}

float Doam(int okok, float TX1, float TX2) {
  float P_hmax1, P_hmax2, a, b, I2, x, d2, r_h1, d1, p_h1;
  if (TX1 != 0.00 && TX2 != 0.00) {
    // điểm khô
    P_hmax1 = exp(12 - (4026.42 / (TX1 + 235.5)));  // áp suất bão hòa hơi nước

    // điểm ướt: mặc định độ ẩm tại điểm khô = 1 nên P_h = 1*P_max
    P_hmax2 = exp(12 - (4026.42 / (TX2 + 235.5)));
    d2 = 0.622 * (1 * P_hmax2 / (1 - 1 * P_hmax2));
    I2 = 1.008 * TX2 + d2 * (2501 + 1.88 * TX2);

    d1 = (I2 - 1.008 * TX1) / (2501 + 1.88 * TX1);
    p_h1 = d1 / (d1 + 0.622);
    r_h1 = p_h1 / P_hmax1;
    r_h1 = r_h1 * 100;
    if (okok == 0)return humi1.updateEstimate(r_h1);
    if (okok == 1)return humi2.updateEstimate(r_h1);
    if (okok == 2)return humi3.updateEstimate(r_h1);
    if (okok == 3)return humi4.updateEstimate(r_h1);
  }
  else
  {
    return 0;
  }

}


// đọc tín hiệu điều khiển từ màn hình
void trigger0()
{
  dds = myNex.readNumber("setting2.dd.val");
  hhs = myNex.readNumber("setting2.hh.val");
  ST = myNex.readStr("home.b1.txt");
  TempSet = myNex.readNumber("home.n0.val");
  number = myNex.readNumber("home.n1.val");
}
void trigger1()
{
  COutput1 = myNex.readNumber("manual.c0.val");
  if (COutput1 == 1)
  {
    myNex.writeStr("manual.t3.txt", "ON");
    digitalWrite(Output1, HIGH);
  }
  else if (COutput1 == 0)
  {
    myNex.writeStr("manual.t3.txt", "OFF");
    digitalWrite(Output1, LOW);
  }
}
void trigger2()
{
  COutput2 = myNex.readNumber("manual.c1.val");
  if (COutput2 == 1)
  {
    digitalWrite(Output2, HIGH);
    myNex.writeStr("manual.t4.txt", "ON");
  }
  else if (COutput2 == 0)
  {
    digitalWrite(Output2, LOW);
    myNex.writeStr("manual.t4.txt", "OFF");
  }
}
void trigger3()
{
  COutput3 = myNex.readNumber("manual.c2.val");
  if (COutput3 == 1)
  {
    digitalWrite(Output3, HIGH);
    myNex.writeStr("manual.t5.txt", "ON");
  }
  else if (COutput3 == 0)
  {
    digitalWrite(Output3, LOW);
    myNex.writeStr("manual.t5.txt", "OFF");
  }
}
void trigger4()
{
  if (COutput1 == 1)
  {
    myNex.writeStr("manual.t3.txt", "ON");
  }
  else if (COutput1 == 0)
  {
    myNex.writeStr("manual.t3.txt", "OFF");
  }

  if (COutput2 == 1)
  {
    myNex.writeStr("manual.t4.txt", "ON");
  }
  else if (COutput2 == 0)
  {
    myNex.writeStr("manual.t4.txt", "OFF");
  }

  if (COutput3 == 1)
  {
    myNex.writeStr("manual.t5.txt", "ON");
  }
  else if (COutput3 == 0)
  {
    myNex.writeStr("manual.t5.txt", "OFF");
  }
  myNex.writeNum("manual.c0.val", COutput1);
  myNex.writeNum("manual.c1.val", COutput2);
  myNex.writeNum("manual.c2.val", COutput3);
}

void trigger5()
{
  number = myNex.readNumber("home.n1.val");
}

void trigger6()
{
  dds = myNex.readNumber("setting2.dd.val");
  hhs = myNex.readNumber("setting2.hh.val");
  TempSet = myNex.readNumber("setting2.n0.val");
}

void trigger7()
{

  Pass = myNex.readStr("setting2.t5.txt");
  Name = myNex.readStr("setting2.t6.txt");
  if (wifiConnect == 1)// conncect wifi
  {
    StaticJsonDocument<1000> doc2;
    doc2["x"] = 3;
    doc2["N"] = Name;
    doc2["P"] = Pass;
    serializeJson(doc2, Serial1);
  }
  else if (wifiConnect == 0)// disconnect wifi
  {
    StaticJsonDocument<1000> doc2;
    doc2["x"] = 4;
    serializeJson(doc2, Serial1);
  }
  //    serializeJson(doc2, Serial);
}

void trigger8()
{
  wifiConnect = myNex.readNumber("setting2.c0.val");
}

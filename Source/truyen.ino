#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Nut Nhan bat/tat tu dong
bool isActive = true;
void pressButton1() {
  isActive = !isActive;
  if(isActive) {
    Serial.println("Bat tu dong");
  }
  else {
    Serial.println("Tat tu dong");
  }
  digitalWrite(5, isActive);
}

//Nut Nhan dong/mo cua
bool isClose = true;
void pressButton0() {
  isClose = !isClose;
  if(isClose) {
    Serial.println("Dong");
  }
  else {
    Serial.println("Mo");
  }
  digitalWrite(6, isClose);
}


RF24 radio(9, 10);// Chọn chân kết nối CE, CSN

const byte address[6] = "000001";//Khai báo địa chỉ phát

void setup()
{
  Serial.begin(9600);   
  radio.begin();
  radio.openWritingPipe(address);//Mở kênh gửi dữ liệu
  radio.stopListening();//Cài đặt chế độ gửi dữ liệu
  attachInterrupt(1, pressButton1, RISING);//Khai bao ngat 1
  attachInterrupt(0, pressButton0, RISING);//Khai bao ngat 0
  pinMode(5, OUTPUT);//Chan led
  pinMode(6, OUTPUT);//Chan led
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  Serial.println("Setup done!");
}
int arr[2];

void loop()
{
  
  arr[0] = isActive;
  arr[1] = isClose;
  radio.write(&arr, sizeof(arr));//Gửi dữ liệu

  delay(10);
}

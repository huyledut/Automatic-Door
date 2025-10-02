#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // Chọn chân kết nối CE, CSN
const byte address[6] = "000001"; //Khai báo địa chỉ nhận

void setup()
{
  pinMode(2, INPUT);
  pinMode(4, OUTPUT); //Direction
  pinMode(5, OUTPUT); //Step
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);//Mở kênh nhận dữ liệu
  radio.startListening();//Cài đặt chế độ nhận dữ liệu
  Serial.println("setup done");
}

int x;
void mo() {
  Serial.println("mo");
  digitalWrite(4, HIGH);
  for(x = 0; x < 180; x++)
    {
      digitalWrite(5,HIGH);
      delayMicroseconds(1000);
      digitalWrite(5,LOW);
      delayMicroseconds(1000);
    }
}

void dong() {
  Serial.println("dong");
  digitalWrite(4, LOW);
  for(x = 0; x < 180; x++)
    {
      digitalWrite(5,HIGH);
      delayMicroseconds(1000);
      digitalWrite(5,LOW);
      delayMicroseconds(1000);
    }
}

int arr[2];
bool isActive = true, old_active=true;
bool isClose = true;
bool old_btn = true, new_btn = true;
bool old_status = true, new_status = true;

void loop()
{
  if (radio.available())//Kiem tra du lieu co nhan duoc hay khong
  {
    radio.read(arr, sizeof(arr));//doc du lieu nhan duoc
    isActive = arr[0];
    isClose = arr[1];
  }
  if(isActive) {
    //Logic khi ở chế độ tự động
    //Nếu cửa hiện tại đang mở thì sẽ tự đóng lại
    if(new_btn==0){
      new_btn=1;
      old_btn=1;
      dong();
    }
    if(old_active != isActive) {
      Serial.println("Che do tu dong: ");
    }
    //Khi phát hiện vật cản thì cửa sẽ mở
    //Khi không có vật cản thì cửa sẽ đóng lại
    new_status = digitalRead(2);
    if(new_status==0 && old_status==1){
      Serial.print("Gia tri cam bien: ");
      Serial.println(new_status);
      mo();
    }
    if(new_status==1 && old_status==0){
      Serial.print("Gia tri cam bien: ");
      Serial.println(new_status);
      dong();
    }
    old_status = new_status;
    old_active = isActive;
  }
  else {
    //Logic khi ở chế độ điều khiển thủ công
    //Khi cửa đang mở thì đóng cửa lại
    if(new_status==0) {
      new_status = 1;
      old_status = 1;
      dong();
    }
    if(old_active != isActive) {
      Serial.println("Che do thu cong: ");
    }
    //Nhận dữ liệu bên gửi
    //khi dữ liệu nhận được (isClose) thay đổi từ 1->0: mở cửa
    //Khi dữ liệu nhạn được (isClose) thay đổi từ 0->1: đóng cửa
    new_btn = isClose;
    if(new_btn==0 && old_btn==1){
      mo();
    }
    if(new_btn==1 && old_btn==0){
      dong();
    }
    old_btn = new_btn;
  }
  old_active = isActive;
  delay(10);
}

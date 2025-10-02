# Automatic-Door
<strong>Automatic-Door</strong>  là hệ thống sử dụng các motor và bo mạch điều khiển cửa, khi vật cản đi vào vùng quét của cửa thì sẽ mở ra, sau một khoảng thời gian nếu cửa không cảm nhận được vật cản thì sẽ tự động đóng lại. Chúng ta chỉ cần đi đến gần cửa và việc còn lại cửa tự động sẽ thực hiện. Ngoài ra còn có thể điều khiển từ xa bằng remote

### Thành phần:
- Arduino UNO
- Module điều khiển động cơ A4988
- Động cơ bước
- Module NRF24L01
- Cảm biến tiệm cận
- Button, LED, và một số dây dẫn

## 🔹 Nguyên lý hoạt động
<p align="center">
  <img src="./Nguyen Ly.png" alt="Nguyên lý hoạt động" width="600"/>
</p>

---

## 🔹 Sơ đồ mạch nhận
<p align="center">
  <img src="./So Do Mach - Nhan.png" alt="Sơ đồ mạch nhận" width="600"/>
</p>

## 🔹 Sơ đồ mạch phát
<p align="center">
  <img src="./So Do Mach - Phat.png" alt="Sơ đồ mạch phát" width="600"/>
</p>2303

### Hướng đẫn cài đặt
- Mở file mã nguồn bằng phần mềm Arduino IDE(bao gồm mã nguồn của hệ thống và điều khiển từ xa)
- Chọn BOARD(Arduino Uno) và PORT(COMX) với COMX là đầu kết nối giữa LAPTOP (PC) với bo mạch 
- Kết nối Arduino với Laptop và tiến hành nạp code
### Trình tự chạy chương trình
* #### Điều khiển từ xa
    1. Cài đặt các thư viện sau:
    ```
        #include <SPI.h>
        #include <nRF24L01.h>
        #include <RF24.h> 
    ```
    2. Mã nguồn điều khiển từ xa ( gửi dữ liệu/ tín hiệu)
    ```
        //Nut Nhan bat/tat tu dong
        bool isActive = true;
        void pressButton1() 
        {
            isActive = !isActive;
            if(isActive) 
            {
                Serial.println("Bat tu dong");
            }
            else 
            {
                Serial.println("Tat tu dong");
            }
            digitalWrite(5, isActive);
        }

        //Nut Nhan dong/mo cua
        bool isClose = true;
        void pressButton0() 
        {
            isClose = !isClose;
            if(isClose) 
            {
                Serial.println("Dong");
            }
            else 
            {
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
    ```
* #### Cửa thông minh
    1. Cài đặt các thư viện sau:
    ```
        #include <SPI.h>
        #include <nRF24L01.h>
        #include <RF24.h>  
    ```
    2. Mã nguồn cửa thông minh (nhận dữ liệu/ tín hiệu)
    ```
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
            if(new_btn==0)
            {
                new_btn=1;
                old_btn=1;
                dong();
            }
            if(old_active != isActive) 
            {
                Serial.println("Che do tu dong: ");
            }
            //Khi phát hiện vật cản thì cửa sẽ mở
            //Khi không có vật cản thì cửa sẽ đóng lại
            new_status = digitalRead(2);
            if(new_status==0 && old_status==1)
            {
                Serial.print("Gia tri cam bien: ");
                Serial.println(new_status);
                mo();
            }
            if(new_status==1 && old_status==0)
            {
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
            if(new_status==0) 
            {
                new_status = 1;
                old_status = 1;
                dong();
            }
            if(old_active != isActive) 
            {
                Serial.println("Che do thu cong: ");
            }
            //Nhận dữ liệu bên gửi
            //khi dữ liệu nhận được (isClose) thay đổi từ 1->0: mở cửa
            //Khi dữ liệu nhạn được (isClose) thay đổi từ 0->1: đóng cửa
            new_btn = isClose;
            if(new_btn==0 && old_btn==1)
            {
                mo();
            }
            if(new_btn==1 && old_btn==0)
            {
                dong();
            }
            old_btn = new_btn;
        }
        old_active = isActive;
        delay(10);
        }
    ```

* Thư viện cài đặt: [Core library for nRF24L01](https://www.arduinolibraries.info/libraries/rf24?fbclid=IwAR3kCrV5tAwd11ltAgqxImG0eIxsus_OCkrkB40w0gZ2PxgQqrTb5uDuuFQ)

## Hết
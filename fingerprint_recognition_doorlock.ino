#include <Servo.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
#else
#endif

SoftwareSerial mySerial(1, 0);
Servo servo;
LiquidCrystal_I2C lcd(0x27,16,2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'X','D','C','A'},
  {'#','9','6','3'},
  {'0','8','5','2'},
  {'*','7','4','1'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {13, 12, 11, 10};

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

uint8_t id; 
char keypressed;
char passwd[4]={'0','0','0','0'};
char chanpw1[4];
char chanpw2[4];
int a=0;
int i=0;
int j=0;
int n=0;
int s=0;


void setup() {
  finger.begin(57600);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  servo.attach(2);
}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("Close the door");
  
  char key = customKeypad.getKey();
  
  if(key=='#'){                    //비밀번호 바꾸기
    changepasswd();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Close the door");
 }
        
  if(key=='*'){                     //비밀번호 입력하기
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Input password");
    getpasswd();
    if(a==4){
      n=0;
      lcd.setCursor(0,0);
      lcd.print("Open the door");
      for(int pos=0;pos<=180;pos++){
      servo.write(pos);
        delay(20);
      }
      delay(1000);
      for(int pos=180;pos>=0;pos--){
        servo.write(pos);
        delay(20);
      }
    }
    else n++;
    a=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Close the door");
  }
  
  getFingerprintID();                //지문인식하기
  switch(n){                        //틀린 횟수에 따라 LED 켜기 & 부저 울리기
    case 0:
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(14, LOW);
      digitalWrite(15, LOW);
      break;
    case 1:
      digitalWrite(3, HIGH);
      break;
    case 2:
      digitalWrite(4, HIGH);
      break;
    case 3:
      digitalWrite(5, HIGH);
      break;
    case 4:
      digitalWrite(14, HIGH);
      break;
    case 5:
      digitalWrite(15, HIGH);
      lcd.setCursor(0,0);
      lcd.print("Only keypad");
      tone(16, 1046.5);
      delay(1000);
      noTone(16);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(14, LOW);
      digitalWrite(15, LOW);
      n=0;
      break;
   default: break;
  }
}


void getpasswd(){        //비밀번호 입력받는 함수
  i=0;
  a=0;
  j=0;                                   
  while(keypressed != 'X'){
    keypressed = customKeypad.getKey();                         
    if(keypressed != NO_KEY && keypressed != 'X' ){
      lcd.setCursor(j,1);
      lcd.print("*");
      j++;
      if(keypressed == passwd[i]&& i<4){
        a++;
        i++;
      }
      else a--;
    }
  }
  keypressed = NO_KEY;
}


void changepasswd(){       //비밀번호 바꾸는 함수
  lcd.clear();
  lcd.print("Enter password");
  getpasswd();
  if(a==4){
    GetNewCode1();
    GetNewCode2();
    s=0;
    for(i=0;i<4;i++){
      if(chanpw1[i]==chanpw2[i]) s++;
    }
    if(s==4){
      for(i=0;i<4;i++){
        passwd[i]=chanpw2[i];
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Password");
      lcd.setCursor(0,1);
      lcd.print("Changed");
      delay(2000);
    }
    else{
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wrong");
      delay(2000);
    }
  }
  else{
    lcd.clear();
    lcd.print("Wrong");
    delay(2000);
  }
}


void GetNewCode1(){                    //새 비밀번호 입력받는 함수
  i=0;
  j=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("New password");
  lcd.setCursor(0,1);
  lcd.print("and press X");
  while(keypressed != 'X'){
    keypressed = customKeypad.getKey();
    if(keypressed != NO_KEY && keypressed != 'X' ){
      lcd.setCursor(j,1);
      lcd.print("*");
      chanpw1[i]=keypressed;
      i++;
      j++;                    
    }
  }
  keypressed = NO_KEY;
}


void GetNewCode2(){                    //새 비밀번호 입력받는 함수2
  i=0;
  j=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Confirm code");
  lcd.setCursor(0,1);
  lcd.print("and press X");
  while(keypressed != 'X'){
    keypressed = customKeypad.getKey();
    if(keypressed != NO_KEY && keypressed != 'X' ){
      lcd.setCursor(j,1);
      lcd.print("*");
      chanpw2[i]=keypressed;
      i++;
      j++;                    
    }
  }
  keypressed = NO_KEY;
}


uint8_t getFingerprintID() {                //지문인식하는 함수
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    default:
      return p;
  }
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    default:
      n++;
      return p;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Open the door");
  n=0;
   for(int pos=0;pos<=180;pos++){
      servo.write(pos);
      delay(20);
    }
    delay(1000);
    for(int pos=180;pos>=0;pos--){
      servo.write(pos);
      delay(20);
    }
  return finger.fingerID;
}

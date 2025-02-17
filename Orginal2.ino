#include<LiquidCrystal.h>
#include<SoftwareSerial.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
SoftwareSerial bluetooth(11, 12); // RX, TX pins for Bluetooth module

#define echo A0    //Echo pin
#define trigger A1 //Trigger pin

#define Recheck A5 // choose the pin for the Recheck button

#define G_led 8 // choose the pin for the Green Led
#define R_led 9 // choose the pin for the Red Led
#define buz  10 // choose the pin for the Buzzer

// defines variables
float Speed=0, distance=0, Time=0;
int flag1=0, flag2=0;
int time1, time2;

// Other configuration
const unsigned int BEEP_FREQUENCY = 3000;

bool deviceOn = false; // Flag to track device status

void setup(){
  pinMode(Recheck, INPUT_PULLUP);

  pinMode(echo, INPUT );// declare ultrasonic sensor Echo pin as input
  pinMode(trigger, OUTPUT); // declare ultrasonic sensor Trigger pin as Output 

  pinMode(R_led,OUTPUT); // declare Red LED as output
  pinMode(G_led,OUTPUT); // declare Green LED as output
  pinMode(buz,OUTPUT);   // declare Buzzer as output
  
  lcd.begin(16,2);
  // Initially display "Pokrenite uredaj"
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("    Pokrenite ");
  lcd.setCursor(0,1);
  lcd.print("      uredaj  ");
  delay(2000);
  lcd.clear();

  bluetooth.begin(9600); // initialize serial communication with Bluetooth module
}

void loop(){
  if (!deviceOn) {
    // Check for incoming data from Bluetooth module
    if (bluetooth.available()) {
      char command = bluetooth.read();
      if (command == '1') { // If '1' is received, turn on the device
        deviceOn = true;
        // Clear LCD and display distance and speed
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Dist. in cm ");
        lcd.setCursor(0,1);
        lcd.print("Speed Km/Hr ");
      }
    }
  } else {
    ultrasonicRead(); //calls ultrasoninicRead() function below

    if(distance<=30 && flag1==0){time1 = millis(); flag1=1;}
    if(distance<=20 && flag2==0){time2 = millis(); flag2=1;}

    if(flag1==1 && flag2==1){flag1=2, flag2=2;
         if(time1 > time2){Time = time1 - time2;}
    else if(time2 > time1){Time = time2 - time1;}
     Time=Time/1000;//convert millisecond to second
     Speed=(10/Time);//v=d/t
     Speed=Speed*3600;//multiply by seconds per hr
     Speed=Speed/1000;//division by meters per Km

    digitalWrite(R_led, 0);
    digitalWrite(G_led, 1);
    tone(buz, BEEP_FREQUENCY, 200);  // Beep when done
    }

    if(digitalRead(Recheck)==0){
    Speed=0;
    flag1=0, flag2=0;
    digitalWrite(R_led, 1);
    digitalWrite(G_led, 0);
    tone(buz, BEEP_FREQUENCY, 400);  // Beep when done
    }

    // Check for incoming data from Bluetooth module
    if (bluetooth.available()) {
      char command = bluetooth.read();
      if (command == '0') { // If '0' is received, turn off the device
        deviceOn = false;
        // Clear LCD and display "Pokrenite uredaj"
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("    Pokrenite ");
        lcd.setCursor(0,1);
        lcd.print("      uredaj  ");
      }
    }

    // If the device is on, display distance and speed
    if (deviceOn) {
      lcd.setCursor(12,0);
      lcd.print(distance);
      lcd.print("   ");

      lcd.setCursor(12,1); 
      lcd.print(Speed); 
    }
    
    delay(10);   
  }
}

void ultrasonicRead(){
  digitalWrite(trigger, LOW); //PULSE ___|---|___
  delayMicroseconds(2); 
  digitalWrite(trigger, HIGH); 
  delayMicroseconds(10); 
  long time = pulseIn(echo, HIGH); 
  distance = time / 28.5 / 2;
}
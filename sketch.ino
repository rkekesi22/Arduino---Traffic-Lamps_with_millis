#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

enum States {
  CAR_GREEN,
  CAR_ORANGE,
  CAR_RED_WALK_GREEN,
  WALK_GREEN_TO_RED
};

States actualState =  CAR_GREEN;

const int red = 10;
const int orange = 9;
const int green = 8;

const int walk_red = 2;
const int walk_green = 4;

const int button = 7;

const int buzzer = 12;

int counter = 0;

bool pressButton = false;

unsigned long starttime = 0;

unsigned long lastPressTime = 0;
unsigned long debounceTime = 10; 

bool lastState = HIGH;
bool ledState = false;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(orange, OUTPUT);
  pinMode(green, OUTPUT);

  pinMode(walk_red, OUTPUT);
  pinMode(walk_green, OUTPUT);

  pinMode(button, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);

  digitalWrite(green, HIGH);
  digitalWrite(walk_red, HIGH);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Please wait!");

}

void loop() {
  unsigned long now = millis();
  bool buttonState = digitalRead(button);

  //Serial.println("Aktuális állapot:");
  //Serial.println(actualState);
  //Serial.println("Utolsó állapot:");
  //Serial.println(lastState);


  if (lastState == HIGH && buttonState == LOW && actualState == CAR_GREEN)
  {
    actualState = CAR_ORANGE;
    starttime = now;

    digitalWrite(green, LOW);
    digitalWrite(orange, HIGH);

    lcd.clear();
    lcd.print("Orange...");
  }

  lastState = buttonState;

  switch(actualState)
  {
    case CAR_ORANGE:
      if( (now - starttime) >= 2000)
      {
        actualState = CAR_RED_WALK_GREEN;
        starttime = now;

        digitalWrite(orange, LOW);
        digitalWrite(red, HIGH);
        
        digitalWrite(walk_red, LOW);
        digitalWrite(walk_green, HIGH);

        tone(buzzer, 1000);

        counter = 3;

        lcd.clear();
      }
    break;

    case CAR_RED_WALK_GREEN:
     if( (now - lastPressTime) > 1000 )
     {
      lastPressTime = now;

      lcd.setCursor(0,0);
      lcd.print("Go");
      lcd.setCursor(0,1);
      lcd.print("Time: ");
      lcd.print(counter);
      lcd.print("    ");


      counter--;
     }

     if( (now - starttime) >= 3000)
      {
        actualState = WALK_GREEN_TO_RED;
        starttime = now;
        lastPressTime = now;
      }
    break;

    case WALK_GREEN_TO_RED:
     if( (now - lastPressTime) >= 250)
      {
        lastPressTime = now;
        ledState = !ledState;

        digitalWrite(walk_green, ledState);

        if( ledState )
        {
          tone(buzzer, 1500);
        }
        else
        {
          noTone(buzzer);
        }
      }

      if( (now - starttime) >= 3000)
      {
        actualState = CAR_GREEN;
        digitalWrite(walk_green, LOW);
        digitalWrite(walk_red, HIGH);
    
        digitalWrite(red, LOW);
        digitalWrite(green, HIGH);
        noTone(buzzer);


        lcd.clear();
        lcd.print("Waiting...");
      }
    break;

    case CAR_GREEN:
    
    break;
  }
}


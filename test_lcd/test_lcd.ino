#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

void setup() {
  lcd.init(); // initialize the lcd
  lcd.backlight();
}

void loop() {
  lcd.clear();              // clear display
  lcd.setCursor(6, 0);      // move cursor to   (0, 0)
  lcd.print("IOT");       // print message at (0, 0)
  lcd.setCursor(5, 1);      // move cursor to   (2, 1)
  lcd.print("EC Dept"); // print message at (2, 1)
  delay(2000);                  // display the above for two seconds

  lcd.clear();                  // clear display
  lcd.setCursor(2, 0);          // move cursor to   (3, 0)
  lcd.print("LCD I2C MODE");        // print message at (3, 0)
  lcd.setCursor(6, 1);          // move cursor to   (0, 1)
  lcd.print("16x2"); // print message at (0, 1)
  delay(2000);                      // display the above for two seconds
}

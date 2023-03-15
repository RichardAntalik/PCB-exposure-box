 #include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

const int swPin = 9;
const int testPin = 8;
const int outPin = 7;
const int potPin = 0;
  
unsigned long last_time = 0;
int exposure_time_set = 0;
bool device_ready;

static float mapf(long x, long in_min, long in_max, float out_min, float out_max) {
  return ((x - in_min) / (float)((in_max - in_min))) * (out_max - out_min) + out_min;
}

static float volt_divider(int val_adc, long r_low, long r_hi){
  float u_low = mapf(val_adc, 0, 1023, 0, 5);
  float i = u_low / r_low;

  return (r_low + r_hi) * i;
}

static bool read_switch(void){
  return !digitalRead(swPin);
}

void lcd_countdown()
{
  led_set(true);
  
  unsigned long start_time = millis();
  unsigned int exposure_time = 0;
  
  while(exposure_time < exposure_time_set && read_switch()){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_logisoso32_tf);
    u8g2.setCursor(0,32);
    u8g2.print("E: ");
    u8g2.print(exposure_time_set - exposure_time);
    Serial.println(exposure_time_set - exposure_time);
    u8g2.sendBuffer();

    exposure_time = (millis() - start_time) / 1000;
  }

  device_ready = false;
  led_set(false);
}

void lcd_time_set()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso32_tf);

  u8g2.setCursor(0,32);
  u8g2.print("S: ");
  u8g2.print(exposure_time_set);
  Serial.println(exposure_time_set);
  u8g2.sendBuffer();
}

void lcd_msg_manual()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(0,32);
  u8g2.print("Manual");
  u8g2.sendBuffer();
}

static bool led_set(bool state){
  digitalWrite(outPin, state);
}

void setup()
{
  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, 0);
  pinMode(testPin, OUTPUT);
  digitalWrite(testPin, 0);
  pinMode(swPin, INPUT_PULLUP);
  u8g2.begin();

  unsigned long last_time = millis();

  device_ready = true;
}

void loop() {
  exposure_time_set = analogRead(potPin);
  exposure_time_set = map(exposure_time_set, 0, 1020, 0, 100);

  led_set(false);
  lcd_time_set();

  if (!read_switch() && !device_ready){
    device_ready = true;
  }

  /* Manual exposure */ 
  if (read_switch() && exposure_time_set == 0){
    led_set(true);
    while (read_switch()){
      lcd_msg_manual();
    }
  }

  /* Timer */
  if (read_switch() && device_ready){
    lcd_countdown();
    led_set(true);
  }
}

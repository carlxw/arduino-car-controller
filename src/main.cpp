#include "Arduino.h"
#include "BleKeyboard.h"
#include "string.h"
#include "LiquidCrystal_I2C.h"

#define DEVICE_NAME "Car Controller"
#define DEBOUNCE_DELAY 50

#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_ADDR 0x27

#define LED_PIN 2
#define PLAY_PAUSE_PIN 14
#define NEXT_PIN 15
#define PREV_PIN 16
#define SDA_PIN 21
#define SCL_PIN 22

typedef struct pin_states {
	bool play_pause_pin;
	bool next_pin;
	bool prev_pin;
	bool event_occured;
} pin_states;

unsigned long last_millis;
pin_states button_states = { false, false, false, false };
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
BleKeyboard keyboard(DEVICE_NAME);

void play_pause() { keyboard.write(KEY_MEDIA_PLAY_PAUSE); } 
void next() { keyboard.write(KEY_MEDIA_NEXT_TRACK); }
void prev() { keyboard.write(KEY_MEDIA_PREVIOUS_TRACK); }
void reset_status() { button_states = { false, false, false, false }; }

void read_pins() {
    button_states.play_pause_pin = digitalRead(PLAY_PAUSE_PIN);
	button_states.next_pin = digitalRead(NEXT_PIN);
	button_states.prev_pin = digitalRead(PREV_PIN);
	button_states.event_occured = 
		button_states.play_pause_pin ||
		button_states.next_pin ||
		button_states.prev_pin;
}

void blink() {
	digitalWrite(LED_PIN, !digitalRead(LED_PIN));
	delay(500);
}

void setup() {
    Serial.begin(115200);

	// Init GPIO
	pinMode(LED_PIN, OUTPUT);
	pinMode(PLAY_PAUSE_PIN, INPUT); 
	pinMode(NEXT_PIN, INPUT);
	pinMode(PREV_PIN, INPUT);

    // Initialize as Bluetooth HID controller
	keyboard.begin();
    Serial.println("ESP32 Spotify Remote started!");
    Serial.println("Commands:");
    Serial.println("p - Play/Pause");
    Serial.println("n - Next Track");
    Serial.println("b - Previous Track");

	// Init I2C Display
	lcd.init();
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Car Controller");
}

void loop() {
	// Blink the LED when the device is not connected
	if (!keyboard.isConnected()) {
		blink();
		return;
	} 

	digitalWrite(LED_PIN, HIGH);

	// The device will either be controlled 
    if (Serial.available()) {
        char cmd = Serial.read();

        switch (cmd) {
            case 'p':
                play_pause();
                Serial.println("Play/Pause");
                break;

            case 'n':
                next();
                Serial.println("Next track");
                break;

            case 'b':
                prev();
                Serial.println("Previous track");
                break;
            
            default:
				Serial.print("Received: ");
				Serial.println(cmd);
                break;
        }
    } else {
		return; 

		if (!(millis() - last_millis > DEBOUNCE_DELAY)) {
			return;
		}

		read_pins();

		if (button_states.event_occured) {
			if (button_states.play_pause_pin) {
				play_pause();
				Serial.println("Play/Pause via button");
			}
			
			if (button_states.next_pin) {
				next();
				Serial.println("Next track via button");
			}
			
			if (button_states.prev_pin) {
				prev();
				Serial.println("Previous track via button");
			}

			last_millis = millis(); 
			reset_status();
		}
	}

    delay(100);
}

// #define DEBUG

#include "Arduino.h"
#include "BleKeyboard.h"
#include "string.h"
#include "LiquidCrystal_I2C.h"

#define DEVICE_NAME "Car Controller"
#define DISCONNECTED_MESSAGE "Searching..."
#define DEBOUNCE_DELAY 500

#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_ADDR 0x27

#define LED_PIN 19 
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

unsigned long last_millis = 0;
bool prev_state = false;
pin_states button_states = { false, false, false, false };
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
BleKeyboard keyboard(DEVICE_NAME);

void play_pause() { keyboard.write(KEY_MEDIA_PLAY_PAUSE); } 
void next() { keyboard.write(KEY_MEDIA_NEXT_TRACK); }
void prev() { keyboard.write(KEY_MEDIA_PREVIOUS_TRACK); }
void reset_status(pin_states *button_states) { *button_states = { false, false, false, false }; }

void read_pins(pin_states *button_states) {
    button_states->play_pause_pin = digitalRead(PLAY_PAUSE_PIN);
#ifndef DEBUG
	if (button_states->play_pause_pin) Serial.println("rp: Play-pause");
#endif
	button_states->next_pin = digitalRead(NEXT_PIN);
#ifndef DEBUG
	if (button_states->next_pin) Serial.println("rp: Next");
#endif
	button_states->prev_pin = digitalRead(PREV_PIN);
#ifndef DEBUG
	if (button_states->prev_pin) Serial.println("rp: Prev");
#endif
	button_states->event_occured = 
		button_states->play_pause_pin ||
		button_states->next_pin ||
		button_states->prev_pin;
}

void write_display(LiquidCrystal_I2C *i2c, std::string lcd_str) {
	i2c->clear();
	i2c->setCursor(0, 0);
	i2c->print(lcd_str.c_str());
	delay(50);
}

void blink() {
	digitalWrite(LED_PIN, !digitalRead(LED_PIN));
	delay(500);
	Serial.println("Searching...");	
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
    Serial.println("ESP32 Car Controller started!");
    Serial.println("Commands:");
    Serial.println("p - Play/Pause");
    Serial.println("n - Next Track");
    Serial.println("b - Previous Track");

	// Init I2C Display
	// lcd.init();
	// lcd.backlight();
	// write_display(&lcd, DISCONNECTED_MESSAGE);
}

void loop() {
	bool current_state = keyboard.isConnected();

	// Blink the LED when the device is not connected
	if (!keyboard.isConnected()) {
		blink();

		if (prev_state != current_state) {
			Serial.println("No connected device found. Advertising...");
			// write_display(&lcd, DISCONNECTED_MESSAGE);
			prev_state = current_state;
		}
		
		return;
	} 

	if (prev_state != current_state) {
		Serial.println("ESP32 is now connected.");
		digitalWrite(LED_PIN, HIGH);
		// write_display(&lcd, "Car Controller");
		prev_state = current_state;
	}

	// The device will either be controlled by the serial terminal
	// or by the push buttons with pull down configuration
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
		if (!(millis() - last_millis > DEBOUNCE_DELAY)) {
			return;
		}

		read_pins(&button_states);

		if (button_states.event_occured) {
			if (button_states.play_pause_pin) {
				play_pause();
				Serial.println("Play/Pause via button");
			}
			
			else if (button_states.next_pin) {
				next();
				Serial.println("Next track via button");
			}
			
			else if (button_states.prev_pin) {
				prev();
				Serial.println("Previous track via button");
			}

			last_millis = millis(); 
			reset_status(&button_states);
		}
	}

    delay(100);
}

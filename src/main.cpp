#include "Arduino.h"
// #include "Wire.h"

#include "MusicPlayer.h"

#define DEVICE_NAME "Car Controller"
#define DEBOUNCE_DELAY 50

// TODO: Choose convenient pins if needed
#define LED_PIN 2
#define PLAY_PAUSE_PIN 0
#define NEXT_PIN 16
#define PREV_PIN 17
#define SDA_PIN 21
#define SCL_PIN 22

unsigned long last_millis;
MusicPlayer music_player(PLAY_PAUSE_PIN, NEXT_PIN, PREV_PIN, DEVICE_NAME, SDA_PIN, SCL_PIN);

void blink() {
	digitalWrite(LED_PIN, !digitalRead(LED_PIN));
	delay(500);
}

void setup() {
    Serial.begin(115200);

	// Init GPIO
	pinMode(LED_PIN, OUTPUT);

    // Initialize as Bluetooth HID controller
	music_player.begin();
    Serial.println("ESP32 Spotify Remote started!");
    Serial.println("Commands:");
    Serial.println("p - Play/Pause");
    Serial.println("n - Next Track");
    Serial.println("b - Previous Track");
}

void loop() {
	// Blink the LED when the device is not connected
	if (!music_player.is_connected()) {
		blink();
		return;
	} 

	digitalWrite(LED_PIN, HIGH);

	// The device will either be controlled 
    if (Serial.available()) {
        char cmd = Serial.read();

        switch (cmd) {
            case 'p':
                music_player.play_pause();
                Serial.println("Play/Pause");
                break;

            case 'n':
                music_player.next();
                Serial.println("Next track");
                break;

            case 'b':
                music_player.prev();
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

		music_player.read_pins();

		if (music_player.get_state().event_occured) {
			if (music_player.get_state().play_pause_pin) {
				music_player.play_pause();
				Serial.println("Play/Pause via button");
			}
			
			if (music_player.get_state().next_pin) {
				music_player.next();
				Serial.println("Next track via button");
			}
			
			if (music_player.get_state().prev_pin) {
				music_player.prev();
				Serial.println("Previous track via button");
			}

			last_millis = millis(); 
			music_player.reset_status();
		}
	}

    delay(100);
}
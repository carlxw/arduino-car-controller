#include "MusicPlayer.h"

MusicPlayer::MusicPlayer(int play_pause, int next, int prev, std::string device_name, 
                         int sda_pin, int scl_pin) : 
						lcd(LCD_ADDR, LCD_COLS, LCD_ROWS),
						keyboard(device_name) {
    this->play_pause_pin = play_pause;
    this->next_pin = next;
    this->prev_pin = prev;
    
    this->is_playing = false;
    this->button_states = { false, false, false, false };

    // this->keyboard = BleKeyboard(device_name);
    // this->lcd = LiquidCrystal_I2C(LCD_ADDR, LCD_COLS, LCD_ROWS);

    pinMode(this->play_pause_pin, INPUT); 
	pinMode(this->next_pin, INPUT);
	pinMode(this->prev_pin, INPUT);

	this->lcd.init();
	this->lcd.backlight();
	this->lcd_write("Car Controller");
}

void MusicPlayer::play_pause() { this->keyboard.write(KEY_MEDIA_PLAY_PAUSE); } 
void MusicPlayer::next() { this->keyboard.write(KEY_MEDIA_NEXT_TRACK); }
void MusicPlayer::prev() { this->keyboard.write(KEY_MEDIA_PREVIOUS_TRACK); }

void MusicPlayer::read_pins() {
    this->button_states.play_pause_pin = digitalRead(this->play_pause_pin);
	this->button_states.next_pin = digitalRead(this->next_pin);
	this->button_states.prev_pin = digitalRead(this->prev_pin);
	this->button_states.event_occured = 
		this->button_states.play_pause_pin ||
		this->button_states.next_pin ||
		this->button_states.prev_pin;
}

void MusicPlayer::reset_status() { this->button_states = { false, false, false, false }; }

void MusicPlayer::begin() { this->keyboard.begin(); }

bool MusicPlayer::is_connected() { return this->keyboard.isConnected(); }

pin_states MusicPlayer::get_state() { return this->button_states; }

void MusicPlayer::lcd_write(std::string text) {
	this->lcd.clear();
	this->lcd.setCursor(0, 0);
	this->lcd.print(text.c_str());
}

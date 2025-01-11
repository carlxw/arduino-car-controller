#include "Arduino.h"
#include "BleKeyboard.h"
#include "string.h"
#include "LiquidCrystal_I2C.h"

#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_ADDR 0x27

typedef struct pin_states {
	bool play_pause_pin;
	bool next_pin;
	bool prev_pin;
	bool event_occured;
} pin_states;

class MusicPlayer {
    public:
        MusicPlayer(int play_pause, int next, int prev, std::string device_name, 
                    int sda_pin, int scl_pin);
        
        void begin();
        void reset_status();
        void read_pins();
        bool is_connected();
        pin_states get_state();

        void play_pause();
        void next();
        void prev();
    private:  
        pin_states button_states;
        BleKeyboard keyboard;
        LiquidCrystal_I2C lcd;

        int play_pause_pin;
        int next_pin;
        int prev_pin;
        bool is_playing;

        void lcd_write(std::string text);
};

#include <BleKeyboard.h>
#include <M5Cardputer.h>

#include <map>

BleKeyboard bleKey;

void notifyConnection() {
  static bool connected = false;
  if (bleKey.isConnected() && !connected) {
    connected = true;
    M5Cardputer.Speaker.tone(2000, 100);
    delay(100);
    M5Cardputer.Speaker.tone(2300, 100);
  } else if (!bleKey.isConnected() && connected) {
    connected = false;
    M5Cardputer.Speaker.tone(2300, 100);
    delay(100);
    M5Cardputer.Speaker.tone(2000, 100);
  }
}
enum class KeyboardMode { normal, numeric, cursor, function, media };

void charSend(char k, KeyboardMode mode) {
  const std::map<char, char> numKeyMap = {
      {',', '*'}, {'.', '0'}, {'/', '#'}, {'l', '7'}, {';', '8'}, {'\'', '9'},
      {'p', '4'}, {'[', '5'}, {']', '6'}, {'0', '1'}, {'-', '2'}, {'=', '3'}};
  const std::map<char, char> cursorKeyMap = {
      {',', KEY_UP_ARROW},    {'.', KEY_DOWN_ARROW}, {'m', KEY_LEFT_ARROW},
      {'/', KEY_RIGHT_ARROW}, {'k', KEY_HOME},       {'\\', KEY_END},
      {'l', KEY_PAGE_UP},     {';', KEY_PAGE_DOWN}};
  if (mode == KeyboardMode::normal) {
    bleKey.write(k);
  } else if (mode == KeyboardMode::numeric) {
    if (numKeyMap.find(k) != numKeyMap.end()) {
      bleKey.write(numKeyMap.at(k));
    }
  } else if (mode == KeyboardMode::cursor) {
    if (cursorKeyMap.find(k) != numKeyMap.end()) {
      bleKey.write(cursorKeyMap.at(k));
    }
  } else if (mode == KeyboardMode::media) {
    switch (k) {
      case '=':
        bleKey.write(KEY_MEDIA_PLAY_PAUSE);
        break;
      case '0':
        bleKey.write(KEY_MEDIA_PREVIOUS_TRACK);
        break;
      case '-':
        bleKey.write(KEY_MEDIA_NEXT_TRACK);
        break;
      default:
        break;
    }
  }
}
void keySend(m5::Keyboard_Class::KeysState key) {
  static KeyboardMode keyboardMode = KeyboardMode::normal;
  static uint8_t currentModifiers =
      0;  // 直前に有効になっているモディファイアキー
  if (key.fn) {
    if (currentModifiers) {
      M5Cardputer.Speaker.tone(1500, 100);
      keyboardMode = KeyboardMode::normal;
      currentModifiers = 0;
      bleKey.releaseAll();
    } else if (keyboardMode == KeyboardMode::normal) {
      M5Cardputer.Speaker.tone(1600, 100);
      keyboardMode = KeyboardMode::cursor;
    } else if (keyboardMode == KeyboardMode::cursor) {
      M5Cardputer.Speaker.tone(1700, 100);
      keyboardMode = KeyboardMode::numeric;
    } else if (keyboardMode == KeyboardMode::numeric) {
      M5Cardputer.Speaker.tone(1800, 100);
      keyboardMode = KeyboardMode::media;
    } else if (keyboardMode == KeyboardMode::media) {
      M5Cardputer.Speaker.tone(1500, 100);
      keyboardMode = KeyboardMode::normal;
    }
  }
  if (key.opt) {
    M5Cardputer.Speaker.tone(1400, 100);
    // bleKey.press(KEY_LEFT_ALT);
  }
  if (key.modifiers) {
    currentModifiers = key.modifiers;
    if (key.alt) {
      M5Cardputer.Speaker.tone(1300, 100);
      bleKey.press(KEY_LEFT_ALT);
    }
    if (key.ctrl) {
      M5Cardputer.Speaker.tone(1200, 100);
      bleKey.press(KEY_LEFT_CTRL);
    }
    if (key.shift) {
      M5Cardputer.Speaker.tone(1100, 100);
      bleKey.press(KEY_LEFT_SHIFT);
    }
  } else {
    if (key.del) {
      bleKey.write(KEY_BACKSPACE);
    }
    if (key.enter) {
      bleKey.write(KEY_RETURN);
    }
    if (key.space) {
    }
    if (key.tab) {
      bleKey.write(KEY_TAB);
    }
    for (auto i : key.word) {
      charSend(i, keyboardMode);
    }
    bleKey.releaseAll();
  }
}

void keyInput() {
  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
      m5::Keyboard_Class::KeysState keys_status =
          M5Cardputer.Keyboard.keysState();
      keySend(keys_status);
    }
  }
}

void setup() {
  auto cfg = M5.config();
  cfg.serial_baudrate = 115200;
  M5Cardputer.begin(cfg, true);
  bleKey.begin();
  delay(300);
  Serial.println("Hello Tiny keyboard.");
  M5Cardputer.Speaker.setVolume(50);
  M5Cardputer.Speaker.tone(2000, 100);
}

void loop() {
  M5Cardputer.update();
  notifyConnection();
  keyInput();
  delay(20);
}

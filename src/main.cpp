#include <BleKeyboard.h>
#include <M5Cardputer.h>

#include <map>

BleKeyboard bleKey;

void notyifyConnection() {
  static bool connected = false;
  if (bleKey.isConnected() && !connected) {
    Serial.println("Connected");
    connected = true;
    M5Cardputer.Speaker.tone(2000, 100);
  } else if (!bleKey.isConnected() && connected) {
    connected = false;
    M5Cardputer.Speaker.tone(1000, 100);
    Serial.println("Disconnected");
  }
}

void keySend(m5::Keyboard_Class::KeysState key) {
  if (key.modifiers) {
    if (key.alt) {
      Serial.println("alt");
      bleKey.press(KEY_LEFT_ALT);
    }
    if (key.ctrl) {
      Serial.println("ctrl");
      bleKey.press(KEY_LEFT_CTRL);
    }
    if (key.shift) {
      Serial.println("shift");
    }
    if (key.fn) {
      Serial.println("fn");
    }
    if (key.opt) {
      Serial.println("opt");
    }
  } else {
    if (key.del) {
      bleKey.write(KEY_BACKSPACE);
    }
    if (key.enter) {
      bleKey.write(KEY_RETURN);
    }
    if (key.space) {
      Serial.println("space");
    }
    if (key.tab) {
      bleKey.write(KEY_TAB);
    }
    for (auto i : key.word) {
      M5Cardputer.Speaker.tone(4000, 5);
      bleKey.write(i);
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
  notyifyConnection();
  keyInput();
  delay(20);
}

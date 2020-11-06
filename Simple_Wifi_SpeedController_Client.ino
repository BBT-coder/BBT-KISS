/*

	Sample sketch for client-server communication using esp-now

	Connections:

	- Slave

		- Middle potmeter => Wemos => A0
		- Left potmeter   => Wemos => 3V3
		- Right potmeter  => Wemos => GND

*/

#include <espnow.h>

#define WIFI_CHANNEL 1	 // The Wifi Channel to use
#define SEND_DELAY 50   // Delay in ms to update the master with the current potmeter value
#define POTMETER_PIN A0  // The Wemos only has 1 analog port A0

// If SIMULATOR is defined, value goes up and down (127..255..0..255)
#define SIMULATOR

// Use the MACADDR of the Server instead of 0xFF if more clients are near by
uint8_t serverAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


void setup() {

	// Init Serial Monitor
	Serial.begin(115200);

	// Init ESP-NOW
	if (esp_now_init() != 0) {
		Serial.println("Error initializing esp_now, aborting");
		return;
	}

	// Set the role of the Slave 
	esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

	// Add peer
	esp_now_add_peer(serverAddress, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);
}

void loop() {

	static uint32_t update_mmaster_timer = 0;

	if ( millis() > update_mmaster_timer ) {

		// Read the Potmeter value which should return a value between 0..1023

#ifdef SIMULATOR
		static u8 pwm_value = 127;
		static bool forwards = true;

		if (forwards)
			pwm_value++;
		else
			pwm_value--;

		if (pwm_value >= 255) {
			forwards = false;
		}

		if (pwm_value == 0) {
			forwards = true;
		}
#else
		int pot_value = analogRead(POTMETER_PIN);

		// Convert this value to 0..255, need for the Server PWM
		u8 pwm_value = map(pot_value, 0, 1023, 0, 255);
#endif

		// Send message to the Server
		esp_now_send(serverAddress, &pwm_value, sizeof(pwm_value));

		Serial.print("Sent value: ");
		Serial.println(pwm_value);

		// Reset the update master timer
		update_mmaster_timer = millis() + SEND_DELAY;
	}
}

/*

	Sample sketch for client-server communication using esp-now

	Connections:

	- Master

		- Wemos 5V		  => VESC PWM 5V  (Red Wire)
		- Wemos GND		  => VESC PWM -	  (Black Wire)
		- Wemos D1		  => VESC PWM     (White Wire)

*/

#include <espnow.h>
#include <servo.h>
#include <ESP8266WiFi.h>

#define WIFI_CHANNEL 1	 // The Wifi Channel to use
#define ESC_PWM_PIN D1   // The pin to connect ESC PWM

#define ESC_LOW_MS 1000
#define ESC_MID_MS 1500
#define ESC_HIGH_MS 2000

Servo vesc;

volatile boolean new_data_received = false;
u8 incoming_value = 0;

// Function is called when data is incoming
void recv_cb(u8 *macaddr, u8 *data, u8 len)
{

	// Add the Peer if not already exists
	if (!esp_now_is_peer_exist(macaddr))
		esp_now_add_peer(macaddr, ESP_NOW_ROLE_COMBO, WIFI_CHANNEL, NULL, 0);

	new_data_received = true;
	memcpy(&incoming_value, data, sizeof(incoming_value));
};


void update_Speed() {

	int pwm_value = map(incoming_value, 0, 255, ESC_LOW_MS, ESC_HIGH_MS);
	vesc.writeMicroseconds(pwm_value);

	Serial.print(incoming_value);
	Serial.print(" => Send to ESC => ");
	Serial.println(pwm_value);

	new_data_received = false;

}

void setup() {

	// Init Serial Monitor
	Serial.begin(115200);

	// Init ESP-NOW
	if (esp_now_init() != 0) {
		Serial.println("Error initializing ESP-NOW");
		return;
	}

	// Register the receive function
	esp_now_register_recv_cb(recv_cb);

	// Set the role of the Server
	esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

	// Attach the ESC
	vesc.attach(ESC_PWM_PIN);

	// Set middle position
	vesc.writeMicroseconds(ESC_MID_MS);

	Serial.println();
	Serial.print("I am Server, my MAC is: ");
	Serial.println(WiFi.macAddress());

}

void loop() {

	if (new_data_received)
		update_Speed();

}

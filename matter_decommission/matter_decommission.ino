#include <Matter.h>

void setup() {
  Serial.begin(115200);
  while(!Serial); // Wait for Serial Monitor
  
  Matter.begin();
  
  // Check if the device is already paired to a smart home hub
  if (Matter.isDeviceCommissioned()) {
    Serial.println("Clearing old Matter commissioning credentials...");
    Matter.decommission(); 
    Serial.println("Reset complete! You can now re-upload your main sketch.");
  } else {
    Serial.println("Device is not commissioned. Ready for initial setup.");
  }
}

void loop() {
  // Leave empty
}
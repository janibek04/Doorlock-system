// Define the Arduino pins connected to the touch outputs
const int touchPins[] = {23, 25, 27, 29, 31, 33, 35, 37}; // TP1 to TP8 connected to pins 2 to 9
const int buzzer = 48;
void setup() {
  Serial.begin(9600); // Start serial communication
  // Initialize touch pins as inputs
  for (int i = 0; i < 8; i++) {
    pinMode(touchPins[i], INPUT);
  }
  pinMode(buzzer, OUTPUT);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    if (digitalRead(touchPins[i]) == HIGH) { // Check if touch detected
      Serial.print("Touch detected on key: ");
      Serial.println(i + 1); // Print the key number
      digitalWrite(buzzer, HIGH);
      delay(20);
      digitalWrite(buzzer, LOW);
    }
  }
  delay(100); // Small delay to debounce and reduce serial output speed
}

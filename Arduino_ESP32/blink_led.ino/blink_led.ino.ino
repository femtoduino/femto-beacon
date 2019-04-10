
int led_r = 32; // GPIO32
int led_g = 33; // GPIO33
int led_b = 27; // GPIO34

void setup() {
  // put your setup code here, to run once:
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led_r, LOW);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_b, HIGH);
  Serial.println("Hello RED!");
  delay(1000);
  digitalWrite(led_r, HIGH);
  digitalWrite(led_g, LOW);
  digitalWrite(led_b, HIGH);
  Serial.println("Hello GREEN!");
  delay(1000);
  digitalWrite(led_r, HIGH);
  digitalWrite(led_g, HIGH);
  digitalWrite(led_b, LOW);
  Serial.println("Hello BLUE!");
  delay(1000);
}

/* ************************************************* *
 * ------ ORC DATASTREAMING IoT ARDUINO NODE ------- *
 * --------- coded by Jaime Laborda Macari---------- *
 * -------------- ORC2.17 - MAKERSUPV -------------- *
 * ------https://github.com/makers-upv/orc-iot ----- *
 * ************************************************* */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:

  int raw = analogRead(A0);

  float volts = (raw*5.0)/4096.0;

  float db = 100-20*volts;

  Serial.println(String(volts)+" | "+String(db));

  delay(100);

}

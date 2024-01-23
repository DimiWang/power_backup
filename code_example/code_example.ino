#define MIN_INPUT_VOLTAGE 11.5f /*V*/
#define MAX_INPUT_VOLTAGE 16.0f /*V*/
#define CUTOFF_VOLTAGE 10.0f    /*V*/
#define BATTERY_CAPACITY 5000   /*Ah*/

//digital
#define LED_YELLOW_PIN (6)
#define LED_GREEN_PIN (5)
#define LED_RED_PIN (4)
#define VOUT_REL_ON_PIN (9)
#define VOUT_REL_OFF_PIN (8)
#define OC_OUTPUT_PIN (7)
//analog
#define VOUT_ANALOG (2)
#define VIN_ANALOG (1)
#define IOUT_ANALOG (0)

#define MCU_VDD 3.3f /*mV*/
#define R1 15        /*k*/
#define R2 100       /*k*/

enum Led { GREEN_LED,
           YELLOW_LED,
           RED_LED };

enum AnalogValue { OUT_VOLTAGE,
                   IN_VOLTAGE,
                   OUT_CURRENT };

enum StateMachine {
  MEASURE,
  CONTROL,
  DELAY
};

#include <CircularBuffer.h>

unsigned long time = 0;

static CircularBuffer<float, 50> Vin_queue;
static CircularBuffer<float, 50> Vout_queue;
static CircularBuffer<float, 50> Iout_queue;


static float Vin;
static float Vout;
static float Iout;
static int step = MEASURE;
static bool input_voltage_present;
static float battery_capacity;
static bool output_on;

void setLedState(Led led, bool s) {
  const uint8_t leds[] = { LED_GREEN_PIN, LED_YELLOW_PIN, LED_RED_PIN };
  if (s == true) {
    pinMode(leds[led], OUTPUT);
    digitalWrite(leds[led], LOW);
  } else {
    pinMode(leds[led], INPUT);
  }
}

void setOutRelayOn(bool s) {
  if (s == true) {
    digitalWrite(VOUT_REL_ON_PIN, HIGH);
    delay(10);
    digitalWrite(VOUT_REL_ON_PIN, LOW);
  } else {
    digitalWrite(VOUT_REL_OFF_PIN, HIGH);
    delay(10);
    digitalWrite(VOUT_REL_OFF_PIN, LOW);
  }
}

float readAnalogValue(AnalogValue a_in) {
  float result;
  const float adc_res = (MCU_VDD / 1024);
  switch (a_in) {
    case OUT_VOLTAGE:
      result = (adc_res * (float)analogRead(VOUT_ANALOG) / R1) * (R1 + R2);
      break;
    case IN_VOLTAGE:
      result = (adc_res * (float)analogRead(VIN_ANALOG) / R1) * (R1 + R2);
      break;
    case OUT_CURRENT:
      result = 0;
      /*TBD*/
      break;
  }
  return result;
}


void calculateBatteryCapacity() {
  // TODO
}



// the setup function runs once when you press reset or power the board
void setup() {

  pinMode(LED_BUILTIN, INPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(LED_YELLOW_PIN, INPUT);
  pinMode(LED_GREEN_PIN, INPUT);
  pinMode(LED_RED_PIN, INPUT);
  pinMode(VOUT_REL_ON_PIN, OUTPUT);
  pinMode(VOUT_REL_OFF_PIN, OUTPUT);
  pinMode(OC_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
  output_on = false;
  time = millis();
}

// the loop function runs over and over again forever
void loop() {
  if (millis() - time >= 100) {
    time = millis();

    Vin = readAnalogValue(IN_VOLTAGE);
    Vout = readAnalogValue(OUT_VOLTAGE);
    Iout = readAnalogValue(IOUT_ANALOG);

    if (millis() - time >= 2000) {
      /*D*/ Serial.println(Vin);
      /*D*/ Serial.println(Vout);
      /*D*/ Serial.println(Iout);

      input_voltage_present = false;
      if (Vin > MIN_INPUT_VOLTAGE) {
        input_voltage_present = true;
      }
      if (!input_voltage_present && Vout < CUTOFF_VOLTAGE) {
        setOutRelayOn(false);
        output_on = false;
      } else if (!output_on) {
        setOutRelayOn(true);
        output_on = true;
        step = DELAY;
      }
      /* set staus leeds*/
      /* turn off relays*/
      /* count battery capacity*/
      /* detect battery output*/
      calculateBatteryCapacity();
    }
  }
}

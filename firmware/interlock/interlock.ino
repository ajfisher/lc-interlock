#include "./OneWire.h"
#include "./DallasTemperature.h"

OneWire  ds(3);  // on pin 2
DallasTemperature temps(&ds);

#define TEMPERATURE_PRECISION 9

DeviceAddress inflowTemp 	= {0x28, 0xFF, 0x6C, 0xF3, 0x64, 0x15, 0x2, 0x15};
DeviceAddress outflowTemp  	= {0x28, 0xFF, 0xD2, 0xE1, 0x64, 0x15, 0x2, 0xC7};
DeviceAddress tankTemp		= {0x28, 0xFF, 0xDB, 0x8D, 0x64, 0x15, 0x2, 0xB6};

#define FLOW_RATE_MEASURE_INTERVAL 1000.0

byte flowInterrupt = 0; // digital 2
byte flowPin = 2; //

volatile byte pulseCount;

uint16_t pulsesPerLitre = 450;
float flowPeriods = 60000.0 / FLOW_RATE_MEASURE_INTERVAL; // used to calc flow per min.

float flowRate;
uint32_t flowOldTime;

void setup() {
    Serial.begin(9600);
    Serial.println("Interlock stuff");

    // temperature sensors
    temps.begin();
    Serial.print("Locating sensors...");
    Serial.print("found ");
    Serial.print(temps.getDeviceCount());
    Serial.println(" devices");

	temps.setResolution(inflowTemp, TEMPERATURE_PRECISION);
	temps.setResolution(outflowTemp, TEMPERATURE_PRECISION);
	temps.setResolution(tankTemp, TEMPERATURE_PRECISION);

    delay(5000);
    // flow meter.
    pulseCount = 0;
    flowOldTime = 0;
    flowRate = 0;

    attachInterrupt(flowInterrupt, pulseCounter, FALLING);
}

void loop() {

    if ((millis() - flowOldTime) > FLOW_RATE_MEASURE_INTERVAL) {
        calculate_flow_rate();

		temps.requestTemperatures();
		Serial.print("Out ");
		printTemperature(outflowTemp);
		Serial.println();
    }
}

void calculate_flow_rate() {
    // does the actual checking to see what the flow rate is and updates flowrate
    // global var appropriately

    detachInterrupt(flowInterrupt);

    Serial.print("pc: ");
    Serial.println(pulseCount);
    // calculate flow rate.
    // this normalises the pulse count to a standardised time period,
    // namely FLOW_RATE_MEASURE_INTERVAL.
    float pcn = (float)FLOW_RATE_MEASURE_INTERVAL / (millis() - flowOldTime) * pulseCount;
    float litres_counted = pcn / pulsesPerLitre;
    flowRate = flowPeriods * litres_counted;

    Serial.print("Flow Rate ");
    Serial.println(flowRate, DEC);

    flowOldTime = millis();

    pulseCount = 0;

    attachInterrupt(flowInterrupt, pulseCounter, FALLING);
}

/** ISR for pulsecounter **/
void pulseCounter() {
    // just increment, don't do anything else.
    pulseCount++;
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = temps.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
}

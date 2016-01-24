#include "./OneWire.h"
#include "./DallasTemperature.h"

OneWire  ds(3);  // on pin 2
DallasTemperature temps(&ds);

#define TEMPERATURE_PRECISION 9

#define TEMP_UPPER_RANGE 28.5
#define TEMP_LOWER_RANGE 10.0

DeviceAddress inflowTemp 	= {0x28, 0xFF, 0x6C, 0xF3, 0x64, 0x15, 0x2, 0x15};
DeviceAddress outflowTemp  	= {0x28, 0xFF, 0xD2, 0xE1, 0x64, 0x15, 0x2, 0xC7};
DeviceAddress tankTemp		= {0x28, 0xFF, 0xDB, 0x8D, 0x64, 0x15, 0x2, 0xB6};

float temperatures[3];

#define FLOW_RATE_MEASURE_INTERVAL 1000.0
#define FLOW_RATE_LOWER 1.2

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

        update_temperatures();

        Serial.print("In ");
		Serial.print(temperatures[0]);
        Serial.print(" Out ");
        Serial.print(temperatures[1]);
        Serial.print(" Tank ");
        Serial.print(temperatures[2]);
		Serial.println();
    }

    // now we do warning checks.
    alarm_checks();

    // TODO switch relay based on alarm.
}


bool alarm_checks() {

    // test flow rate to see if too low
    bool flowAlarm = false;
    if (flowRate < FLOW_RATE_LOWER) {
        flowAlarm = true;
    }

    // test all three temps and make sure they are in healthy range
    bool tempAlarm = false;
    for (uint8_t i=0; i < 3; i++) {
        if (temperatures[i] > TEMP_UPPER_RANGE || temperatures[i] < TEMP_LOWER_RANGE) {
            tempAlarm = true;
        }
    }

    if (flowAlarm || tempAlarm) {
        return (true);
    } else {
        return(false);
    }

    return(true);
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

void update_temperatures() {
    // updates the temperature values;
	temps.requestTemperatures();
    temperatures[0] = temps.getTempC(inflowTemp);
    temperatures[1] = temps.getTempC(outflowTemp);
    temperatures[2] = temps.getTempC(tankTemp);
}

/** ISR for pulsecounter **/
void pulseCounter() {
    // just increment, don't do anything else.
    pulseCount++;
}



void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = temps.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
}

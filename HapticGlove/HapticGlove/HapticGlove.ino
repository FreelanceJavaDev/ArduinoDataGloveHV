#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/*
   BNO055 Connections
   ==================
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3.3-5V DC
   Connect GROUND to common ground
 */

 /* Set the delay between fresh samples */
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100; //shortened to 11ms for actual
uint16_t PRINT_DELAY_MS = 500; //testing, shortened to 55 for actual
const uint8_t PRECISION = 3;
uint8_t printCount = 0;
bool activateHaptics = false; //activate capacitor
double dx, dy, dz;
//const uint8_t calibrationData[22] = {0x9,0x0,0xCF,0xFF,0xF4,0xFF,0xB4,0xFD,0xD,0x0,0xC7,0xFF,0xFF,0xFF,0xFD,0xFF,0x1,0x0,0xE8,0x3,0xDD,0x2};


//velocity = accel*dt (dt in seconds)
//position = 0.5*accel*dt^2
const double ACCEL_VEL_TRANSLATION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0; // dt
const double ACCEL_POS_TRANSLATION = 0.5 * ACCEL_VEL_TRANSLATION * ACCEL_VEL_TRANSLATION; //0.5*dt^2
const double DEG_2_RAD = 0.01745329251; //trig functions require radians, BNO055 outputs degrees

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);


int capacitor_pin = 4;  //pin 4
int piezo_pin = 8; //vibration motor pin signal driver
int motor_pin = 7; //motor activation signal pin

void reelBack() {
  unsigned long t_m = micros();
  digitalWrite(motor_pin, LOW); //ON
  while((micros() - t_m) < 5000); // 5ms
  digitalWrite(motor_pin, HIGH); //OFF
}

void capacitorClamp(){
  digitalWrite(capacitor_pin, HIGH); //ON
}

void capacitorRelease() {
  digitalWrite(capacitor_pin, LOW); // OFF
}

//setup hardware, runs once
void setup() {
  Serial.begin(9600);
  pinMode(capacitor_pin, OUTPUT);
  pinMode(piezo_pin, OUTPUT);
  digitalWrite(piezo_pin, LOW);
  digitalWrite(capacitor_pin, LOW); //OFF
  pinMode(motor_pin, OUTPUT);
  digitalWrite(motor_pin, HIGH);

  while(!Serial);
  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  
  //bno.setSensorOffsets(calibrationData);
  dx = 0;
  dy = 0;
  dz = 0;
  delay(1000);

}

//runs repeatedly after setup()
void loop() {
  
  unsigned long t_0 = micros();
  sensors_event_t orientationData , angVelocityData , linearAccelData;
  //imu::Vector<3> euler = bno.getQuat().toEuler();
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  
  //absolute postion determined in Unity
  dx = dx + ACCEL_POS_TRANSLATION * linearAccelData.acceleration.x;
  dy = dy + ACCEL_POS_TRANSLATION * linearAccelData.acceleration.y;
  dz = dz + ACCEL_POS_TRANSLATION * linearAccelData.acceleration.z;
  
  if(printCount * BNO055_SAMPLERATE_DELAY_MS >= PRINT_DELAY_MS) {
    printSensor(&orientationData, "_O");
    Serial.flush();
    //String orientationPrint = "_O" + String(euler[0], PRECISION)+ "," + String(euler[1], PRECISION) + "," + String(euler[2], PRECISION) + "_";
    printSensor(&angVelocityData, "_G");
    Serial.flush();
    printSensor(&linearAccelData, "_A");
    Serial.flush();
    String posPrint = "_P" + String(dx, PRECISION) + "," + String(dy, PRECISION) + "," + String(dz, PRECISION) + "_";
    Serial.println(posPrint);
    Serial.flush();
    printCount = 0;
    dx = 0;
    dy = 0;
    dz = 0;
  }
  else { ++printCount; }

    while ((micros() - t_0) < (BNO055_SAMPLERATE_DELAY_MS * 1000))
  {
    if(Serial.available() < 0) { //somethig to read
      bool rd_value = Serial.parseInt() == 1 ? true : false; 
      hapticFeedback(rd_value);
    }
  }

}

//haptic feedback Unity will send 0 for release or 1 for charge
void hapticFeedback(bool unity_sig) {
  if(!activateHaptics && unity_sig) {
    digitalWrite(piezo_pin, HIGH);
    delay(5); //5ms
    digitalWrite(piezo_pin, LOW);
    activateHaptics = unity_sig;
  }
  else if(activateHaptics && !unity_sig) {
    activateHaptics = unity_sig;
  }
  if(activateHaptics){ capacitorClamp(); }
  else { 
      capacitorRelease();
      reelBack(); //activate capactor reel motor
  }
}

//outputs sensor data to string then string is printed, note print and println can only print one varaible OR one string literal per call.
//Unity may take individual prints as individual messages ensures 1 message per sensor
void printSensor(sensors_event_t* event, const String prefix) {
  String output = prefix;
    switch(event->type) {
      case SENSOR_TYPE_ACCELEROMETER:
        output += String(event->acceleration.x, PRECISION) + "," + String(event->acceleration.y, PRECISION) + "," + String(event->acceleration.z, PRECISION);
          break;
      case SENSOR_TYPE_ORIENTATION:
        output += String(event->orientation.x, PRECISION) + "," + String(event->orientation.y, PRECISION) + "," + String(event->orientation.z, PRECISION); 
        break;
      case SENSOR_TYPE_GYROSCOPE:
      case SENSOR_TYPE_ROTATION_VECTOR: //gyro & rotation are the same thing
        output += String(event->gyro.x, PRECISION) + "," + String(event->gyro.y, PRECISION) + "," + String(event->gyro.z, PRECISION);
        break;
      default: return; //something's gone wrong
    }
    output += "_";
    Serial.println(output);
}

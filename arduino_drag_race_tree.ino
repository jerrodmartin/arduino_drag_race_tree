// LEDs
const int prestageLed = 13;
const int stageLed = 3;
const int yellowLedOne = 12;
const int yellowLedTwo = 11;
const int yellowLedThree = 10;
const int startLed = 9;
const int faultLed = 8;

// Stage Sensors
const int prestageSensor = 7;
const int stageSensor = 6;
const int guardSensor = 5;
const int finishSensor = 4;

// Other Constants
const int FEET_PER_MILE = 5280;
const int TRACK_LENGTH_FEET = 200;

enum Phases {
  standby,
  prestaged,
  staged,
  started,
  finished,
  faulted,
};

int phase = Phases::standby;
int readyTime, startTime, finishedTimeMs;

void setup() {
  Serial.begin(9600);
  pinMode(prestageLed, OUTPUT);
  pinMode(stageLed, OUTPUT);
  pinMode(yellowLedOne, OUTPUT);
  pinMode(yellowLedTwo, OUTPUT);
  pinMode(yellowLedThree, OUTPUT);
  pinMode(startLed, OUTPUT);
  pinMode(faultLed, OUTPUT);

  pinMode(prestageSensor, INPUT);
  pinMode(stageSensor, INPUT);
  pinMode(guardSensor, INPUT);
  pinMode(finishSensor, INPUT);
}

void loop() {
  switch (phase) {
    case Phases::standby: {
      Serial.println("we are standing by");
      if (digitalRead(prestageSensor) == 1) {
        digitalWrite(prestageLed, 1);
        phase = Phases::prestaged;
      }
      break;
    }
    case Phases::prestaged: {
      Serial.println("we are prestaged");
      if (digitalRead(prestageSensor) == 1 && digitalRead(stageSensor) == 1) {
        digitalWrite(stageLed, 1);
        readyTime = millis();
        phase = Phases::staged;
      }
      break;
    }
    case Phases::staged: {
      Serial.println("we are staged");
      // Check if guard triggered
      if (digitalRead(guardSensor) == 1) {
        phase = Phases::faulted;
        break;
      }
      // Check if unstaged
      if (digitalRead(stageSensor) == 0) {
        digitalWrite(stageLed, 0);
        digitalWrite(yellowLedOne, 0);
        digitalWrite(yellowLedTwo, 0);
        digitalWrite(yellowLedThree, 0);
        phase = Phases::prestaged;
        break;
      }
      int elapsed = millis() - readyTime;
      if (elapsed >= 5500 && elapsed < 6000) {
        digitalWrite(yellowLedOne, 1);
      }
      else if (elapsed >= 6000 && elapsed < 6500) {
        digitalWrite(yellowLedTwo, 1);
      }
      else if (elapsed >= 6500 && elapsed < 7000) {
        digitalWrite(yellowLedThree, 1);
      }
      else if (elapsed >= 7000) {
        digitalWrite(startLed, 1);
        startTime = millis();
        phase = Phases::started;
      }
      break;
    }
    case Phases::started: {
      Serial.println("We are started");
      if (digitalRead(finishSensor) == 1) {
        finishedTimeMs = millis();
        phase = Phases::finished;
      }
      break;
    }
    case Phases::finished: {
      float secs = float(finishedTimeMs - startTime) / 1000;
      Serial.print("Elapsed time was: ");
      Serial.print(secs);
      Serial.println(" seconds");

      float fps = (TRACK_LENGTH_FEET / secs);
      Serial.print("fps:"); 
      Serial.println(fps, 4);
      float mph = (fps / FEET_PER_MILE) / 0.00028;
      Serial.print("mph:");
      Serial.println(mph, 4);

      digitalWrite(faultLed, 0);
      digitalWrite(prestageLed, 0);
      digitalWrite(stageLed, 0);
      digitalWrite(yellowLedOne, 0);
      digitalWrite(yellowLedTwo, 0);
      digitalWrite(yellowLedThree, 0);
      digitalWrite(startLed, 1);
      break;
    }
    case Phases::faulted: {
      Serial.println("We are faulted");
      digitalWrite(faultLed, 1);
      digitalWrite(prestageLed, 0);
      digitalWrite(stageLed, 0);
      digitalWrite(yellowLedOne, 0);
      digitalWrite(yellowLedTwo, 0);
      digitalWrite(yellowLedThree, 0);
      digitalWrite(startLed, 0);
      break;
    }
  }
}

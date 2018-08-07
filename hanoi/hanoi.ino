/*
 * Hanoi Robot
 * 
 * Sort the Tower of Hanoi using 2 stepper motors and switches.
 * This example code is in the public domain.
 * Created June 2018 by -T.K.-
 * 
 * Demo video is here: 
 */

class HanoiBot {
  public:
    int VERT_SPEEDS[2] = {1, 10};                       // different vertical speed due to gravity{down, up}
    int HORI_SPEED = 12, VERT_SPEED = VERT_SPEEDS[1];

    // Define pins
    int hori_dirP = 4, hori_stepP = 5, vert_dirP = 7, vert_stepP = 6, reset_horiSWP = 3, reset_vertSWP = 2, valveP = 9;
    
    int AB_distance = 125, AC_distance = 253;           // the distance between three towers
    
    int target_position = 0, current_position = 0;
    int A_count = 8, B_count = 0, C_count = 0;
    
    HanoiBot() {
      /*
       * Constructor
       */
      for (int i=4; i<13; i++) {pinMode(i, OUTPUT);}
      pinMode(2, INPUT); pinMode(3, INPUT); pinMode(13, INPUT);
    };
    
    void rotate(int motor, int steps) {
      /*
       * Rotate <motor> for <step> steps
       * 
       * @int motor: 0 for horizontal motor, 1 for vertical motor
       * @int steps: number of steps, negative value for turning opposite direction.
       */
      int dirPin, stepPin, vel;
      if (motor == 0) {
        dirPin = hori_dirP; stepPin = hori_stepP; vel = HORI_SPEED;
      } else {
        dirPin = vert_dirP; stepPin = vert_stepP; vel = VERT_SPEED;
      }
      
      digitalWrite(dirPin, steps <= 0);             // change the direction accordint to the sign of <steps>
      
      for (int i=0; i < abs(steps); i++) {
        digitalWrite(stepPin, LOW); delayMicroseconds(1);
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(int(800 / i + 1) + vel);
      }
    }
    
    void grab(int level) {
      /*
       * Grab the disk via pneumatic sucker
       * 
       * @int level: level of the disk
       */
      VERT_SPEED = VERT_SPEEDS[0];
      rotate(1, level*1600 + 4650);
      delay(70);                                    // wait 70ms for pneumatic sucker to hold on 
      VERT_SPEED = VERT_SPEEDS[1];
      rotate(1, -(level*1600 + 4650));
    }
    
    void jettison() {
      /*
       * Release the disk
       */
      digitalWrite(valveP, HIGH);
      delay(35); 
      digitalWrite(valveP, LOW);
    }
    
    void reset() {
      /*
       * Reset the motor position
       */
      while (!digitalRead(reset_vertSWP)) {rotate(1, -3);}
      while (!digitalRead(reset_horiSWP)) {rotate(0, -3);}
    }
    
    void rotateDistance(int distance) {
      int steps = int(distance * 107.142857);       // convert into centimeters
      rotate(0, steps);
    }
    
    void moveToPoint(char point) {
      int diff_distance;
      switch (point) {
        case 'A':
          diff_distance = -current_position;
          if (diff_distance < -5) rotateDistance(diff_distance + 5);
          reset();
          current_position = 0;
          break;
        case 'B':
          diff_distance = AB_distance - current_position;
          rotateDistance(diff_distance);
          current_position = AB_distance;
          break;
        case 'C':
          diff_distance = AC_distance - current_position;
          rotateDistance(diff_distance);
          current_position = AC_distance;
          break;
      }
    }

    void perform(char a, char b) {
      /*
       * Execute the entire procedure once
       * 
       * @char a: grab one disk from position a
       * @char b: release the disk to position b
       */
      moveToPoint(a);
      switch (a) {
        case 'A':
          grab(9 - A_count);
          A_count --;                     // keep track of the number of disks in each tower 
          if (b == 'B') {
            moveToPoint('B');
            B_count ++;                   // same for below
          } else {
            moveToPoint('C');
            C_count ++;
          }
          break;
        case 'B':
          grab(9 - B_count);
          B_count --;
          if (b == 'A') {
            moveToPoint('A');
            A_count ++;
          } else {
            moveToPoint('C');
            C_count ++;
          }
          break;
        case 'C':
          grab(9 - C_count);
          C_count --;
          if (b == 'A') {
            moveToPoint('A');
            A_count ++;
          } else {
            moveToPoint('B');
            B_count ++;
          }
          break;
      }
      jettison();
    }
    
    void solve(int n, char x, char y, char z) {
      /*
       * Run the process by recursion
       * 
       * @int n: the number of disks to sort out. (how many levels will be in the result)
       * @char x, y, z: placeholders for passing parameters within recursion progress
       */
      if (n==1) { 
        perform(x, z);
      } else {
        solve(n - 1, x, z, y);
        solve(1, x, y, z);
        solve(n - 1, y, x, z);
      }
    }
};

HanoiBot machine = HanoiBot();                    // instantiate a Hanoi Robot object.

void setup() {
  machine.reset();                                // reset the robot to starting position
}

void loop() {
  if(digitalRead(13)){                            // if start button is pushed
    machine.solve(6, 'A', 'B', 'C');
  }
  delay(2000);
}


#include <Pixy2.h>
#define DELTA 25
#define MIN_SPEED_VAL 100
#define K 1

#define min_width 70
#define normal_width 90
#define max_width 110

#define min_width_working_spectrum 10
#define max_width_working_spectrum 316 // ?

#define min_x 150
#define normal_x 165
#define max_x 180

#define min_x_working_spectrum 20  // min left position of x
#define max_x_working_spectrum 316 // max left position of x

#define speed_value 160 

#define pin_en_a 9            // roata Dreapta
#define pin_en_b 10           // roata Stanga
#define pin_forward_right 7   // roata dreapta in fata(IN1)
#define pin_forward_left 5   // roata stanga in fata(IN3)
#define pin_backwards_left 4  // roata stanga in spate(IN4)
#define pin_backwards_right 6 // roata dreapta in spate(IN2)

int pow_right_wheel;
int pow_left_wheel;

int speed = 0;
int obj_x = 0;
int obj_width = 0;
int mapped_x;
int i = 0;

float interp_x;
float interp_w;
Pixy2 pixy;

// int D_W[] = {0,   40,  60,  105, 145, 175, 225, 240, 255, 270, 316};
int D_W[] = {0,   20,  30,  40,  60,   min_width,          max_width,   190,  240, 280, 316};
int D_X[] = {0,   15,  40,  65,  90,    min_x,               max_x,     220,  250, 275, 316};

int R_W[] = {255, 255, 225, 200, 175,  MIN_SPEED_VAL,    MIN_SPEED_VAL,   120,  130,   140, 150};
int R_X[] = {255,  230,  200,  180,  150,  MIN_SPEED_VAL,    MIN_SPEED_VAL,   150,  180,   200,  255};

int filterExpMovingAverage(int value, bool isWidth) {
  static int filteredValueWidth = 0;
  static int filteredValueX = 0;

  if(isWidth) {
      filteredValueWidth = (filteredValueWidth >> 1) + (value >> 1);
    return filteredValueWidth;
  } else {
      filteredValueX = (filteredValueX >> 1) + (value >> 1);
    return filteredValueX;
  }
}
void setup() {

  Serial.begin(9600);
  pixy.init();

  pinMode(pin_forward_right, OUTPUT);
  pinMode(pin_forward_left, OUTPUT);

  pinMode(pin_backwards_right, OUTPUT);
  pinMode(pin_backwards_left, OUTPUT);

  pinMode(pin_en_a, OUTPUT);
  pinMode(pin_en_b, OUTPUT);

  digitalWrite(pin_forward_right, LOW);
  digitalWrite(pin_forward_left, LOW);
  digitalWrite(pin_backwards_right, LOW);
  digitalWrite(pin_backwards_left, LOW);
}

int filterExpFrontZone(int value) {
  static int filteredValue = 0;

  filteredValue += ((value << 4) - filteredValue) >> K;
  return (filteredValue >> 4);
}

// Iterative implementation of the binary search algorithm to return
// the position of `target` in array `nums` of size `n`
int binarySearch(int nums[], int n, int target)
{
    // search space is nums[low…high]
    int low = 0, high = n - 1;
 
    // loop till the search space is exhausted
    while (low <= high)
    {
        // find the mid-value in the search space and
        // compares it with the target
 
        //int mid = (low + high)/2;    // overflow can happen
         int mid = low + (high - low)/2;
        // int mid = high - (high - low)/2;
 
        // target value is found
        if (target == nums[mid]) {
            return mid;
        }
 
        // if the target is less than the middle element, discard all elements
        // in the right search space, including the middle element
        else if (target < nums[mid]) {
            high = mid - 1;
        }
 
        // if the target is more than the middle element, discard all elements
        // in the left search space, including the middle element
        else {
            low = mid + 1;
        }
    }
 
    // target doesn't exist in the array
    return low;
}

void process_data()
{
  obj_x = pixy.ccc.blocks[i].m_x;
  obj_width = pixy.ccc.blocks[i].m_width; 

  obj_width = filterExpMovingAverage(obj_width, 1);
  obj_x = filterExpMovingAverage(obj_x, 0);

  int pos_x = binarySearch(D_X, 11, obj_x);
  int pos_w = binarySearch(D_W, 11, obj_width);
  interp_x = R_X[pos_x - 1] + float((obj_x - D_X[pos_x - 1])) / float((D_X[pos_x] - D_X[pos_x - 1])) * (R_X[pos_x] - R_X[pos_x - 1]);
  interp_w = R_W[pos_w - 1] + (float((obj_width - D_W[pos_w - 1])) / float((D_W[pos_w] - D_W[pos_w - 1]))) * (R_W[pos_w] - R_W[pos_w - 1]);
  
  // case when object is too close
  // move backwards
  if (obj_width >= max_width)
  {
    // set left wheel 
    digitalWrite(pin_forward_left, LOW);
    digitalWrite(pin_backwards_left, HIGH);

    // set right wheel 
    digitalWrite(pin_forward_right, LOW);
    digitalWrite(pin_backwards_right, HIGH); 

    speed = speed_value;
    
    pow_left_wheel = speed;
    pow_right_wheel = speed;
 
  }
  // case when object is too far
  // move forward
  else if(obj_width <= min_width)
  {

    // set left wheel 
    digitalWrite(pin_forward_left, HIGH);
    digitalWrite(pin_backwards_left, LOW);

    // set right wheel 
    digitalWrite(pin_forward_right, HIGH);
    digitalWrite(pin_backwards_right, LOW); 


    speed = interp_w ;// R_W
    
    // set the direction for wheels acording to x position of object
    if(obj_x <=  min_x)
    {
      pow_left_wheel = speed - interp_x;
      pow_left_wheel = constrain(pow_left_wheel, 50, 255);
      pow_right_wheel = speed;
    }
    else if (obj_x>min_x && obj_x <= max_x)
    {
      pow_left_wheel = speed;
      pow_right_wheel = speed;
    }
    else if(obj_x>max_x)
    {
      pow_right_wheel = speed - interp_x;
      pow_right_wheel = constrain(pow_right_wheel, 50, 255);
      pow_left_wheel = speed;
    }
    //end forward
  }
  // case when object is at normal distance
  else {
    pow_left_wheel = 0;
    pow_right_wheel = 0;
  }

}

void loop() {
  if (pixy.ccc.getBlocks() > 0)
  {
    process_data();  
    pow_left_wheel  = constrain(pow_left_wheel, 0, 255);
    pow_right_wheel = constrain(pow_right_wheel,0, 255);
    Serial.println();    
    Serial.print("Width:");
    Serial.print(obj_width);
    Serial.print(",");
    Serial.print("X:");
    Serial.print(obj_x);
    Serial.print(",");
    
    Serial.print("Left_Speed:");
    Serial.print(pow_left_wheel);
    Serial.print(",");
    Serial.print("Right_Speed:");
    Serial.print(pow_right_wheel);
  }
  else
  {
    pow_left_wheel = 0;
    pow_right_wheel = 0;
    Serial.println("No object found!");
  }
  
  // set speed for wheels
  analogWrite(pin_en_a, pow_right_wheel);
  analogWrite(pin_en_b, pow_left_wheel);
}

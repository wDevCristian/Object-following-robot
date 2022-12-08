#include <Pixy2.h>
#define DELTA 25
#define MIN_SPEED_VAL 40
#define K 2

#define min_width 70
#define normal_width 90
#define max_width 110

#define min_width_working_spectrum 10
#define max_width_working_spectrum 316 // ?

#define min_x 140
#define normal_x 165
#define max_x 190

#define min_x_working_spectrum 20  // min left position of x
#define max_x_working_spectrum 316 // max left position of x

#define speed_value 150 

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
int D_W[] = {0,   20,  30,  40,  60,   min_width, max_width, 190, 240, 280, 316};
int D_X[] = {0,   15,  40,  65,  90,    min_x,        max_x, 220, 250, 275, 316};

int R_W[] = {220, 220, 150, 110,  70,      31,         31,    70,  86,  91, 101};
int R_X[] = {100, 100,  90, 85,   60,      30,         30,    60,  85,  90, 100};

int filterExpMovingAverage(int value, bool isWidth) {
  static int filteredValueWidth = 0;
  static int filteredValueX = 0;

  if(isWidth) {
    //if((filteredValueWidth-value) >= 20)
      filteredValueWidth = (filteredValueWidth >> 1) + (value >> 1);
     // filteredValueWidth += ((value << 4) - filteredValueWidth) >> 1;
//else
     //filteredValueWidth += ((value << 4) - filteredValueWidth) >> K_COEF;
    
    return filteredValueWidth;
  } else {
    // if((filteredValueX-value) >= 10)
      // filteredValueX += ((value << 4) - filteredValueX) >> 1;
    // else
      //filteredValueX += ((value << 4) - filteredValueX) >> K_COEF;
    
      filteredValueX = (filteredValueX >> 1) + (value >> 1);
    return filteredValueX;
  }
}
int filterExpFrontZone(int value) {
  static int filteredValue = 0;

  filteredValue += ((value << 4) - filteredValue) >> K;
  return (filteredValue >> 4);
}
int binarySearch(int arr[], int l, int r, int x)
{
    if (r >= l) {

        int mid = l + (r - l) / 2;
 
        // If the element is present at the middle
        // itself
        if (arr[mid] == x)
            return mid;
 
        // If element is smaller than mid, then
        // it can only be present in left subarray
        if (arr[mid] > x)
            return binarySearch(arr, l, mid - 1, x);
 
        // Else the element can only be present
        // in right subarray
        return binarySearch(arr, mid + 1, r, x);
    }
 
    // We reach here when element is not
    // present in array
    return l;
}

void setup() {

  // setare frecventa pin D9, D10
  //TCCR1B = 0b00000100;
  //TCCR1A = 0b00000001;


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
void limit()
{
    if(pow_right_wheel < MIN_SPEED_VAL)
    {
      pow_right_wheel = 0;
    }

    if (pow_right_wheel > 255 )
    {
      pow_right_wheel = 255;
    }

    if (pow_left_wheel < MIN_SPEED_VAL)
    {
      pow_left_wheel = 0;
    }

    if (pow_left_wheel > 255 )
    {
      pow_left_wheel = 255;
    }
}
void  move_x()
{
  // Serial.print("NF_Interp_X:");
  // Serial.print(interp_x);
  interp_x = filterExpFrontZone(interp_x);
   
    // Serial.print(",");
    // Serial.print("F_Interp_X:");
    // Serial.print(interp_x);
  if(obj_x <=  min_x)
  {
    // object is at left 
    //mapped_x = map(obj_x, min_x_working_spectrum , min_x, speed_value, 0);
    
    // set left wheel 
    digitalWrite(pin_forward_left, LOW);
    digitalWrite(pin_backwards_left, HIGH);

    // set right wheel 
    digitalWrite(pin_forward_right, HIGH);
    digitalWrite(pin_backwards_right, LOW);

    Serial.println(" Loc left ");
    pow_left_wheel = interp_x;
    pow_right_wheel = interp_x;
  }
  else if(obj_x > max_x)
  {
    // object is at right 
    //mapped_x = map(obj_x, max_x , max_x_working_spectrum, 0, speed_value);    
      
    // set left wheel 
    digitalWrite(pin_forward_left, HIGH);
    digitalWrite(pin_backwards_left, LOW);
    // set right wheel 
    digitalWrite(pin_forward_right, LOW);
    digitalWrite(pin_backwards_right, HIGH);
      
    pow_left_wheel = interp_x;
    pow_right_wheel = interp_x;

    Serial.println(" Loc right ");
  }
  else{
    // object is in front
    // do not move
    pow_left_wheel = 0;
    pow_right_wheel = 0;

    // set left wheel 
    digitalWrite(pin_forward_left, LOW);
    digitalWrite(pin_backwards_left, LOW);

    // set right wheel 
    digitalWrite(pin_forward_right, LOW);
    digitalWrite(pin_backwards_right, LOW);    
    Serial.println(" Loc front ");
  }
}
void process_data()
{
  obj_x = pixy.ccc.blocks[i].m_x;
  obj_width = pixy.ccc.blocks[i].m_width; 

  obj_width = filterExpMovingAverage(obj_width, 1);
  obj_x = filterExpMovingAverage(obj_x, 0);
  int pos_x = binarySearch(D_X, 0, 10, obj_x);
  int pos_w = binarySearch(D_W, 0, 10, obj_width);
  interp_x = R_X[pos_x - 1] + float((obj_x - D_X[pos_x - 1])) / float((D_X[pos_x] - D_X[pos_x - 1])) * (R_X[pos_x] - R_X[pos_x - 1]);
  interp_w = R_W[pos_w - 1] + (float((obj_width - D_W[pos_w - 1])) / float((D_W[pos_w] - D_W[pos_w - 1]))) * (R_W[pos_w] - R_W[pos_w - 1]);
  
  if (obj_width >= max_width)
  {
    // case when object is too close
    // move backwards

    // set left wheel 
    digitalWrite(pin_forward_left, LOW);
    digitalWrite(pin_backwards_left, HIGH);

    // set right wheel 
    digitalWrite(pin_forward_right, LOW);
    digitalWrite(pin_backwards_right, HIGH); 

    speed = interp_w;
    
    // set the direction for wheels acording to x position of object
    // if(obj_x <=  min_x)
    // {
    //   //mapped_x = map(obj_x, min_x_working_spectrum , min_x, speed_value, 0);
    //   pow_left_wheel = speed - interp_x;
    //   pow_right_wheel = speed;
    // }
    // else if (obj_x>min_x && obj_x <= max_x)
    // {
      pow_left_wheel = speed;
      pow_right_wheel = speed;
    // }
    // else if(obj_x>max_x)
    // {
    //   //mapped_x = map(obj_x, max_x , max_x_working_spectrum, 0, speed_value);
    //   pow_right_wheel = speed - interp_x;
    //   pow_left_wheel = speed;
    // }
    // end backwards
  }
  else if(obj_width <= min_width)
  {
    // case when object is too far
    // move forward

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
      //mapped_x = map(obj_x, min_x_working_spectrum , min_x, speed_value, 0); // R_X
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
      //mapped_x = map(obj_x, max_x, max_x_working_spectrum, 0, speed_value);
      pow_right_wheel = speed - interp_x;
      pow_right_wheel = constrain(pow_right_wheel, 50, 255);
      pow_left_wheel = speed;
    }
    //end forward
  }
  else {
    // case when object is at normal distance
    pow_left_wheel = 0;
    pow_right_wheel = 0;
    move_x();
  }

  limit();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (pixy.ccc.getBlocks() > 0)
  {
    process_data();  
    pow_left_wheel = constrain(pow_left_wheel, 0, 255);
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

    // Serial.print("Width:");
    // Serial.print(pixy.ccc.blocks[i].m_width);
    // Serial.print(",");
    // Serial.print("X:");
    // Serial.print(pixy.ccc.blocks[i].m_x);
    // Serial.println();  
    // Serial.print(",");  
    // Serial.print("Filtered_Width:");
    // Serial.print(filterExpMovingAverage(obj_width, 1));
    // Serial.print(",");
    // Serial.print("Filtered_X:");
    // Serial.println(filterExpMovingAverage(obj_x, 0));
    // Serial.print("X:");
    // Serial.print(obj_x);
    // Serial.print(",");
    // Serial.print("Distance:");
    // Serial.println(obj_width);
  }
  else
  {
    pow_left_wheel = 0;
    pow_right_wheel = 0;
   // Serial.print(pixy.ccc.getBlocks());
    Serial.println("No object found!");
  }

  
  
  // set speed for wheels
  analogWrite(pin_en_a, pow_right_wheel);
  analogWrite(pin_en_b, pow_left_wheel);
  // analogWrite(pin_en_a, 0);
  // analogWrite(pin_en_b, 0);
  
}

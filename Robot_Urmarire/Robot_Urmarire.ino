#include <Pixy2.h>
#define DELTA 25
#define MIN_SPEED_VAL 30

// NEW
// witdh 190
#define min_width 175
#define normal_width 200
#define max_width 225

#define min_width_working_spectrum 40
#define max_width_working_spectrum 316 // ?

#define min_x 140
#define normal_x 165
#define max_x 190

#define min_x_working_spectrum 20  // min left position of x
#define max_x_working_spectrum 316 // max left position of x

#define speed_value 150 

#define pin_en_a 9            // roata Dreapta
#define pin_en_b 10           // roata Stanga
#define pin_forward_right 6   // roata dreapta in fata(IN1)
#define pin_forward_left 4   // roata stanga in fata(IN3)
#define pin_backwards_left 5  // roata stanga in spate(IN4)
#define pin_backwards_right 7 // roata dreapta in spate(IN2)

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

int D_W[] = {0,   40,  60,  105, 145, 175, 225, 240, 255, 270, 316};
int D_X[] = {0,   15,  40,  65,  90,  140, 190, 220, 250, 275, 316};

int R_W[] = {220, 220, 150, 110,  60,  0,   0,  50,   60,  70,  80};
int R_X[] = {180, 180, 130, 75,   30,  0,   0,  30,   75, 130, 180};

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
    if(obj_x <=  min_x)
    {
      //mapped_x = map(obj_x, min_x_working_spectrum , min_x, speed_value, 0);
      pow_left_wheel = speed - interp_x;
      pow_right_wheel = speed;
    }
    else if (obj_x>min_x && obj_x <= max_x)
    {
      pow_left_wheel = speed;
      pow_right_wheel = speed;
    }
    else if(obj_x>max_x)
    {
      //mapped_x = map(obj_x, max_x , max_x_working_spectrum, 0, speed_value);
      pow_right_wheel = speed - interp_x;
      pow_left_wheel = speed;
    }
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
    constrain(pow_left_wheel, 0, 255);
    constrain(pow_right_wheel,0, 255);
    Serial.println();    
    Serial.print("Width: ");
    Serial.print(obj_width);
    Serial.print(" X: ");
    Serial.print(obj_x);
    
    Serial.print("Left_Speed: ");
    Serial.print(pow_left_wheel);
    Serial.print(" Right_Speed: ");
    Serial.print(pow_right_wheel);

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
    Serial.print(pixy.ccc.getBlocks());
    Serial.println("No object found!");
  }

  
  
  // set speed for wheels
  analogWrite(pin_en_a, pow_right_wheel);
  analogWrite(pin_en_b, pow_left_wheel);
  // analogWrite(pin_en_a, 0);
  // analogWrite(pin_en_b, 0);
  
}

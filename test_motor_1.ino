#include <Pixy2.h>
#define DELTA 25
#define MIN_SPEED_VAL 30
//int port = 13;
// 32 - 307
// center - 140 ( 120 - 160 px)
// left - 20 (20 - 120px)
// right - 307 ( 160 - 309px)
// 140 width = 2 cm 
// 50  width - normal
// 20'


// NEW
// witdh 190
int min_width = 175;
int normal_width = 200;
int max_width = 225;

int min_x = 140;
int normal_x = 165;
int max_x = 190;

int min_width_working_spectrum = 40;
int max_width_working_spectrum = 316; // ?


int pin_en_a = 5;           // roata Dreapta
int pin_en_b = 6;           // roata stanga
int pin_forward_right = 4;  // roata dreapta in fata
int pin_forward_left = 7;   // roata stanga in fata
int pin_backward_left = 2;  // roata stanga in spate
int pin_backward_right = 3; // roata dreapta in spate
int pow_right_wheel;
int pow_left_wheel;

int speed = 0;
int obj_x = 0;
int obj_width = 0;
int mapped_x;
int i = 0;
Pixy2 pixy;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pixy.init();
  pinMode(pin_forward_right, OUTPUT);
  pinMode(pin_forward_left, OUTPUT);
  pinMode(pin_backward_right, OUTPUT);
  pinMode(pin_backward_left, OUTPUT);
  pinMode(pin_en_a, OUTPUT);
  pinMode(pin_en_b, OUTPUT);
  // analogWrite(pin_en_a,255);
  // analogWrite(pin_en_b,255);
  digitalWrite(pin_forward_right, LOW);
  digitalWrite(pin_forward_left, LOW);
  digitalWrite(pin_backward_right, LOW);
  digitalWrite(pin_backward_left, LOW);
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
   if(obj_x <=  119)
    {
      mapped_x = map(obj_x, 20 , 119, 0, 255);
      
      // set left wheel 
      digitalWrite(pin_forward_left, LOW);
      digitalWrite(pin_backward_left, HIGH);
      // set right wheel 
      digitalWrite(pin_forward_right, HIGH);
      digitalWrite(pin_backward_right, LOW);
      Serial.print(" Stanga ");
      pow_left_wheel = mapped_x;
      pow_right_wheel = mapped_x;
    }
    else if(obj_x >= 161)
    {

      mapped_x = map(obj_x, 161 , 307, 0, 255);    
        
      // set left wheel 
      digitalWrite(pin_forward_left, HIGH);
      digitalWrite(pin_backward_left, LOW);
      // set right wheel 
      digitalWrite(pin_forward_right, LOW);
      digitalWrite(pin_backward_right, HIGH);
        
      pow_left_wheel = mapped_x;
      pow_right_wheel = mapped_x;
    }
    else{
      pow_left_wheel = 0;
      pow_right_wheel = 0;
      // set left wheel 
      digitalWrite(pin_forward_left, LOW);
      digitalWrite(pin_backward_left, LOW);

      // set right wheel 
      digitalWrite(pin_forward_right, LOW);
      digitalWrite(pin_backward_right, LOW);    
    }
}
void process_data()
{
  obj_x = pixy.ccc.blocks[i].m_x;
  obj_width = pixy.ccc.blocks[i].m_width; 
  if (obj_width > max_width)
  {
    // case when object is too close
    
   
  }
  else if(obj_width < min_width)
  {
    // case when object is too far

    // set left wheel 
      digitalWrite(pin_forward_left, HIGH);
      digitalWrite(pin_backward_left, LOW);

      // set right wheel 
      digitalWrite(pin_forward_right, HIGH);
      digitalWrite(pin_backward_right, LOW); 

    speed = map(obj_width, 40 , min_width, 255 , 0  );
    
    if(obj_x <=  119)
    {
      mapped_x = map(obj_x, 20 , 119, 255, 0);
      pow_left_wheel = speed - mapped_x;
      pow_right_wheel = speed;
    }
    else if (obj_x>=120 && obj_x <= 160)
    {
      pow_left_wheel = speed;
      pow_right_wheel = speed;
    }
    else 
    {
      mapped_x = map(obj_x, 161 , 307, 255, 0);
      pow_right_wheel = speed - mapped_x;
      pow_left_wheel = speed;
    }
    

  }
  else{
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
    Serial.println();    
    Serial.print("Width: ");
    Serial.print(obj_width);
    Serial.print(" X: ");
    Serial.print(obj_x);
    
    Serial.print("Left_Speed: ");
    Serial.print(pow_left_wheel);
    Serial.print(" Right_Speed: ");
    Serial.print(pow_right_wheel);
    if(pin_forward_left != LOW || pin_forward_right != LOW || pin_backward_left != LOW || pin_backward_right != LOW)
    {
      Serial.println();
      if(pin_forward_left == HIGH && pin_backward_right == HIGH)
      {
        Serial.print("Left Forward Right Backward");
      }
      
      if(pin_forward_right == HIGH && pin_backward_left == HIGH)
      {
        Serial.print("Right Forward Left Backward");
      }
      
      if(pin_forward_left == HIGH && pin_forward_right == HIGH)
      {
        Serial.print("Left Forward Right Forward");
      }
      
      if(pin_backward_left == HIGH && pin_backward_right == HIGH)
      {
        Serial.print("Left Backward Right Backward");
      }
      
      Serial.println();
    }
  }
  else
  {
    pow_left_wheel = 0;
    pow_right_wheel = 0;
    Serial.print(pixy.ccc.getBlocks());
    Serial.println("No object found!");
  }
  analogWrite(pin_en_a, pow_right_wheel);
  analogWrite(pin_en_b, pow_left_wheel);
  
}

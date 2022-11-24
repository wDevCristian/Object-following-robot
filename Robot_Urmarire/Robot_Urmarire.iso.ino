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

#define speed_value 1023 // value to move backwards 

#define pin_en_a 9            // roata Dreapta
#define pin_en_b 10           // roata Stanga
#define pin_forward_right 4   // roata dreapta in fata(IN1)
#define pin_forward_left 7    // roata stanga in fata(IN3)
#define pin_backwards_left 2  // roata stanga in spate(IN4)
#define pin_backwards_right 5 // roata dreapta in spate(IN2)

int pow_right_wheel;
int pow_left_wheel;

int speed = 0;
int obj_x = 0;
int obj_width = 0;
int mapped_x;
int i = 0;

Pixy2 pixy;

void setup() {

  // setare frecventa pin D3, D11
  // TCCR1B = 0b00000001;
  // TCCR1A = 0b00000011;


  Serial.begin(115200);
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

    if (pow_right_wheel > speed_value )
    {
      pow_right_wheel = speed_value;
    }

    if (pow_left_wheel < MIN_SPEED_VAL)
    {
      pow_left_wheel = 0;
    }

    if (pow_left_wheel > speed_value )
    {
      pow_left_wheel = speed_value;
    }
}
void  move_x()
{
   if(obj_x <=  min_x)
    {
      // object is at left 
      mapped_x = map(obj_x, min_x_working_spectrum , min_x, 0, speed_value);
      
      // set left wheel 
      digitalWrite(pin_forward_left, LOW);
      digitalWrite(pin_backwards_left, HIGH);

      // set right wheel 
      digitalWrite(pin_forward_right, HIGH);
      digitalWrite(pin_backwards_right, LOW);

      Serial.println(" Loc left ");
      pow_left_wheel = mapped_x;
      pow_right_wheel = mapped_x;
    }
    else if(obj_x > max_x)
    {
      // object is at right 
      mapped_x = map(obj_x, max_x , max_x_working_spectrum, 0, speed_value);    
        
      // set left wheel 
      digitalWrite(pin_forward_left, HIGH);
      digitalWrite(pin_backwards_left, LOW);
      // set right wheel 
      digitalWrite(pin_forward_right, LOW);
      digitalWrite(pin_backwards_right, HIGH);
        
      pow_left_wheel = mapped_x;
      pow_right_wheel = mapped_x;

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
  if (obj_width > max_width)
  {
    // case when object is too close
    // move backwards

    // set left wheel 
    digitalWrite(pin_forward_left, LOW);
    digitalWrite(pin_backwards_left, HIGH);

    // set right wheel 
    digitalWrite(pin_forward_right, LOW);
    digitalWrite(pin_backwards_right, HIGH); 

    speed = speed;
    
    // set the direction for wheels acording to x position of object
    if(obj_x <=  min_x)
    {
      mapped_x = map(obj_x, min_x_working_spectrum , min_x, speed_value, 0);
      pow_left_wheel = speed - mapped_x;
      pow_right_wheel = speed;
    }
    else if (obj_x>min_x && obj_x <= max_x)
    {
      pow_left_wheel = speed;
      pow_right_wheel = speed;
    }
    else 
    {
      mapped_x = map(obj_x, max_x , max_x_working_spectrum, speed_value, 0);
      pow_right_wheel = speed - mapped_x;
      pow_left_wheel = speed;
    }
    // end backwards
  }
  else if(obj_width < min_width)
  {
    // case when object is too far
    // move forward

    // set left wheel 
    digitalWrite(pin_forward_left, HIGH);
    digitalWrite(pin_backwards_left, LOW);

    // set right wheel 
    digitalWrite(pin_forward_right, HIGH);
    digitalWrite(pin_backwards_right, LOW); 

    speed = map(obj_width, min_width_working_spectrum , min_width, speed_value , 0  );
    
    // set the direction for wheels acording to x position of object
    if(obj_x <=  min_x)
    {
      mapped_x = map(obj_x, min_x_working_spectrum , min_x, speed_value, 0);
      pow_left_wheel = speed - mapped_x;
      pow_right_wheel = speed;
    }
    else if (obj_x>min_x && obj_x <= max_x)
    {
      pow_left_wheel = speed;
      pow_right_wheel = speed;
    }
    else 
    {
      mapped_x = map(obj_x, max_x , max_x_working_spectrum, speed_value, 0);
      pow_right_wheel = speed - mapped_x;
      pow_left_wheel = speed;
    }
    //end forward
  }
  else{
    // case when object is at normal distance
    pow_left_wheel = 0;
    pow_right_wheel = 0;
    move_x();
  }

  //limit();
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
  
}

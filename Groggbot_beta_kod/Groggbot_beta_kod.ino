//groggbot till SMART22 LuND 22

#include <LiquidCrystal.h>


// select the pins used on the LCD panel

int rs=12, en=11, d4=5, d5=4, d6=3, d7=2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//defines
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//global constants
int lcd_key     = 0;
int adc_key_in  = 0;
float logic_Hz = 1; //number of time each second that the logic of the program will execute
float render_Hz = 0.5; //renders the display at this Hz
float input_Hz = 10; //check input at this frequency

//global variables
long time_counter = 0;
long logic_timer_counter = 0;
long render_timer_counter = 0;
long input_timer_counter = 0;
int selected_button = btnNONE;



//global function
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 //Serial.println(adc_key_in);
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 150)   return btnLEFT;  
 if (adc_key_in < 225)  return btnDOWN; //195 
 if (adc_key_in < 380)  return btnSELECT; 
 if (adc_key_in < 555)  return btnRIGHT;
 if (adc_key_in < 790)  return btnUP;
  
 return btnNONE;  // when all others fail, return this...
}

//returns the current value of the loadcell and returns it
int read_loadcell(){

}

void PrintStartupScreen(){
   lcd.setCursor(0,1);
  lcd.print("Groggbot starting up"); // print a simple message
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Groggbot");
  lcd.setCursor(0,1);
  lcd.print("Fran Ztyret Chalmers");
  lcd.setCursor(0,2);
  lcd.print("Till Maskinstyret");
  lcd.setCursor(0,3);
  lcd.print("SMART 22VT LUND");
  delay(1000);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  

  lcd.begin(20,4);              // start the library
  lcd.clear();
  PrintStartupScreen();
  
  randomSeed(analogRead(2)); //initates random seed for random nrs

  Serial.begin(115200);
  Serial.println("test123");
  
}

//waits in a loop until no key is pressed, avoids skipping menus
void WaitForNoKeypress(){
  while(read_LCD_buttons() != btnNONE){
    delay(1);
  }
  return;
}

//waits until user has given an input, then returns the input
int WaitForUserInput(){
  bool done = false;
  int returnKey = btnNONE;
  while(!done){
    returnKey = read_LCD_buttons();
    
    if(returnKey != btnNONE){
      done = true;
    }
  }

  return returnKey;
}

//int b[ 2 ][ 2 ] = { { 1 }, { 3, 4 } };
int pump_amounts[4][2]={{0, 0}, {0, 0}, {0, 0}, {0, 0}}; //each pump has three values, aka you can create three types of drinks, default value zero
int btn_right = 0;
int btn_left = 0;
int btn_up = 0;
int btn_down = 0;
int btn_select = 0;
const int numMenus = 5; 
int menuPos = 0; //what menu is currently selected, controlled with right/left buttons
int verticalMenuPos = 0; //can take value of 0-3, controlled with up/down buttons

bool selectingMenu = true;
bool incrementingValue = false; //true if user is currently incrementing a value


bool NewMenu_logic(int inputButton){

  //inputbutton -> the latest button that has been pressed
  bool redraw_next_cycle = false;
  if( inputButton != btnNONE) {
    redraw_next_cycle = true; //an input has been registred, should redraw the menu next cycle

    //handle the buton logic
    switch(inputButton){
    case btnRIGHT:
    //if one is currently selecting menu (and not incrementing a value for example)
    if(verticalMenuPos == 0){
      menuPos++;
      if(menuPos > numMenus){
        menuPos = numMenus;
      }
    }else if(menuPos >= 1 && menuPos <= 4 && verticalMenuPos >= 1 && verticalMenuPos <= 2){
      //adjust drink settings at hardcoded menu places
      //add amount to this pump

      //verticalmenupos is either 1 or 2
      pump_amounts[menuPos-1][verticalMenuPos-1] += 1;
      
    }
    
    break;

    case btnUP:
    verticalMenuPos--;
    if(verticalMenuPos < 0){
      verticalMenuPos = 3;
    }

    break;

    case btnDOWN:
    verticalMenuPos++;
    if(verticalMenuPos > 3){
      verticalMenuPos = 0;
    }
    break;

    case btnLEFT:
    //if one is currently selecting menu (and not incrementing a value for example)
    if(verticalMenuPos == 0){
      menuPos--;
      if(menuPos < 0){
        menuPos = 0;
      }
    }else if(menuPos >= 1 && menuPos <= 4 && verticalMenuPos >= 1 && verticalMenuPos <= 2){
      //adjust drink settings at hardcoded menu places
      //remove amount from this place

      //verticalmenupos is either 1 or 2
      pump_amounts[menuPos-1][verticalMenuPos-1] -= 1;

      //constrain to positive amounts
      if(pump_amounts[menuPos-1][verticalMenuPos-1] < 0){
        pump_amounts[menuPos-1][verticalMenuPos-1] = 0;
      }
      
    }
      
    break;

    case btnSELECT:
    
    if(menuPos == 0 && verticalMenuPos == 1){
      //pour drink 1
      PourDrink(pump_amounts[0][0],pump_amounts[1][0],pump_amounts[2][0],pump_amounts[3][0]);

    } else  if(menuPos == 0 && verticalMenuPos == 2){
      //pour drink 2
      PourDrink(pump_amounts[0][1],pump_amounts[1][1],pump_amounts[2][1],pump_amounts[3][1]);

    }else if(menuPos == 0 && verticalMenuPos == 3){
      RandomGrogg();
      delay(250);
    }
    break;

  }

    //update the strings which should be drawn on screen, i.e adjust what value is shown to be dispensed

  }


  //after doing the menu logic, return if menu has to be redrawn or not
  return redraw_next_cycle;
}

String menuGUI[][4] = {{"MENU      Settings>", "POUR DRINK 1", "POUR DRINK 2", "Random groggg 30cl"}, 
                              {"<POUR  PUMP 1   P2>", "Drink 1", "Drink 2", ""}, 
                              {"<P1   PUMP 2    P3>","Drink 1", "Drink 2", ""}, 
                              {"<P2   PUMP 3    P4>","Drink 1", "Drink 2", ""},
                              {"<P3   PUMP 4      >","Drink 1", "Drink 2", ""}, 
                              {"<P4 Purge system", "Backa pumpar", "temp2", "temp3"}};

void NewMenu_draw(bool redraw){
  //only redraw
  if(redraw){
    lcd.clear();

    //draw 4 rows of characters
    for( int i = 0; i < 4; i++){
      lcd.setCursor(1,i);
      lcd.print(menuGUI[menuPos][i]);

     
      //Draw pump amount on screen if the right screens are displayed

      if(menuPos >= 1 && menuPos <= 4 && i >= 1 && i <= 2){
        lcd.setCursor(10,i);
        lcd.print("-[" + String(pump_amounts[menuPos-1][i-1]) + " cl]+");
      }
    }
    //draw arrow to show which submenu is selected
    lcd.setCursor(0,verticalMenuPos);
    lcd.print("#");
  }
}

//function to pour a drink, takes 4 measurements as input, in cl.
//takes over display
void PourDrink(int amount_p1, int amount_p2, int amount_p3, int amount_p4){
      //1. Draw new menu on display, user selects if to continue or not

      int cursor_position = 0;      
      bool done = false;
      while(!done){
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Ready to pour drink!");
        lcd.setCursor(0,1);
        lcd.print("Place empty glass");
        lcd.setCursor(0,2);
        lcd.print("and press confirm");
        lcd.setCursor(1,3);
        lcd.print("[EXIT]   [CONFIRM]");

        //draw "cursor"

        if(cursor_position == 0){
          lcd.setCursor(0,3);
          lcd.print(">");
          lcd.setCursor(7,3);
          lcd.print("<");

        }else if(cursor_position == 1){
          lcd.setCursor(9,3);
          lcd.print(">");
          lcd.setCursor(19,3);
          lcd.print("<");
        }

        int button_input = WaitForUserInput();

        switch(button_input){
          case btnRIGHT:
          cursor_position++;
          if(cursor_position > 1){
            cursor_position = 1;
          }
          break;

          case btnLEFT:
          cursor_position--;
          if(cursor_position < 0){
            cursor_position = 0;
          }
          break;

          case btnSELECT:
          if(cursor_position == 0){
            //pressed exit
            delay(50);            
            return;
          }else if(cursor_position == 1){
            //pressed confirm, continue with pouring drink
            done = true;
            delay(50);
          }
          break;
        }

        
      }
      
      //2. Pour the drink, write static text to screen
      
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Pouring drink...");
      lcd.setCursor(0,1);
      lcd.print("ABORT??? >[OK]<");
      lcd.setCursor(0,2);
      lcd.print("P1:"+ String(amount_p1) + "P2:" + String(amount_p2) + "P3:" + String(amount_p3) + "P4:" + String(amount_p4));
      
      WaitForNoKeypress();
      

      int amounts[] = {amount_p2, amount_p2, amount_p3, amount_p4};

      for(int i = 0; i < 4; i++){

        lcd.setCursor(0,3);
        lcd.print("Pump " + String(i+1) + " active!");

        bool pouring_drink = true;

        //zero the loadcell value before each run
        int current_weight = 0; //TODO READ SENSOR VALUE

        //TODO TURN PUMP[i] on

        while(pouring_drink){
        //read the loadcell data
        //TODO current_weight = 
        
        if(read_LCD_buttons() != btnNONE){
          delay(150);
          //TODO turn pump off
          return;
        }

        //TODO PROPER WEIGHT SCALING
        /*if(current_weight > amounts[i]*999){
          //TODO turn pump off
          pouring_drink = false; 
        }     */   

        delay(50); //add some delay, let pump work for 50ms
        //

        delay(1500);
        pouring_drink = false;
      }
      
      //TURN PUMP[i] off
      }
      
      

    //3. let user confirm that he or she is finnished
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Drink ready");
    lcd.setCursor(0,1);

    int totalNumDrinks = 0; //TODO READ WRITE TO EEPROM to save total nr of drinks made by groggbot

    lcd.print("Made " + String(totalNumDrinks) + " in total!");
    lcd.setCursor(0,2);
    lcd.print("Please remove glass");
    lcd.setCursor(0,3);
    lcd.print("and press any key!");


    while(read_LCD_buttons() == btnNONE){
      delay(50);
      //"pauses" screen while waiting for user input
    }
}

//randomizes a 
void RandomGrogg(){
      int cursor_position = 0;   
      
      bool done = false;

      int amount1 = 0;
      int amount2 = 0;
      int amount3 = 0;
      int amount4 = 0;

      while(!done){
        
        WaitForNoKeypress();
        


        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Random grogg 30cl");
        lcd.setCursor(0,1);
        lcd.print("Press [R] to shuffle ");
        lcd.setCursor(0,2);
        lcd.print("["+ String(amount1) + "cl-" + String(amount2) + "cl-" + String(amount3) + "cl-" + String(amount4)+"cl]");
        lcd.setCursor(1,3);
        lcd.print("[EXIT]  [R]  [OK]");

        //draw "cursor"

        if(cursor_position == 0){
          lcd.setCursor(0,3);
          lcd.print(">");
          lcd.setCursor(7,3);
          lcd.print("<");

        }else if(cursor_position == 1){
          lcd.setCursor(8,3);
          lcd.print(">");
          lcd.setCursor(12,3);
          lcd.print("<");
        }
        else if (cursor_position == 2){
           lcd.setCursor(13,3);
          lcd.print(">");
          lcd.setCursor(18,3);
          lcd.print("<");
        }

        int button_input = WaitForUserInput();

        switch(button_input){
          case btnRIGHT:
          cursor_position++;
          if(cursor_position > 2){
            cursor_position = 2;
          }
          break;

          case btnLEFT:
          cursor_position--;
          if(cursor_position < 0){
            cursor_position = 0;
          }
          break;

          case btnSELECT:
          if(cursor_position == 0){
            //pressed exit
            delay(100);            
            return;
          }else if(cursor_position == 1){
            //randomize drink
            int max= 30; //cl
            amount1 = random(0, max);
            amount2 = random(0, max - amount1);
            amount3 = random(0, max - amount1 - amount2);
            amount4 = max - amount1 - amount2 - amount3;

          }else if(cursor_position == 2){
            //pour drink
            delay(250);
            PourDrink(amount1, amount2, amount3, amount4);

            done = true;
            delay(100);
          }
          break;
        }

        
      }
}



void HandleMenu(int inputButton){
  //check what button was pressed
  switch(inputButton){
    case btnRIGHT:
    btn_right++;
    break;

    case btnUP:
    btn_up++;
    break;

    case btnDOWN:
    btn_down++;
    break;

    case btnLEFT:
    btn_left++;
    break;

    case btnSELECT:
    btn_select++;
    break;

  }

  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Right: " + String(btn_right));
  lcd.setCursor(0,1);
  lcd.print("Left: " + String(btn_left));
  lcd.setCursor(0,2);
  lcd.print("Up: " + String(btn_up));
  lcd.setCursor(0,3);
  lcd.print("Down: " + String(btn_down));

  lcd.setCursor(14,2);
  lcd.print(String(btn_select));

  
}




void loop() {

  selected_button = btnNONE; //resets selected button
   
  long temp_time = time_counter;
  long current_time = millis(); //get current microcontroller time
  time_counter = current_time;
  long delta_time  = ( current_time - temp_time ); //calculate delta time

  //update timer values
  logic_timer_counter += delta_time;
  render_timer_counter += delta_time;
  input_timer_counter += delta_time;


  //if enough time has elapsed, check for new input (multiply with 1000 since inputer_timer_coutner is in ms)
  if( input_timer_counter >= float(1/input_Hz)*1000){
    input_timer_counter = 0; //reset
    
     
     selected_button = read_LCD_buttons();
    //Serial.println("Checking input... Selected button is: " + String(selected_button));

    //HandleMenu(selected_button);

    bool temp = NewMenu_logic(selected_button);
    NewMenu_draw(temp);
  }

  


}



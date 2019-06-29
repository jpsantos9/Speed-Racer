// include the library code
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define  STATE_GAME       1
#define  STATE_GAME_LOST  2
#define  STATE_GAME_WON   3
#define  STATE_MENU       0
/**********************************************************/
int state = STATE_MENU;
int lcd_width = 16;
int tim = 300;

const int xPin = A1;     //X attach to A0
const int yPin = A2;     //Y attach to A1
const int btPin = 7;     //Bt attach to digital 7

// define the player and starting position
String player_ship =    "`O=o";
int player_position_y = 1;
int player_position_x = 0;
int score = 0;

int number_of_enemies = 20;
// This defines the enemies if you want to build your level by hand.
// Below, these values will be overwritten by random values. But I 
// wanted to leave this in so you can see how to define a level. 
// Simply comment out the code below that fills in random the values.
int enemies_x[20];
int enemies_y[20];
int enemies_x2[20];
int enemies_y2[20];

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd2(0x23,16,2);
/*********************************************************/
void setup()
{
  //setup lcd
  lcd.begin();  //initialize the lcd
  lcd.backlight();  //open the backlight
  lcd2.begin();
  lcd2.backlight();
  //setup controller
  pinMode(btPin,INPUT); //set btpin as INPUT
  digitalWrite(btPin, HIGH); //and HIGH
  //setup serial monitor
  Serial.begin(9600); //initialize serial monitor
  
  // Startup Screen
  startLoading();
  
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0)); 
  initialize();
}
/*********************************************************/
void loop() 
{
  delay(tim);
  if(state == STATE_MENU) {
    menuDisplay();  //press start display
    //check if button pressed
    if (digitalRead(btPin)==0) {
      state=STATE_GAME;
    }
  }
  else if(state == STATE_GAME) {  
      // clear the screen and draw the player
      lcd.clear();
      lcd2.clear();
      if (player_position_y<2){
        lcd.setCursor(player_position_x, player_position_y);
        lcd.print(player_ship); 
      }else {
        lcd2.setCursor(player_position_x, player_position_y-2);
        lcd2.print(player_ship);
      }
      
      // for each enemy
      for(int i=0; i<number_of_enemies; i++) {
        // move them to the left 1 space
        enemies_x[i] = enemies_x[i] - 1;
        enemies_x2[i] = enemies_x2[i] - 1;

        // and draw them where they should appear
        if(enemies_x[i] >= 0 && enemies_x[i] <= lcd_width) { 
          lcd.setCursor(enemies_x[i], enemies_y[i]);
          lcd.print("|");
          lcd2.setCursor(enemies_x2[i], enemies_y2[i]);
          lcd2.print("|");
          score++;
        }

        // check to see if any enemy touches the player, if so, GAME OVER!!!
        if (player_position_y<2){
          if((enemies_x[i] <= player_position_x+player_ship.length()) 
          && (enemies_x[i] >= player_position_x)
          && (enemies_y[i] == player_position_y))
          state = STATE_GAME_LOST;
        } else {
          if((enemies_x2[i] <= player_position_x+player_ship.length()) 
          && (enemies_x2[i] >= player_position_x)
          && (enemies_y2[i] == player_position_y-2))
          state = STATE_GAME_LOST;
        }
      }

      // check to see if all enemies have been passed, if so, GAME WON!!!
      if(enemies_x[(number_of_enemies-1)] < 0)
        state = STATE_GAME_WON;

      // check for controls
      controller();
  } 
  else if(state == STATE_GAME_LOST) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("GAME OVER!!!");
      lcd.setCursor(3, 1);
      lcd.print("SCORE: ");
      lcd.setCursor(10, 1);
      lcd.print(score);
      lcd2.clear();
      lcd2.setCursor(2, 0);
      lcd2.print("Press Button");
      lcd2.setCursor(3, 1);
      lcd2.print("To Restart");
      
      //check if button pressed
      if (digitalRead(btPin)==0) {
        initialize();
        state=STATE_GAME;
      }
      delay(2000);
  } 
  else if(state == STATE_GAME_WON) {
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("You WIN!!!");
      lcd2.clear();
      lcd2.setCursor(2, 0);
      lcd2.print("Press Button");
      lcd2.setCursor(3, 1);
      lcd2.print("To Restart");
      
      //check if button pressed
      if (digitalRead(btPin)==0) {
        initialize();
        state=STATE_GAME;
      }
  }
}
/************************************************************/
void startLoading () {
  for (int i=0; i<lcd_width; i++){
    lcd.setCursor(2,0);
    lcd.print("SPEED RACER!");
    lcd.setCursor(i,1);
    lcd.print(player_ship);
    lcd2.setCursor(i*2,0);
    lcd2.print(player_ship);
    lcd2.setCursor(i*3,1);
    lcd2.print(player_ship);
    delay(tim);
    lcd2.clear();
    lcd.clear();
  }
}

void menuDisplay (){
  lcd.clear();
  lcd2.clear();
  delay(tim);
  lcd.setCursor(0,0);
  lcd.print("Press Button"); 
  lcd.setCursor(0,1);
  lcd.print("to Start");
  lcd2.setCursor(0,0);
  lcd2.print("by: Paolo Santos");
  lcd2.setCursor(4,1);
  lcd2.print("CMSC 135");
}

void initialize () {
  int accumulation = lcd_width;
    for(int i=0; i<number_of_enemies; i++) {
      // randomly pick a number between the ship's length and 8
      enemies_x2[i] = accumulation+random((player_ship.length()+1), 8);
      enemies_y2[i] = enemies_y[i]; 
      //for the bottom screen
      accumulation += random((player_ship.length()+1), 8);
      enemies_x[i] = accumulation;
      enemies_y[i] = random(0,2);
    }
    player_position_y = 1;
    player_position_x = 0;
    score=0;
}

void controller () {
  if (analogRead(xPin)>700 && player_position_y<3) { //down
    player_position_y++;
  } else if (analogRead(xPin)<300 && player_position_y>0) {  //up
    player_position_y--;
  }

  if (analogRead(yPin)>700 and player_position_x<lcd_width-player_ship.length()) {
    player_position_x++;
  } else if (analogRead(yPin)<300 and player_position_x>0) {
    player_position_x--;
  } 
  Serial.println(analogRead(yPin),DEC); //read the value of A1 and print it in decimal
}

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <TM1637Display.h>

// I2C Addresses
#define PCF8574_ADDRESS_4 0x20
#define PCF8574_ADDRESS_5 0x21
#define PCF8574_ADDRESS_6 0x22
#define PCF8574_ADDRESS_7 0x23
#define PCF8574_ADDRESS_8 0x24
#define PCF8574_ADDRESS_9 0x25
//Timer Constants
TM1637Display display(A1,A2);
int timerdurationW =1;
int timerdurationB =1;
int timeron=1;
int SecondsB=1;
int SecondsW=1;
int RookCastleW[2]={1,1};
int KingCastleW=1;
int RookCastleB[2]={1,1};
int KingCastleB=1;
// SPI Constants
const int CS_10 = 10;
const int CS_9 = 9;
const byte PORT_A = 0x13;
const byte PORT_B = 0x12;
// NeoPixel Constants
const int TOTAL_LEDS = 64;
byte myArray[64];
//Array to determine positions for the rows and columns calculated for Pawn movement
int Rows[8][8] = {
  {0, 1, 2, 3, 4, 5, 6, 7},
  {8, 9, 10, 11, 12, 13, 14, 15},
  {16, 17, 18, 19, 20, 21, 22, 23},
  {24, 25, 26, 27, 28, 29, 30, 31},
  {32, 33, 34, 35, 36, 37, 38, 39},
  {40, 41, 42, 43, 44, 45, 46, 47},
  {48, 49, 50, 51, 52, 53, 54, 55},
  {56, 57, 58, 59, 60, 61, 62, 63}
};
int checkW=0;
int checkB=0;
int promW[8]={0,0,0,0,0,0,0,0};
int promB[8]={0,0,0,0,0,0,0,0};
int onetime=1;
int Tracker[32] = {
  0, 1, 2, 3, 4, 5, 6, 7, 
  8, 9, 10, 11, 12, 13, 14, 15, 
  63, 62, 61, 60, 59, 58, 57, 56, 
  55, 54, 53, 52, 51, 50, 49, 48
};
int victoryW=0;
int victoryB=0;
int GlobalKingW=4;
int GlobalKingB=59;
int turn=0;
int CaptureStandby=0;
bool turnedYellow[64] = {false}; // Array to keep track of LEDs that have been turned yellow.
byte initialState[64] = {0}; 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, 2, NEO_GRB + NEO_KHZ800);

void setup() {
  // Initialize the I2C and SPI interfaces
  Wire.begin();
  SPI.begin();
  pinMode(CS_10, OUTPUT);
  pinMode(CS_9, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  display.setBrightness(0x0f);
  noInterrupts(); 
  TCCR1A = 0; 
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  OCR1A = 15625;
  TIMSK1 |= (1 << OCIE1A);
  interrupts();  
  // Initialize the NeoPixel strip
  strip.begin();
  strip.setBrightness(65);
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);  // in
}

void dataCollection() {
  byte data1, data2, data3, data4, data5, data6, data7, data8;

  //Data Collection
  data1 = readSPIRegister(CS_9, PORT_B); //ROW 1
  data2 = readSPIRegister(CS_9, PORT_A); //ROW2
  data3 = readSPIRegister(CS_10, PORT_B); //ROW 1
  data4 = readSPIRegister(CS_10, PORT_A); //ROW2
  Wire.requestFrom(PCF8574_ADDRESS_4, 1);//ROW5
  data5 = Wire.read();
  Wire.requestFrom(PCF8574_ADDRESS_5, 1);//ROW6
  data6 = Wire.read();
  Wire.requestFrom(PCF8574_ADDRESS_6, 1);//ROW7
  data7 = Wire.read();
  Wire.requestFrom(PCF8574_ADDRESS_7, 1);//ROW8
  data8 = Wire.read();
  //Data Collection

  //Data Assignment
  myArray[0] = (data1) & 1;
  myArray[1] = (data1 >> 1) & 1;
  myArray[2] = (data1 >> 2) & 1;
  myArray[3] = (data1 >> 3) & 1;
  myArray[4] = (data1 >> 4) & 1;
  myArray[5] = (data1 >> 5) & 1;
  myArray[6] = (data1 >> 6) & 1;
  myArray[7] = (data1 >> 7) & 1;

  myArray[8] = (data2) & 1;
  myArray[9] = (data2 >> 1) & 1;
  myArray[10] = (data2 >> 2) & 1;
  myArray[11] = (data2 >> 3) & 1;
  myArray[12] = (data2 >> 4) & 1;
  myArray[13] = (data2 >> 5) & 1;
  myArray[14] = (data2 >> 6) & 1;
  myArray[15] = (data2 >> 7) & 1;

  myArray[16] = (data3) & 1;
  myArray[17] = (data3 >> 1) & 1;
  myArray[18] = (data3 >> 2) & 1;
  myArray[19] = (data3 >> 3) & 1;
  myArray[20] = (data3 >> 4) & 1;
  myArray[21] = (data3 >> 5) & 1;
  myArray[22] = (data3 >> 6) & 1;
  myArray[23] = (data3 >> 7) & 1;

  myArray[24] = (data4) & 1;
  myArray[25] = (data4 >> 1) & 1;
  myArray[26] = (data4 >> 2) & 1;
  myArray[27] = (data4 >> 3) & 1;
  myArray[28] = (data4 >> 4) & 1;
  myArray[29] = (data4 >> 5) & 1;
  myArray[30] = (data4 >> 6) & 1;
  myArray[31] = (data4 >> 7) & 1;

  myArray[32] = (data5) & 1;
  myArray[33] = (data5 >> 1) & 1;
  myArray[34] = (data5 >> 2) & 1;
  myArray[35] = (data5 >> 3) & 1;
  myArray[36] = (data5 >> 4) & 1;
  myArray[37] = (data5 >> 5) & 1;
  myArray[38] = (data5 >> 6) & 1;
  myArray[39] = (data5 >> 7) & 1;

  myArray[40] = (data6) & 1;
  myArray[41] = (data6 >> 1) & 1;
  myArray[42] = (data6 >> 2) & 1;
  myArray[43] = (data6 >> 3) & 1;
  myArray[44] = (data6 >> 4) & 1;
  myArray[45] = (data6 >> 5) & 1;
  myArray[46] = (data6 >> 6) & 1;
  myArray[47] = (data6 >> 7) & 1;

  myArray[48] = (data7) & 1;
  myArray[49] = (data7 >> 1) & 1;
  myArray[50] = (data7 >> 2) & 1;
  myArray[51] = (data7 >> 3) & 1;
  myArray[52] = (data7 >> 4) & 1;
  myArray[53] = (data7 >> 5) & 1;
  myArray[54] = (data7 >> 6) & 1;
  myArray[55] = (data7 >> 7) & 1;

  myArray[56] = (data8) & 1;
  myArray[57] = (data8 >> 1) & 1;
  myArray[58] = (data8 >> 2) & 1;
  myArray[59] = (data8 >> 3) & 1;
  myArray[60] = (data8 >> 4) & 1;
  myArray[61] = (data8 >> 5) & 1;
  myArray[62] = (data8 >> 6) & 1;
  myArray[63] = (data8 >> 7) & 1;
  /*
  Serial.print("\nbegin\n");
  for(int i=0;i<=7;i++){
    Serial.print("\n");
    Serial.print(myArray[7+(8*i)]);
    Serial.print(myArray[6+(8*i)]);
    Serial.print(myArray[5+(8*i)]);
    Serial.print(myArray[4+(8*i)]);
    Serial.print(myArray[3+(8*i)]);
    Serial.print(myArray[2+(8*i)]);
    Serial.print(myArray[1+(8*i)]);
    Serial.print(myArray[0+(8*i)]);
    Serial.print("\n");
    Serial.print(i);
    Serial.print("\n");
  }
  Serial.print("\nend\n");
  */
}

int KingFunct(int KG) {
  if (KG  == 65){
    return KG;
  }
  dataCollection(); // Call a function to collect data needed for chess board state (not defined in this snippet).
  int KingTrackerID;
  int King = myArray[KG]; // Get the current state of the king from the 'myArray'. 0 means picked up, 1 means placed on board.
  int king_row = KG / 8; // Determine the row in which the king is placed.
  int king_col = KG % 8; // Determine the column in which the king is placed.
  int king_mov[8]; 
  int row_moves[] = {1, 1, 1, 0, 0, -1, -1, -1};
  int col_moves[] = {-1, 0, 1, -1, 1, -1, 0, 1};
  int KingLight[8] = {127, 127, 127, 127, 127, 127, 127, 127}; // 127 indicates maximum brightness.
  int castle1W=0;
  int castle2W=0;
  int castle1B=0;
  int castle2B=0;
  int castling=0;
  for (int i=0;i<32;i++) {
    if (Tracker[i] == KG) {
      KingTrackerID = i;  // Store the index where the value is found in z
    }
  }
  if(KingTrackerID<16){
    GlobalKingW=KG;
  }
  if(KingTrackerID>15){
    GlobalKingB=KG;
  }
  for (int i = 0; i < 8; ++i) {
    int new_row = king_row + row_moves[i];
    int new_col = king_col + col_moves[i];
    if (new_row >= 0 && new_row <= 7 && new_col >= 0 && new_col <= 7) {
      king_mov[i] = Rows[new_row][new_col];
    } 
    else {
      king_mov[i] = 65;  // Use 65 to denote an invalid or out-of-bound move.
    }
  }
  int king_cap[8] = {
    65, 65, 65, 65, 65, 65, 65, 65
  };
  if (KingTrackerID < 16) {
    for (int i = 0; i < 8; i++) {
      for (int j = 16; j < 32; j++) {
        if (Tracker[j] != KG) {
          if (Tracker[j] == king_mov[i]) {
          king_cap[i] = king_mov[i];
          if(GlobalKingB==king_cap[i]){
            checkB=1;
          }
          }
        }
      }
    }
  }

  if (KingTrackerID > 16) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 16; j++) {
        if (Tracker[j] != KG) {
          if (Tracker[j] == king_mov[i]) {
            king_cap[i] = king_mov[i];
            if(GlobalKingW==king_cap[i]){
              checkW=1;
            }
          }
        }
      }
    }
  }

  invalidatePositions(king_mov, 32, Tracker, strip, turnedYellow, (byte*)myArray, initialState, KG);//Invalid Code
  
  for (int i=0;i<=7;i++){
    for (int j=0;j<32;j++){
      if(Tracker[j]!=KG){
        if ((Tracker[j]==(king_mov[i])) ){
         king_mov[i]=65;
        }
      }
    }
  }
  if((RookCastleW[0]==1)&&(KingCastleW==1)&&(KingTrackerID<16)&&(myArray[1]==0)&&(myArray[2]==0)&&(myArray[0]==1)){
    castle1W=1;
  }
  if((RookCastleW[1]==1)&&(KingCastleW==1)&&(KingTrackerID<16)&&(myArray[4]==0)&&(myArray[5]==0)&&(myArray[6]==0)){
    castle2W=1;
  }
  if((RookCastleB[0]==1)&&(KingCastleB==1)&&(KingTrackerID>15)&&(myArray[57]==0)&&(myArray[58]==0)){
    castle1B=1;
  }
  if((RookCastleB[1]==1)&&(KingCastleB==1)&&(KingTrackerID>15)&&(myArray[62]==0)&&(myArray[61]==0)&&(myArray[60]==0)){
    castle2B=1;
  }
  if(onetime==0){
  while (King == 0) { // Execute the loop if the king is picked up (i.e., King == 0).
    dataCollection(); // Collect data again to update the board state.
        //piece in the tracker, made exception to self due to interference later in the code
      strip.setPixelColor(KG, strip.Color(0, 127, 127));
      strip.show();
    if(castle1W==1){
      strip.setPixelColor(1, strip.Color(0, 0, 127));
      strip.show(); 
    }
    if(castle2W==1){
      strip.setPixelColor(5, strip.Color(0, 0, 127));
      strip.show(); 
    }
    if(castle2B==1){
      strip.setPixelColor(61, strip.Color(0, 0, 127));
      strip.show(); 
    }
    if(castle1B==1){
      strip.setPixelColor(57, strip.Color(0, 0, 127));
      strip.show(); 
    }
    for (int i = 0; i < 8; i++) {
      strip.setPixelColor(king_mov[i], strip.Color(0, KingLight[i], 0));
      strip.show();
    }

    for (int i = 0; i < 8; i++) {
      strip.setPixelColor(king_cap[i], strip.Color(127, 0, 0));
      strip.show();
    }

    for (int i=0; i < 8; i++) { // Iterate from 0 to 8 for king's capturing positions
      if (myArray[king_cap[i]] == 0 && king_cap[i] != 65) { // Check if the king can capture the piece at this position
        CaptureStandby = 1;
        while (CaptureStandby == 1) {
          for (int j=0; j < 32; j++) {
            if (Tracker[j] == king_cap[i]){ // If a piece that can be captured is found
              Tracker[j] = 65; // Remove that piece (setting to 65 to indicate it is no longer on the board)
            }
          }
          CaptureStandby = 2; // Change CaptureStandby state
          lightsoff(); // Turn off LEDs, function assumed to be defined elsewhere
          strip.setPixelColor(king_cap[i], strip.Color(0, 0, 127)); // Set the LED color for the king's new position
          strip.show(); // Update the LED display
          while (CaptureStandby == 2) {
            dataCollection(); // Collect data, function assumed to be defined elsewhere
            if (myArray[king_cap[i]] == 1) { // If the king is moved to the new position
              if(KingTrackerID<16){
                KingCastleW=0;
              }
              if(KingTrackerID>15){
                KingCastleB=0;
              }
            lightsoff();  // Turn off LEDs
            KG = king_cap[i];  // Update king's position
            Tracker[KingTrackerID] = KG; // Update Tracker array
            turn = turn + 1;  // Increment the turn counter
            onetime=1;
        if(KingTrackerID<16){
          KingCastleW=0;
        }
        if(KingTrackerID>15){
          KingCastleB=0;
        }            CaptureStandby = 0; // Reset CaptureStandby to exit the while loops
            return KG; // Return the updated king's position
            }
          }
        }
      } 
    }
    for (int i = 0; i < 8; i++) {
      if (myArray[king_mov[i]] == 1 && king_mov[i] != KG) { // Check if a piece is placed in one of the possible moves.
        lightsoff(); // Turn off the LEDs.
        KG = king_mov[i]; // Update the king's position.
        Tracker[KingTrackerID] = KG; //The Rook Tracker index is updated with a new position at the pawns corresponding ID.
        if(KingTrackerID<16){
          KingCastleW=0;
        }
        if(KingTrackerID>15){
          KingCastleB=0;
        }
        onetime=1;
        turn = turn + 1; // Increment the turn counter.
        break;
      }
    }
    
      if ((myArray[1] == 1)&&(castle1W==1)) { // Castling White Right
        lightsoff(); // Turn off the LEDs.
        castling=1;
        while(castling==1){
          strip.setPixelColor(0, strip.Color(0, 127, 0));
            strip.setPixelColor(2, strip.Color(0, 0, 127));
          strip.show();
          dataCollection();
          if((myArray[2]==1) && (myArray[1]==1)&&(myArray[0]==0)){
            castling=0;
          }
        }
        Tracker[0] = 2; //The Rook Tracker index is updated with a new position at the pawns corresponding ID.
        KingCastleW=0;
        onetime=1;
        turn = turn + 1; // Increment the turn counter.
          KG = 1; // Update the king's position
        Tracker[KingTrackerID] = KG; 
        break;
      }
      if ((myArray[5] == 1)&&(castle2W==1)) { // Castling White Right
        lightsoff(); // Turn off the LEDs.
        castling=1;
        while(castling==1){
          strip.setPixelColor(7, strip.Color(0, 127, 0));
            strip.setPixelColor(4, strip.Color(0, 0, 127));
          strip.show();
          dataCollection();
          if((myArray[5]==1) && (myArray[4]==1)&&(myArray[7]==0)){
            castling=0;
          }
        }
        Tracker[7] = 4; //The Rook Tracker index is updated with a new position at the pawns corresponding ID.
        KingCastleW=0;
        onetime=1;
        turn = turn + 1; // Increment the turn counter.
          KG = 5; // Update the king's position
        Tracker[KingTrackerID] = KG; 
        break;
      }
      if ((myArray[61] == 1)&&(castle2B==1)) { // Castling White Right
        lightsoff(); // Turn off the LEDs.
        castling=1;
        while(castling==1){
          strip.setPixelColor(63, strip.Color(0, 127, 0));
            strip.setPixelColor(60, strip.Color(0, 0, 127));
          strip.show();
          dataCollection();
          if(  (myArray[60]==1)&&(myArray[63]==0)&& (myArray[61]==1)){
            castling=0;
          }
        }
        Tracker[16] = 60; //The Rook Tracker index is updated with a new position at the pawns corresponding ID.
        KingCastleB=0;
        onetime=1;
        turn = turn + 1; // Increment the turn counter.
          KG = 61; // Update the king's position
        Tracker[KingTrackerID] = KG; 
        break;
      }
      if ((myArray[57] == 1)&&(castle1B==1)) { // Castling White Right
        lightsoff(); // Turn off the LEDs.
        castling=1;
        while(castling==1){
          strip.setPixelColor(56, strip.Color(0, 127, 0));
            strip.setPixelColor(58, strip.Color(0, 0, 127));
          strip.show();
          dataCollection();
          if(  (myArray[57]==1)&&(myArray[56]==0)&& (myArray[58]==1)){
            castling=0;
          }
        }
        Tracker[23] = 58; //The Rook Tracker index is updated with a new position at the pawns corresponding ID.
        KingCastleB=0;
        onetime=1;
        turn = turn + 1; // Increment the turn counter.
          KG = 57; // Update the king's position
        Tracker[KingTrackerID] = KG; 
        break;
      }
    King = myArray[KG]; // Update the king's current state.
  }
  }
  return KG; // Return the updated position of the king.
}

int PawnFunct(int PW){//Pawn Calculations

  if (PW == 65){//Eject Pieces Declared Captured
    return PW;
  }

  dataCollection(); // Collecting data
  int PawnTrackerID; //Holds Global Tracker Pawn Index
  int PawnInitial[16] = {8,9,10,11,12,13,14,15,55,54,53,52,51,50,49,48};
  int pawn_row = PW / 8; //Determines which Row the Pawn is in
  int pawn_col = PW % 8; // Determines which Column the Pawn is in
  int pawn_mov[2]; //An array which will holds the front two positions of the Pawn's movement
  int pawn_cap[2] = {65, 65};//I think this holds potential captures
  //int initialState[64];//I think this holds a mini chess board inside itself
  int Pawn = myArray[PW];//Holds whether the actual sensor is high or low
  int PawnLight[2] = {127,127}; //This Pawn Light function is what lights up the strip, these can be used likely in the Lock to comment out positions that are known friendly pieces, and we might be able to even create one for captures.
  //bool turnedYellow[64] = {false};//holds whether a piece is to show its invalid or not
  int passant[2]={0,0};

  for (int i=0;i<32;i++) {//Identifies the Pawn Tracker ID within the tracker
    if (Tracker[i] == PW) {
      PawnTrackerID = i;  // Store the index where the value is found in z
    }
  }

  if (PawnTrackerID>7 && PawnTrackerID<16){ //Calculate Pawn Mov for White Pieces
    for (int i=1;i<=2;i++){ 
      pawn_mov[i-1] = Rows[pawn_row+i][pawn_col];
    }
  }

  if (PawnTrackerID>23 && PawnTrackerID<32){//Calculate Pawn Mov for Black Pieces
    for (int i=1;i<=2;i++){ 
      pawn_mov[i-1] = Rows[pawn_row-i][pawn_col];
    }
  }

  int pm2 = pawn_mov[1];//Temporary Pawn Mov 2 Storage

  for (int i=0;i<16;i++) {//Checks if Pawn is in the initial state to decide whether to assign 2 moves foward or not
      if (PW == PawnInitial[i]) {
        PawnInitial[i] = 404;
        pawn_mov[1] = pm2;
        break;
      }
      else{
        pawn_mov[1] = 65;
      }
  }
  
  if (PawnTrackerID < 16){//Capture Logic for White Pawn
    for (int i=16;i<32;i++) {
      if(i>23){
        if((promB[i-24]==5)&&((Tracker[i]+8)== Rows[pawn_row + 1][pawn_col+1])) {
          passant[0]=1;
          pawn_cap[0] = Rows[pawn_row + 1][pawn_col+1];
        }
      }
      if (Tracker[i] == Rows[pawn_row + 1][pawn_col+1]){//Detect if Capture Range Is triggered on the Right Side
        pawn_cap[0] = Rows[pawn_row + 1][pawn_col+1];
         if(GlobalKingB==pawn_cap[0]){
            checkB=1;
            }
      }     
      if (pawn_row + 1 > 7 || pawn_col + 1 > 7) {//Detect if the Capture Rang is overflowing on the right side
        pawn_cap[0] = 65;
      }
      if(i>23){
        if((promB[i-24]==5)&&((Tracker[i]+8)== Rows[pawn_row + 1][pawn_col-1])) {
          passant[1]=1;
          pawn_cap[1] = Rows[pawn_row + 1][pawn_col-1];
        }
      }
      if (Tracker[i] == Rows[pawn_row + 1][pawn_col-1]){//Detect if the capture range is triggered on the left side
        pawn_cap[1] = Rows[pawn_row + 1][pawn_col-1];
        if(GlobalKingB==pawn_cap[1]){
            checkB=1;
        }
      }     
      if (pawn_row + 1 > 7 || pawn_col - 1 < 0) {//detect if the capture range is overflowing on the left side
        pawn_cap[1] = 65;
      }

    }
  }

  if (PawnTrackerID > 15){//Capture Logic for Black Pawn
    for (int i=0;i<16;i++) {
      if(i>7){
        if((promW[i-8]==5)&&((Tracker[i]-8)== Rows[pawn_row - 1][pawn_col+1])) {
          pawn_cap[0] = Rows[pawn_row-1 ][pawn_col+1];
          passant[0]=1;
        }
      }
      if (Tracker[i] == Rows[pawn_row - 1][pawn_col+1]){//Detect if Capture Range Is triggered on the Right Side
        pawn_cap[0] = Rows[pawn_row - 1][pawn_col+1];
        if(GlobalKingW==pawn_cap[0]){
          checkW=1;
        }
      }     
      if (pawn_row - 1 < 0 || pawn_col + 1 > 7) {//Detect if the Capture Rang is overflowing on the right side
        pawn_cap[0] = 65;
      }
      if(i>7){
        if((promW[i-8]==5)&&((Tracker[i]-8)== Rows[pawn_row - 1][pawn_col-1])) {
          pawn_cap[1] = Rows[pawn_row-1 ][pawn_col-1];
          passant[1]=1;
        }
      }
      if (Tracker[i] == Rows[pawn_row - 1][pawn_col-1]){//Detect if the capture range is triggered on the left side
        pawn_cap[1] = Rows[pawn_row - 1][pawn_col-1];
        if(GlobalKingW==pawn_cap[1]){
            checkW=1;
        }
      }     
      if (pawn_row - 1 < 0 || pawn_col - 1 < 0) {//detect if the capture range is overflowing on the left side
        pawn_cap[1] = 65;
      }
    }
  }

  for (int i = 0; i < 64; ++i) {//Preserves original board pre while loop
    initialState[i] = myArray[i];
  }

  if(onetime==0){//"Pickup" State
    while (Pawn== 0) {  // While loop starts if Pawn = 0, or if a Pawn is picked up
      dataCollection(); //Needed or Else Data not properly collected
      for (int i=0;i<=1;i++) {//Locks disables move 
        for (int j=0;j<32;j++) {
          if (Tracker[j]==pawn_mov[i]) {
            pawn_mov[i]=65; 
          }
        }
      }
      for(int i=0;i<=0;i++){//Locks Iteration
        if (pawn_mov[i]==65){
          pawn_mov[i+1]=65;
        }
      }
        strip.setPixelColor(PW, strip.Color(0, 127, 127));
        strip.show();
      for (int i=0;i<=1;i++){//Lightup
        if((pawn_mov[i]<56)&&(pawn_mov[i]>7)){//General Case
          strip.setPixelColor(pawn_mov[i],strip.Color(0, PawnLight[i], 0));
          strip.show();
        }
        if((pawn_mov[i]>=56)||(pawn_mov[i]<=7)){//Promotion
            if((analogRead(A0)<=1023)&&(analogRead(A0)>767)){//Queen Brown
              strip.setPixelColor(pawn_mov[i],strip.Color(165,42,42));
              strip.show();
            }
            if((analogRead(A0)<=767)&&(analogRead(A0)>511)){//Knight Purple
              strip.setPixelColor(pawn_mov[i],strip.Color(128,0,128));
              strip.show();
            }
            if((analogRead(A0)<=511)&&(analogRead(A0)>255)){//Bishop Orange
              strip.setPixelColor(pawn_mov[i],strip.Color(255,165,0));
              strip.show();
            }
            if((analogRead(A0)<=255)&&(analogRead(A0)>=0)){//Rook White
              strip.setPixelColor(pawn_mov[i],strip.Color(255,255,255));
              strip.show();
            }
        }
      }
      for (int i=0;i<=1;i++){//Lightup Capture
        if((pawn_cap[i]<56)||(pawn_cap[i]>7)){//General Case
            if(passant[i]==0){
              strip.setPixelColor(pawn_cap[i],strip.Color(127, 0, 0));
              strip.show();
            }
          if(passant[i]==1){
            if(PawnTrackerID<16){
              strip.setPixelColor(pawn_cap[i]-8,strip.Color(127, 0, 0));
              strip.show();
            }
            if(PawnTrackerID>15){
              strip.setPixelColor(pawn_cap[i]+8,strip.Color(127, 0, 0));
              strip.show();
            }
          }
        }
        if(((pawn_cap[i]>=56)||(pawn_cap[i]<=7))&&(pawn_cap[i]!=65)){//Promotion Lightup
            if((analogRead(A0)<=1023)&&(analogRead(A0)>767)){//Queen Brown
              strip.setPixelColor(pawn_cap[i],strip.Color(165,42,42));
              strip.show();
            }
            if((analogRead(A0)<=767)&&(analogRead(A0)>511)){//Knight Purple
              strip.setPixelColor(pawn_cap[i],strip.Color(128,0,128));
              strip.show();
            }
            if((analogRead(A0)<=511)&&(analogRead(A0)>255)){//Bishop Orange
              strip.setPixelColor(pawn_cap[i],strip.Color(255,165,0));
              strip.show();
            }
            if((analogRead(A0)<=255)&&(analogRead(A0)>=0)){//Rook White
              strip.setPixelColor(pawn_cap[i],strip.Color(255,255,255));
              strip.show();
            }
        }
      }
  
      for (int i=0; i < 2; i++) { // Iterate from 0 to 2 for the pawn's capturing positions
        if(passant[i]==0){
          if ((myArray[pawn_cap[i]] == 0) && (pawn_cap[i] != 65) ) { // Check if the pawn can capture a piece at this position
            CaptureStandby = 1; // Set the capture standby state to 1
            while (CaptureStandby == 1) { // Loop while CaptureStandby is 1
            for (int j=0; j < 32; j++) { // Loop through the Tracker array
                if (Tracker[j] == pawn_cap[i]){ // If a capturable piece is found at this position
                  Tracker[j] = 65;
                } // Remove that piece (set to 65 to indicate it's no longer on the board)
              }
              CaptureStandby = 2; // Set CaptureStandby to 2
              lightsoff(); // Turn off all LEDs; assumed to be defined elsewhere
              strip.setPixelColor(pawn_cap[i], strip.Color(0, 0, 127)); // Set LED at the pawn's new position
              strip.show(); // Update the LED display
              while (CaptureStandby == 2) { // Loop while CaptureStandby is 2
                dataCollection(); // Collect data; function assumed to be defined elsewhere
                if (myArray[pawn_cap[i]] == 1) { // If the pawn is moved to the new position
                    lightsoff(); // Turn off LEDs
                    PW = pawn_cap[i]; // Update the pawn's position
                    Tracker[PawnTrackerID] = PW; // Update Tracker array with new pawn position
                    
                    onetime=1;
                    turn = turn + 1; // Increment turn counter
                      CaptureStandby = 0; // Reset CaptureStandby to 0 to exit while loops
                      if(PW>=56){
            if((analogRead(A0)<=1023)&&(analogRead(A0)>767)){//White Queen
              promW[PawnTrackerID-8]=4;
            }
            if((analogRead(A0)<=767)&&(analogRead(A0)>511)){//White Knight
              promW[PawnTrackerID-8]=3;
            }
            if((analogRead(A0)<=511)&&(analogRead(A0)>255)){//White Bishop
              promW[PawnTrackerID-8]=2;
            }
            if((analogRead(A0)<=255)&&(analogRead(A0)>=0)){//White Rook
              promW[PawnTrackerID-8]=1;
            }
          }
          if(PW<=7){
            if((analogRead(A0)<=1023)&&(analogRead(A0)>767)){//Black Queen
              promB[PawnTrackerID-24]=4;
            }
            if((analogRead(A0)<=767)&&(analogRead(A0)>511)){//Black Knight
              promB[PawnTrackerID-24]=3;
            }
            if((analogRead(A0)<=511)&&(analogRead(A0)>255)){//Black Bishop
              promB[PawnTrackerID-24]=2;
            }
            if((analogRead(A0)<=255)&&(analogRead(A0)>=0)){//Black Pawn
              promB[PawnTrackerID-24]=1;
            }

          }
                      return PW; // Return the new pawn position
                }
              }
            }
          }
        }
        if(passant[i]==1){
          if(PawnTrackerID<16){
            if ((myArray[pawn_cap[i]-8] == 0) && (pawn_cap[i] != 65)) { // Check if the pawn can capture a piece at this position
              CaptureStandby = 1; // Set the capture standby state to 1
              while (CaptureStandby == 1) { // Loop while CaptureStandby is 1
              for (int j=23; j < 32; j++) { // Loop through the Tracker array
                  if (Tracker[j] == pawn_cap[i]-8){ // If a capturable piece is found at this position
                    Tracker[j] = 65;
                  } // Remove that piece (set to 65 to indicate it's no longer on the board)
                }
                CaptureStandby = 2; // Set CaptureStandby to 2
                lightsoff(); // Turn off all LEDs; assumed to be defined elsewhere
                strip.setPixelColor(pawn_cap[i], strip.Color(0, 0, 127)); // Set LED at the pawn's new position
                strip.show(); // Update the LED display
                while (CaptureStandby == 2) { // Loop while CaptureStandby is 2
                  dataCollection(); // Collect data; function assumed to be defined elsewhere
                  if (myArray[pawn_cap[i]] == 1) { // If the pawn is moved to the new position
                      lightsoff(); // Turn off LEDs
                      PW = pawn_cap[i]; // Update the pawn's position
                      Tracker[PawnTrackerID] = PW; // Update Tracker array with new pawn position
                      turn = turn + 1; // Increment turn counter
                      passant[i]=0;
                      onetime=1;
                      CaptureStandby = 0; // Reset CaptureStandby to 0 to exit while loops
                      return PW; // Return the new pawn position
                  }
                }
              }
            }
          }
          if(PawnTrackerID>15){
            if ((myArray[pawn_cap[i]+8] == 0) && (pawn_cap[i] != 65)) { // Check if the pawn can capture a piece at this position
              CaptureStandby = 1; // Set the capture standby state to 1
              while (CaptureStandby == 1) { // Loop while CaptureStandby is 1
              for (int j=8; j < 16; j++) { // Loop through the Tracker array
                  if (Tracker[j] == pawn_cap[i]+8){ // If a capturable piece is found at this position
                    Tracker[j] = 65;
                  } // Remove that piece (set to 65 to indicate it's no longer on the board)
                }
                CaptureStandby = 2; // Set CaptureStandby to 2
                lightsoff(); // Turn off all LEDs; assumed to be defined elsewhere
                strip.setPixelColor(pawn_cap[i], strip.Color(0, 0, 127)); // Set LED at the pawn's new position
                strip.show(); // Update the LED display
                while (CaptureStandby == 2) { // Loop while CaptureStandby is 2
                  dataCollection(); // Collect data; function assumed to be defined elsewhere
                  if (myArray[pawn_cap[i]] == 1) { // If the pawn is moved to the new position
                      lightsoff(); // Turn off LEDs
                      PW = pawn_cap[i]; // Update the pawn's position
                      Tracker[PawnTrackerID] = PW; // Update Tracker array with new pawn position
                      turn = turn + 1; // Increment turn counter
                      passant[i]=0;
                      onetime=1;
                      CaptureStandby = 0; // Reset CaptureStandby to 0 to exit while loops
                      return PW; // Return the new pawn position
                  }
                }
              }
            }
          }
        }
      }

      dataCollection();
      invalidatePositions(pawn_mov, 16, Tracker, strip, turnedYellow, (byte*)myArray, initialState, PW);

      for (int i=0;i<=1;i++){//Move Loop
        if (((myArray[pawn_mov[i]]==1)&&(pawn_mov[i] != PW) && (pawn_mov[i]!=65))) {
          lightsoff();
          PW = pawn_mov[i];
          if(i==1){
            promW[PawnTrackerID-8]=5;
            promB[PawnTrackerID-24]=5;
          }
          onetime=1;
          Tracker[PawnTrackerID] = PW; //The Pawn Tracker index is updated with a new position at the pawns corresponding ID.
          turn=turn+1; //Update turns 
          if(PW>=56){
            if((analogRead(A0)<=1023)&&(analogRead(A0)>767)){//White Queen
              promW[PawnTrackerID-8]=4;
            }
            if((analogRead(A0)<=767)&&(analogRead(A0)>511)){//White Knight
              promW[PawnTrackerID-8]=3;
            }
            if((analogRead(A0)<=511)&&(analogRead(A0)>255)){//White Bishop
              promW[PawnTrackerID-8]=2;
            }
            if((analogRead(A0)<=255)&&(analogRead(A0)>=0)){//White Rook
              promW[PawnTrackerID-8]=1;
            }
          }
          if(PW<=7){
            if((analogRead(A0)<=1023)&&(analogRead(A0)>767)){//Black Queen
              promB[PawnTrackerID-24]=4;
            }
            if((analogRead(A0)<=767)&&(analogRead(A0)>511)){//Black Knight
              promB[PawnTrackerID-24]=3;
            }
            if((analogRead(A0)<=511)&&(analogRead(A0)>255)){//Black Bishop
              promB[PawnTrackerID-24]=2;
            }
            if((analogRead(A0)<=255)&&(analogRead(A0)>=0)){//Black Pawn
              promB[PawnTrackerID-24]=1;
            }

          }
          break;
        }
      }
      Pawn = myArray[PW];
    }
  }
  return PW;

}

int RookFunct(int RK) {
  if (RK == 65){
    return RK;
  }
  dataCollection(); // Invoke the dataCollection function to gather up-to-date board state. The specifics of this function are not provided.
  // Retrieve the current state of the Rook from myArray using the index RK.
  int Rook = myArray[RK]; 
  int RookTrackerID;
  int rook_row = RK / 8; // Integer division to find the row.
  int rook_col = RK % 8; // Modulus to find the column.
  int rook_mov[16]; 
  int w = 0; 
  int y = 0;
  int z = 0;
  int rook_cap[16] = {
  65, 65, 65, 65, 65, 65, 65, 65,
  65, 65, 65, 65, 65, 65, 65, 65
  };
  int RookLight[16] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};
  RookLight[y] = 0;
  RookLight[z] = 0;
  for (int i=0;i<32;i++) {
    if (Tracker[i] == RK) {
      RookTrackerID = i;  // Store the index where the value is found in z
    }
  }
  // Calculate the row and column positions of the Rook based on the index RK.

  // Populate the first half of rook_mov with positions in the same row as the Rook.
  for (int i = 0; i <= 7; i++) {
    rook_mov[i] = Rows[rook_row][i];
  }

  // Populate the second half of rook_mov with positions in the same column as the Rook.
  for (int i = 0; i <= 7; i++) {
    rook_mov[i + 8] = Rows[i][rook_col];
  }

  // Loop to find the index at which RK appears in rook_mov. Two positions will match, hence the usage of w, y, and z.
  for (int i = 0; i < 16; ++i) {
    if (rook_mov[i] == RK && w == 0) {
      y = i;  // Store the first occurrence index.
      w = 1;  // Update w so the next occurrence can be captured.
    }
    if (rook_mov[i] == RK && w == 1) {
      z = i;  // Store the second occurrence index.
    }
  }

  // Initialize rook_cap array with size 16, all elements set to 65


  if (RookTrackerID < 16) {
  // Limit the loop to iterate only 16 times
  for (int i = 0; i < 16; i++) {
    // Change the inner loop bounds accordingly
    for (int j = 16; j < 32; j++) {
      // Replace QN with RK
      if (Tracker[j] != RK) {
        // Change queen_mov to rook_mov and queen_cap to rook_cap
        if (Tracker[j] == rook_mov[i]) {
          rook_cap[i] = rook_mov[i];
        }
      }
    }
  }
  }

  if (RookTrackerID > 15) {
  // Limit the loop to iterate only 16 times
  for (int i = 0; i < 16; i++) {
    // Change the inner loop bounds to 0 to 15 (0 to <16)
    for (int j = 0; j < 16; j++) {
      // Replace QN with RK
      if (Tracker[j] != RK) {
        // Change queen_mov to rook_mov and queen_cap to rook_cap
        if (Tracker[j] == rook_mov[i]) {
          rook_cap[i] = rook_mov[i];
        }
      }
    }
  }
  }

    for (int i=0;i<=15;i++) {
    for (int j=0;j<32;j++) {
      if(Tracker[j]!=RK) {
        if ((Tracker[j]==(rook_mov[i]))){
          rook_mov[i]=65; 
        }
      }
    }
    }

    //next we need to set which pieces is the actual player piece, this is straightfoward, needs to just compare data, if
    //looking to reduce delay i recommend starting here, note this is why an exception for disabling needed to be made, as it would disbale the ability
    //to find RK in the array
    //this is where it all comes together, this series of sequence sequentially analyzese the data front and back from the rook position
    //in order to assign the null pieces
    for (int i = 7; i >= 1; i--) {
    if (rook_mov[i] == 65 && Rows[rook_row][i] < RK) {
    rook_mov[i - 1] = 65;
    if (rook_mov[i - 1] != rook_cap[i - 1]) {
      rook_cap[i - 1] = 65;
    }
    }
    }

    for (int i = 0; i < 7; i++) {//LOCKS
    if (rook_mov[i] == 65 && Rows[rook_row][i] > RK) {
    rook_mov[i + 1] = 65;
    if (rook_mov[i + 1] != rook_cap[i + 1]) {
      rook_cap[i + 1] = 65;
    }
    }
    }

    for (int i = 7; i >= 1; i--) {
    if (rook_mov[i + 8] == 65 && Rows[i][rook_col] < RK) {
    rook_mov[i + 7] = 65;
    if (rook_mov[i + 7] != rook_cap[i + 7]) {
      rook_cap[i + 7] = 65;
    }
    }
    }

    for (int i = 0; i < 7; i++) {
    if (rook_mov[i + 8] == 65 && Rows[i][rook_col] > RK) {
    rook_mov[i + 9] = 65;
    if (rook_mov[i + 9] != rook_cap[i + 9]) {
      rook_cap[i + 9] = 65;
    }
   }
    }
  // Initialize an array that will be used to set the brightness of NeoPixel LEDs, with 127 representing maximum brightness.

  // Turn off the LED at the Rook's positions by setting brightness to 0 for those indices.

    if (RookTrackerID < 16) {
  // Limit the loop to iterate only 16 times
  for (int i = 0; i < 16; i++) {
    // Change the inner loop bounds accordingly
          if(GlobalKingB==rook_cap[i]){
            checkB=1;
        
  }
  }
  }
  if (RookTrackerID > 15) {
  for (int i = 0; i < 16; i++) {
    // Change the inner loop bounds accordingly
          if(GlobalKingW==rook_cap[i]){
            checkW=1;      
  }
  }
  }
  if(onetime==0){
  while (Rook == 0) {
    // Sample Rows[rook_row][rook_col]
    dataCollection(); // Refresh data.
    //The basic Locking function, collects info to detect whether or not theres another
    //piece in the tracker, made exception to self due to interference later in the code
        strip.setPixelColor(RK, strip.Color(0, 127, 127));
        strip.show();
    // Loop through each valid move and set the LED color based on the value in RookLight.
    for (int i = 0; i <= 15; i++) {
      if(rook_mov[i]!=RK){
        strip.setPixelColor(rook_mov[i], strip.Color(0, RookLight[i], 0));
        strip.show();
      }
    }
    for (int i = 0; i < 16; i++) {
      strip.setPixelColor(rook_cap[i], strip.Color(127, 0, 0));
      strip.show();
    }

    for (int i=0; i < 16; i++) { // Iterate from 0 to 16 for rook's capturing positions
    if (myArray[rook_cap[i]] == 0 && rook_cap[i] != 65) { // Check if the rook can capture a piece at this position
        CaptureStandby = 1; // Initiate capture standby state
        while (CaptureStandby == 1) { // Continue as long as CaptureStandby is 1
            for (int j=0; j < 32; j++) { // Iterate through the Tracker array
                if (Tracker[j] == rook_cap[i]) // If a piece that can be captured is found
                    Tracker[j] = 65; // Remove that piece (set to 65 to indicate it's no longer on the board)
            }
            CaptureStandby = 2; // Transition to the next CaptureStandby state
            lightsoff(); // Turn off all LEDs; function assumed to be defined elsewhere
            strip.setPixelColor(rook_cap[i], strip.Color(0, 0, 127)); // Set the LED color for the rook's new position
            strip.show(); // Update the LED display
            while (CaptureStandby == 2) { // Continue as long as CaptureStandby is 2
                dataCollection(); // Collect data, function assumed to be defined elsewhere
                if (myArray[rook_cap[i]] == 1) { // If the rook is moved to the new position
                    lightsoff(); // Turn off LEDs
                    RK = rook_cap[i]; // Update rook's position
            if(RookTrackerID==0){
              RookCastleW[0]=0;
            }
            if(RookTrackerID==7){
              RookCastleW[1]=0;
            }
            if(RookTrackerID==15){
              RookCastleB[0]=0;
            }
            if(RookTrackerID==22){
              RookCastleB[1]=0;
            }
                    Tracker[RookTrackerID] = RK; // Update Tracker array with the new rook position
                    turn = turn + 1; // Increment the turn counter
                    onetime=1;
                    CaptureStandby = 0; // Reset CaptureStandby to exit the while loops
                    return RK; // Return the updated rook's position
                }
            }
        }
    }
    }

    // Loop to detect if a piece is placed in a valid position. If so, update RK and turn off lights.
    for (int i = 0; i <= 15; i++) {
      if(rook_mov[i]!=RK) {
        if (myArray[rook_mov[i]] == 1 && rook_mov[i] != RK) {
          lightsoff(); // Function to turn off all lights; its definition is not provided.
            if(RookTrackerID==0){
              RookCastleW[0]=0;
            }
            if(RookTrackerID==7){
              RookCastleW[1]=0;
            }
            if(RookTrackerID==15){
              RookCastleB[0]=0;
            }
            if(RookTrackerID==22){
              RookCastleB[1]=0;
            }
          RK = rook_mov[i]; // Update Rook's position.
          Tracker[RookTrackerID] = RK; //The Rook Tracker index is updated with a new position at the pawns corresponding ID.
          onetime=1;
          turn = turn + 1; // Increment the turn counter.
          Serial.print("Cool");
          Serial.print("\n");
          CaptureStandby=0;
          return RK;
        }
      }
  }
    Rook = myArray[RK];
  }
    // Update Rook's state based on its new position.
    Rook = myArray[RK];
    }
  
  return RK; // Return the new position of the Rook.
}

int KnightFunct(int KN) {
  if (KN == 65){
    return KN;
  }
  // Calls the dataCollection function to update the board state. The specific details of this function are not provided.
  dataCollection();

  // Retrieves the current state (presumably 0 or 1 to indicate empty or filled) of the Knight from the array myArray at index KN.
  int Knight = myArray[KN];
  int KnightTrackerID;
  // Determines the row and column of the Knight based on its index, using integer division and modulus operations.
  int knight_row = KN / 8;
  int knight_col = KN % 8;
  for (int i=0;i<32;i++) {
    if (Tracker[i] == KN) {
      KnightTrackerID = i;  // Store the index where the value is found in z
    }
  }

  // Declares an array knight_mov to hold the indices of the 8 potential positions to which the Knight can move.
  int knight_mov[8];

  // Initialize arrays to hold the row and column moves for a knight. These represent the 8 possible moves for a knight on a chessboard.
  int row_moves[] = {2, 1, -1, -2, -2, -1, 1, 2};
  int col_moves[] = {1, 2, 2, 1, -1, -2, -2, -1};

  // Loop through each of the 8 possible moves and calculate the new row and column after making each move.
  for (int i = 0; i < 8; ++i) {
    int new_row = knight_row + row_moves[i];
    int new_col = knight_col + col_moves[i];

    // Checks if the new position is within the bounds of the 8x8 board. If yes, store it; otherwise, store 65 to indicate an invalid move.
    if (new_row >= 0 && new_row <= 7 && new_col >= 0 && new_col <= 7) {
      knight_mov[i] = Rows[new_row][new_col];
    } else {
      knight_mov[i] = 65; // Invalid or out-of-boundary move.
    }
  }
  // Initialize the knight_cap array with size 8, all elements set to 65
  int knight_cap[8] = {
  65, 65, 65, 65, 65, 65, 65, 65
  };

  // Case when KnightTrackerID is less than 16
  if (KnightTrackerID < 16) {
    // Limit the outer loop to iterate only 8 times
    for (int i = 0; i < 8; i++) {
      // Inner loop to iterate from 16 to 31 (16 to <32)
      for (int j = 16; j < 32; j++) {
        // Replace BP with KN
        if (Tracker[j] != KN) {
          // Compare Tracker[j] with knight_mov[i] and update knight_cap[i] accordingly
          if (Tracker[j] == knight_mov[i]) {
            knight_cap[i] = knight_mov[i];
            if(GlobalKingB==knight_cap[i]){
              checkB=1;
            }
          }
        }
      }
    }
  }

  // Case when KnightTrackerID is greater than 16
  if (KnightTrackerID > 16) {
    // Limit the outer loop to iterate only 8 times
    for (int i = 0; i < 8; i++) {
      // Inner loop to iterate from 0 to 15 (0 to <16)
      for (int j = 0; j < 16; j++) {
        // Replace BP with KN
        if (Tracker[j] != KN) {
          // Compare Tracker[j] with knight_mov[i] and update knight_cap[i] accordingly
          if (Tracker[j] == knight_mov[i]) {
            knight_cap[i] = knight_mov[i];
            if(GlobalKingW==knight_cap[i]){
              checkW=1;
            }
          }
        }
      }
    }
  }

  // Initializes an array, KnightLight, that will control the brightness of NeoPixel LEDs for the Knight's valid moves. Here, 127 signifies maximum brightness.
  int KnightLight[8] = {127, 127, 127, 127, 127, 127, 127, 127};

  for (int i=0;i<=7;i++) {
    for (int j=0;j<32;j++) {
      if(Tracker[j]!=KN) {
        if ((Tracker[j]==(knight_mov[i]))) {
          knight_mov[i]=65;
        }
      }
    }
  }


  // The following while loop executes when the Knight piece is picked up, indicated by Knight == 0.
  if(onetime==0){
    while (Knight == 0) {
      // Refreshes the board state by calling the dataCollection function again.
      dataCollection();
      strip.setPixelColor(KN, strip.Color(0, 127, 127));
      strip.show();
      // Loop through each valid move position stored in knight_mov. Light up each corresponding LED using the brightness specified in KnightLight.
      for (int i = 0; i < 8; i++) {
        strip.setPixelColor(knight_mov[i], strip.Color(0, KnightLight[i], 0));
        strip.show();
      }
      for (int i = 0; i < 8; i++) {
        strip.setPixelColor(knight_cap[i], strip.Color(127, 0, 0));
        strip.show();
      }
      for (int i=0; i < 8; i++) { // Iterate from 0 to 8 for knight's capturing positions
        if (myArray[knight_cap[i]] == 0 && knight_cap[i] != 65) { // Check if the knight can capture the piece at this position
            CaptureStandby = 1; // Initiate capture standby state
            while (CaptureStandby == 1) { // Continue as long as CaptureStandby is 1
                for (int j=0; j < 32; j++) { // Iterate through the Tracker array
                    if (Tracker[j] == knight_cap[i]) // If a piece that can be captured is found
                        Tracker[j] = 65; // Remove that piece (set to 65 to indicate it is no longer on the board)
                }
                CaptureStandby = 2; // Transition to next CaptureStandby state
                lightsoff(); // Turn off all LEDs; function assumed to be defined elsewhere
                strip.setPixelColor(knight_cap[i], strip.Color(0, 0, 127)); // Set the LED color for the knight's new position
                strip.show(); // Update the LED display
                while (CaptureStandby == 2) { // Continue as long as CaptureStandby is 2
                    dataCollection(); // Collect data, function assumed to be defined elsewhere
                    if (myArray[knight_cap[i]] == 1) { // If the knight is moved to the new position
                        lightsoff(); // Turn off LEDs
                        KN = knight_cap[i]; // Update knight's position
                        Tracker[KnightTrackerID] = KN; // Update Tracker array with the new knight position
                        turn = turn + 1; // Increment the turn counter
                        onetime=1;
                        CaptureStandby = 0; // Reset CaptureStandby to exit the while loops
                        return KN; // Return the updated knight's position
                    }
                }
            }
        }
      }

      dataCollection();
      invalidatePositions(knight_mov, 32, Tracker, strip, turnedYellow, (byte*)myArray, initialState, KN);

      // Check if a piece is placed in a valid position other than the Knight's original position. If so, update KN, turn off the lights, and increment the turn counter.
      for (int i = 0; i < 8; i++) {
        if (myArray[knight_mov[i]] == 1 && knight_mov[i] != KN) {
          lightsoff(); // Turns off all LEDs. Specifics not provided in the snippet.
          KN = knight_mov[i]; // Update the Knight's new position.
          Tracker[KnightTrackerID] = KN; //The Rook Tracker index is updated with a new position at the pawns corresponding ID.
          onetime=1;
          turn = turn + 1; // Increment the turn counter.
          break; // Exit the loop.
        }
      }

      // Update the Knight's state to reflect its new position.
      Knight = myArray[KN];
    }
  }
    // Return the new position of the Knight.
  return KN;
}

int BishopFunct(int BP) {
  if (BP == 65){
    return BP;
  }
  // Calls an external function dataCollection to update the game state. The function's implementation is not provided in this code snippet.
  dataCollection();
  
  // Fetches the value at the index 'BP' from 'myArray' to determine the state of the Bishop's position.
  // The value is likely either 0 or 1, representing empty or occupied respectively.
  int BishopTrackerID;
  int Bishop = myArray[BP]; 
  int bishop_row = BP / 8;
  int bishop_col = BP % 8;
  int bishop_mov[32];
  int bishop_cap[32] = {
    65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65
  };
  for (int i=0;i<32;i++) {
    bishop_mov[i]=0;
    if (Tracker[i] == BP) {
      BishopTrackerID = i;  // Store the index where the value is found in z
    }
  } 

  // These next for-loops populate bishop_mov with the board positions the Bishop can move to along the diagonals.
  for (int i = 0; i <= 7; i++) {
    // Populate positions for the southeast diagonal.
    bishop_mov[i] = Rows[bishop_row + i][bishop_col + i];
    // Checks if the calculated row or column index is out of bounds (8x8 board), if so, sets to 65 (invalid position).
    if (bishop_row + i > 7 || bishop_col + i > 7) {
      bishop_mov[i] = 65;
    }
    // Populate positions for the northeast diagonal.
    bishop_mov[i + 8] = Rows[bishop_row + i][bishop_col - i];
    // Checks for out-of-bound row or column indices.
    if (bishop_row + i > 7 || bishop_col - i < 0) {
      bishop_mov[i + 8] = 65;
    }  
    // Populate positions for the northwest diagonal.
    bishop_mov[i + 16] = Rows[bishop_row - i][bishop_col + i];
    // Checks for out-of-bound row or column indices.
    if (bishop_row - i < 0 || bishop_col + i > 7) {
      bishop_mov[i + 16] = 65;
    }
    // Populate positions for the southwest diagonal.
    bishop_mov[i + 24] = Rows[bishop_row - i][bishop_col - i];
    // Checks for out-of-bound row or column indices.
    if (bishop_row - i < 0 || bishop_col - i < 0) {
      bishop_mov[i + 24] = 65;
    }
  }



  if (BishopTrackerID < 16){//Cap White
    for (int i = 0; i < 32; i++) {
      for (int j = 16; j < 32; j++) {
        if (Tracker[j] != BP) {
          if (Tracker[j] == bishop_mov[i]) {
            bishop_cap[i] = bishop_mov[i];
          }
        }
      }
    }
  }

  if (BishopTrackerID > 16){//Cap Black
    for (int i = 0; i < 32; i++) {
      for (int j = 0; j < 16; j++) {
        if (Tracker[j] != BP) {
          if (Tracker[j] == bishop_mov[i]) {
            bishop_cap[i] = bishop_mov[i];
          }
        }
      }
    }
  }

  // Initialize an array BishopLight with brightness values (127 for maximum brightness) for each potential move's LED.
  int BishopLight[32] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};

  for (int i=0;i<32;i++) {
      for (int j=0;j<32;j++) {
        if(Tracker[j]!=BP){
          if ((Tracker[j]==(bishop_mov[i]))) { 
            bishop_mov[i]=65;
          }
        }
      }
    }
    //this is where it all comes together, this series of sequence sequentially analyzese the data front and back from the rook position
    //in order to assign the null pieces

    for (int i = 0; i < 7; i++) {
      if (bishop_mov[i] == 65) {
      bishop_mov[i + 1] = 65;
      if (bishop_mov[i + 1] != bishop_cap[i + 1]) {
        bishop_cap[i + 1] = 65;
     }
      }
    }

    for (int i = 0; i < 7; i++) {
    if (bishop_mov[i + 8] == 65) {
    bishop_mov[i + 9] = 65;
    if (bishop_mov[i + 9] != bishop_cap[i + 9]) {
      bishop_cap[i + 9] = 65;
    }
    }
    }

    for (int i = 0; i < 7; i++) {
    if (bishop_mov[i + 16] == 65) {
    bishop_mov[i + 17] = 65;
    if (bishop_mov[i + 17] != bishop_cap[i + 17]) {
      bishop_cap[i + 17] = 65;
    }
    }
    }

    for (int i = 0; i < 7; i++) {
    if (bishop_mov[i + 24] == 65) {
    bishop_mov[i + 25] = 65;
    if (bishop_mov[i + 25] != bishop_cap[i + 25]) {
      bishop_cap[i + 25] = 65;
    }
    }
    }

  for(int i=0;i<=31;i++){
    if(BishopTrackerID>15){
      if(GlobalKingW==bishop_cap[i]){
        checkW=1;
      }
      }
    if(BishopTrackerID<16){
      if(GlobalKingB==bishop_cap[i]){
        checkB=1;
      }
    }

  }
  // A while loop that runs as long as the Bishop's current position is empty (Bishop == 0), which is when the piece has been picked up.
  if(onetime==0){
  while (Bishop == 0) {
    // Refreshes the board data.
    dataCollection();
      strip.setPixelColor(BP, strip.Color(0, 127, 127));
      strip.show();
  

    // Prints the brightness values to the Serial monitor for debugging or other purposes.
      BishopLight[0] = 0;
    BishopLight[8] = 0;
    BishopLight[16] = 0;
    BishopLight[24] = 0;
    // Iterates through bishop_mov and sets the LED color at each position with the corresponding brightness in BishopLight.
    for (int i = 0; i < 32; i++) {
        strip.setPixelColor(bishop_mov[i], strip.Color(0, BishopLight[i], 0));
        strip.show();
    }

    for (int i = 0; i < 32; i++) {
      strip.setPixelColor(bishop_cap[i], strip.Color(127, 0, 0));
      strip.show();
    }

    for (int i=0; i < 32; i++) {
      if (myArray[bishop_cap[i]] == 0 && bishop_cap[i] != 65){
        CaptureStandby=1;
        while (CaptureStandby==1){
          for (int j=0; j < 32; j++){
            if (Tracker[j] == bishop_cap[i])
              Tracker[j] = 65;
          }
          CaptureStandby=2;
          lightsoff();
          strip.setPixelColor(bishop_cap[i], strip.Color(0, 0, 127));
          strip.show();
          while (CaptureStandby==2){
            dataCollection();
            if (myArray[bishop_cap[i]] == 1){
              lightsoff();  // Turn off LEDs.
              BP = bishop_cap[i];  // Update queen's position.
              Tracker[BishopTrackerID] = BP;
              onetime=1;
              turn = turn + 1;  // Increment turn counter.
              CaptureStandby=0;
              return BP;
            }
          }
        }
      }
    }
    
    dataCollection();
    invalidatePositions(bishop_mov, 32, Tracker, strip, turnedYellow, (byte*)myArray, initialState, BP);


    // Checks if a piece has been placed in any of the potential positions. If so, updates the Bishop's position, turns off the LEDs, and increments the turn counter.
    for (int i = 0; i < 32; i++) {
        if (myArray[bishop_mov[i]] == 1 && bishop_mov[i] != BP) {
          lightsoff();
          BP = bishop_mov[i];
          Tracker[BishopTrackerID] = BP; //The Rook Tracker index is updated with a new position at the pawns corresponding ID.
          onetime=1;
          turn = turn + 1;
          break;
        }
      
    }
    // Updates the Bishop variable to reflect any changes.
    Bishop = myArray[BP];
  }
  }
    // Returns the updated position of the Bishop.
    return BP;
}

int QueenFunct(int QN) {
  if (QN == 65){
    return QN;
  }

  dataCollection();  // Call a function to collect data about the current state of the board.
  int QueenTrackerID;
  int horizontal;
  int verticle;
  // Get the state of the queen from 'myArray'.
  int Queen = myArray[QN];
  for (int i=0;i<32;i++) {
      if (Tracker[i] == QN) {
        QueenTrackerID = i; }}
  // Calculate row and column index based on the given index (QN).
  int queen_row = QN / 8;
  int queen_col = QN % 8;

  // Array to store possible moves of the queen.
  int queen_mov[48];  // A queen can move in 8 directions to a max of 7 squares in each.

  int w = 0;
  int y = 0;
  int z = 0;

  for (int i = 0; i <= 7; i++) {
  // Bottom right diagonal
  if (queen_row + i <= 7 && queen_col + i <= 7) {
    queen_mov[i] = Rows[queen_row + i][queen_col + i];
  } else {
    queen_mov[i] = 65; // Invalid move
  }

  // Top right diagonal
  if (queen_row - i >= 0 && queen_col + i <= 7) {
    queen_mov[i + 8] = Rows[queen_row - i][queen_col + i];
  } else {
    queen_mov[i + 8] = 65; // Invalid move
  }

  // Bottom left diagonal
  if (queen_row + i <= 7 && queen_col - i >= 0) {
    queen_mov[i + 16] = Rows[queen_row + i][queen_col - i];
  } else {
    queen_mov[i + 16] = 65; // Invalid move
  }

  // Top left diagonal
  if (queen_row - i >= 0 && queen_col - i >= 0) {
    queen_mov[i + 24] = Rows[queen_row - i][queen_col - i];
  } else {
    queen_mov[i + 24] = 65; // Invalid move
  }
  }


  
  for (int i = 0; i <= 7; i++) {
    queen_mov[i + 32] = Rows[queen_row][i];
    queen_mov[i + 40] = Rows[i][queen_col];
  }

  // Identify the index of the original position within the array of possible moves.
  for (int i = 32; i < 48; ++i) {
    if (queen_mov[i] == QN && w == 0) {
      y = i;
      w = 1;
    }
    if (queen_mov[i] == QN && w == 1) {
      z = i;
    }
  }

  int queen_cap[48] ={
  65, 65, 65, 65, 65, 65, 65, 65,
  65, 65, 65, 65, 65, 65, 65, 65,
  65, 65, 65, 65, 65, 65, 65, 65,
  65, 65, 65, 65, 65, 65, 65, 65,
  65, 65, 65, 65, 65, 65, 65, 65,
  65, 65, 65, 65, 65, 65, 65, 65
  };

  if (QueenTrackerID < 16){
    for (int i=0;i<48;i++) {
      for (int j=16;j<32;j++) {
        if(Tracker[j]!=QN) {
          if ((Tracker[j]==(queen_mov[i]))){
            queen_cap[i] = queen_mov[i]; 
          }
        }
      }
    }
  }

  if (QueenTrackerID > 16){
    for (int i=0;i<48;i++) {
      for (int j=0;j<16;j++) {
        if(Tracker[j]!=QN) {
          if ((Tracker[j]==(queen_mov[i]))) {
            queen_cap[i] = queen_mov[i]; 
          }
        }
      }
    }
  }

  // Initialize an array to control the brightness of LEDs.
  int QueenLight[48] = {
  127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 127, 127, 127, 127,
  };

 for (int i=0;i<=47;i++){
    for (int j=0;j<32;j++){
      if(Tracker[j]!=QN){
        if ((Tracker[j]==(queen_mov[i])) ) {
          queen_mov[i]=65;
      }
      }}
      }


  for(int i=0;i<7;i++){
  if(queen_mov[i]==65){
    queen_mov[i+1]=65;
    if (queen_mov[i+1] != queen_cap[i+1]) {
      queen_cap[i+1] = 65;
    }
  }
  }

  for(int i=0;i<7;i++){
  if(queen_mov[i+8]==65){
    queen_mov[i+9]=65;
    if (queen_mov[i+9] != queen_cap[i+9]) {
      queen_cap[i+9] = 65;
    }
  }
  }

  for(int i=0;i<7;i++){
  if(queen_mov[i+16]==65){
    queen_mov[i+17]=65;
    if (queen_mov[i+17] != queen_cap[i+17]) {
      queen_cap[i+17] = 65;
    }
  }
  }

  for(int i=0;i<7;i++){
  if(queen_mov[i+24]==65){
    queen_mov[i+25]=65;
    if (queen_mov[i+25] != queen_cap[i+25]) {
    queen_cap[i+25] = 65;
    }
  }
  }

  for(int i=7;i>=1;i--){
  if(queen_mov[i+32]==65 && Rows[queen_row][i] < QN){
    queen_mov[i+31] = 65;
    if (queen_mov[i+31] != queen_cap[i+31]) {
    queen_cap[i+31] = 65;
    }
  }
  }

  for(int i=0;i<7;i++){
  if(queen_mov[i+32]==65 && Rows[queen_row][i] > QN){
    queen_mov[i+33] = 65;
    if (queen_mov[i+33] != queen_cap[i+33]) {
    queen_cap[i+33] = 65;
    }
  }
  }

  for(int i=7;i>=1;i--){
  if(queen_mov[i+40]==65 && Rows[i][queen_col] < QN){
    queen_mov[i+39] = 65;
    if (queen_mov[i+39] != queen_cap[i+39]) {
    queen_cap[i+39] = 65;
    }
  }
  }

  for(int i=0;i<7;i++){
  if(queen_mov[i+40]==65 && Rows[i][queen_col] > QN){
    queen_mov[i+41] = 65;
    if (queen_mov[i+41] != queen_cap[i+41]) {
    queen_cap[i+41] = 65;
    }
  }
  }
  for(int i=0;i<=47;i++){
    if(QueenTrackerID<16){
      if(GlobalKingB==queen_cap[i]){
        checkB=1;
      }
    }
    if(QueenTrackerID>15){
      if(GlobalKingW==queen_cap[i]){
        checkW=1;
      }
    }
  }
    if(onetime==0){
    while (Queen == 0) {  // Execute if the queen is picked up.
      dataCollection();  // Update board state.
      strip.setPixelColor(QN, strip.Color(0, 127, 127));
      strip.show();

      // Light up LEDs for potential moves based on 'QueenLight' brightness levels.
      for (int i = 0; i < 48; i++) {
      if (queen_mov[i] != QN && queen_mov[i] != 65) { // Do not light up the Queen's current position or invalid moves.
      strip.setPixelColor(queen_mov[i], strip.Color(0, QueenLight[i], 0));
      }
      if (queen_cap[i] != 65) { // Only light up valid capture positions.
      strip.setPixelColor(queen_cap[i], strip.Color(127, 0, 0));
      }
      }
      strip.show();


      // Position tracking: Update queen's position if a valid move is made.
      for (int i = 0; i < 48; i++) {
      if (myArray[queen_mov[i]] == 1 && queen_mov[i] != QN) {
      lightsoff();  // Turn off LEDs.
      QN = queen_mov[i];  // Update queen's position.
      Tracker[QueenTrackerID] = QN;
      onetime=1;
      turn = turn + 1;  // Increment turn counter.
      break;
      }
      }

      for (int i=0; i < 48; i++) {
      if (myArray[queen_cap[i]] == 0 && queen_cap[i] != 65){
      CaptureStandby=1;
        while (CaptureStandby==1){
          for (int j=0; j < 48; j++){
            if (Tracker[j] == queen_cap[i])
              Tracker[j] = 65;
          }
          CaptureStandby=2;
          lightsoff();
          strip.setPixelColor(queen_cap[i], strip.Color(0, 0, 127));
          strip.show();
          while (CaptureStandby==2){
            dataCollection();
            if (myArray[queen_cap[i]] == 1){
              lightsoff();  // Turn off LEDs.
              QN = queen_cap[i];  // Update queen's position.
              Tracker[QueenTrackerID] = QN;
              turn = turn + 1;  // Increment turn counter.
              onetime=1;
              CaptureStandby=0;
              break;
            }
          }
        }
      }
      }
  


      dataCollection();
      invalidatePositions(queen_mov, 32, Tracker, strip, turnedYellow, (byte*)myArray, initialState, QN);
    
      Queen = myArray[QN];  // Update the state of the queen.
    }
    }
  
  return QN;  // Return the new position of the queen.
}

void invalidatePositions(int* positions, int length, int* tracker, Adafruit_NeoPixel& strip, bool* turnedYellow, byte* myArray, byte* initialState, int currentPosition) {
  for (int i = 0; i < 64; i++) {
    bool inCurrentMov = false;
    bool positionOccupied = false;

    // Check if the position is in the current moves array.
    for (int j = 0; j < length; j++) {
      if (i == positions[j]) {
        inCurrentMov = true;
        break;
      }
    }

    // Check if the position is occupied by another piece.
    for (int j = 0; j < 32; j++) { // Assuming there are 32 pieces on the board.
      if (tracker[j] == i && i != currentPosition) {
        positionOccupied = true;
        break;
      }
    }

    // If in current moves, the initialState is 0, the position is not the current position of the piece,
    // no other piece is occupying the position, and it's not the same as the original position,
    // turn the LED yellow.
    if (myArray[i] == 1 && initialState[i] == 0 && !inCurrentMov && !positionOccupied && i != currentPosition) {
      strip.setPixelColor(i, strip.Color(127, 127, 0)); // Yellow color
      strip.show();
      turnedYellow[i] = true;
    } 
    // If the position is no longer valid (myArray[i] == 0) and it was previously turned yellow,
    // or if the position is the original position, turn off the LED.
    else if ((myArray[i] == 0 && turnedYellow[i]) || i == currentPosition) {
      strip.setPixelColor(i, strip.Color(0, 0, 0)); // Off
      strip.show();
      turnedYellow[i] = false;
    }
  }
}

void loop(){
  if(turn==0){
    int counter=0;
    dataCollection();
    for (int i=0; i<64; i++) {
      if ((myArray[i]==1)&&((i<16)||(i>47))){
        strip.setPixelColor(i, strip.Color(0, 0, 127));
        strip.show();
      }
      if ((myArray[i]==1)&&((i>15)&&(i<48))){
        strip.setPixelColor(i, strip.Color(255, 255, 0));
        strip.show();
      }
      if (myArray[i]==0){
        strip.setPixelColor(i, strip.Color(0, 0, 0));
        strip.show();
      }
      if ((myArray[i]==1)&&(i<16)){
        counter=counter+1;
      }
      if ((myArray[i]==1)&&(i>47)){
        counter=counter+1;
      }
    }
    if(counter==32){
      turn=1;
    }
  }
  if(turn!=0){
    if (turn==1){//Timer Setting
      if(analogRead(A0)>23){
        timerdurationW  = map(analogRead(A0), 23, 1023, 1, 30); 
        timerdurationB =timerdurationW;
        timeron=1;
        displayTime(timerdurationW);
      }
      if(analogRead(A0)<=23){
        timerdurationW=65;
        timerdurationB=65;
        timeron=0;
        displayTime(0);
      }
    }
    if(timeron==1){//Timer Based Victory
      if(SecondsW<0){//Timer Based White Victory
        victoryB=1;
        timeron=0;
      }
      if(SecondsB<0){//Timer Based Black Victory
        victoryW=1;
        timeron=0;
      }
    }
    if((victoryB==0)&&(victoryW==0)){
      lightsoff();
      if (turn%2!=0){//White Pawn Function Calling
        Tracker[3] = KingFunct(Tracker[3]);
        if(onetime==1){
          checkW=0;
          Tracker[20] = KingFunct(Tracker[20]);
          /*
            for(int i=0;i<=7;i++){//Pawn Section
            if (promB[i]==5){//General Case
            promB[i]=0;     
            }
            if (promB[i]==0){//General Case
            Tracker[i+24]=PawnFunct(Tracker[i+24]);    
            }
            if (promB[i]==1){//Rook Promotion Case
            Tracker[i+24]=RookFunct(Tracker[i+24]);    
            }
            if (promB[i]==2){//Bishop Promotion Case
            Tracker[i+24]=BishopFunct(Tracker[i+24]);    
            }
            if (promB[i]==3){//Knight Promotion Case
            Tracker[i+24]=KnightFunct(Tracker[i+24]);    
            }
            if (promB[i]==4){//Queen Promotion Case
            Tracker[i+24]=QueenFunct(Tracker[i+24]);    
            }   
            }
          */
          Tracker[16] = RookFunct(Tracker[16]);
          Tracker[23] = RookFunct(Tracker[23]);
          Tracker[17] = KnightFunct(Tracker[17]);
          Tracker[22] = KnightFunct(Tracker[22]);
          Tracker[18] = BishopFunct(Tracker[18]);
          Tracker[21] = BishopFunct(Tracker[21]);
          Tracker[19] = QueenFunct(Tracker[19]);  
          onetime=0;
        }   
        for(int i=0;i<=7;i++){//Pawn Section
          if (promW[i]==5){//General Case
            promW[i]=0;      
          }
          if (promW[i]==0){//General Case
            Tracker[i+8]=PawnFunct(Tracker[i+8]);    
          }
          if (promW[i]==1){//Rook Promotion Case
            Tracker[i+8]=RookFunct(Tracker[i+8]);    
          }
          if (promW[i]==2){//Bishop Promotion Case
            Tracker[i+8]=BishopFunct(Tracker[i+8]);    
          }
          if (promW[i]==3){//Knight Promotion Case
            Tracker[i+8]=KnightFunct(Tracker[i+8]);    
          }
          if (promW[i]==4){//Queen Promotion Case
            Tracker[i+8]=QueenFunct(Tracker[i+8]);    
          }
        }
        Tracker[0] = RookFunct(Tracker[0]);
        Tracker[7] = RookFunct(Tracker[7]);
        Tracker[1] = KnightFunct(Tracker[1]);
        Tracker[6] = KnightFunct(Tracker[6]);
        Tracker[2] = BishopFunct(Tracker[2]);
        Tracker[5] = BishopFunct(Tracker[5]);
        Tracker[4] = QueenFunct(Tracker[4]);
        Tracker[3] = KingFunct(Tracker[3]);
        if(checkW==0){
          strip.setBrightness(65);
        }
        if(checkW==1){
          strip.setBrightness(125);
          strip.setPixelColor(GlobalKingW, strip.Color(128, 0, 128));
          strip.show();
        }
      }
      if (turn%2==0){//Black Pawn Function Calling
        Tracker[20] = KingFunct(Tracker[20]);
        if(onetime==1){
          checkB=0;
          for(int i=0;i<=7;i++){//Pawn Section
            if (promW[i]==5){//General Case
              promW[i]=0;      
            }
            if (promW[i]==0){//General Case
              Tracker[i+8]=PawnFunct(Tracker[i+8]);    
            }
            if (promW[i]==1){//Rook Promotion Case
              Tracker[i+8]=RookFunct(Tracker[i+8]);    
            }
            if (promW[i]==2){//Bishop Promotion Case
              Tracker[i+8]=BishopFunct(Tracker[i+8]);    
            }
            if (promW[i]==3){//Knight Promotion Case
              Tracker[i+8]=KnightFunct(Tracker[i+8]);    
            }
            if (promW[i]==4){//Queen Promotion Case
            Tracker[i+8]=QueenFunct(Tracker[i+8]);    
            }
          }
          Tracker[0] = RookFunct(Tracker[0]);
          Tracker[7] = RookFunct(Tracker[7]);
          Tracker[1] = KnightFunct(Tracker[1]);
          Tracker[6] = KnightFunct(Tracker[6]);
          Tracker[2] = BishopFunct(Tracker[2]);
          Tracker[5] = BishopFunct(Tracker[5]);
          Tracker[4] = QueenFunct(Tracker[4]);
          Tracker[3] = KingFunct(Tracker[3]);
          onetime=0;
        }
        for(int i=0;i<=7;i++){//Pawn Section
          if (promB[i]==5){//General Case
            promB[i]=0;     
          }
          if (promB[i]==0){//General Case
            Tracker[i+24]=PawnFunct(Tracker[i+24]);    
          }
          if (promB[i]==1){//Rook Promotion Case
            Tracker[i+24]=RookFunct(Tracker[i+24]);    
          }
          if (promB[i]==2){//Bishop Promotion Case
            Tracker[i+24]=BishopFunct(Tracker[i+24]);    
          }
          if (promB[i]==3){//Knight Promotion Case
            Tracker[i+24]=KnightFunct(Tracker[i+24]);    
          }
          if (promB[i]==4){//Queen Promotion Case
            Tracker[i+24]=QueenFunct(Tracker[i+24]);    
          }   
        }
        Tracker[16] = RookFunct(Tracker[16]);
        Tracker[23] = RookFunct(Tracker[23]);
        Tracker[17] = KnightFunct(Tracker[17]);
        Tracker[22] = KnightFunct(Tracker[22]);
        Tracker[18] = BishopFunct(Tracker[18]);
        Tracker[21] = BishopFunct(Tracker[21]);
        Tracker[19] = QueenFunct(Tracker[19]);
        if(checkB==0){
          strip.setBrightness(65);
        }
        if(checkB==1){
          strip.setBrightness(125);
          strip.setPixelColor(GlobalKingB, strip.Color(128, 0, 128));
          strip.show();
        }

      }
    }
    if(Tracker[3]==65){
      victoryB=1;
    }
    if(Tracker[20]==65){
      victoryW=1;
    }
    if(victoryW==1){
      for(int i=0;i<=31;i++){
        strip.setPixelColor(i, strip.Color(0,127, 0));
        strip.show();
      }
      for(int i=32;i<=63;i++){
        strip.setPixelColor(i, strip.Color(127,0, 0));
        strip.show();
      }
    }
    if(victoryB==1){
      for(int i=0;i<=31;i++){
        strip.setPixelColor(i, strip.Color(127,0, 0));
        strip.show();
      }
      for(int i=32;i<=63;i++){
        strip.setPixelColor(i, strip.Color(0,127, 0));
        strip.show();
      }
    }
  }
}

ISR(TIMER1_COMPA_vect) {//Clock
  if((turn==1)||(turn==0)){
  SecondsW=timerdurationB*60;
  SecondsB=timerdurationW*60;
  }
  if(timeron==1){//Timer On Checker
    if(((turn!=1))&&(turn%2!=0)){//White Timer
      SecondsW-=1;
      displayTime(SecondsW);
    }
    if(turn%2==0){//Black Timer
      SecondsB-=1;
      displayTime(SecondsB);
    }
  }
}

void displayTime(long totalSeconds) {//LED Timer Disaply
  if(turn==0){
    display.clear();  
  }  
  if(turn!=0){  
  int minutes = totalSeconds / 60;
  int seconds = totalSeconds % 60;
  int displayValue = minutes * 100 + seconds; // Convert to MMSS format
  if (turn==1){
  displayValue=totalSeconds*100;
  }
  if(timeron==1){//Timer Display Case
    display.showNumberDecEx(displayValue, 0b01000000, false); // Display with colon
  }
  if((timeron==0)&&(turn==1)){//Timer No Display Case
    display.clear();  
  }
  }
}

void lightsoff(){
  for(int i=0;i<=63;i++){
  strip.setPixelColor(i,strip.Color(0, 0, 0));
  strip.show();
  }
}

byte readSPIRegister(int csPin, byte reg) {
  byte data;
  digitalWrite(csPin, LOW);
  SPI.transfer(0x41);  // Opcode for MCP23S17
  SPI.transfer(reg);
  data = SPI.transfer(0xFF);
  digitalWrite(csPin, HIGH);
  return data;
}
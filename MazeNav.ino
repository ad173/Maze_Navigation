#pragma once
#include "MyServos.h"
#include "MyEncoders.h"
#include "MySharpSensor.h"
#include <Adafruit_RGBLCDShield.h>
#include <QueueList.h>

enum Execute {MAP, GOAL};

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

#define WEST 0
#define NORTH 1
#define EAST 2
#define SOUTH 3

#define RED 0x1
#define BLUE 0x4
#define WHITE 0x7

//each cell will contain the following format:
// { (visited bit), (path to goal bit), (previous cell num), (west wall bit), (north wall bit), (east wall bit), (south wall bit)}
int maze[16][7] = {{0,0,0,1,1,0,0}, {0,0,0,0,1,0,0}, {0,0,0,0,1,0,0}, {0,0,0,0,1,1,0},    
                   {0,0,0,1,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,1,0},
                   {0,0,0,1,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,1,0},
                   {0,0,0,1,0,0,1}, {0,0,0,0,0,0,1}, {0,0,0,0,0,0,1}, {0,0,0,0,0,1,1}};
const char* erraseString  = "  ";

Execute e = (Execute)MAP;

int Min = 0;
int i = 0, j = 0, temp = 0;
float Distances[3] = {0, 0, 0};
int k, num_readings = 0;

bool Stop = true;

String Color = "";

float LSpeed = 0;
float RSpeed = 0;

int RGB[3] = {0,0,0};

uint8_t buttons;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

int grid = 0;
int start = -1;
int goal = -1;
int dir = -1;

int compass[4] = {0,0,0,0};

void setup() 
{
  // set color sensor 
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // set frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  servos.initServos();
  
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Start Locat.");
  lcd.setCursor(0, 1);
  lcd.print("1");
  
  servos.setSpeeds(0,0);
   
  i = 0;
  while(1)
  {
    buttons = lcd.readButtons();

    if(buttons & BUTTON_SELECT)
    {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0,0);
      if(start < 0)
      {
        start = i;
        lcd.print("Ending Locat.");
        lcd.setCursor(0,1);
        lcd.print("1");
        i = 0;
      }
      else if(goal < 0)
      {
        goal = i;
        lcd.print("Orientation");
        lcd.setCursor(0,1);
        lcd.print("West");
        i = 0;
      }
      else if(dir < 0)
      {
        dir = i;
        lcd.print("Exit");
        lcd.setCursor(0,1);
        lcd.print("            ");
      }
      else
      {
        break;
      }
      delay(100);
    }
    else if(buttons & BUTTON_UP)
    {
      i += 1;
      lcd.setCursor(0,1);
      lcd.print("           ");
      lcd.setCursor(0,1);  
      if(start < 0 || goal < 0)
      {
        if(i > 15)
        {
          i = 0;
        }
        lcd.print(i + 1);
      }
      else if(dir < 0)
      {
        if(i > 3)
        {
          i = 0;
        }
        switch(i)
        {
          case 0:
            lcd.print("West");
            break;
          case 1:
            lcd.print("North");
            break;
          case 2:
            lcd.print("East");
            break;
          case 3:
            lcd.print("South");
            break;
        }
      }
      delay(100);
    }
    else if(buttons & BUTTON_DOWN)
    {
      i -= 1;
      lcd.setCursor(0,1);
      lcd.print("           ");
      lcd.setCursor(0,1);  
      if(start < 0 || goal < 0)
      {
        if(i < 0)
        {
          i = 15;
        }
        lcd.print(i + 1);
      }
      else if(dir < 0)
      {
        if(i < 0)
        {
          i = 3;
        }
        switch(i)
        {
          case 0:
            lcd.print("West");
            break;
          case 1:
            lcd.print("North");
            break;
          case 2:
            lcd.print("East");
            break;
          case 3:
            lcd.print("South");
            break;
        }
      }
      delay(100);
    }
  }

  lcd.setCursor(0, 0);

  for(i = 0; i < 16; i++)
  {
    if(i != start)
    {
      maze[i][0] = 0;
      lcd.print("O");
    }
    else
    {
      grid = start;
      maze[i][0] = 1;
      lcd.print("X");
    }
  }
  lcd.setCursor(0, 1);
  lcd.print("G  O");
  switch(dir)
  {
    case WEST:
      lcd.print("W");
      break;
    case NORTH:
      lcd.print("N");
      break;
    case EAST:
      lcd.print("E");
      break;
    case SOUTH:
      lcd.print("S");
      break;
  }
  lcd.setCursor(1,1);
  lcd.print(grid+1);
  
  while(Stop)
  {
    buttons = lcd.readButtons();
    if(buttons && BUTTON_SELECT)
    {
      Stop = false;
      delay(100);
    }
    
    currentTime = millis();
    if(currentTime - lastTime >= 20)
    {
      updateSensors();
    }
    else if(currentTime < lastTime)
    {
      lastTime = 0;
    }
    
    if(num_readings == 0)
    {
      UpdateCell();
    }
  }
  DetermineDirection();
}

bool line = false;
bool finished = false;

void loop() 
{
  if(finished)
  {
    servos.setSpeeds(0);
    switch(e)
    {
      case MAP:
        int num = -1;
        while(num < 3)
        {
          for(i = 0; num >= 0 && i < 2; i++)
          {
            lcd.setCursor(0,i);
            for(k = 0; k < 4; k++)
            {
              for(x = 3; x < 7; x++)
              {
                if(maze[k + (i * 4) + (num * 8)][x])
                {
                   lcd.print("W");
                }
                else
                {
                  lcd.print("o");
                }
              } 
            }
          }
          
          if(num < 0)
          {
            lcd.setCursor(0,0);
            lcd.print("      DONE      ");
            lcd.setCursor(0,1);
            lcd.print(erraseString);
          }
          while(1)
          {
            buttons = lcd.readButtons();
            if(buttons && BUTTON_SELECT)
            {
              num++;
              delay(100);
              break;
            }
          }
        }
        
        finished = false;
        grid = start;
        for(i = 0; i < 16; i++)
        {
          if(i != grid)
          {
            maze[i][0] = 0;
          }
          else
          {
            maze[i][0] = 1;
          }
        }
        
        e = GOAL;
        FindPath(grid);

        lcd.setCursor(0, 0);
        for(i = 0; i < 16; i++)
        {
          if(i != grid)
          {
            maze[i][0] = 0;
            lcd.print("O");
          }
          else
          {
            maze[i][0] = 1;
            lcd.print("X");
          }
        }
        
        lcd.setCursor(0,1);
        lcd.print(erraseString);
        lcd.setCursor(0,1);
        lcd.print("G   O");
        switch(dir)
        {
          case WEST:
            lcd.print("W");
            break;
          case NORTH:
            lcd.print("N");
            break;
          case EAST:
            lcd.print("E");
            break;
          case SOUTH:
            lcd.print("S");
            break;
        }
        lcd.setCursor(1,1);
        lcd.print(grid + 1);
        
        while(1)
        {
          buttons = lcd.readButtons();
          if(buttons && BUTTON_SELECT)
          {
            delay(100);
            break;
          }
    
          currentTime = millis();
          if(currentTime - lastTime >= 20)
          {
            updateSensors();
          }
          else if(currentTime < lastTime)
          {
            lastTime = 0;
          }
    
          if(num_readings == 0)
          {
            UpdateCell();
          }
        }
        DetermineDirection();
        break;
        
      case GOAL:
        Stop = true;
        break; 
    }

    servos.setSpeeds(0,0);
    if(currentTime - lastTime >= 20)
    {
      updateSensors();
    }
    else if(currentTime < lastTime)
    {
      lastTime = 0;
    }
    if(num_readings == 0)
    {
      UpdateCell();
    }
  }

  if(currentTime - lastTime >= 20)
  {
    updateSensors();
  }
  else if(currentTime < lastTime)
  {
    lastTime = 0;
  }
  
  Movement();
  
  if(LSpeed > 100)
  {
    LSpeed = 100;
  }
  if(RSpeed > 100)
  {
    RSpeed = 100;
  }
  servos.setSpeeds(LSpeed, RSpeed);
  delay(10);
}

bool CheckForFinish()
{
  switch(e)
  {
    case MAP:
      for(i = 0; i < 16; i++)
      {
        if(maze[i][0] == 0)
        {
          return false;
        }
      }
      return true;
      
    case GOAL:
      if(grid != goal)
      {
        return false;
      }
      return true;
  }
}

void FindPath(int g)
{
  QueueList<int> q;
  maze[g][0] = 1;
  maze[g][1] = 1;
  maze[g][2] = -1;

  q.push(g);
  while(!q.isEmpty())
  {
    g = q.pop();
    maze[g][0] = 1;

    for(i = 0; i < 4; i++)
    {
      if(!maze[g][i + 3])
      {
        switch(i)
        {
          case 0:
            if(!maze[g-1][0])
            {
              q.push(g - 1);
              maze[g - 1][2] = g;
            }
            break;
          case 1:
            if(!maze[g - 4][0])
            {
              q.push(g - 4);
              maze[g - 4][2] = g;
            }
            break;
          case 2:
            if(!maze[g+1][0])
            {
              q.push(g+1);
              maze[g+1][2] = g;
            }
            break;
          case 3:
            if(!maze[g+4][0])
            {
              q.push(g+4);
              maze[g+4][2] = g;
            }
            break;
        }
      }
    }
  }

  switch(e)
  {
    case GOAL:
      g = goal;
      break;
  }
  
  while(maze[g][2] != -1)
  {
    maze[g][1] = 1;
    g = maze[g][2];
  }
  maze[g][1] = 1;
}

bool LRecent = false;
bool RRecent = false;
int counts[2] = {0,0};

void Movement()
{
  if(Stop)
  {
    LSpeed = 1500;
    RSpeed = 1500;
    UpdateCell();
  }
  else
  {
    if(Color != "Floor")
    {
      line = true;
      encoders.resetCounts();
    }
    if(Distances[0] < 4 && Distances[0] != 0 && !RRecent)
    {
      encoders.resetCounts();
      encoders.getCounts(counts);
      while(counts[0] < 2 || counts[1] < 2)
      {
        servos.setSpeeds(20,-20);
        encoders.getCounts(counts);
      }
      RRecent = true;
    }
    else if ((Distances[0] > 5 || Distances[0] == 0) && RRecent)
    {
      RRecent = false;
    }
    
    if(Distances[2] < 4 && Distances[2] != 0 && !LRecent)
    {
      encoders.resetCounts();
      encoders.getCounts(counts);
      while(counts[0] < 2 || counts[1] < 2)
      {
        servos.setSpeeds(-20,20);
        encoders.getCounts(counts);
      }
      LRecent = true;
    }
    else if((Distances[2] > 5 || Distances[2] == 0 )&& LRecent)
    {
      LRecent = false;
    }
    
    if(line)
    {
      encoders.resetCounts();
      encoders.getCounts(counts);
      if(counts[0] < 58 || counts[1] < 58)
      {
        servos.setSpeeds(20,20);
        if((Distances[0] < 6) || (Distances[0] == 10 && Distances[2] > 5))
        {
          LSpeed += 5;
          //LSpeed += 6 - Distances[0];
        }
        if((Distances[2] < 6) || (Distances[2] == 10 && Distances[0] > 5))
        {
          RSpeed -= 5;
          //RSpeed -= 6 - Distances[2];
        }
      }
      else
      {
        servos.setSpeeds(0,0);

        num_readings = 0;
        updateSensors();
        delay(100);
        while(num_readings > 0)
        {
          updateSensors();
          delay(100);
        }
        
        UpdateCell();
        
        if (CheckForFinish())
        {
          finished = true;
          line = false;
          return;
        }
        DetermineDirection(); 
        
        line = false;
      }
    }
    else
    {
      servos.setSpeeds(20,20);
      if((Distances[0] < 6)|| (Distances[0] == 10 && Distances[2] > 5))
      {
        LSpeed += 5;
        //LSpeed += 6 - Distances[0];
      }
      if((Distances[2] < 6)|| (Distances[2] == 10 && Distances[0] > 5))
      {
        RSpeed -= 5;
        //RSpeed -= 6 - Distances[2];
      }
    }
  }
}

void left(int num)
{
  for(i = 0; i < num; i++)
  {
    encoders.resetCounts();
    encoders.getCounts(counts);
    while(counts[0] < 25 || counts[1] < 25)
    {
      servos.setSpeeds(-20, 20);
      encoders.getCounts(counts);
    }
    
    dir--;
    if(dir < 0)
    {
      dir = 3;
    }
  }
}

void right(int num)
{
  for(i = 0; i < num; i++)
  {
    encoders.resetCounts();
    encoders.getCounts(counts);
    while(counts[0] < 25 || counts[1] < 25)
    {
      servos.setSpeeds(-20, 20);
      encoders.getCounts(counts);
    }
    
    dir++;
    if(dir > 3)
    {
      dir = 0;
    }
  }
}

int dirTesting;
void DetermineDirection()
{
  switch(e)
  {
    case MAP:
      dirTesting = dir - 1;
      if(dirTesting < 0)
      {
        dirTesting = 3;
      }
      for(i = 0; i < 4; i++)
      {
        if(compass[dirTesting] == 0 && !maze[grid][dirTesting + 3])
        {
          switch(dirTesting)
          {
            case WEST:
              if(!maze[grid - 1][0] && (grid % 4 != 0))
              {
                if(dir == 3)
                {
                  right(1);
                }
                else if(dir != dirTesting)
                {
                  left(dir - dirTesting);
                }
                return;
              }
              break;
            case NORTH:
              if(!maze[grid - 4][0] && grid / 4 != 0)
              {
                if(dir == 0)
                {
                  right(1);
                }
                else if(dir != dirTesting)
                {
                  left(dir - dirTesting);
                }
                return;
              }
              break;
            case EAST:
              if(!maze[grid + 1][0] && grid % 4 != 3)
              {
                if(dir == 3)
                {
                  left(1);
                }
                else if(dir != dirTesting)
                {
                  right(dirTesting - dir);
                }
                return;
              }
              break;
            case SOUTH:
              if(!maze[grid + 4][0] && grid / 4 != 3)
              {
                if(dir == 0)
                {
                  left(1);
                }
                else if(dir != dirTesting)
                {
                  right(dirTesting - dir);
                }
                return;
              }
              break;
          }
        }
        
        dirTesting++;
        if(dirTesting > 3)
        {
          dirTesting = 0;
        }
      }

      dirTesting = dir - 1;
      
      if(dirTesting < 0)
      {
        dirTesting = 3;
      }
      
      for(i = 0; i < 4; i++)
      {
        if(compass[dirTesting] == 0 && !maze[grid][dirTesting + 3])
        {
          switch(dirTesting)
          {
            case WEST:
              if(grid % 4 == 0)
              {
                break;
              }
              if(dir == 3)
              {
                right(1);
              }
              else if(dir != dirTesting)
              {
                left(dir - dirTesting);
              }
              return;
              break;
              
            case NORTH:
              if(grid / 4 == 0)
              {
                break;
              }
              
              if(dir < dirTesting)
              {
                right(dirTesting - dir);
              }
              else if(dir > dirTesting)
              {
                left(dir - dirTesting);
              }
              return;
              break;
              
            case EAST:
              if(grid % 4 == 3)
              {
                break;
              }
              
              if(dir < dirTesting)
              {
                right(dirTesting - dir);
              }
              else if(dir > dirTesting)
              {
                left(dir - dirTesting);
              }
              return;
              break;
              
            case SOUTH:
              if(grid / 4 == 3)
              { 
                break;
              }
              if(dir == 0)
              {
                left(1);
              }
              else if(dir != dirTesting)
              {
                right(dirTesting - dir);
              } 
              return;
              break;
          }
        }
        dirTesting++;
        if(dirTesting > 3)
        {
          dirTesting = 0;
        }
      }
      left(2);
      break;
      
    case GOAL:
      dirTesting = -1;
      for(i = 0; i < 4; i++)
      {
        if(!maze[grid][i + 3])
        {
          switch(i)
          {
            case WEST:
              if(maze[grid - 1][1] && !maze[grid - 1][0])
              {
                dirTesting = 0;
              }
              break;
            case NORTH:
              if(maze[grid - 4][1] && !maze[grid - 4][0])
              {
                dirTesting = 1;
              }
              break;
            case EAST:
              if(maze[grid + 1][1] && !maze[grid + 1][0])
              {
                dirTesting = 2;
              }
              break;
            case SOUTH:
              if(maze[grid + 4][1] && !maze[grid + 4][0])
              {
                dirTesting = 3;
              }
              break;
          }

          if(dirTesting != -1)
          {
            break;
          }
        }
      }
  
      switch(dirTesting)
      {
        case WEST:
          if(dir == 3)
          {
            right(1);
          }
          else if(dir != dirTesting)
          {
            left(dir - dirTesting);
          }
          break;
        case NORTH:
        case EAST:
          if(dir < dirTesting)
          {
            right(dirTesting - dir);
          }
          else if(dir > dirTesting)
          {
            left(dir - dirTesting);
          }
          break;
        case SOUTH:
          if(dir == 0)
          {
            left(1);
          }
          else if(dir != dirTesting)
          {
            right(dirTesting - dir);
          }
          break;
      }
      break;
    }
}

void UpdateCell()  //locality update
{
  dirTesting = dir - 1;
  if(dirTesting < 0)
  {
    dirTesting = 3;
  }
  
  for(i = 0; i < 3; i++)
  {
    if(Distances[i] != 0 && Distances[i] < 8)
    {
      compass[dirTesting] = 1;
      if(e == MAP)
      {
        maze[grid][dirTesting + 3] = 1;
        switch(dirTesting)
        {
          case WEST:
            if(grid % 4 != 0)
            {
              maze[grid - 1][5] = 1;
            }
            break;
          case NORTH:
            if(grid / 4 != 0)
            {
              maze[grid - 4][6] = 1;
            }
            break;
          case EAST:
            if(grid % 4 != 3)
            {
              maze[grid + 1][3] = 1;
            }
            break;
          case SOUTH:
            if(grid / 4 != 3)
            {
              maze[grid + 4][4] = 1;
            }
            break;
        }
      }
    }
    else
    {
      compass[dirTesting] = 0;
    }
    
    dirTesting++;
    if(dirTesting > 3)
    {
      dirTesting = 0;
    }
  }

  compass[dirTesting] = -1;
  if(e == MAP)
  {
    maze[grid][dirTesting + 3] = 0;
  }
}

void updateSensors()
{
  if(num_readings < 10)
  {
    lastTime = millis();
    for(k = 0; k < 3; k++)
    {
      sensors.takeNewMeasurement(k, num_readings);
    }
    num_readings++;
  }
  else
  {
    num_readings = 0;
    for(k = 0; k < 3; k++)
    {
      dist[k] = sensors.getCombinedDistance(k);
      if(dist[k] > 10)
      {
        dist[k] = 10;
      }
    }
  }
  // get red filtered photodiodes 
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  RGB[0] = pulseIn(sensorOut, LOW);
  
  // get Green filtered photodiodes
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  RGB[1] = pulseIn(sensorOut, LOW);
  
  // get Blue filtered photodiodes
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  RGB[2] = pulseIn(sensorOut, LOW);

  Color = color();

  switch(Color)
  {
    case "Red":
      lcd.setBacklight(RED);
      break;
    case "Blue":
      lcd.setBacklight(BLUE);
      break;
    case "Floor":
      lcd.setBacklight(WHITE);
      break;
  }
}

String color()
{
  if ((RGB[0] <= 150)) {
    // red
    if(Color == "Floor")
    {
      if(dir == 1)
      {
        grid -= 4;
      }
      else if (dir == 3)
      {
        grid += 4;
      }
      maze[grid][0] = 1;
      lcd.setCursor(grid,0);
      lcd.print("X");
      lcd.setCursor(1, 1);
      lcd.print("  ");
      lcd.setCursor(1, 1);
      lcd.print(grid + 1);
    }
    return "Red";
  } else if (RGB[1] <= 150) {
    // blue
    if(Color == "Floor")
    {
      if(dir == 0)
      {
        grid -= 1;
      }
      else if(dir == 2)
      {
        grid += 1;
      }
      maze[grid][0] = 1;
      lcd.setCursor(grid,0);
      lcd.print("X");
      lcd.setCursor(1, 1);
      lcd.print("  ");
      lcd.setCursor(1, 1);
      lcd.print(grid + 1);
    }
    return "Blue";
  } else {
    // floor
    return "Floor";
  }
}

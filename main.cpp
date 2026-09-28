#include <iostream>
#include <windows.h>
#include <chrono>
#include <math.h>

using namespace std;

class Vector2f
{
    public:
    float x;
    float y;
    Vector2f(float x_in = 0 , float y_in = 0)
    {
        x = x_in;
        y = y_in;
    };
    Vector2f operator!=(const Vector2f& other) const
    {
        Vector2f result;
        
        result.x = this->x != other.x;
        result.y = this->y != other.y;
        return result;
    } 
    Vector2f operator==(const Vector2f& other) const
    {
        Vector2f result;
        
        result.x = this->x == other.x;
        result.y = this->y == other.y;
        return result;
    } 
    Vector2f operator+(const Vector2f& other) const
    {
        Vector2f result;
        
        result.x = this->x + other.x;
        result.y = this->y + other.y;
        return result;
    }
    Vector2f operator-(const Vector2f& other) const
    {
        Vector2f result;
        
        result.x = this->x - other.x;
        result.y = this->y - other.y;
        return result;
    } 
};

class Player
{
    public:
        //in degrees
        float fov = 45.0;
        float current_rotation = 0;
        //end

        float max = 0;
        bool Change_max = false;
        Vector2f player_pos = Vector2f(0, 0);
        Vector2f ray_direction = Vector2f(0, 0);
        float vectorlength = 30;
        float health = 100;
        float ammo = 250;
};
//screen vars
int screen_width = 120;
int screen_hight = 80;
const  float PI = 3.14159265f;
const float DEGREECONV =  (PI / 180);
//map vars
int map_width = 88;
int map_hight = 25;
string map;
string postion;
CHAR_INFO *screen = new CHAR_INFO [screen_hight * screen_width];

Vector2f FindPlayerLocationOnMap(string map)
{
    for(int y = 0; y < map_hight; y++)
        for(int x = 0; x < map_width; x++)
            if(map[x + y * map_width] == 'S') 
                return Vector2f(x, y);
    return Vector2f(0,0);
}
Vector2f line_algorthim(Vector2f& p1, Vector2f p2, int&  current_increment, float& maximum, bool& MaximumBool)
{

    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    if(MaximumBool)
    {
	    maximum = max(abs(dx), abs(dy));
        MaximumBool = false;
    }

    float increment_x = dx / maximum;
    
    float increment_y = dy / maximum;
    
    Vector2f ReturnVec;
    if(current_increment <= maximum)
    {
        ReturnVec.x = (p1.x + (increment_x * current_increment));
        ReturnVec.y = (p1.y + (increment_y * current_increment));
    }
    return ReturnVec;
}
void render(int CurrentColumn, float distance)
{
    CHAR_INFO GroundChar;
    CHAR_INFO  SkyChar;
    CHAR_INFO WallChar;
    SkyChar.Char.UnicodeChar = ' ';
    GroundChar.Char.UnicodeChar = '.';
    SkyChar.Attributes = BACKGROUND_BLUE;
    WallChar.Attributes = BACKGROUND_RED;
    GroundChar.Attributes = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN;
    float depth = 16.0f;
    if(distance <= depth / 4.0f)
    {
        GroundChar.Char.UnicodeChar = L' ';
        WallChar.Char.UnicodeChar = ' ';
    }
    else if(distance < depth / 3.0f)
    {
        WallChar.Char.UnicodeChar = 0x2591;
    }
    else if(distance < depth / 2.0f)
    {
        WallChar.Char.UnicodeChar = 0x2592;
    }
    else
    {
        WallChar.Char.UnicodeChar = 0x2593;
    }
    int sky_half = (float)(screen_hight / 2.0) - (float)(screen_hight / distance);
    int ground = screen_hight - sky_half;
    for(int i = 0; i < screen_hight; i++)
    {

        if(i < sky_half)
        {
            screen[CurrentColumn + (i * screen_width)] = SkyChar;
        }
        else if(i <= ground && i > sky_half)
        {
            screen[CurrentColumn + (i * screen_width)] = WallChar;
        }
        else if (i > ground)
        {
            screen[CurrentColumn + (i * screen_width)] = GroundChar;
        }

    }

}

void raycast_calc_Render(Player player)
{
    float n = player.fov / screen_width;

    for(int c = 0; c < screen_width; c++)
    {
        player.Change_max = true;
        float CurrentRotatedFovSegment = (c * n) + player.current_rotation;

        float CurrentRotatedFovSegment_radian = CurrentRotatedFovSegment * DEGREECONV;

        Vector2f RaySegment = Vector2f(cosf(CurrentRotatedFovSegment_radian) * player.vectorlength, sinf(CurrentRotatedFovSegment_radian) * player.vectorlength);
        RaySegment = RaySegment + player.player_pos;
        int i = 0;
        Vector2f currentcell = line_algorthim(player.player_pos, RaySegment, i, player.max, player.Change_max);
        while(i <= player.max)
        {
            currentcell = line_algorthim(player.player_pos, RaySegment, i, player.max, player.Change_max);
            if( currentcell.y < map_hight && currentcell.x < map_width && currentcell.x > 0 &&  currentcell.y > 0)
            {
                if(map[(int)((currentcell.x) + (int)(currentcell.y) * map_width)] == 'X' || i >= 11)
                {
                    render(c , i);
                    break;
                }
            }
            i++;
        }
    }
}
void minimap(string &minimap, Player player)
{
    for(int x = 0; x < map_width; x++)
    {
        for(int y = 0; y < map_hight; y++)
        {
            minimap[(int)(player.player_pos.x) + (int)(player.player_pos.y) * map_width] = 'p';
            screen[x + screen_width * y].Char.UnicodeChar = minimap[x + map_width * y];
            screen[x + screen_width * y].Attributes = BACKGROUND_GREEN; 
        }
    }
}
void resetingmap(string map, string &newmap)
{
    for(int x = 0; x < map_width; x++)
    {
        for(int y = 0; y < map_hight; y++)
        {
            newmap[x + map_width * y] = map[x + map_width * y];
        }
    }
}
void refresh()
{
    for(int h = 0; h < screen_width * screen_hight; h++)
    {
            screen[h].Char.UnicodeChar = L' ';
            screen[h].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
    }
}
void inputhandling (Player &player)
{
    const float SPEED = 0.009;
    float rotation_radian = (player.current_rotation) * DEGREECONV;
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        player.player_pos.y += sinf(rotation_radian) * SPEED;
        player.player_pos.x += cosf(rotation_radian) * SPEED;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        player.player_pos.y -= sinf(rotation_radian) * SPEED;
        player.player_pos.x -= cosf(rotation_radian) * SPEED;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 1)
    {
        player.current_rotation++;
    }
    else if (GetAsyncKeyState(VK_LEFT) & 1)
    {
        --player.current_rotation;
    }

}
int main()
{
    HANDLE hBuffer = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    
    SetConsoleActiveScreenBuffer(hBuffer);
    DWORD bytesWritten = 0;
    Player player;
    map += "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    map += "X                                                                                      X";
    map += "X                                                                                      X";
    map += "X   S                                                                                  X";
    map += "X                                                                                      X";
    map += "X                                                                                      X";
    map += "X                                                               XXXXXXXXXXXX           X";
    map += "X                                                               X          X           X";
    map += "X                                                               X          X           X";
    map += "X                                                               X          X           X";
    map += "X                                                               X          X           X";
    map += "X   XXXXXXXXX                                                   X          X           X";
    map += "X   X       X                                                   X          X           X";
    map += "X   X       X                                                   X          X           X";
    map += "X   X       X               XXXXXXXXXXXXXXXXXXXXXXX             XXXXXXXXXXXX           X";
    map += "X   X       X               X                     X                                    X";
    map += "X   XXXXXXXXX               X                     X                                    X";
    map += "X                           X                     X                                    X";
    map += "X                           X                     X                                    X";
    map += "X                           X                     X                                    X";
    map += "X                           X                     X                                    X";
    map += "X                           X                     X                                    X";
    map += "X                           XXXXXXXXXXXXXXXXXXXXXXX                                    X";
    map += "X                                                                                      X";
    map += "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

    string playerMap = map;
    player.player_pos = FindPlayerLocationOnMap(map);

    refresh();
    while (1)
    {
        inputhandling(player);
        raycast_calc_Render(player);
        minimap(playerMap, player);
        SMALL_RECT writeRegion = 
        {
            0,
            0,
            (short)(screen_width - 1),
            (short)(screen_hight - 1)
        };

        WriteConsoleOutputW(hBuffer, screen, {(short)screen_width , (short)screen_hight}, {0,0}, &writeRegion);
        resetingmap(map, playerMap);
        refresh();
    }
}

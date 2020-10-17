#include "..\..\Public\View\ConsoleRender.h"
#include <fstream>
#include <string>
#include <Windows.h>

#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

using namespace std;

void IConsoleRender::LoadMap(MapData LoadedMap)
{
    //Populate whole map with walkable tiles to init size
    for (int i = 0; i <= LoadedMap.mapsize.X; i++)
    {
        Map2D newRow;
        for (int j = 0; j <= LoadedMap.mapsize.Y; j++)
        {
            newRow.Add(' ');
        }
        map.push_back(newRow);
    }

    //Set console window size based on the map size
    SetWindowSize(LoadedMap.mapsize.X, LoadedMap.mapsize.Y);
    //Draw straight lines
    LineDraw(LoadedMap.StraightLines);
    //Draw diagonal lines
    DiagDraw(LoadedMap.DiagonalLines);
}

void IConsoleRender::BeginPlay(MapData LoadedMap)
{
    //Clear map and screen to be safe everything will be drawn from scratch
    map.clear();
    system("cls");

    //Load map information into the array
    LoadMap(LoadedMap);

    //Draw legend and controls
    DrawLegend();
}

//Character movement function 
bool IConsoleRender::UpdateMap(int X, int Y, int MoveX, int MoveY, char UpdateChar)
{
    //Check if tile is walkable - If player is checking allow him to step on items
    if (map.at(Y + MoveY)[X + MoveX] == ' ' || (map.at(Y + MoveY)[X + MoveX] == ItemChar && UpdateChar == PlayerChar))
    {
        //Clear previously held tile
        MoveCursor(X, Y);
        map.at(Y).SetChar(X, ' ');
        cout << ' ';

        //Draw character on the new tile
        MoveCursor(X + MoveX, Y + MoveY);
        map.at(Y + MoveY).SetChar(X + MoveX, UpdateChar);
        cout << UpdateChar;

        return true;
    }
    return false;
}

//Vision check for enemy pawns
bool IConsoleRender::CheckTile(int X, int Y)
{
    if (map.at(Y)[X] == ' ' || map.at(Y)[X] == PlayerChar || map.at(Y)[X] == EnemyChar)
    {
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

//Check if given character is present on the tile
bool IConsoleRender::CheckForCharacter(int X, int Y, char CharToCheck)
{
    if (map.at(Y)[X] == CharToCheck)
    {
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

//Redraw the whole map from array for UI switching
void IConsoleRender::DrawMapFromArray()
{
    system("cls");
    //Loop through 2D map array and redraw map
    for (int i = 0; i < map.size(); i++)
    {
        for (int j = 0; j < map.at(i).GetSize(); j++)
        {
            MoveCursor(j, i);
            //Change color based on the char
            switch (map.at(i)[j])
            {
            case '#':
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                break;
            case '^':
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
                break;
            case '=':
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
                break;
            case PlayerChar:
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PlayerColor);
                break;
            case EnemyChar:
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), EnemyColor);
                break;
            case ItemChar:
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ItemColor);
                break;
            }
            cout << map.at(i)[j];
        }
    }
    //Redraw legend
    DrawLegend();
}

//Clear player and enemy chars from array for quick load
void IConsoleRender::ClearCharactersFromArray()
{
    for (int i = 0; i < map.size(); i++)
    {
        for (int j = 0; j < map.at(i).GetSize(); j++)
        {
            if (map.at(i)[j] == PlayerChar || map.at(i)[j] == EnemyChar)
            {
                MoveCursor(j, i);
                cout << ' ';
                map.at(i).SetChar(j, ' ');
            }
        }
    }
}

//Draw in game UI with space for enemy/player information, legend and controls
void IConsoleRender::DrawLegend()
{
    int startX = 1;
    int startY = map.size() + 1;
    for (int i = 0; i < map.size(); i++)
    {
        if (map.at(i).GetSize() > startX)
        {
            startX = map.at(i).GetSize();
        }
    }
    startX += 5;
    //Draw right side legend and controls UI
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    MoveCursor(startX - 5, 0);
    cout << "--------------------------";
    MoveCursor(startX, 1);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "     LEGEND ";

    MoveCursor(startX, 3);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PlayerColor);
    cout << PlayerChar <<" -> PLAYER";

    MoveCursor(startX, 5);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PlayerAttackColor);
    cout << PlayerChar << " -> PLAYER ATTACK MODE";

    MoveCursor(startX, 7);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PlayerFortifyColor);
    cout << PlayerChar << " -> PLAYER FORTIFY MODE";

    MoveCursor(startX, 9);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), EnemyColor);
    cout << EnemyChar << " -> ENEMY";

    MoveCursor(startX, 11);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), EnemyFortifyColor);
    cout << EnemyChar << " -> ENEMY FORTIFY MODE";

    MoveCursor(startX, 13);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "# ";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    cout << "= ";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
    cout << "^ ";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "-> Impassable Terrain";

    MoveCursor(startX, 15);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ItemColor);
    cout << ItemChar << " -> Item";

    MoveCursor(startX - 5, 16);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    cout << "--------------------------";

    MoveCursor(startX, 17);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "     CONTROLS ";

    MoveCursor(startX, 19);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    cout << "WASD/Arrows -> Move/Attack Direction";

    MoveCursor(startX, 21);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    cout << "Space -> Attack Mode";

    MoveCursor(startX, 23);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PlayerFortifyColor);
    cout << "F -> Fortify Mode";

    MoveCursor(startX, 25);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "I -> Character Screen";

    MoveCursor(startX, 27);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "X -> End Turn";

    MoveCursor(startX, 29);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "ESC -> Open Menu";

    MoveCursor(startX, 31);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "K -> Quick Save";

    MoveCursor(startX, 333);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "L -> Quick Load";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    for (int i = 0; i <= 9; i++)
    {
        MoveCursor(60, startY + i);
        cout << "|";
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    for (int i = 0; i <= 9; i++)
    {
        MoveCursor(30, startY + i);
        cout << "|";
    }

}

//Move cursor to the given location on the console
void IConsoleRender::MoveCursor(int X, int Y)
{
    COORD new_coord;
    new_coord.X = X;
    new_coord.Y = Y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), new_coord);
}


//Resize the console window
void IConsoleRender::SetWindowSize(int SizeX, int SizeY)
{
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, SizeY * 14, SizeX * 21, TRUE);
}

//Draw straight line from the map data
void IConsoleRender::LineDraw(vector<MapLine> StraightLines)
{
    for (MapLine line : StraightLines)
    {
        //Switch the color and char based on the type of the line
        char CharToWrite = '#';
        if (line.type == "border")
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            CharToWrite = '#';
        }
        else if (line.type == "mountain")
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            CharToWrite = '^';
        }
        else if (line.type == "river")
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
            CharToWrite = '=';
        }

        //Draw based on direction - straight lines are always drawn in positive direction, no need to check
        //Horizontal line
        if (line.start.X == line.end.X)
        {
            for (int i = line.start.Y; i <= line.end.Y; i++)
            {
                map.at(line.start.X).SetChar(i, CharToWrite);
                MoveCursor(i, line.start.X);
                cout << CharToWrite;
            }
        }
        //Vertical line
        else if (line.start.Y == line.end.Y)
        {
            for (int i = line.start.X; i <= line.end.X; i++)
            {
                map.at(i).SetChar(line.start.Y, CharToWrite);
                MoveCursor(line.start.Y, i);
                cout << CharToWrite;
            }
        }
    }
}

//Drawing of diagonal lines
void IConsoleRender::DiagDraw(vector<MapLine> DiagonalLines)
{
    for (MapLine line : DiagonalLines)
    {
        //Switch the color and char based on the type of the line
        char CharToWrite = '#';
        if (line.type == "border")
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            CharToWrite = '#';
        }
        else if (line.type == "mountain")
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
            CharToWrite = '^';
        }
        else if (line.type == "river")
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
            CharToWrite = '=';
        }

        //Draw based on direction - diagonal lines can got in all four ways
        int up = 0;
        int right = 0;

        //Check if line is going upwards or downards
        if (line.start.X > line.end.X)
        {
            up = -1;
        }
        else
        {
            up = 1;
        }
        //Check if line is going to the right or left
        if (line.start.Y > line.end.Y)
        {
            right = -1;
        }
        else
        {
            right = 1;
        }
        //Itereate through the line length based on directions and draw - Y and X lenght is always the same for diagonal lines
        for (int i = 0; i <= abs(line.start.X - line.end.X); i++)
        {
            map.at(line.start.X + i * up).SetChar(line.start.Y + i * right, CharToWrite);
            MoveCursor(line.start.Y + i * right, line.start.X + i * up);
            cout << CharToWrite;
        }
    }
}

//Draw UI information about the player or nearby enemies
void IConsoleRender::DrawCharacterUI(int StartX, int StartY, CharacterData CharacterInfo, bool Enemy, string Direction)
{
    MoveCursor(StartX, StartY);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    if (Enemy)
    {
        cout << "Enemy lvl" << CharacterInfo.level << "    " << Direction; 
    }
    else
    {
        cout << "Player lvl" << CharacterInfo.level << "    " << Direction;
    }

    int CurrentHealth = CharacterInfo.CurrentHealth;
    int MaxHealth = CharacterInfo.MaxHealth;

    //Health bar drawing
    MoveCursor(StartX, StartY + 1);
    cout << "|";

    int BarSize = int(ceil((float(CurrentHealth) / float(MaxHealth)) * 10));
    for (int i = 1; i <= BarSize; i++)
    {
        MoveCursor(StartX + i, StartY + 1);
        cout << "#";
    }
    MoveCursor(StartX + 11, StartY + 1);
    cout << "| " << CurrentHealth << "/" << MaxHealth << "HP";
}

//Remove UI information about the player or nearby enemies
void IConsoleRender::ClearCharacterUI(int StartX, int StartY)
{
    MoveCursor(StartX, StartY);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    for (int i = 0; i < 25; i++)
    {
        MoveCursor(StartX + i, StartY);
        cout << " ";
        MoveCursor(StartX + i, StartY + 1);
        cout << " ";
    }
}

//Clear the tile at the given coordinates
void IConsoleRender::ClearTile(int X, int Y)
{
    MoveCursor(X, Y);
    map.at(Y).SetChar(X, ' ');
    cout << ' ';
}

//Draw item char on the given coordinates
void IConsoleRender::SpawnItem(int X, int Y)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ItemColor);
    MoveCursor(X, Y);
    map.at(Y).SetChar(X, ItemChar);
    cout << ItemChar;
}

//Change the pawn character color at the given coordinates
void IConsoleRender::ChangePawnColor(Vec2D Position, int Color, char PawnChar)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color);
    MoveCursor(Position.X, Position.Y);
    map.at(Position.Y).SetChar(Position.X, PawnChar);
    cout << PawnChar;
}

//Draw player inventory information
void IConsoleRender::DrawInventory(CharacterData PawnData)
{
    //Item info array for drawing item comparison
    vector<string> ItemInfo = { "Damage: ", "Health: " , "Armor: ", "--------------", "Strength: ", "Stamina: ", "Dexterity: ", "Level: " };

    uint i = 0;
    uint j = 0;
    uint startX = 1;
    uint startY = 0;
    MoveCursor(startX, 25);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);

    //Inventory grid drawing
    for (uint j = 0; j <= 12; j += 3)
    {
        for (uint i = 0; i <= 8; i++)
        {
            MoveCursor(startX + j, startY + i);
            cout << "|";
        }
    }
    for (uint j = 0; j <= 9; j += 2)
    {
        for (uint i = 0; i <= 12; i++)
        {
            MoveCursor(startX + i, startY + j);
            cout << "-";
        }
    }

    //Items info boxes drawing
    for (uint j = 0; j < 31; j += 15)
    {
        for (uint i = 0; i <= 8; i++)
        {
            MoveCursor(startX + j, startY + i + 11);
            cout << "|";
        }
    }
    for (uint j = 11; j < 21; j += 9)
    {
        for (uint i = 0; i <= 30; i++)
        {
            MoveCursor(startX + i, startY + j);
            cout << "-";
        }
    }

    //Item info text drawing
    MoveCursor(startX + 1, startY + 10);
    cout << "Item comparison:";

    i = j = 0;
    for (string Info : ItemInfo)
    {
        MoveCursor(startX + 1, startY + 12 + i);
        cout << Info;
        i++;
    }

    i = j = 0;
    for (string Info : ItemInfo)
    {
        MoveCursor(startX + 16, startY + 12 + i);
        cout << Info;
        i++;
    }

    //Segments titles drawing
    MoveCursor(startX + 1, startY + 11);
    cout << "Inventory";
    MoveCursor(startX + 16, startY + 11);
    cout << "Equipped";

    //Equipment list drawing with information about slot being equiped or not
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY+1);
    cout << "Helmet:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Helmet));
    if (PawnData.Equipment.HelmetEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY+3);
    cout << "Weapon:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Weapon));
    if (PawnData.Equipment.WeaponEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY+5);
    cout << "Armor:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Armor));
    if (PawnData.Equipment.ArmorEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY+7);
    cout << "Shield:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Shield));
    if (PawnData.Equipment.ShieldEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY+9);
    cout << "Boots:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Boots));
    if (PawnData.Equipment.BootsEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);

    //Draw icons for each item in the inventroy with characters based on item type and color based on item rarity
    i = j = 0;
    for (ItemData Item : PawnData.Inventory)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(Item));
        if (i > 3)
        {
            i = 0;
            j++;
        }
        MoveCursor(startX + i * 3 + 1, startY + j * 2 + 1);
        if (Item.type == "armor") cout << "A";
        else if (Item.type == "weapon") cout << "W";
        else if (Item.type == "shield") cout << "S";
        else if (Item.type == "boots") cout << "B";
        else if (Item.type == "helmet") cout << "H";
        i++;
    }

    //Draw right side legend and controls UI
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    MoveCursor(startX + 35, 11);
    cout << "--------------------------";
    MoveCursor(startX + 37, 12);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "     CONTROLS";

    MoveCursor(startX + 37, 14);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    cout << "WASD/Arrows -> Navigate";

    MoveCursor(startX + 37, 16);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    cout << "Space -> Equip / Add Skill Point";

    MoveCursor(startX + 37, 18);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "I -> Switch Inventory / Character Screen";

    MoveCursor(startX + 37, 20);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "ESC -> Return to Game";

    MoveCursor(startX + 37, 22);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "Q -> Delete Item";


    //Draw left side legend for item rarity color information
    MoveCursor(startX + 2, 21);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    cout << "  ITEM RARITY LEGEND";

    MoveCursor(startX + 2, 23);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "Commmon";
    MoveCursor(startX + 2, 25);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
    cout << "Uncommon";
    MoveCursor(startX + 2, 27);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    cout << "Rare";
    MoveCursor(startX + 2, 29);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 5);
    cout << "Epic";
    MoveCursor(startX + 2, 31);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    cout << "Legendary";

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    MoveInventoryCursor(1, 1, 0, 0, PawnData);
}

//2D navigation through inventory grid
void IConsoleRender::MoveInventoryCursor(int X, int Y, int MoveX, int MoveY, CharacterData PawnData)
{
    uint startX = 1;
    uint startY = 0;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    //Clear previously held tile
    MoveCursor(X * 3, Y * 2 - 1);
    cout << ' ';

    //Draw character on the new tile
    MoveCursor((X + MoveX) * 3, (Y + MoveY) * 2 - 1);
    cout << '<';

    //Clear previous item comparison info
    ClearItemInfo();

    //Draw currently hovered item info and equiped item of the same type if slot is filled
    if ((X + MoveX - 1) + (Y + MoveY - 1) * 4 < PawnData.Inventory.size())
    {
        ItemData SelectedItem = PawnData.Inventory.at((X + MoveX - 1) + (Y + MoveY - 1) * 4);
        DrawItemInfo(SelectedItem, false);
        if (SelectedItem.type == "armor")
        {
            if (PawnData.Equipment.ArmorEquiped)
            {
                DrawItemInfo(PawnData.Equipment.Armor, true);
            }
        }
        else if (SelectedItem.type == "weapon")
        {
            if (PawnData.Equipment.WeaponEquiped)
            {
                DrawItemInfo(PawnData.Equipment.Weapon, true);
            }
        }
        else if (SelectedItem.type == "helmet")
        {
            if (PawnData.Equipment.HelmetEquiped)
            {
                DrawItemInfo(PawnData.Equipment.Helmet, true);
            }
        }
        else if (SelectedItem.type == "boots")
        {
            if (PawnData.Equipment.BootsEquiped)
            {
                DrawItemInfo(PawnData.Equipment.Boots, true);
            }
        }
        else if (SelectedItem.type == "shield")
        {
            if (PawnData.Equipment.ShieldEquiped)
            {
                DrawItemInfo(PawnData.Equipment.Shield, true);
            }
        }
    }
}

//Function from drawing item informations to their corresponding place
void IConsoleRender::DrawItemInfo(ItemData Item, bool Equiped)
{
    uint startX = 2;
    if (Equiped) startX = 17;
    uint startY = 12;
   
    MoveCursor(startX + 8, startY);
    cout << Item.AddDamage;
    MoveCursor(startX + 8, startY + 1);
    cout << Item.AddHealth;
    MoveCursor(startX + 7, startY + 2);
    cout << Item.AddArmor;
    MoveCursor(startX + 10, startY + 4);
    cout << Item.ReqStrength;
    MoveCursor(startX + 9, startY + 5);
    cout << Item.ReqStamina;
    MoveCursor(startX + 11, startY + 6);
    cout << Item.ReqDexterity;
    MoveCursor(startX + 7, startY + 7);
    cout << Item.ReqLevel;
}

//Update inventory and equipment UI if item is switched/equiped
void IConsoleRender::SwitchedItems(int X, int Y, CharacterData PawnData)
{
    //Update equipment status
    uint startX = 1;
    uint startY = 0;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY + 1);
    cout << "Helmet:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Helmet));
    if (PawnData.Equipment.HelmetEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY + 3);
    cout << "Weapon:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Weapon));
    if (PawnData.Equipment.WeaponEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY + 5);
    cout << "Armor:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Armor));
    if (PawnData.Equipment.ArmorEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY + 7);
    cout << "Shield:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Shield));
    if (PawnData.Equipment.ShieldEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    MoveCursor(startX + 15, startY + 9);
    cout << "Boots:  ";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(PawnData.Equipment.Boots));
    if (PawnData.Equipment.BootsEquiped) cout << "Equiped";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);

    for (uint j = 0; j < 4; j++)
    {
        for (uint i = 0; i < 4; i++)
        {
            MoveCursor(startX + i * 3 + 1, startY + j * 2 + 1);
            cout << ' ';
        }
    }

    uint i, j;
    i = j = 0;
    for (ItemData Item : PawnData.Inventory)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GetRarityColor(Item));
        if (i > 3)
        {
            i = 0;
            j++;
        }
        MoveCursor(startX + i * 3 + 1, startY + j * 2 + 1);
        if (Item.type == "armor") cout << "A";
        else if (Item.type == "weapon") cout << "W";
        else if (Item.type == "shield") cout << "S";
        else if (Item.type == "boots") cout << "B";
        else if (Item.type == "helmet") cout << "H";
        i++;
    }
}

//Clear item information for comparison
void IConsoleRender::ClearItemInfo()
{
    uint startX = 2;
    uint startY = 12;
    MoveCursor(startX + 8, startY);
    cout << "   ";
    MoveCursor(startX + 8, startY + 1);
    cout << "   ";
    MoveCursor(startX + 7, startY + 2);
    cout << "   ";
    MoveCursor(startX + 10, startY + 4);
    cout << "   ";
    MoveCursor(startX + 9, startY + 5);
    cout << "   ";
    MoveCursor(startX + 11, startY + 6);
    cout << "   ";
    MoveCursor(startX + 7, startY + 7);
    cout << "   ";

    startX = 17;
    MoveCursor(startX + 8, startY);
    cout << "   ";
    MoveCursor(startX + 8, startY + 1);
    cout << "   ";
    MoveCursor(startX + 7, startY + 2);
    cout << "   ";
    MoveCursor(startX + 10, startY + 4);
    cout << "   ";
    MoveCursor(startX + 9, startY + 5);
    cout << "   ";
    MoveCursor(startX + 11, startY + 6);
    cout << "   ";
    MoveCursor(startX + 7, startY + 7);
    cout << "   ";
}

//Draw information about player character skills and stats
void IConsoleRender::DrawCharacterStats()
{
    system("cls");
    //Charcater skill vector
    vector<string> traits = { "Level: ", "Strength: ", "Stamina: ", "Dexterity: ", "Skill Points: " };
    //Character stats vector
    vector<string> stats = { "Damage: ", "Armor: " , "Health: ", "Experience: "};
    uint i = 0;
    uint j = 0;
    uint startX = 40;
    uint startY = 1;
    //Title drawing
    MoveCursor(startX, 25);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
    
   
    //Draw character skills slots
    MoveCursor(startX, startY - 1);
    cout << "Details";
    i = j = 0;
    for (string trait : traits)
    {
        MoveCursor(startX + 1, startY + i);
        cout << trait;
        i += 2;
    }

    //Draw character stats slots
    i = j = 0;
    MoveCursor(startX + 19, startY - 1);
    cout << "Stats";
    for (string stat : stats)
    {
        MoveCursor(startX + 20, startY + i);
        cout << stat;
        i += 2;
    }
}

//Draw character actually skill and stats number to the corresponding slots
void IConsoleRender::UpdateCharacterStats(CharacterData Data)
{
    uint startX = 40;
    uint startY = 1;

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

    MoveCursor(startX + 8, startY);
    cout << "     ";
    MoveCursor(startX + 8, startY);
    cout << Data.level;

    MoveCursor(startX + 11, startY + 2);
    cout << "     ";
    MoveCursor(startX + 11, startY + 2);
    cout << Data.Strength;

    MoveCursor(startX + 10, startY + 4);
    cout << "     ";
    MoveCursor(startX + 10, startY + 4);
    cout << Data.Stamina;

    MoveCursor(startX + 12, startY + 6);
    cout << "     ";
    MoveCursor(startX + 12, startY + 6);
    cout << Data.Dexterity;

    MoveCursor(startX + 15, startY + 8);
    cout << "     ";
    MoveCursor(startX + 15, startY + 8);
    cout << Data.SkillPoints;

    MoveCursor(startX + 27, startY);
    cout << "     ";
    MoveCursor(startX + 27, startY);
    cout << Data.Damage;


    MoveCursor(startX + 26, startY + 2);
    cout << "    ";
    MoveCursor(startX + 26, startY + 2);
    cout << Data.Armor;

    MoveCursor(startX + 27, startY + 4);
    cout << "                 ";
    MoveCursor(startX + 27, startY + 4);
    cout << Data.CurrentHealth << "/" << Data.MaxHealth;

    MoveCursor(startX + 31, startY + 6);
    cout << "                 ";
    MoveCursor(startX + 31, startY + 6);
    cout << Data.ExperiencePoints << "/" << ((Data.level + 1) * (80 + (Data.level + 1) * 2));
}

//Switch item comparison stats of the items if they only switched places in inventory and equipment
void IConsoleRender::SwitchInvStat(int X, int Y, bool Inv)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    if (Inv)
    {
        //Clear previously held tile
        MoveCursor(X * 3, Y * 2 - 1);
        cout << ' ';
        ClearItemInfo();

        MoveCursor(39, 3);
        cout << '>';
    }
    else
    {
        MoveCursor(39, 3 + (Y - 1) * 2);
        cout << ' ';
    }
}

//Move character skills navigation cursor up/down
void IConsoleRender::MoveCharacterCursor(int Y, int MoveY)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    MoveCursor(39, 3 + (Y - 1) * 2);
    cout << ' ';

    MoveCursor(39, 3 + (Y + MoveY - 1) * 2);
    cout << '>';
}

//Draw main menu - InGame switches will the save option be present
void IConsoleRender::DrawMainMenu(bool InGame)
{
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    MoveCursor(20, 4);
    cout << "New Game";
    MoveCursor(20, 6);
    cout << "Load Map";
    MoveCursor(20, 8);
    cout << "Load Game";

    if (InGame)
    {
        MoveCursor(20, 10);
        cout << "Save Game";
        MoveCursor(20, 12);
        cout << "Quit Game";
    }
    else
    {
        MoveCursor(20, 10);
        cout << "Quit Game";
    }
}

//Move main menu cursor up and down
void IConsoleRender::MoveMMCursor(int Y, int MoveY)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    MoveCursor(19, 4 + (Y - 1) * 2);
    cout << ' ';

    MoveCursor(19, 4 + (Y + MoveY - 1) * 2);
    cout << '>';
}

//Show game over message above main menu
void IConsoleRender::EndGameUI(bool Victory)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    MoveCursor(20, 2);
    if(Victory) cout << "STAGE CLEARED";
    else cout << "GAME OVER";
}

//Show main menu screen for loading game/map or saving the game
void IConsoleRender::MainMenuTypeUI(string Type)
{
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    MoveCursor(20, 4);
    cout << Type << ": ";
}

//Show error if saved game was not found
void IConsoleRender::LoadError()
{
    MainMenuTypeUI("Load Game");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    MoveCursor(20, 2);
    cout << "Entered name does not exist. Try again.";
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    MoveCursor(31, 4);
}

//Get color for drawing based on the rarity of the item
int IConsoleRender::GetRarityColor(ItemData Item)
{
    if (Item.rarity == "common") return 7;
    else if (Item.rarity == "uncommon") return 10;
    else if (Item.rarity == "rare") return 9;
    else if (Item.rarity == "epic") return 5;
    else return 14;
}



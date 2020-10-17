#include "..\Public\GameMode.h"
#include "..\Public\Characters\Player\PlayerCharacterFactory.h"
#include "..\Public\Characters\Enemy\EnemyCharacterFactory.h"
#include "..\Public\View\MainMenu.h"
#include <Windows.h>
#include <fstream>
#include <iostream>


void AGameMode::BeginPlay()
{
    //Load map data from json
    LoadData(true, lastData);
    //Send map data to console renderer and draw the map
    InitRender(LoadedMap);
    //Initialize all loaded characters and draw them
	SpawnCharacters(LoadedCharacters);
    //Initialize all loaded items and draw them or add them to inventory/equipment
	SpawnItems(LoadedCharacters);
    //Show player/enemy UI
    EndTurnUI();
    //Start game process
    GameFlow();
}

void AGameMode::EndGame(bool Victory)
{
    //Switch controller input to main menu UI
    GetPlayerController().SetCurrentState(MenuOpened::MainMenu);
    //Show main menu
    GetConsoleRender().DrawMainMenu(false);
    //Show game over/victory messsage
    GetConsoleRender().EndGameUI(Victory);

    EndGameState = true;
}

bool AGameMode::GetEndGameState()
{
    return EndGameState;
}

bool AGameMode::LoadData(bool Map, string filePath)
{
    //Parse data from .json
    ifstream ifs(filePath);
    Json::Value obj;
    JSONCPP_STRING err;
    Json::CharReaderBuilder builder;
    parseFromStream(builder, ifs, &obj, &err);

    //If we are doing full load with the map
    if (Map)
    {
        //Get map size
        MapData newMap;
        newMap.mapsize.X = obj["map"]["size"]["x"].asUInt();
        newMap.mapsize.Y = obj["map"]["size"]["y"].asUInt();

        //Get data about straight lines
        const Json::Value& lines = obj["map"]["lines"];
        for (int i = 0; i < lines.size(); i++)
        {
            MapLine newLine;
            newLine.type = lines[i]["type"].asString();
            newLine.start.X = lines[i]["start"]["x"].asUInt();
            newLine.start.Y = lines[i]["start"]["y"].asUInt();
            newLine.end.X = lines[i]["end"]["x"].asUInt();
            newLine.end.Y = lines[i]["end"]["y"].asUInt();
            newMap.StraightLines.push_back(newLine);
        }
        //Get data about diagonal lines
        const Json::Value& diags = obj["map"]["diags"];
        for (int i = 0; i < diags.size(); i++)
        {
            MapLine newLine;
            newLine.type = diags[i]["type"].asString();
            newLine.start.X = diags[i]["start"]["x"].asUInt();
            newLine.start.Y = diags[i]["start"]["y"].asUInt();
            newLine.end.X = diags[i]["end"]["x"].asUInt();
            newLine.end.Y = diags[i]["end"]["y"].asUInt();
            newMap.DiagonalLines.push_back(newLine);
        }

        LoadedMap = newMap;
    }
    //Get player data
    SpawnData newSpawn;
    const Json::Value player = obj["characters"]["player"];
    newSpawn.PlayerInfo.ID = player["ID"].asUInt();
    newSpawn.PlayerInfo.position.X = player["start"]["x"].asUInt();
    newSpawn.PlayerInfo.position.Y = player["start"]["y"].asUInt();
    newSpawn.PlayerInfo.HealthPercentage = player["HealthPercentage"].asUInt();
    newSpawn.PlayerInfo.level = player["level"].asUInt();
    newSpawn.PlayerInfo.Stamina = player["Stamina"].asUInt();
    newSpawn.PlayerInfo.Dexterity = player["Dexterity"].asUInt();
    newSpawn.PlayerInfo.Strength = player["Strength"].asUInt();
    newSpawn.PlayerInfo.SkillPoints = player["SkillPoints"].asUInt();
    newSpawn.PlayerInfo.ExperiencePoints = player["ExperiencePoints"].asUInt();

    //Get enemies data
    const Json::Value& enemies = obj["characters"]["enemies"];
    for (int i = 0; i < enemies.size(); i++)
    {
        CharacterData newEnemy;
        if (enemies[i]["randomStats"].asBool())
        {
            newEnemy = FMath::RandomEnemyStats(enemies[i]["level"].asUInt());
        }
        else
        {
            newEnemy.Stamina = enemies[i]["Stamina"].asUInt();
            newEnemy.Dexterity = enemies[i]["Dexterity"].asUInt();
            newEnemy.Strength = enemies[i]["Strength"].asUInt();
        }
        newEnemy.ID = enemies[i]["ID"].asUInt();
        newEnemy.position.Y = enemies[i]["start"]["x"].asUInt();
        newEnemy.position.X = enemies[i]["start"]["y"].asUInt();
        newEnemy.level = enemies[i]["level"].asUInt();
        newEnemy.HealthPercentage = enemies[i]["HealthPercentage"].asUInt();
        newSpawn.EnemyInfo.push_back(newEnemy);
    }
    //Get data of all items
    const Json::Value& items = obj["items"];
    for (int i = 0; i < items.size(); i++)
    {
        //Generate item stats based on the rarity and type
        ItemData newItem = FMath::GenerateItemDrop(items[i]["reqlevel"].asUInt(), FMath::GetRarity(items[i]["rarity"].asString()), FMath::GetType(items[i]["type"].asString()));
        newItem.ID = items[i]["ID"].asUInt();
        newItem.position.Y = items[i]["start"]["x"].asUInt();
        newItem.position.X = items[i]["start"]["y"].asUInt();
        newItem.ownerID = items[i]["ownerID"].asUInt();
        newItem.equipedPlayer = items[i]["equipedPlayer"].asBool();
        newSpawn.ItemInfo.push_back(newItem);
    }

    LoadedCharacters = newSpawn;
    
    //If no errors are present return true
    if (err == "") return true;
    else return false;
}

void AGameMode::InitRender(MapData Map)
{
    //Tell console render to map and legend
    GetConsoleRender().BeginPlay(Map);
}

//Spawn all loaded characters
void AGameMode::SpawnCharacters(SpawnData Spawns)
{
    //unique pointers used so they would be cleared at the end of the function
    //Init player character factory
    unique_ptr<UPlayerCharacterFactory> PFactory(new UPlayerCharacterFactory());
    //Init player enemy character factory
    unique_ptr<UEnemyCharacterFactory> EFactory(new UEnemyCharacterFactory());

    //Create new unique_ptr for player controller and save it in the class as we will need the reference
    unique_ptr<AController> Player(&CreateCharacter(*PFactory, &GetConsoleRender()));
    PlayerController = move(Player);

    //Setup starting data for player character
    PlayerController->GetPawn().SetPosition(Spawns.PlayerInfo.position.X, Spawns.PlayerInfo.position.Y);
    PlayerController->GetPawn().SetGameMode(this);
    PlayerController->GetPawn().SetCharacterData(Spawns.PlayerInfo);
   
    //Spawn each enemy character and save them in array
    for (CharacterData Enemies : Spawns.EnemyInfo)
    {
        //Set the ID counter to the highest ID from the array - not neccessary right now as we do not spawn additional enemies
        if (Enemies.ID > EnemyIDCounter)
        {
            EnemyIDCounter = Enemies.ID;
        }
        EnemyControllers.push_back(make_unique<AController>(CreateCharacter(*EFactory, &GetConsoleRender())));
        EnemyControllers.back()->GetPawn().SetPosition(Enemies.position.X, Enemies.position.Y);
        EnemyControllers.back()->GetPawn().SetGameMode(this);
        EnemyControllers.back()->GetPawn().SetCharacterData(Enemies);
    }
}

//Create all loaded items
void AGameMode::SpawnItems(SpawnData Spawns)
{
    //Loop through all the times
    for (ItemData Item : Spawns.ItemInfo)
    {
        //Set ID counter to the highest ID of all items
        if (Item.ID > ItemIDCounter)
        {
            ItemIDCounter = Item.ID;
        }
        //Owner ID 12 means they are on the ground as it is not a valid ID for characters
        if (Item.ownerID == 12)
        {
            SpawnedItems.push_back(Item);
            GetConsoleRender().SpawnItem(Item.position.X, Item.position.Y);
        }
        else
        {
            //Owner ID 0 means they belong to the player character
            if (Item.ownerID == 0)
            {
                //Check if they are in the equipment slots or in the inventory
                if (Item.equipedPlayer)
                {
                    GetPlayerController().GetPawn().EquipItem(Item);  
                }
                else
                {
                    GetPlayerController().GetPawn().GetPawnData().Inventory.push_back(Item);
                }
            }
            //Check if they are owned by a enemy character - not neccessary right now as enemies don't have inventories (they drop randomized items)
            else
            {
                for (uint i = 0; i < EnemyControllers.size(); i++)
                {
                    if (EnemyControllers.at(i)->GetPawn().GetPawnData().ID == Item.ownerID)
                    {
                        EnemyControllers.at(i)->GetPawn().GetPawnData().Inventory.push_back(Item);
                        break;
                    }
                }
            }
        }
    }
}

//End turn process to loop through all characters
void AGameMode::EndTurnProcess(bool ByPlayer)
{
    TurnIterator++;
    if (PlayerController->GetPawn().GetPawnData().CurrentHealth > 0)
    {
        //If player called the process reset the iterator
        if (ByPlayer)
        {
            TurnIterator = 0;
        }
        //If iteretor is a higher number than the number of enemies characters it means the complete turn is over
        if (TurnIterator == EnemyControllers.size())
        {
            TurnIterator = 0;
            EndTurnUI();
            PlayerController->GetPawn().StartTurn();
            GetConsoleRender().MoveCursor(GetPlayerPos().X, GetPlayerPos().Y);
        }
        //Proceed with the next enemy character
        else
        {
            EnemyControllers.at(TurnIterator)->GetPawn().StartTurn();
        }
    }
}

//Returns position of the player pawn as Vector2D
Vec2D AGameMode::GetPlayerPos() const
{
    if (PlayerController)
    {
        return PlayerController->GetPawn().GetPawnPosition();
    }
    else return Vec2D();
}

//Game process for sending continues input to player character
void AGameMode::GameFlow()
{
    //Reset cursor to 0,0 to show the whole screen
    GetConsoleRender().MoveCursor(0, 0);
    //Set end game to false as the new game is in progress
    EndGameState = false;
    while (InGame)
    {
        if (TurnIterator == 0)
        {
            PlayerController->ReceiveInput();
        }
    }
}

//Quick sort eight closest enemies to the player and present them in UI by distance starting from top of the left column
int AGameMode::partition(vector<reference_wrapper<AController>> &InArray, Vec2D PlayerPos, int low, int high)
{
    int i = low - 1;
    float checkDis = FMath::GetVec2DDistance(PlayerPos, InArray.at(high).get().GetPawn().GetPawnPosition());
    for (int j = low; j < high; j++)
    {
        if (FMath::GetVec2DDistance(PlayerPos, InArray.at(j).get().GetPawn().GetPawnPosition()) < checkDis)
        {
            i++;
            swap(InArray.at(i), InArray.at(j));
        }
    }
    swap(InArray[i + 1], InArray[high]);
    return (i + 1);
}

//Quick sort eight closest enemies to the player and present them in UI by distance starting from top of the left column
void AGameMode::QuickSort(vector<reference_wrapper<AController>> &InArray, Vec2D PlayerPos, int low, int high)
{
    if (low < high)
    {
        int pivot = partition(InArray, PlayerPos, low, high);

        QuickSort(InArray, PlayerPos, low, pivot - 1);
        QuickSort(InArray, PlayerPos, pivot + 1, high);
    }
}

void AGameMode::EndTurnUI()
{
    //Clear all previous drawn character UI
    GetConsoleRender().ClearCharacterUI(1, LoadedMap.mapsize.X + 1);
    for (int i = 0; i < NumberOfCharacterSlots; i++)
    {
        int X = 32;
        if (i > 3) X = 62;
        GetConsoleRender().ClearCharacterUI(X, LoadedMap.mapsize.X + 1 + i * 3);
    }

    //Draw player UI
    GetConsoleRender().DrawCharacterUI(1, LoadedMap.mapsize.X + 1, PlayerController->GetPawn().GetPawnData(), false, "");

    //Get all enemies within distance of 8 tiles
    vector<reference_wrapper<AController>> ClosestEnemies;
    for(int i = 0; i < EnemyControllers.size(); i++)
    {
        float distance = FMath::GetVec2DDistance(GetPlayerPos(), EnemyControllers[i]->GetPawn().GetPawnPosition());
        if (distance < 8)
        {
            ClosestEnemies.push_back(*EnemyControllers[i].get());
        }
    }
    //Quick sort enemies by the distance in order to show only the closest ones
    //Enemies are show from top of the left column in UI first so the player can easier read which UI represents which enemies
    if (ClosestEnemies.size() > 0)
    {
        QuickSort(ClosestEnemies, GetPlayerPos(), 0, ClosestEnemies.size() - 1);
        //Limit to 8 closest enemies for drawing

        int numDraws = ClosestEnemies.size();
        if (ClosestEnemies.size() > 8)
        {
            numDraws = 8;
        }
        //Save the number of enemies drawn so we do not clear unecessary character UI in the next turn
        NumberOfCharacterSlots = numDraws;

        //Draw character UI for each enemy in the array
        for (int i = 0; i < numDraws; i++)
        {
            int X = 32;
            if (i > 3) X = 62;
            GetConsoleRender().DrawCharacterUI(X, LoadedMap.mapsize.X + 1 + i * 3, ClosestEnemies.at(i).get().GetPawn().GetPawnData(), true, FMath::GetVec2DDirection(GetPlayerPos(), ClosestEnemies.at(i).get().GetPawn().GetPawnPosition()));
        }
    }
}

//Set console renderer reference from main menu
void AGameMode::SetConsoleRender(IConsoleRender& NewRender)
{
    ConsoleRender = &NewRender;
}

//Get console renderer reference
IConsoleRender& AGameMode::GetConsoleRender()
{
    return *ConsoleRender;
}

//Set main menu reference
void AGameMode::SetMainMenu(IMainMenu& NewMM)
{
    MainMenu = &NewMM;
}

//Return main menu reference
IMainMenu& AGameMode::GetMainMenu()
{
    return *MainMenu;
}

//Character creation function based on the abstract factory method
AController& AGameMode::CreateCharacter(UAbstractCharacterFactory& Factory, IConsoleRender* ConsoleRender)
{
    AController& NewController(Factory.CreateController());
    APawn& NewPawn(Factory.CreatePawn());
    NewPawn.SetConsoleRender(ConsoleRender);
    NewController.SetPawn(NewPawn);
    return NewController;
}

//Write new json for saving the game
void AGameMode::WriteNewJSON(bool Map, string filePath)
{
    Json::Value obj;

    //If we are doing full save with the map
    if (Map)
    {
        //Load the data from the currently opened map in order to copy it to save game
        ifstream ifs(lastData);
        Json::Value loadobj;
        JSONCPP_STRING err;
        Json::CharReaderBuilder builder;
        parseFromStream(builder, ifs, &loadobj, &err);

        //Map size data
        obj["map"]["size"]["x"] = loadobj["map"]["size"]["x"].asUInt();
        obj["map"]["size"]["y"] = loadobj["map"]["size"]["y"].asUInt();

        //Vector for straight lines
        Json::Value lines_vec(Json::arrayValue);
        const Json::Value& lines = loadobj["map"]["lines"];
        for (int i = 0; i < lines.size(); i++)
        {
            Json::Value newLine;
            newLine["type"] = lines[i]["type"].asString();
            newLine["start"]["x"] = lines[i]["start"]["x"].asUInt();
            newLine["start"]["y"] = lines[i]["start"]["y"].asUInt();
            newLine["end"]["x"] = lines[i]["end"]["x"].asUInt();
            newLine["end"]["y"] = lines[i]["end"]["y"].asUInt();
            lines_vec.append(newLine);
        }
        obj["map"]["lines"] = lines_vec;

        //Vector for diagonal lines
        Json::Value diags_vec(Json::arrayValue);
        const Json::Value& diags = loadobj["map"]["diags"];
        for (int i = 0; i < diags.size(); i++)
        {
            Json::Value newLine;
            newLine["type"] = diags[i]["type"].asString();
            newLine["start"]["x"] = diags[i]["start"]["x"].asUInt();
            newLine["start"]["y"] = diags[i]["start"]["y"].asUInt();
            newLine["end"]["x"] = diags[i]["end"]["x"].asUInt();
            newLine["end"]["y"] = diags[i]["end"]["y"].asUInt();
            diags_vec.append(newLine);
        }
        obj["map"]["diags"] = diags_vec;
    }

    //Get all necessary information from pawn data
    Json::Value item_vec(Json::arrayValue);
    Vec2D PlayerPos = GetPlayerController().GetPawn().GetPawnPosition();
    CharacterData PlayerData = GetPlayerController().GetPawn().GetPawnData();
    obj["characters"]["player"]["ID"] = PlayerData.ID;
    obj["characters"]["player"]["start"]["x"] = PlayerPos.X;
    obj["characters"]["player"]["start"]["y"] = PlayerPos.Y;
    obj["characters"]["player"]["HealthPercentage"] = PlayerData.HealthPercentage;
    obj["characters"]["player"]["level"] = PlayerData.level;
    obj["characters"]["player"]["Stamina"] = PlayerData.Stamina;
    obj["characters"]["player"]["Dexterity"] = PlayerData.Dexterity;
    obj["characters"]["player"]["Strength"] = PlayerData.Strength;
    obj["characters"]["player"]["SkillPoints"] = PlayerData.SkillPoints;
    obj["characters"]["player"]["ExperiencePoints"] = PlayerData.ExperiencePoints;
    //Save each item from player inventory into items vector
    for (ItemData Item : PlayerData.Inventory)
    {
        Json::Value newItem;
        newItem["ID"] = Item.ID;
        newItem["start"]["x"] = 0;
        newItem["start"]["y"] = 0;
        newItem["type"] = Item.type;
        newItem["reqlevel"] = Item.ReqLevel;
        newItem["ownerID"] = PlayerData.ID;
        newItem["equipedPlayer"] = false;
        newItem["rarity"] = Item.rarity;
        item_vec.append(newItem);
    }
    //Save each item from player equipment into items vector
    vector<ItemData> PlayerEquipment;
    if (PlayerData.Equipment.ArmorEquiped)
    {
        PlayerEquipment.push_back(PlayerData.Equipment.Armor);
    }
    if (PlayerData.Equipment.WeaponEquiped)
    {
        PlayerEquipment.push_back(PlayerData.Equipment.Weapon);
    }
    if (PlayerData.Equipment.HelmetEquiped)
    {
        PlayerEquipment.push_back(PlayerData.Equipment.Helmet);
    }
    if (PlayerData.Equipment.ShieldEquiped)
    {
        PlayerEquipment.push_back(PlayerData.Equipment.Shield);
    }
    if (PlayerData.Equipment.BootsEquiped)
    {
        PlayerEquipment.push_back(PlayerData.Equipment.Boots);
    }
    for (ItemData Item : PlayerEquipment)
    {
        Json::Value newItem;
        newItem["ID"] = Item.ID;
        newItem["start"]["x"] = 0;
        newItem["start"]["y"] = 0;
        newItem["type"] = Item.type;
        newItem["reqlevel"] = Item.ReqLevel;
        newItem["ownerID"] = PlayerData.ID;
        newItem["equipedPlayer"] = true;
        newItem["rarity"] = Item.rarity;
        item_vec.append(newItem);
    }

    //Save all necessary enemy data into enemy characters vector
    Json::Value vec(Json::arrayValue);
    for(int i = 0; i < EnemyControllers.size(); i++)
    {
        Json::Value newEnemy;
        Vec2D EnemyPos = EnemyControllers.at(i)->GetPawn().GetPawnPosition();
        CharacterData EnemyData = EnemyControllers.at(i)->GetPawn().GetPawnData();
        newEnemy["ID"] = EnemyData.ID;
        newEnemy["start"]["x"] = EnemyPos.Y;
        newEnemy["start"]["y"] = EnemyPos.X;
        newEnemy["HealthPercentage"] = EnemyData.HealthPercentage;
        newEnemy["level"] = EnemyData.level;
        newEnemy["Stamina"] = EnemyData.Stamina;
        newEnemy["Dexterity"] = EnemyData.Dexterity;
        newEnemy["Strength"] = EnemyData.Strength;
        newEnemy["SkillPoints"] = EnemyData.SkillPoints;
        newEnemy["randomStats"] = false;
        for (ItemData Item : EnemyData.Inventory)
        {
            Json::Value newItem;
            newItem["ID"] = Item.ID;
            newItem["start"]["x"] = 0;
            newItem["start"]["y"] = 0;
            newItem["type"] = Item.type;
            newItem["reqlevel"] = Item.ReqLevel;
            newItem["ownerID"] = EnemyData.ID;
            newItem["equipedPlayer"] = false;
            newItem["rarity"] = Item.rarity;
            item_vec.append(newItem);
        }
        vec.append(newEnemy);
    }
    obj["characters"]["enemies"] = vec;
    //Save all ground items into items vector
    for (uint i = 0; i < SpawnedItems.size(); i++)
    {
        Json::Value newItem;
        newItem["ID"] = SpawnedItems.at(i).ID;
        newItem["start"]["y"] = SpawnedItems.at(i).position.X;
        newItem["start"]["x"] = SpawnedItems.at(i).position.Y;
        newItem["type"] = SpawnedItems.at(i).type;
        newItem["reqlevel"] = SpawnedItems.at(i).ReqLevel;
        newItem["ownerID"] = 12;
        newItem["equipedPlayer"] = false;
        newItem["rarity"] = SpawnedItems.at(i).rarity;
        item_vec.append(newItem);
    }
    obj["items"] = item_vec;

    //Write to file
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "   ";
    unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream outputFileStream(filePath);
    writer->write(obj, &outputFileStream);
}

//Save only the information about characters and items, without the map
void AGameMode::QuickSave()
{
    WriteNewJSON(false, "Saves/quicksave.json");
}

//Load only the information about characters and items, without the map
void AGameMode::QuickLoad()
{
    //Clear all drawn characters and items
    GetConsoleRender().ClearCharactersFromArray();
    LoadData(false, "Saves/quicksave.json");

    //No need to recreate player character, just revert all the data
    GetPlayerController().GetPawn().SetPosition(LoadedCharacters.PlayerInfo.position.X, LoadedCharacters.PlayerInfo.position.Y);
    GetPlayerController().GetPawn().SetCharacterData(LoadedCharacters.PlayerInfo);

    //Enemy character factory
    unique_ptr<UEnemyCharacterFactory> EFactory(new UEnemyCharacterFactory());
    //Loop through enemy characters data
    for (CharacterData Enemies : LoadedCharacters.EnemyInfo)
    {
        //Check if the enemy with the same ID already exist and revert its state
        bool Exists = false;
        for (uint i = 0; i < EnemyControllers.size(); i++)
        {
            if (Enemies.ID == EnemyControllers.at(i)->GetPawn().GetPawnData().ID)
            {
                EnemyControllers.at(i)->GetPawn().SetPosition(Enemies.position.X, Enemies.position.Y);
                EnemyControllers.at(i)->GetPawn().SetCharacterData(Enemies);
                Exists = true;
                break;
            }
        }
        //Recreate the enemy character if it does not exist
        if (!Exists)
        {
            if (Enemies.ID > EnemyIDCounter)
            {
                EnemyIDCounter = Enemies.ID;
            }
            EnemyControllers.push_back(make_unique<AController>(CreateCharacter(*EFactory, &GetConsoleRender())));
            EnemyControllers.back()->GetPawn().SetPosition(Enemies.position.X, Enemies.position.Y);
            EnemyControllers.back()->GetPawn().SetGameMode(this);
            EnemyControllers.back()->GetPawn().SetCharacterData(Enemies);
        }
    }

    //Check all loaded items
    for (ItemData Item : LoadedCharacters.ItemInfo)
    {
        //If ground item with same ID already exists do not do anything
        bool Exists = false;
        for (uint i = 0; i < SpawnedItems.size(); i++)
        {
            if (Item.ID == SpawnedItems.at(i).ID)
            {
                Exists = true;
                break;
            }
        }
        //If the item does not exist
        if (!Exists)
        {
            //Set item ID counter to the highest ID
            if (Item.ID > ItemIDCounter)
            {
                ItemIDCounter = Item.ID;
            }
            //Check if the item should be on the ground
            if (Item.ownerID == 12)
            {
                SpawnedItems.push_back(Item);
                GetConsoleRender().SpawnItem(Item.position.X, Item.position.Y);
            }
            else
            {
                //Check if the item is owned by the player
                if (Item.ownerID == 0)
                {
                    //Check if the item is in the inventory or equiped
                    if (Item.equipedPlayer)
                    {
                        GetPlayerController().GetPawn().EquipItem(Item);
                    }
                    else
                    {
                        GetPlayerController().GetPawn().GetPawnData().Inventory.push_back(Item);
                    }
                }
                //Check if item should be in enemy inventory
                else
                {
                    for (uint i = 0; i < EnemyControllers.size(); i++)
                    {
                        if (EnemyControllers.at(i)->GetPawn().GetPawnData().ID == Item.ownerID)
                        {
                            EnemyControllers.at(i)->GetPawn().GetPawnData().Inventory.push_back(Item);
                            break;
                        }
                    }
                }
            }
        }
    }
}

//Perform a full save game with the map
void AGameMode::SaveGame(string Name)
{
    WriteNewJSON(true, Name);
}

//Perform a full load game with the map
void AGameMode::LoadGame(string Name)
{
    lastData = Name;
    BeginPlay();
}

//Return the reference to the player in game controller
AController& AGameMode::GetPlayerController() const
{
    return *PlayerController;
}

//Calculate the damage in the fight between attacker and defender
void AGameMode::PerformAttack(APawn* Attacker, APawn* Target)
{
    //If both pointers are valid proceed
    if (Attacker && Target)
    {
        //Get attacker and defender pawn data
        CharacterData AttackerStats = Attacker->GetPawnData();
        CharacterData DefenderStats = Target->GetPawnData();

        //Get ratio of defender armor and attacker damage in order to reduce the damage based on the armor
        float DefenderArmorDifference = 1 - float(DefenderStats.Armor) / float(AttackerStats.Damage);
        float AttackerArmorDifference = 1 - float(AttackerStats.Armor) / float(DefenderStats.Damage);
       
        //Limit the armor reduce between 0.1 and 0.9
        if (DefenderArmorDifference < 0.3f)
        {
            DefenderArmorDifference = 0.3f;
        }
        if (DefenderArmorDifference > 0.9f)
        {
            DefenderArmorDifference = 0.9f;
        }
        if (AttackerArmorDifference < 0.3f)
        {
            AttackerArmorDifference = 0.3f;
        }
        if (AttackerArmorDifference > 0.9f)
        {
            AttackerArmorDifference = 0.9f;
        }
        //Lower attackes damage if defender is in fortify mode
        float multiplier = 1;
        if (Target->GetFortify())
        {
            multiplier = 0.85;
        }
        //Calculate both damages
        int AttackerDamage = int(ceilf(AttackerStats.Damage * DefenderArmorDifference * (1 + (AttackerStats.level - DefenderStats.level) / 5) * multiplier));
        int DefenderDamage = int(ceilf(DefenderStats.Damage * AttackerArmorDifference * (1 + (DefenderStats.level - AttackerStats.level) / 5)));
        
        //Apply damage to defender
        if (Target->TakeDamage(AttackerDamage))
        {
            //If defender died add experience points to the attacked based on the level
            Attacker->IncreaseEXP(DefenderStats.level * 20);
        }
        //Apply damage to attacker
        if (Attacker->TakeDamage(DefenderDamage))
        {
            //If attacker died add experience points to the defender based on the level
            Target->IncreaseEXP(AttackerStats.level * 20);
        }
    }
}

//Returns the enemy pawn at the given coordinates
APawn* AGameMode::GetEnemy(int X, int Y) const
{
    for (int i = 0; i < EnemyControllers.size(); i++)
    {
        Vec2D EnemyPos = EnemyControllers[i]->GetPawn().GetPawnPosition();
        if (EnemyPos.X == X && EnemyPos.Y == Y)
        {
            return &EnemyControllers[i]->GetPawn();
        }
    }
    return nullptr;
}

//Death handling for enemies
void AGameMode::EnemyDeath(APawn& Enemy)
{
    for (int i = 0; i < EnemyControllers.size(); i++)
    {
        //Find the enemy controller which posses the given pawn reference
        if (&EnemyControllers[i].get()->GetPawn() == &Enemy)
        {
            //Clear the tile on which the enemy was
            GetConsoleRender().ClearTile(Enemy.GetPawnPosition().X, Enemy.GetPawnPosition().Y);
            //Make enemy drop item on death
            EnemyDrop(Enemy.GetPawnData(), Enemy.GetPawnPosition());
            //Delete enemy object and remove the reference from the array
            delete &Enemy;
            EnemyControllers.erase(EnemyControllers.begin() + i);
            break;
        }
    }
    //If all enemies are dead show victory screen
    if (EnemyControllers.size() < 1)
    {
        EndGame(true);
    }
}

//Handle enemy item drops
void AGameMode::EnemyDrop(CharacterData Enemy, Vec2D Position)
{
    //Generate item drop using the item drop randomized based on the enemy level
    ItemData ItemToDrop = FMath::GenerateItemDrop(Enemy.level, 5, 5);
    //Setup item position
    ItemToDrop.position.X = Position.X;
    ItemToDrop.position.Y = Position.Y;
    //Calculate the new increase in ID by separating the index and increase only the pureId numbers
    ItemIDCounter = FMath::SplitIDIncrease(ItemIDCounter);
    ItemToDrop.ID = ItemIDCounter;
    //Add spawned item to the floor items array
    SpawnedItems.push_back(ItemToDrop);
    //Draw item character
    GetConsoleRender().SpawnItem(Position.X, Position.Y);
}

//Check if the item is present on the given coordinates and add them to the player
void AGameMode::AddToPlayer(Vec2D Position, int X, int Y)
{
    for (int i = 0; i < SpawnedItems.size(); i++)
    {
        //If the item has the given coordinates
        if (SpawnedItems.at(i).position.X == Position.X + X && SpawnedItems.at(i).position.Y == Position.Y + Y)
        {
            //Set owner to the player ID and add it to inventory
            SpawnedItems.at(i).ownerID = 0;
            GetPlayerController().GetPawn().GetPawnData().Inventory.push_back(SpawnedItems.at(i));
            //Remove the item from floor items
            SpawnedItems.erase(SpawnedItems.begin() + i);
            break;
        }
    }
}

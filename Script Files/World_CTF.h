#ifndef GRUMBOZ_WORLD_CTF_H
#define GRUMBOZ_WORLD_CTF_H

#include <unordered_map>

struct WorldFlagGps_Elements
{
	uint32 id;
	uint32 guid;
	std::string name;
	uint32 map_id;
	uint32 area_id;
	uint32 zone_id;
	float x;
	float y;
	float z;
	float o;
};

struct WorldCtfFlagInfo_Elements
{
	uint32 entry_id;
	GameobjectTypes type;
	uint32 display_id;
	std::string name;
	float size;
	uint32 faction_id;
};

struct WorldCtfPlayerInfo_Elements
{
	bool aura;
	uint32 captures;
};

struct WorldCtfScore_Elements
{
	uint32 score;
};

struct WorldCtfAura_Elements
{
	uint32 aura;
};

class TC_GAME_API WorldCtf
{

private:
	WorldCtf();
	~WorldCtf();

public:
	static WorldCtf* instance();

	// public methods
		// Getters
		uint32 GetActiveGO_ID() { return ActiveGO_ID; }
		uint32 GetActiveGO_Team_ID() { return ActiveGO_Team_ID; }
		std::string GetActiveGO_Team_Name(uint8 v) { return WorldCtfFlagInfo[v].name; }
		bool GetWilOWhisp() { return Default_Wil_O_Whisp; }
		bool GetFlagReset() { return Default_Flag_Reset; }
		bool GetTimerType() { return Default_Timer_Type; }
		uint32 GetDefaultTimerDurationMinimum() { return Default_Timer_Duration_Minimum; }
		uint32 GetDefaultTimerDuration() { return Default_Timer_Duration; }
		uint32 GetDefaultWorldFlagID() { return Default_World_Flag_ID; }
		float GetDefaultWorldFlagScale() { return Default_World_Flag_Scale; }
		bool GetFlagStorageType() { return Default_Flag_Storage_Type; }
		uint8 GetHintSystem() { return Default_Hint_System; }
		uint64 GetWinningGameTime() {return WinningGameTime; }
		uint32 GetCooldownTimer() { return ActiveCooldDownTime; }
		bool GetRequireFlagAura() { return Default_Require_Flag_Aura; }
		uint8 GetRequiredGMMinimumRank() { return Default_Required_GM_Rank; }
		uint32 GetFlagCtfID(uint32 guid);
		uint8 GetPlayerAuraCheckerTimer() { return Default_Player_Aura_Checker_Timer; }

		// Setters
		void UpdateWorldFlagDataBase(uint8 key, uint32 guid, std::string name, uint32 map_id, uint32 area_id, uint32 zone_id, float x, float y, float z, float o);
		void SetActiveGO_ID(uint32 v) { ActiveGO_ID = v; }
		void SetActiveGO_Team_ID(uint8 v) { ActiveGO_Team_ID = v; }
		void SetWilOWhisp(bool v) { Default_Wil_O_Whisp = v; }
		void SetFlagReset(bool v) { Default_Flag_Reset = v; }
		void SetTimerType(bool v) { Default_Timer_Type = v; }
		void SetDefaultTimerDurationMinimum(uint32 v) { Default_Timer_Duration_Minimum = v; }
		void SetDefaultTimerDuration(uint32 v) { Default_Timer_Duration = v; }
		void SetDefaultWorldFlagID(uint32 v) { Default_World_Flag_ID = v; }
		void SetDefaultWorldFlagScale(float v) { Default_World_Flag_Scale = v; }
		void SetFlagStorageType(bool v) { Default_Flag_Storage_Type = v; }
		void SetHintSystem(uint8 v) { Default_Hint_System = v; }
		void SetCooldownTimer(uint32 v) { ActiveCooldDownTime = v; }
		void SetWinningGameTime(uint64 v) { WinningGameTime = v; }
		void SetRequireFlagAura(bool v) { Default_Require_Flag_Aura = v; }
		void SetRequiredGMMinimumRank(uint8 v) { Default_Required_GM_Rank = v; }
		void SetPlayerAuraCheckerTimer(uint8 v) { Default_Player_Aura_Checker_Timer = v; }
		void SetTest(bool v) { test = v; }
		// Tools
		std::string ConvertNumberToString(uint64 numberX);
		void AnnounceFlagSpawned();
		void LoadWorldFlags();
		void GenerateCoolDownTimer();
		void GenerateNewRandomFlagGps();
		uint32 GenerateFlagInWorld(Player* player, Map* map, uint32 id, std::string name, float x, float y, float z, float o);
		void AddNewWorldFlag(Player* player);
		void SendWorldMsg(uint8 type, std::string message);
		void ResetPlayer(Player* player);
		void ResetAllPlayers();
		void UpdateGameObject(GameObject* go);

	// Public Tables
	std::unordered_map<uint32, WorldFlagGps_Elements> WorldFlagGps;
	std::unordered_map<uint32, WorldCtfPlayerInfo_Elements> WorldCtfPlayerInfo;
	std::unordered_map<uint32, WorldCtfScore_Elements> WorldCtfScore;
	std::unordered_map<uint8, WorldCtfAura_Elements> WorldCtfAura;
	std::unordered_map<uint8, WorldCtfFlagInfo_Elements> WorldCtfFlagInfo;

	// Public Variables
//	std::default_random_engine generator;
	bool test;

private:
	// Methods
	// Local Variables
	uint32 ActiveGO_ID;
	uint8 ActiveGO_Team_ID = 3;
	uint32 ActiveCooldDownTime;
	uint64 WinningGameTime;
	bool Default_Wil_O_Whisp;
	bool Default_Flag_Reset;
	bool Default_Timer_Type;
	uint32 Default_Timer_Duration_Minimum;
	uint32 Default_Timer_Duration;
	uint32 Default_World_Flag_ID;
	float Default_World_Flag_Scale;
	bool Default_Flag_Storage_Type;
	uint8 Default_Hint_System;
	bool Default_Require_Flag_Aura;
	uint8 Default_Required_GM_Rank;
	uint8 Default_Player_Aura_Checker_Timer;
	
};

#define sWorldCtf WorldCtf::instance()
#endif

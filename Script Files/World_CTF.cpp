// © Grumboz World Capture the Flag System © 
// © By slp13at420 of EmuDevs.com © 
// © an EmuDevs NomSoft - Only - release © 
// © http://emudevs.com/showthread.php/5993-CPP-Grumbo-z-Capture-the-Flag-System?p=39857#post39857

// © Language:CPP © 
// © Platform:TrinityCore © 
// © Start:10-05-2016 © 
// © Finish:10-07-2016 © 
// © Release:10-07-2016 © 
// © Primary Programmer:slp13at420 © 
// © Secondary Programmers:none © 

// © My latest version of my beloved blood shed system ;) © 
// ©  Do NOT remove any credits © 
// ©  Don't share/rerelease on any other site other than EmuDevs.com © 
// © Dont attempt to claim as your own work ... © 

#include "chat.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GameTime.h"
#include "GossipDef.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "QueryResult.h"
#include "Random.h"
#include "RBAC.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include <unordered_map>
#include "World.h"
#include "World_CTF.h"
#include "WorldSession.h"

WorldCtf::WorldCtf() { }

WorldCtf::~WorldCtf()
{
	for (std::unordered_map<uint32, WorldFlagGps_Elements>::iterator itr = WorldFlagGps.begin(); itr != WorldFlagGps.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint32, WorldCtfPlayerInfo_Elements>::iterator itr = WorldCtfPlayerInfo.begin(); itr != WorldCtfPlayerInfo.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint32, WorldCtfScore_Elements>::iterator itr = WorldCtfScore.begin(); itr != WorldCtfScore.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint8, WorldCtfAura_Elements>::iterator itr = WorldCtfAura.begin(); itr != WorldCtfAura.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint8, WorldCtfFlagInfo_Elements>::iterator itr = WorldCtfFlagInfo.begin(); itr != WorldCtfFlagInfo.end(); ++itr)
		delete &itr->second;

	WorldFlagGps.clear();
	WorldCtfPlayerInfo.clear();
	WorldCtfScore.clear();
	WorldCtfAura.clear();
	WorldCtfFlagInfo.clear();
}

WorldCtf* WorldCtf::instance()
{
	static WorldCtf instance;
	return &instance;
}

std::string WorldCtf::ConvertNumberToString(uint64 numberX)
{
	auto number = numberX;
	std::stringstream convert;
	std::string number32_to_string;
	convert << number;
	number32_to_string = convert.str();

	return number32_to_string;
};

void WorldCtf::LoadWorldFlags()
{
	// Loading prestored World Flag's info from db

	uint32 flag_count = 1;

	QueryResult WorldFlagGps_Query = WorldDatabase.PQuery("SELECT * FROM grumboz_wctf_flag_gps;"); // id, guid, name, map_id, area_id, zone_id, x, y, z, o

	if (WorldFlagGps_Query)
	{
		do
		{
			Field* fields = WorldFlagGps_Query->Fetch();
			uint32 guid = fields[0].GetUInt32();
			std::string name = fields[1].GetString();
			uint32 map_id = fields[2].GetUInt32();
			uint32 area_id = fields[3].GetUInt32();
			uint32 zone_id = fields[4].GetUInt32();
			float x = fields[5].GetFloat();
			float y = fields[6].GetFloat();
			float z = fields[7].GetFloat();
			float o = fields[8].GetFloat();

			WorldFlagGps_Elements& data = sWorldCtf->WorldFlagGps[flag_count];
			// Save the DB values to the MyData object
			data.id = flag_count;
			data.guid = guid;
			data.name = name;
			data.map_id = map_id;
			data.area_id = area_id;
			data.zone_id = zone_id;
			data.x = x;
			data.y = y;
			data.z = z;
			data.o = o;

			flag_count += 1;

		} while (WorldFlagGps_Query->NextRow());
	}
}

uint32 WorldCtf::GetFlagCtfID(uint32 guid)
{
	if (sWorldCtf->WorldFlagGps.size() != 0)
	{
		uint32 id;

		for (id = 1; id < WorldFlagGps.size(); id++)
		{
			if (sWorldCtf->WorldFlagGps[id].guid == guid)
			{
				return id;

					if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "GET_ACTIVE_FLAG_ID:%u", id); }

				break;
			}
		}

			if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "GET_ACTIVE_FLAG_ID:%u", id); }
	}
	return false;
}

void WorldCtf::GenerateNewRandomFlagGps()
{
	int id = 1;

		if (sWorldCtf->GetWilOWhisp() && sWorldCtf->WorldFlagGps.size() > 1)
		{
			id = urand(1, sWorldCtf->WorldFlagGps.size()); 

			if (!sWorldCtf->WorldFlagGps[id].guid) { sWorldCtf->GenerateNewRandomFlagGps(); }
		}

	sWorldCtf->SetActiveGO_ID(id);

	if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "GENERATE_NEW_ACTIVE_FLAG ID:%u", id); }
}

void WorldCtf::AddNewWorldFlag(Player* player)
{
	Map* map = player->GetMap();

	uint32 id = sWorldCtf->GetDefaultWorldFlagID() + 2;

	float x = player->GetPositionX();
	float y = player->GetPositionY();
	float z = player->GetPositionZ();
	float o = player->GetOrientation();

	int startphasemask = player->GetPhaseMask();
	
	const GameObjectTemplate* objectInfo = sObjectMgr->GetGameObjectTemplate(id);

	if (objectInfo)
	{
		GameObject* GO = new GameObject(); // creating an empty GameObject shell.

		ObjectGuid::LowType guidLow = map->GenerateLowGuid<HighGuid::GameObject>();

		QuaternionData gps = QuaternionData::fromEulerAnglesZYX(player->GetOrientation(), 0.f, 0.f);

		if (!GO->Create(guidLow, id, map, startphasemask, *player, gps, 255, GO_STATE_READY)) // attempt to create a GO with the guidlow, id and fill the shell with data retrieved with id.
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Flag build error..");
			delete GO;
			sWorldCtf->AddNewWorldFlag(player);
		};

		map->AddToMap(GO); // attempt to add th object to the map.

		if (!GO->isSpawned())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Flag spawn error..");
			delete GO;
			sWorldCtf->AddNewWorldFlag(player);
		}

		if (GO->isSpawned())
		{
			GO->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), startphasemask); // finally passes all checks, so save it to DB.
		}
	}
}

void WorldCtf::UpdateWorldFlagDataBase(uint8 key, uint32 guid, std::string name, uint32 map_id, uint32 area_id, uint32 zone_id, float x, float y, float z, float o)
{
	std::string wctf_query;
	uint32 id;

	if (key == 0) // add entry
	{
		wctf_query = "INSERT INTO grumboz_wctf_flag_gps VALUES('%u', '%s', '%u', '%u', '%u', '%.4f', '%.4f', '%.4f', '%.4f');"; // (`guid`, `name`, `map_id`, `area_id`, `zone_id`, `x`, `y`, `z`, `o`) 

		id = sWorldCtf->WorldFlagGps.size() + 1;

		WorldFlagGps_Elements& data = sWorldCtf->WorldFlagGps[id];
		// Save the DB values to the MyData object
		data.id = id;
		data.guid = guid;
		data.name = name;
		data.map_id = map_id;
		data.area_id = area_id;
		data.zone_id = zone_id;
		data.x = x;
		data.y = y;
		data.z = z;
		data.o = o;

		if (id == 1) { sWorldCtf->GenerateNewRandomFlagGps(); }

		if(sWorldCtf->test) {TC_LOG_INFO("server.loading", "UPDATE_GO_DATABASE ID:%u", id); }
	}

	if (key == 1) // remove entry
	{
		wctf_query = "DELETE FROM grumboz_wctf_flag_gps WHERE `guid` = '%u';";

		id = sWorldCtf->GetFlagCtfID(guid);

		sWorldCtf->WorldFlagGps[id].guid = 0;
	}

	if(sWorldCtf->GetFlagStorageType()) { WorldDatabase.PExecute(wctf_query, guid, name.c_str(), map_id, area_id, zone_id, x, y, z, o); }
}

void WorldCtf::GenerateCoolDownTimer()
{
	uint32 t = sWorldCtf->GetDefaultTimerDuration();

	if (sWorldCtf->GetTimerType()) {t = urand(t, sWorldCtf->GetDefaultTimerDuration());}

	sWorldCtf->SetCooldownTimer(t);
}

class WCTF_Load_Conf  : public WorldScript
{
public: WCTF_Load_Conf() : WorldScript("WCTF_Load_Conf") { };

		virtual void OnConfigLoad(bool /*reload*/)
		{
			TC_LOG_INFO("server.loading", "___________________________________");
			TC_LOG_INFO("server.loading", "-        Grumboz World CTF        -");
			TC_LOG_INFO("server.loading", "___________________________________");

			// Storing flag carrier aura ids by teamId
			WorldCtfAura_Elements& data3 = sWorldCtf->WorldCtfAura[0];
			data3.aura = 23335;
			WorldCtfAura_Elements& data4 = sWorldCtf->WorldCtfAura[1];
			data4.aura = 23333;

			// Load and Store the World conf entries
			sWorldCtf->SetWilOWhisp(sConfigMgr->GetBoolDefault("CTF.WIL_O_WHISP", true));
			sWorldCtf->SetFlagReset(sConfigMgr->GetBoolDefault("CTF.FLAG_RESET", false));
			sWorldCtf->SetTimerType(sConfigMgr->GetBoolDefault("CTF.CD_TIMER_TYPE", true));
			sWorldCtf->SetDefaultTimerDurationMinimum(sConfigMgr->GetIntDefault("CTF.CD_TIMER_MINIMUM", 60));
			sWorldCtf->SetDefaultTimerDuration(sConfigMgr->GetIntDefault("CTF.CD_TIMER", 300));
			sWorldCtf->SetDefaultWorldFlagID(sConfigMgr->GetIntDefault("CTF.DEFAULT_WORLD_FLAG_ID", 600002));
			sWorldCtf->SetDefaultWorldFlagScale(sConfigMgr->GetFloatDefault("CTF.DEFAULT_WORLD_FLAG_SCALE", 30.00));
			sWorldCtf->SetHintSystem(sConfigMgr->GetIntDefault("CTF.HINT_SYSTEM", 0));
			sWorldCtf->SetRequireFlagAura(sConfigMgr->GetBoolDefault("CTF.REQUIRE_FLAG_AURA", true));
			sWorldCtf->SetRequiredGMMinimumRank(sConfigMgr->GetIntDefault("CTF.GM_RANK", 3));
			sWorldCtf->SetFlagStorageType(sConfigMgr->GetBoolDefault("CTF.FLAG_STORAGE_TYPE", true));
			sWorldCtf->SetPlayerAuraCheckerTimer(sConfigMgr->GetIntDefault("CTF.PLAYER_AURA_CHECK_TIMER", 5));
			sWorldCtf->SetTest(sConfigMgr->GetBoolDefault("CTF.TEST", false));

			if(sWorldCtf->GetFlagStorageType()) { sWorldCtf->LoadWorldFlags(); }

			uint32 flag_count = sWorldCtf->WorldFlagGps.size();

			TC_LOG_INFO("server.loading", "- %u flag locations loaded", flag_count);

				// Post Settings to console
				if (sWorldCtf->GetWilOWhisp()) { TC_LOG_INFO("server.loading", "- Wil-o-Whisp:Active. (Random spawning)"); }
				if (sWorldCtf->GetTimerType()) { TC_LOG_INFO("server.loading", "- Random Cooldown:Active."); }
				if (sWorldCtf->GetHintSystem() == 0) { TC_LOG_INFO("server.loading", "- Hint System:Idle."); }
				if (sWorldCtf->GetHintSystem() == 1) { TC_LOG_INFO("server.loading", "- Hint System:Hint to Holding Team."); }
				if (sWorldCtf->GetHintSystem() == 2) { TC_LOG_INFO("server.loading", "- Hint System:Hint to both Teams."); }
				if (sWorldCtf->GetRequireFlagAura()) { TC_LOG_INFO("server.loading", "- Flag Holder Aura:required."); }
				if (!sWorldCtf->GetFlagStorageType()) { TC_LOG_INFO("server.loading", "- Storage Type:Dynamic."); }
				if (sWorldCtf->GetFlagStorageType()) { TC_LOG_INFO("server.loading", "- Storage Type:PreStored."); }
				if (!sWorldCtf->GetFlagReset()) { TC_LOG_INFO("server.loading", "- Flag morphs to winning team."); }
				if (sWorldCtf->GetFlagReset()) { TC_LOG_INFO("server.loading", "- Flag resets at each capture."); }

				TC_LOG_INFO("server.loading", "- Player aura check timer duration:%u seconds.", sWorldCtf->GetPlayerAuraCheckerTimer());
				TC_LOG_INFO("server.loading", "- World Flag Scale Size :%.2f.", sWorldCtf->GetDefaultWorldFlagScale());
				TC_LOG_INFO("server.loading", "- Minimum required GM rank:%u.", sWorldCtf->GetRequiredGMMinimumRank());

				if (sWorldCtf->test) { TC_LOG_INFO("server.loading", ">>    <[{Test Mode Active}]>    <<"); }

			TC_LOG_INFO("server.loading", "___________________________________");

			sWorldCtf->WorldCtfScore[0].score = 0;
			sWorldCtf->WorldCtfScore[1].score = 0;
			sWorldCtf->SetActiveGO_Team_ID(2);
			sWorldCtf->SetWinningGameTime(0);

				if (flag_count >= 1) { sWorldCtf->GenerateNewRandomFlagGps(); }
				
				// Storing flag info  / SMSG_GAMEOBJECT_RESET_STATE
				WorldCtfFlagInfo_Elements& data5 = sWorldCtf->WorldCtfFlagInfo[0];
				data5.entry_id = sWorldCtf->GetDefaultWorldFlagID();
				data5.type = GAMEOBJECT_TYPE_GOOBER; // GAMEOBJECT_TYPE_BUTTON / GAMEOBJECT_TYPE_GOOBER / GAMEOBJECT_TYPE_GENERIC / GAMEOBJECT_TYPE_DOOR
				data5.display_id = 5912;
				data5.name = "Alliance"; 
				data5.size = sWorldCtf->GetDefaultWorldFlagScale();
				data5.faction_id = 2; // 2; // 35; // 83; // 2;

				WorldCtfFlagInfo_Elements& data6 = sWorldCtf->WorldCtfFlagInfo[1];
				data6.entry_id = sWorldCtf->GetDefaultWorldFlagID() + 1;
				data6.type = GAMEOBJECT_TYPE_GOOBER;
				data6.display_id = 5913;
				data6.name = "Horde";
				data6.size = sWorldCtf->GetDefaultWorldFlagScale();
				data6.faction_id = 1; // 1; // 35; // 84; // 1;

				WorldCtfFlagInfo_Elements& data7 = sWorldCtf->WorldCtfFlagInfo[2];
				data7.entry_id = sWorldCtf->GetDefaultWorldFlagID() + 2;
				data7.type = GAMEOBJECT_TYPE_GOOBER;
				data7.display_id = 7953;
				data7.name = "Realm";
				data7.size = sWorldCtf->GetDefaultWorldFlagScale();
				data7.faction_id = 35;
		}
};

void WorldCtf::SendWorldMsg(uint8 type, std::string message)
{ // type [ 0 = global via hint system // 1 = team via hint system // 2 = bypass hint and announce to all]
	SessionMap sessions = sWorld->GetAllSessions();

	for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
	{

		if (!itr->second)
			continue;

		Player *player = itr->second->GetPlayer();

		if ((player->GetTeamId() == sWorldCtf->GetActiveGO_Team_ID() && sWorldCtf->GetHintSystem() == 1) || (sWorldCtf->GetHintSystem() == 2 && type == 0) || type == 2)
		{
			ChatHandler(player->GetSession()).PSendSysMessage(message.c_str());
		}
	}

};

void WorldCtf::UpdateGameObject(GameObject* go)
{
	uint32 defaultflagid = sWorldCtf->GetDefaultWorldFlagID();
	uint32 ActiveGO_ID = sWorldCtf->GetActiveGO_ID();

	if (sWorldCtf->GetFlagReset())
		{ 
			sWorldCtf->SetActiveGO_Team_ID(2); 
		}

	uint8 activeteamid = sWorldCtf->GetActiveGO_Team_ID();

		if (go->GetDisplayId() != sWorldCtf->WorldCtfFlagInfo[activeteamid].display_id)
		{
			go->SetDisplayId(sWorldCtf->WorldCtfFlagInfo[activeteamid].display_id);
			go->Respawn();

			if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "UPDATE_GO DISPLAY_ID:%u", sWorldCtf->WorldCtfFlagInfo[activeteamid].display_id); }
		}

		if (go->GetFaction() != sWorldCtf->WorldCtfFlagInfo[activeteamid].faction_id)
		{
			go->SetFaction(sWorldCtf->WorldCtfFlagInfo[activeteamid].faction_id);

			if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "UPDATE_GO FACTION_ID:%u", sWorldCtf->WorldCtfFlagInfo[activeteamid].faction_id); }
		}
	
		if (go->GetName() != sWorldCtf->WorldCtfFlagInfo[activeteamid].name)
		{
			go->SetName(sWorldCtf->WorldCtfFlagInfo[activeteamid].name);

			if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "UPDATE_GO NAME:%s", sWorldCtf->WorldCtfFlagInfo[activeteamid].name); }
		}

		if (go->GetObjectScale() != sWorldCtf->WorldCtfFlagInfo[activeteamid].size)
		{
			go->SetObjectScale(sWorldCtf->WorldCtfFlagInfo[activeteamid].size);

			if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "UPDATE_GO SCALE:%.2f", sWorldCtf->WorldCtfFlagInfo[activeteamid].size); }
		}

		if (go->GetPhaseMask() != PHASEMASK_NORMAL) // Adjust phase mask. PHASEMASK_NORMAL = 1 PHASEMASK_ANYWHERE = -1
		{
			go->SetPhaseMask(PHASEMASK_NORMAL, true);

			std::string msg = "The " + sWorldCtf->WorldCtfFlagInfo[activeteamid].name + " World Flag has spawned.";

			sWorldCtf->SendWorldMsg(2, msg);

			msg = "At " + sWorldCtf->WorldFlagGps[defaultflagid].name; // activeteamid

			sWorldCtf->SendWorldMsg(1, msg);

			if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "UPDATE_GO PHASE_MASK"); }
		}
}

class WCTF_Reset_Timer : public BasicEvent
{
public:
	WCTF_Reset_Timer(Player* player) : player(player)
	{
		uint64 current_time = GameTime::GetGameTime();

		player->m_Events.AddEvent(this, player->m_Events.CalculateTime(sWorldCtf->GetPlayerAuraCheckerTimer() * 1000)); // timed events are in ms while everything else is stored in seconds...
	}

	bool Execute(uint64, uint32) override
	{
		if (player->IsInWorld()) // here we will add catch-22's
		{
			uint32 guid = player->GetGUID();

			if (sWorldCtf->WorldCtfPlayerInfo[guid].aura)
			{
				uint32 aura_id = sWorldCtf->WorldCtfAura[player->GetTeamId()].aura;

					if (!player->HasAura(aura_id)) { player->AddAura(aura_id, player); }

					// Checking player eligibility to continue to carry flag aura.

					if (player->IsGameMaster())
					{
						ChatHandler(player->GetSession()).PSendSysMessage("You are in GM mode. Aura removed.");

						sWorldCtf->ResetPlayer(player);

						return true;
					}

					if (sWorldCtf->WorldCtfPlayerInfo[guid].aura) { new WCTF_Reset_Timer(player); }
			}
		}
		return true;
	}

	Player* player;
};

class WCTF_Team_Flag : public GameObjectScript
{
public: WCTF_Team_Flag() : GameObjectScript("WCTF_Team_Flag") { }

		bool OnGossipHello(Player* player, GameObject* go) // override
		{
			if (player->IsGameMaster())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You are in GM mode. Exit GM mode to enjoy.|r");

				return true;
			}
			else
			{
				auto team_id = player->GetTeamId();
				uint32 aura_id = sWorldCtf->WorldCtfAura[player->GetTeamId()].aura;
				
				if (!player->HasAura(aura_id)) { player->AddAura(aura_id, player); }

				sWorldCtf->WorldCtfPlayerInfo[player->GetGUID()].aura = true;

				new WCTF_Reset_Timer(player);

				return true;
			}
		}
};

class WCTF_World_Flag : public GameObjectScript
{
	public: WCTF_World_Flag() : GameObjectScript("WCTF_World_Flag") { }

		 bool OnGossipHello(Player* player, GameObject* go) // override // virtual
		{
			if (player->IsGameMaster())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You are in GM mode. Exit GM mode to enjoy.|r");

				return true;
			}
			else
			{
				auto team_id = player->GetTeamId();

					if(sWorldCtf->GetRequireFlagAura() && !player->HasAura(sWorldCtf->WorldCtfAura[team_id].aura))
					{
						ChatHandler(player->GetSession()).PSendSysMessage("You need your `Flag Carrier` aura to tag this flag.");

						return true;
					}

					if (!sWorldCtf->GetRequireFlagAura() || player->HasAura(sWorldCtf->WorldCtfAura[team_id].aura))
					{
						if (team_id != sWorldCtf->GetActiveGO_Team_ID())
						{
							uint64 currTime = GameTime::GetGameTime();
							uint32 guid = player->GetGUID();

							sWorldCtf->WorldCtfScore[team_id].score += 1;
							sWorldCtf->WorldCtfPlayerInfo[guid].captures += 1;

							sWorldCtf->ResetPlayer(player);
							sWorldCtf->ResetAllPlayers();

							std::string msg1 = player->GetName() + " of the " + sWorldCtf->WorldCtfFlagInfo[team_id].name + " has claimed the World flag.";

							std::string msg2 = "Current Score - Alliance:" + sWorldCtf->ConvertNumberToString(sWorldCtf->WorldCtfScore[0].score);
							msg2 += " || Horde:" + sWorldCtf->ConvertNumberToString(sWorldCtf->WorldCtfScore[1].score);

							ChatHandler(player->GetSession()).PSendSysMessage("Captures:%u", sWorldCtf->WorldCtfPlayerInfo[guid].captures);

								if (!sWorldCtf->GetFlagReset()) 
								{ 
									sWorldCtf->SetActiveGO_Team_ID(team_id); 
								} 
								else 
								{ 
									sWorldCtf->SetActiveGO_Team_ID(2); 
								}

							sWorldCtf->SetWinningGameTime(currTime);
							sWorldCtf->GenerateCoolDownTimer();
							sWorldCtf->GenerateNewRandomFlagGps();
							
							sWorldCtf->SendWorldMsg(2, msg1);
							sWorldCtf->SendWorldMsg(2, msg2);

							go->SetPhaseMask(0, true);
						}

					}
			}

			return true;
		}

		struct World_Flag : public GameObjectAI
		{

			World_Flag(GameObject* go) : GameObjectAI(go) { } 

				void UpdateAI(uint32 diff) // override // This function updates every 1000 (I believe) and is used for the timers, etc
				{
					uint32 guid = go->GetSpawnId();

						if(!sWorldCtf->GetFlagCtfID(guid))
						{
							if (sWorldCtf->test) { TC_LOG_INFO("server.loading", "NEW_GO_UPDATE_AI GUID:%u", guid); }
					
							sWorldCtf->UpdateWorldFlagDataBase(0, guid, sWorldCtf->WorldCtfFlagInfo[2].name, go->GetMapId(), go->GetAreaId(), go->GetZoneId(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation());
						}
						else
						{
							uint32 defaultflagid = sWorldCtf->GetDefaultWorldFlagID();
							uint8 activeteamid = sWorldCtf->GetActiveGO_Team_ID();

							uint64 currTime = GameTime::GetGameTime();
							uint32 delay = sWorldCtf->GetCooldownTimer();
							uint64 winTime = sWorldCtf->GetWinningGameTime();

							if (guid == sWorldCtf->WorldFlagGps[sWorldCtf->GetActiveGO_ID()].guid)
							{
								if (currTime > (winTime + uint64(delay)))
								{
									sWorldCtf->UpdateGameObject(go);
								}
							}
							else
							{
								go->SetPhaseMask(0, true); // PHASEMASK_ANYWHERE -1
							}
						}
				}
		};

		GameObjectAI* GetAI(GameObject* go) const override
		{
			return new World_Flag(go);
		}
};

void WorldCtf::ResetPlayer(Player* player)
{
	uint32 guid = player->GetGUID();

	if (sWorldCtf->WorldCtfPlayerInfo[guid].aura)
	{
		sWorldCtf->WorldCtfPlayerInfo[guid].aura = false;
		player->RemoveAura(sWorldCtf->WorldCtfAura[player->GetTeamId()].aura);
	}
}

void WorldCtf::ResetAllPlayers()
{
	SessionMap sessions = sWorld->GetAllSessions();

	for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
	{

		if (!itr->second)
			continue;

		Player *player = itr->second->GetPlayer();

		auto team_id = player->GetTeamId();
		uint32 guid = player->GetGUID();

		if (sWorldCtf->WorldCtfPlayerInfo[guid].aura)
		{
			sWorldCtf->WorldCtfPlayerInfo[guid].aura = false;
			player->RemoveAura(sWorldCtf->WorldCtfAura[team_id].aura);
		}
	}
}

class WCTF_Player_Actions : public PlayerScript
{
public: WCTF_Player_Actions() : PlayerScript("WCTF_Player_Actions") { };

		int BannedAreaZone[1][3] = { {0,0,0}, }; // {{map_id, area_id, zone_id}, {map_id, area_id, zone_id}, ... . .  .   .  .}

		virtual void OnLogout(Player* player)
		{ 
			sWorldCtf->WorldCtfPlayerInfo.erase(player->GetGUID());
		}

		virtual void OnLogin(Player* player, bool firstLogin)
		{
			WorldCtfPlayerInfo_Elements& data = sWorldCtf->WorldCtfPlayerInfo[player->GetGUID()];
			data.aura = false;
			data.captures = 0;
		}
		
		void OnPVPKill(Player* killer, Player* killed)
		{
			sWorldCtf->ResetPlayer(killed);
		}

		void OnPlayerKilledByCreature(Creature* killer, Player* killed)
		{
			sWorldCtf->ResetPlayer(killed);
		}

		virtual void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
		{
			uint32 guid = player->GetGUID();
			uint32 aura_id = sWorldCtf->WorldCtfAura[player->GetTeamId()].aura;

				if (sWorldCtf->WorldCtfPlayerInfo[guid].aura)
				{
					if (!player->HasAura(aura_id)) { player->AddAura(aura_id, player); }

					for (uint8 i = 0; i < (sizeof(BannedAreaZone) / sizeof(*BannedAreaZone)); i++)
					{
						if (BannedAreaZone[i][1] == player->GetMapId() && BannedAreaZone[i][2] == player->GetAreaId() && BannedAreaZone[i][3] == player->GetZoneId())
						{
							sWorldCtf->ResetPlayer(player);
							break;
						}
					}
				}
		}
};

class WCTF_commands : public CommandScript
{
public:
	WCTF_commands() : CommandScript("WCTF_commands") { }

	std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> WCTFSpawnCommandTable =
		{
			{ "world",		rbac::RBAC_IN_GRANTED_LIST, true, &HandleWCTFSpawnWorldFlagCommand, "world capture-the-flag command to display settings." },
		};

		static std::vector<ChatCommand> WCTFCommandTable =
		{
			{ "setup",		rbac::RBAC_IN_GRANTED_LIST, true, &HandleWCTFSetupCommand, "world capture-the-flag command to display current settings for players." },
			{ "tele",		rbac::RBAC_PERM_COMMAND_SERVER, true, &HandleWCTFTeleCommand, "use .tele x where x is the id or null to tele to current active flag." },
			{ "cycle",		rbac::RBAC_PERM_COMMAND_SERVER, true, &HandleWCTFCycleCommand, "use to cycle a new current active flag ." },
			{ "random",		rbac::RBAC_PERM_COMMAND_SERVER, true, &HandleWCTFRandomCommand, "use to set random spawning on/off." },
			{ "reset",		rbac::RBAC_PERM_COMMAND_SERVER, true, &HandleWCTFFlagResetCommand, "use to set `reset flag team after capture` on/off." },
			{ "despawn",	rbac::RBAC_PERM_COMMAND_SERVER, true, &HandleWCTFDeSpawnWorldFlagCommand, "despawn nearby World Flag. must be close to it. use teleport command." },
			{ "spawn",		rbac::RBAC_PERM_COMMAND_SERVER, true, NULL, "spawn flag commands.", WCTFSpawnCommandTable },
		};

		static std::vector<ChatCommand> commandTable =
		{
			{ "wctf", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "custom world capture the flag commands by Grumbo.", WCTFCommandTable },
		};

		return commandTable;
	}

	static bool HandleWCTFSetupCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		uint32 guid = player->GetGUID();

		ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");
		ChatHandler(player->GetSession()).PSendSysMessage("     World Capture the Flag settings           ");
		ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");

		ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");
		ChatHandler(player->GetSession()).PSendSysMessage("                  Global data                  ");
		ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");

		if (!sWorldCtf->GetWilOWhisp()) { ChatHandler(player->GetSession()).PSendSysMessage("- Single Fixed flag spawning."); }
		if (sWorldCtf->GetWilOWhisp()) { ChatHandler(player->GetSession()).PSendSysMessage("- Wil-o-Whisp:Active. (Random spawning)"); }
		if (sWorldCtf->GetTimerType()) { ChatHandler(player->GetSession()).PSendSysMessage("- Random Cooldown:Active."); }
		if (sWorldCtf->GetHintSystem() == 0) { ChatHandler(player->GetSession()).PSendSysMessage("- Hint System:Idle."); }
		if (sWorldCtf->GetHintSystem() == 1) { ChatHandler(player->GetSession()).PSendSysMessage("- Hint System:Hint to Holding Team."); }
		if (sWorldCtf->GetHintSystem() == 2) { ChatHandler(player->GetSession()).PSendSysMessage("- Hint System:Hint to both Teams."); }
		if (!sWorldCtf->GetRequireFlagAura()) { ChatHandler(player->GetSession()).PSendSysMessage("- Flag Holder Aura:ByPassed. (instant captures allowed)"); }
		if (sWorldCtf->GetRequireFlagAura()) { ChatHandler(player->GetSession()).PSendSysMessage("- Flag Holder Aura:required."); }
		if (!sWorldCtf->GetFlagReset()) { ChatHandler(player->GetSession()).PSendSysMessage("- Flags owned by winning team. defending required."); }
		if (sWorldCtf->GetFlagReset()) { ChatHandler(player->GetSession()).PSendSysMessage("- Flag resets each round. Recapture required."); }
		if (!sWorldCtf->GetFlagStorageType()) { ChatHandler(player->GetSession()).PSendSysMessage("- Storage Type:Dynamic."); }
		if (sWorldCtf->GetFlagStorageType()) { ChatHandler(player->GetSession()).PSendSysMessage("- Storage Type:PreStored."); }

		ChatHandler(player->GetSession()).PSendSysMessage("- Player aura check timer duration :%u seconds.", sWorldCtf->GetPlayerAuraCheckerTimer() / 1000);

		ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");
		ChatHandler(player->GetSession()).PSendSysMessage("                 Player data                   ");
		ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");

		if (sWorldCtf->WorldCtfPlayerInfo[guid].aura) { ChatHandler(player->GetSession()).PSendSysMessage("Aura  :Active"); }

		ChatHandler(player->GetSession()).PSendSysMessage("- Current Flag Holder:%s", (sWorldCtf->WorldCtfFlagInfo[sWorldCtf->GetActiveGO_Team_ID()].name).c_str());
		ChatHandler(player->GetSession()).PSendSysMessage("- Captures:%u", sWorldCtf->WorldCtfPlayerInfo[guid].captures);
		ChatHandler(player->GetSession()).PSendSysMessage("- Total Active Flags:%u", sWorldCtf->WorldFlagGps.size());
		ChatHandler(player->GetSession()).PSendSysMessage("- Current Score:");
		ChatHandler(player->GetSession()).PSendSysMessage("- Alliance:%u || Horde:%u", sWorldCtf->WorldCtfScore[0].score, sWorldCtf->WorldCtfScore[1].score);

		if (handler->GetSession()->GetSecurity() >= sWorldCtf->GetRequiredGMMinimumRank())
		{ 
			ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");
			ChatHandler(player->GetSession()).PSendSysMessage("                    GM data                    ");
			ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");

			ChatHandler(player->GetSession()).PSendSysMessage("- Minimum required GM rank:%u.", sWorldCtf->GetRequiredGMMinimumRank());
		}

		ChatHandler(player->GetSession()).PSendSysMessage("-----------------------------------------------");

		return true;
	}

	static bool HandleWCTFTeleCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player->IsGameMaster())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You need to be in GM mode.");
		}
		else
		{
			if (handler->GetSession()->GetSecurity() < sWorldCtf->GetRequiredGMMinimumRank())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You need to be GM with rank:%u.", sWorldCtf->GetRequiredGMMinimumRank());
			}
			else
			{
				uint32 id = 1;

				if (*args)
					id = (uint32)atoi((char*)args);

				if (sWorldCtf->WorldFlagGps[id].id == id)
				{
					player->TeleportTo(sWorldCtf->WorldFlagGps[id].map_id, sWorldCtf->WorldFlagGps[id].x, sWorldCtf->WorldFlagGps[id].y, sWorldCtf->WorldFlagGps[id].z, sWorldCtf->WorldFlagGps[id].o);
				}
				else
				{
					ChatHandler(player->GetSession()).PSendSysMessage("Bad flag id:%u.", id);
				}
			}
		}
		return true;
	}

	static bool HandleWCTFSpawnWorldFlagCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player->IsGameMaster())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You need to be in GM mode.");
		}
		else
		{
			if (handler->GetSession()->GetSecurity() < sWorldCtf->GetRequiredGMMinimumRank())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You need to be GM with rank:%u.", sWorldCtf->GetRequiredGMMinimumRank());
			}
			else
			{
				sWorldCtf->AddNewWorldFlag(player);
			}
		}

		return true;
	}
	
	static bool HandleWCTFDeSpawnWorldFlagCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player->IsGameMaster())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You need to be in GM mode.");
		}
		else
		{
			if (handler->GetSession()->GetSecurity() < sWorldCtf->GetRequiredGMMinimumRank())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You need to be GM with rank:%u.", sWorldCtf->GetRequiredGMMinimumRank());
			}
			else
			{
				GameObject *object = player->FindNearestGameObject(sWorldCtf->GetDefaultWorldFlagID(), 5);

				if (!object)
				{
					ChatHandler(player->GetSession()).PSendSysMessage("You must stand closer to a flag.|r");
				}

				if (object)
				{
					object->RemoveFromWorld();
					object->DeleteFromDB();

					sWorldCtf->UpdateWorldFlagDataBase(1, object->GetSpawnId(), "", 0, 0, 0, 0.0, 0.0, 0.0, 0.0);
				}
			}
		}
		return true;
	}

	static bool HandleWCTFCycleCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player->IsGameMaster())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You need to be in GM mode.");
		}
		else
		{
			if (handler->GetSession()->GetSecurity() < sWorldCtf->GetRequiredGMMinimumRank())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You need to be GM with rank:%u.", sWorldCtf->GetRequiredGMMinimumRank());
			}
			else
			{
				sWorldCtf->GenerateNewRandomFlagGps();
			}
		}
		return true;
	}

	static bool HandleWCTFRandomCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player->IsGameMaster())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You need to be in GM mode.");
		}
		else
		{
			if (handler->GetSession()->GetSecurity() < sWorldCtf->GetRequiredGMMinimumRank())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You need to be GM with rank:%u.", sWorldCtf->GetRequiredGMMinimumRank());
			}
			else
			{
				if (*args)
				{
					std::string param = (char*)args;

					if ((param == "on") || (param == "off"))
					{
						bool v;

						if (param == "on") { v = true; }
						if (param == "off") { v = false; }

						sWorldCtf->SetWilOWhisp(v);
					}
				}
			}
		}
		return true;
	}

	static bool HandleWCTFFlagResetCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player->IsGameMaster())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You need to be in GM mode.");
		}
		else
		{
			if (handler->GetSession()->GetSecurity() < sWorldCtf->GetRequiredGMMinimumRank())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You need to be GM with rank:%u.", sWorldCtf->GetRequiredGMMinimumRank());
			}
			else
			{
				if (*args)
				{
					std::string param = (char*)args;

					if ((param == "on") || (param == "off"))
					{
						bool v;

							if (param == "on") { v = true; }
							if (param == "off") { v = false; }

						sWorldCtf->SetFlagReset(v);
					}
				}
			}
		}
		return true;
	}
};

void AddSC_Grumboz_World_Ctf()
{
	new WCTF_Load_Conf;
	new WCTF_Team_Flag;
	new WCTF_World_Flag;
	new WCTF_Player_Actions;
	new WCTF_commands;
}

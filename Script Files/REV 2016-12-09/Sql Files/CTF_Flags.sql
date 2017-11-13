-- By Slp13at420 of EmuDevs.com
-- for `Grumbo'z Capture the Flag`

REPLACE INTO `gameobject_template` (`entry`, `type`, `displayId`, `name`, `IconName`, `size`, `Data3`, `ScriptName`, `VerifiedBuild`) VALUES 
(600000, 10, 5912, 'Alliance Team Flag', 'interact', 2, 3000, 'WCTF_Team_Flag', 12340),
(600001, 10, 5913, 'Horde Team Flag', 'interact', 2, 3000, 'WCTF_Team_Flag', 12340),
(600002, 10, 7953, 'Realm World Flag', 'interact', 25, 0, 'WCTF_World_Flag', 12340);

REPLACE INTO `gameobject_template_addon` (`entry`, `faction`, `VerifiedBuild`) VALUES 
(600000, 1, 1234),
(600001, 2, 1234),
(600002, 35, 1234);

-- team `aura` flags spawned in Throne rooms IronForge/Orgrimmar
REPLACE INTO `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES 
(600000, 0, 0, 0, 1, 1, -4858.69, -1031.16, 502.188, 5.38339, 0, 0, 0.434874, -0.900491, 300, 0, 1, 0),
(600001, 1, 0, 0, 1, 1, 1921.07, -4140.72, 40.6032, 4.76124, 0, 0, 0.689628, -0.724164, 300, 0, 1, 0);

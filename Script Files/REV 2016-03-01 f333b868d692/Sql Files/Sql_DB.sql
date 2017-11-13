-- By Slp13at420	of EmuDevs.com
-- table for `Grumbo'z World Capture the Flag`

DROP TABLE IF EXISTS `grumboz_wctf_flag_gps`;
CREATE TABLE `grumboz_wctf_flag_gps` (
	`guid` BIGINT(20) UNSIGNED NOT NULL COMMENT 'guid of flag',
	`name` VARCHAR(255) NOT NULL,
	`map_id` MEDIUMINT(8) UNSIGNED NOT NULL COMMENT 'map id',
	`area_id` MEDIUMINT(8) UNSIGNED NOT NULL COMMENT 'area id',
	`zone id` MEDIUMINT(8) UNSIGNED NOT NULL COMMENT 'zone id',
	`x` FLOAT NOT NULL COMMENT 'x',
	`y` FLOAT NOT NULL COMMENT 'y',
	`z` FLOAT NOT NULL COMMENT 'z',
	`o` FLOAT NOT NULL COMMENT 'o',
	UNIQUE INDEX `guid` (`guid`)
)
COLLATE='utf8_general_ci'
ENGINE=InnoDB
;

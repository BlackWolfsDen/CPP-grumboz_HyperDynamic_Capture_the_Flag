# CPP-grumboz_HyperDynamic_Capture_the_Flag
Grumbo'z
HyperDynamic-MultiPhasic-InterDimentional
Capture the Flag
System
 

Ver. 1.0c/12-07-2016
 

 
What is this?
This allows both teams to search the world for the World flag. When a player finds the flag they must tag it to claim it for there team.

Adjustable flag reset system.
When a team claims a flag, it can either morph to the teams flag, OR reset allowing both teams to try and claim it. When a flag morphs to a teams World Flag then it is that teams objective to hurry and seek it out to defend it from the apposing team.

Adjustable `Flag Carrier` aura requirement.

This system can be set to require a player to have the `flag carrier` aura when they try to capture a World Flag. Or it will allow players to just directly seek-out and capture a World Flag without going to there teams throne room in IronForge/orgrimmar.

Adjustable `Wil-o-Whisp` random spawning system.

This System can be set to spawn at 1 fixed location OR spawn randomly around your world.


Adjustable cooldown timer.

A cooldown timer can be set for between rounds or set so the flag instant respawns. This can also be set to use a randomly generated cooldown timer.


Adjustable GameObject ID's.
If the default go id's don't work fdor you then you can just change them in the sql AND the conf file.


Adjustable World Flag scale.
You can Set the scale size of the World Flags in the conf file. the default scale is 30.00 . you can set them larger or smaller depending on how easy/hard you want the flags visibility.


Adjustable hint system.
You can set this to broadcast to the (holding team, both teams, or none) , The map name.


Adjustable Flag data storing system.
Every time you add a new default world flag to the system. its vital data gets stored to a table for easy cross checking. You can set it to also store this info to your world DB in a custom table so it can be preloaded during server startups. when you set this to dynamic then it will only store the flags data to the table when ever a flag becomes active by a map/grid loading.


Adjustable GM command minimum access.
You can adjust the minimum required GM rank to access and use the custom GM commands.


Scoring System.
This system will keep score and display current score during each capture along with keeping score of each players capture count. player captures will reset during player relog's. Team scores reset after server restarts.


Info Command.
Players/GM's can now use `.wctf setup` to show them how the system is setup while also showing GM's gm settings.


Plug n Play System.
Just install the script as any other CPP script. add the 3 GameObjects to your DB plus 2 pre spawned `Team` aura flags. Add the custom table to your world db(Yea multi-realm support). Add the custom entries for your worldserver.conf file. start server, add flags , kill the enemy. :)


Easy to add World Flag's.
just use`.gob add xxx` or the built in command `.wctf spawn world` command and a default world flag will spawn but quickly vanish unless it happens to get picked as a `Flag-in-Play` then it will become visible.


Easy to adjust settings :).
all settings listed above can easily be adjusted via the world conf custom entry. included.
 


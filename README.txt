# Prerequisites
	- file1.txt : for 'card flip' game  
	- short_input.txt : for 'practice short sentence' game 
	- term.txt : for 'acid rain' game

	- Library pthread, curses

# Execution
	- Type command 'gcc -o [filename] game.c -lpthread -lcurses' to compile
	- Type command './[filename] [multicast_group_address] [port_number] [player_number]' to run game
		- [multicast_group_address] : 224.0.0.0 ~ 239.255.255.255 (ex. 225.1.1.2)
		- [player_number] : 1 or 2 (no exception!!!)

# Usage
	- Before running this code, scale to fullscreen your terminal
	- Single Play : Just play yourself!
	- Multi Play : 2 players have to join together and type "READY" (case-sensitive!!!)
				   [!CAUTION!] If one player is out of game and the other typed "READY", both players can't recognize whether other player is ready or not
	- Multi Play 
		Caution1. 2 players have to join together and type "READY" (case-sensitive!!!) 
				  If one player is out of game and the other typed "READY", both players can't recognize whether other player is ready or not
		Caution2. 2 players should use the same (W)LAN to play together on remote machines

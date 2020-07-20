# Instant_Melee

Start finding online SSBM matches as fast as possible by launching Project Slippi with only your GameCube controller!

## What It Does

For those who are constantly striving to be optimal in all things, you can now start Slippi with only the press of single GameCube button.

This program reads inputs from your GameCube Adapter and launches Project Slippi.

Simply grab your controller and press the START button and Dolphin will launch immediately.

Now you can find Slippi Online matches in as few frames as possible.

## How to Setup

1. Download the latest release [here](https://github.com/RucksP/Instant_Melee/releases/download/v0.9/Instant.Melee.Release.7z) 
2. Unzip Instant Melee.7z
3. Put the path to your Dolphin.exe in configs.txt (ex: C:/FM-Slippi/Dolphin.exe)\
   -Remember to use forward slashes, and use the full path to the exe, not just the folder its in!
4. Launch instant_melee.exe
5. (Optional) Set instant_melee to launch on startup\
    a) Press the windows key and R (Win + R)\
    b) Type "shell:startup" into the window that just popped up (without the quotes)\
    c) Create a shortcut for instant_melee and put it in the folder which just popped up
6. (Optional) Set Dolphin to auto-launch SSBM\
    a) Open Dolphin\
    b) Right click on SSBM and set it as your default iso\
    c) In config/general check the box next to "Start Default ISO on Launch"
   
If you follow all the steps above, you will perpetually be only 3 GCC inputs away from searching for a Slippi Match!

## Things that might cause this program to not work properly

1. **Having an incorrect path in config.txt**\
    -edit config.txt to contain your path \
    -remember to use the path to the exe, not just the folder it's in\
    -use forward slashes
2. Dolphin was already open when you launched instant_melee
3. Opening Dolphin normally when instant_melee is open (it will still work, but a 2nd dolphin process will open and instant_melee may exit)
4. Not having the adapter plugged in when the instant_melee was started
5. Unplugging your adapter and plugging it back in while instant_melee is running
6. Trying to start Slippi with a controller that is not plugged into the first port
7. Being on Linux or Mac
  
If none of these helped fix your problem, check logs.txt and send me a message

## Why?

"The Melee community is full of visionaries. We don't like to settle for 'good enough' " - roughtongue5

"Because we strive for optimal" - PM_ME_THEM_BOOTS

"Because its *possible*" - Dweebl

"An engineer will spend 2 weeks trying to save 5 minutes" - My High School Physics Teacher

"Because when someone asks me to play melee I instinctively grab my controller and lean back in my chair. Unfortunately this creates the problem where I have to lean back forward and click on the dolphin icon to load up the game. Sometimes I have to alt-tab too!" - Me

V1.0 GUI
How many times were you working on a project and told yourself you would stop after some time but never did? No need to answer, the solution is here.
Work-Break, your guardian angel that stops you from overworking is back and it is more powerful than ever, coming with an easy to use GUI and the freedom to modify everything setting with ease is here.

What is new?
- GUI written in Qt Community (C++)
  * (Only Windows at the moment is working because of the file_parser class, will update for macOS and Linux systems in next version)
- Processes, the time period of working and of the break and the song name/path are now stored between sessions
  * (Stored in (Windows install drive):/ProgramData/Work-Break/conf.ini)
- Ability to modify every setting using the GUI or the conf.ini file
  * (In case you break the conf.ini do not worry, the file_parser is able to detect and set it back to default)
- Ability to play both .mp3 and .wav files when taking a break (or none)
- Full Unicode support
- Code split between classes to enhance readability
- Optimized performance

Known bugs:
- Not able to detect all processes, a fix is in the works already
- In case a process is opened multiple times, its timer will get incremented every second by the number of processes opened, a fix is in the works already

Fixed bugs:
- App could be run more than once if the name was changed.





(old version)
V0.9 CLI
Do you want to take breaks when working on a Word, Excel, Access or PowerPoint project and go outside or make your bed?

Look no further, this app will basically block every input to your PC after the time set in settings.ini (or default, 50min work and 15min break)!!!
Also, it will play a cool song of your choosing, just make sure it is in the same folder as the executable and that it is named "BreakSound.wav"! exactly like that or it will default to the windows default error sound on a loop and you will go crazy
it must be a wav :O!!!
After the break time is over the computer unblocks and you can go back to your boring stuff.

The app needs to be in administrator mode otherwise it just wont block your input and it will tell you that it cant block because????

FEATURES:
- Good error handling, that is why, the app checks for every case where something could go wrong and it tells the user.
- Good memory handling!! YUP, no pointers left in the air or handles not closed or anything!!!
- Good performance! It is written in C++23, what do you expect? also I am a good developer (I like to tell myself that)
- Great customization!! You can set your own song and times, what more do you want???
- Not annoying!! YES!! because you cannot see the windows unless an error has come up or its time for the break
- Only one app possible at runtime! (unless you prick dont run it two times with different names at the same time, then the double blocking is on you)

WHY???
my gf wanted the app so I made it

Jokes aside, I tried my best to code the app while keeping performance at a maximum and readability too. I tried to keep it simple and concise so anyone can read it and I tried to follow the recommended C++23 standard of programming.
Hope you like it.


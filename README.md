How many times were you working on a project and told yourself you would stop after some time but never did? No need to answer, the solution is here.
Work-Break, your guardian angel that stops you from overworking is back and it is more powerful than ever, coming with an easy to use GUI and the freedom to modify everything setting with ease is here.

![Work-Break-V1.0](https://i.imgur.com/ZxYdFXr.png)

What is new? (current V1.0 GUI, oldest V0.9 CLI, check commits for older version)
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
- Even better error handling and user-app communication.

Known bugs:
- Not able to detect all processes, a fix is in the works already
- In case a process is opened multiple times, its timer will get incremented every second by the number of processes opened, a fix is in the works already

Fixed bugs:
- App could be run more than once if the name was changed.

There is the executale version if you do not know how to setup Qt and compile the project but compiling it yourself is recommended!
The app must be elevated when running or BlockInput() will not work.

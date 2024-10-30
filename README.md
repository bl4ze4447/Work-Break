# Work-Break, the solution to overworking, now has an easy to use GUI. ![C++](https://img.shields.io/badge/Language-C++-green) ![Qt Community](https://img.shields.io/badge/GUI-Qt-blue) ![Love](https://img.shields.io/badge/Built_with-love❤️-red)

## What is new? 
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

![Work-Break-V1.0](https://i.imgur.com/ZxYdFXr.png) *Important: The app must be elevated when running or BlockInput() will not work.

## Known bugs:
- In case a process is opened multiple times, its timer will just reset since it thinks it was reopened, a fix is in the works already

## Fixed bugs:
- App could be run more than once if the name was changed.



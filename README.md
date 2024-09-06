# DoresPub

A pub management simulation game made in Unreal Engine 5

 <p align="center">
  <img src="https://github.com/profdambledore/DoresPub/blob/main/Images/DoresPubIconOne.png" />
</p>
 
In this project, I am creating a pub management simulation, where the player takes a birds-eye view of control.  Inspired by games like The Sims and Prison Archetech, the player can build walls, place objects, control staff and fufil orders to make their customers regualrs.

## Project Document
 
 [Link](https://github.com/profdambledore/DoresPub/blob/main/Documents/DoresPub.pdf)

 ## Latest Update
<table><tr><td valign="center" width="100%">

### v0.6e - Build Tool Mk4.4

 <p align="center">
  <img src="https://github.com/profdambledore/DoresPub/blob/main/Images/v06-005.PNG" />
</p>

-- UI - Build --

Added a new widget switcher, used to swap between erase mode display and normal mode display\
Renamed UpdateEraseButtonEnabled to UpdateEraseModeEnabled and also added a call to change the active widget of EraseModeSwitcher

-- Player - Tools --

Added a check for erase mode in OnTick()\
Added BuildToolEraseTick, which fires a trace to the mouse position, updates and snaps it to the selected snap size, updates LastPosition then calls GenerateBuildDisplay in the BuildToolDisplay
- Also updated PrimaryRelease with a similar code block

-- BuildToolDisplay --

Added function UpdateEraseMode, which sets bInEraseMode to the inputted argument\
Also made bInEraseMode protected.\
Added path for EraseMode in GenerateBuildDisplay, which is checked before the sub-tool switch\
Removed the erase mode code from CreateLineOnX\
Implemented utility function GetPointInsideRectange, which checks if an inputted point is between a min and max point of a rectange\
Added function GenerateEraseDisplay
- Starts as usual, by moving the BuildToolDisplay to the StartPosition
- Then calculates the minimum and maximum points of a rectangle from the StartPosition and EndPosition (a and d)
- Next, calculates how many static mesh components are needed in total for the size of the box 'ad'
- Then rounds down to the closest 250, 250, 1 vector
- Follows by checking if we should grab the X Wall, by checking if the bottom of the wall is inside the box, then check if the top of the wall is inside the box
-- If it is, set a static mesh to its position and mesh from the BuildData
- Repeat with the Y wall
- Finally clear any excess SMC's
- // Currently draws the box points, as well as the box the player has drawn via DrawDebugBox (see Images)

// The first iteration of the improved erase mode is implemented. Took a little while to implement as I didnt understand the c++ code from GetPointInsideRectange. Now erase mode isnt limited by the snapping of the walls and can be free selected. A wall just needs to be fully overlapped by the player's draw to capture it for deletion

// Next is to add options, such as partial overlap as well as floor erasure at once. Gonna implement the Cohen-Sutherland algorithm first, then look into the improvements

// Also the previous 3 commits should be titled 0.6, not 0.4

## Assets Used:
- Polygon Nightclubs by SyntyStudios
- Polygon Shops by SyntyStudios

## Programs Used:
- Unreal Engine 5.2 - Game Engine.
- Visual Studio 2022 - Code Editing.
- VS Code with LaTeX - Document Creation
- GitHub Desktop - Git Control. 

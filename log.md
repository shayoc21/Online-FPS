This is a big project, so I will use this text file to track progress and plan.

# Documentation

---
#### Building:

Ensure you have GLFW and GLEW, build with the "build" script

So far only tested in my dev environment on linux

----

# Development

----
## v0.1

    v0.1 is the first documented version. 
    As of v0.1 the game:
        - Loads a map from a wavefront .obj (right now its a copy of e1m1 from DOOM 1993)
        - Draws a skybox
        - Draws the map with a red brick texture
        - Allows the player to fly around and "sprint" with shift, though no gravity implemented yet
        - Applies a screen shader with chromatic aberration, vignette, film grain, and other aesthetic qualities

    v0.1 uses a BSP tree to optimise collision, this BSP tree is created at runtime, delaying the start of the game by a few seconds


#### Next Steps:
    
    The next steps are to create a proper level loader, to make texturing easier. I intend to use a level format that works well with BSP, similar to Quake's level format.

    I find that wavefront .obj, while flexible with tools like blender, overcomplicates texturing for my purposes. My obj loader also does not optimise with an index buffer, so this will be optimised.

    Beyond level loading, I intend next to add legitimate player movement with physics and acceleration, rather than naively incrementing the player position on key press.

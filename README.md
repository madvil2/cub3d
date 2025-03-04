# cub3D
A Wolfenstein 3D-inspired Raycasting Engine 🏰

## Table of Contents
- [Introduction](#introduction)
- [How It Works](#how-it-works)
- [Features](#features)
- [Technical Implementation](#technical-implementation)
- [Map Configuration](#map-configuration)
- [Installation](#installation)
- [Controls](#controls)
- [Challenges](#challenges)
- [Summary](#summary)

## Introduction
The cub3D project at 42 challenges students to create a realistic 3D graphical representation of a maze using raycasting principles, similar to the legendary Wolfenstein 3D game. This project focuses on graphics programming, optimization, and the mathematics behind 3D rendering.

## Game Requirements
The game must follow specific criteria:
- Map format: `.cub`
- Valid map elements:
  - `0`: Empty space
  - `1`: Wall
  - `N/S/E/W`: Player starting position and orientation
- Texture configuration for walls (NO, SO, WE, EA)
- Floor and ceiling colors in RGB format
- Proper error handling for invalid configurations

## How It Works

### Raycasting Engine
The game uses raycasting to create a 3D perspective:
1. For each vertical screen strip, a ray is cast from the player's position
2. The ray's direction is calculated based on the player's direction and the camera plane
3. Using DDA (Digital Differential Analysis) algorithm to find wall intersections
4. Wall height is calculated based on the perpendicular distance to avoid fisheye effect
5. Textures are mapped onto walls based on the intersection point

### Event Handling
- Keyboard input for movement and rotation
- Mouse control for view rotation
- Clean exit handling
- Collision detection with walls

## Features
- Textured walls with different textures for each direction
- Smooth player movement and rotation
- Collision detection
- Mini-map display
- Mouse-controlled camera
- Configurable resolution and window size

## Technical Implementation

### Graphics Engine
- Uses MinilibX for rendering
- Double buffering for smooth animation
- Optimized texture mapping
- Efficient ray calculation algorithms

### Memory Management
- Dynamic memory allocation for map loading
- Proper cleanup on exit
- Resource management for textures

## Map Configuration
Maps are defined in `.cub` files with the following structure:
```
NO ./path_to_north_texture.xpm
SO ./path_to_south_texture.xpm
WE ./path_to_west_texture.xpm
EA ./path_to_east_texture.xpm

F R,G,B    # Floor color
C R,G,B    # Ceiling color

111111
100101
101001
1000N1
111111
```

## Installation

### Dependencies
- MinilibX library
- C compiler (gcc/clang)
- Make

### Building the Game
```bash
git clone https://github.com/yourusername/cub3d.git
cd cub3d
make
```

## Controls
- `W/A/S/D`: Move forward/left/backward/right
- `←/→`: Rotate camera left/right
- Mouse movement: Look around
- `ESC`: Exit game

## Challenges
- Implementing efficient raycasting algorithms
- Managing texture mapping and wall rendering
- Optimizing performance for smooth gameplay
- Handling various edge cases in map parsing
- Implementing precise collision detection
- Coordinating mouse and keyboard controls

## Summary
This project demonstrates proficiency in:
- 3D graphics programming
- Mathematical concepts (vectors, trigonometry)
- Optimization techniques
- Event handling
- File parsing and validation
- Memory management

The cub3D project provides valuable experience in graphics programming while creating an engaging 3D environment. It combines mathematical precision with practical programming skills, resulting in a functional and entertaining game engine. 
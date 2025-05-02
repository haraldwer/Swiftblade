# Introduction
Swiftblade is a first-person movement-focused speedrunning game.

The game runs in a home-made engine that I call RayEngine, which I've built using Raylib, a C library for making games. 
Complete list of libraries / tools used:
 - Raylib 5.5
 - PhysX 5.1.0
 - Nakama 3.26.0
 - ImGui (docking branch)
 - Tracy 0.11.1
 - rapidjson
 - FastNoiseLite
 - Blender (for modelling)
 - Rider (IDE)

Here are some features of the engine: 
 - Dense, single-threaded ECS
 - Property serialization
 - Deferred rendering
 - A custom UI library
 - Resource management with hot reloading
 - Isolated engine instances
 - Fixed tick rate
 - A basic profiler
 - An input manager with input mapping

# Prerequsites
- CMake
- A c++ compiler

# Setup
1. Call ``RayEngine/gen.bat`` 
2. Open ``RayEngine/build/RayEngine.sln``
Or call CMake manually.

# Standards
Code: 
```
#define MACRO()

namespace Context
{
	class ExampleClass
	{
		void Func(Var InParameter)
		{
			Var variable
		}

		Var Member;
	}

	enum ExampleEnum
	{
		VALUE_0,
		VALUE_1,
		...
	}
}
```
Files:
```
content/Folder
	FileName.h
	FileName.cpp
	FileName.hpp
```

## Principles
No external dependencies
Interactions limited to sandbox environments
Examples:
 - Resource wrappers
 - Renderer converts MeshInstance data



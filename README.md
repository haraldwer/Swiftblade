# Introduction
Swiftblade is a first-person movement-focused speedrunning game.

The game runs in a home-made engine that I call RayEngine, which I've built using Raylib, a C library for making games. 
Complete list of libraries / tools used:
 - Raylib 5.5
 - PhysX 5.6.0
 - Nakama 3.26.0 (nakama-cpp 2.8.5)
 - ImGui (docking branch)
 - Tracy 0.11.1
 - rapidjson
 - FastNoiseLite
 - Blender (for modelling)
 - Rider (IDE)

Here are some features of the engine: 
 - Dense, single-threaded ECS
 - Property serialization
 - An opengl renderer
 - Custom UI layout code
 - Resource management with hot reloading
 - Isolated engine instances
 - Fixed tick rate
 - An input manager with input mapping

# Prerequsites
- CMake
- A c++ compiler (tested on msvc 17.14, g++-12 and clang-14)

# Setup
## Windows
1. Call ``RayEngine/gen.bat``
2. Open ``RayEngine/build/RayEngine.sln``
Or call CMake manually.

## Linux
Use CMake :) 

## Include-what-you-use
You can analyze headers with iwyu. [Check it out!](https://github.com/include-what-you-use/include-what-you-use) If you are lucky, clang and iwyu will already be correctly set up after downloading. Test by comparing ``clang -print-resource-dir`` to ``include-what-you-use -print-resource-dir``. These have to match exactly.

Otherwise (or if using custom clang-version):
- Find the path to your clang-version:
	+ ``clang-(version) -print-resource-dir`` -> path
- Compile iwyu with the path by adding these to your cmake command:
	+ ``-DIWYU_RESOURCE_RELATIVE_TO=clang``
	+ ``-DIWYU_RESOURCE_DIR=[path]``

Append these to your cmake command (can be done in the "profiles" section in Rider/CLion): 
``-DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=path/to/include-what-you-use``

If you are using a custom clang version you have to [create a fake gcc toolchain](https://stackoverflow.com/questions/41962611/how-to-select-a-particular-gcc-toolchain-in-clang) for iwyu:
``
mkdir $MYTOOLCHAIN
cd $MYTOOLCHAIN
ln -s /usr/include include
ln -s /usr/bin bin
mkdir -p lib/gcc/x86_64-linux-gnu/ # clang will deduce which gcc version based on what's available 
cd lib/gcc/x86_64-linux-gnu/
ln -s /path/to/gcc/$VERSION $VERSION
``
And finally, add to your cmake command:
``--gcc-toolchain=$MYTOOLCHAIN``

# Standards
Code: 
```
#define MACRO()

namespace Context
{
	class ExampleClass
	{
		void Func(Var InParameter, Var& OutParameter)
		{
			Var localVar;
			OutParameter = someVariable.DoSomething();
		}

		Var _memberVar;
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



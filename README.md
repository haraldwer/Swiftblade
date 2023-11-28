
# Prerequsites
- CMake
- A c++ compiler

# Setup
1. Call gen.bat
2. Open the project

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
Folder
	FileName.h
	FileName.cpp
	FileName.hpp
```

# Principles
No external dependencies
Interactions limited to sandbox environments
Examples:
 - Resource wrappers
 - Renderer converts MeshInstance data



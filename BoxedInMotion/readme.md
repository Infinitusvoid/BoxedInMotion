# BoxedInMotion

## Instalation quide

* Configutaion Properties > general > C++20 Standard (/std:c++20)  
  
#### Configuration Properties > C/C++ > Additional Include Directories

```
$(SolutionDir)External_libs\GLEW\glew-2.1.0\include
```
  
```
$(SolutionDir)External_libs\GLFW\glfw-3.3.8.bin.WIN64\include
```

```
$(SolutionDir)External_libs\glm_0_9_9_7\glm\
```

```
$(SolutionDir)External_libs\ASSIMP\include
```

```
$(SolutionDir)External_libs\glad\include
```

```
$(SolutionDir)External_libs\stb
```

```
$(SolutionDir)External_libs\miniaudio
```

```
$(SolutionDir)External_libs\tinyply-master\tinyply-master\source
```

```
$(SolutionDir)External_libs\plog\include
```

```
$(SolutionDir)External_libs\imgui-docking
```

```
$(SolutionDir)External_libs\FastNoiseLite
```
  
  
#### Configuration Properties > Linker > General > Additional Library Directories

```
$(SolutionDir)External_libs\GLEW\glew-2.1.0\lib\Release\x64
```

```
$(SolutionDir)External_libs\GLFW\glfw-3.3.8.bin.WIN64\lib-vc2022
```

```
$(SolutionDir)External_libs\ASSIMP\lib
```


#### Configuration Properties > Linker > Input > Additional Dependencies

```
opengl32.lib
```

```
glew32.lib
```

```
glfw3.lib
```

```
assimp-vc140-mt.lib
```

Be sure that inherit from parent of project default is checked



# You should change the way you think about video ? 


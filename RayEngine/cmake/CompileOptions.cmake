set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
set(CMAKE_BUILD_PARALLEL_LEVEL 8)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

# Shared properties
set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    CXX_STANDARD 26
    UNITY_BUILD false)

# Shared definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC 
  "$<$<CONFIG:Release>:NDEBUG>"
  "$<$<CONFIG:Debug>:_DEBUG>")

if(MSVC)
  
  message("-- Setting custom MSVC flags")
  
  # Shared compile options
  target_compile_options(${PROJECT_NAME} PUBLIC
    # "/Wall" # More warnings
    "/permissive" # Stricter types
  )
  
  # Debug compile options
  target_compile_options(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Debug>:/MDd>" # Multithreaded debud DLL
    "$<$<CONFIG:Debug>:/Od>" # Disable optimization
    #"$<$<CONFIG:Debug>:/RTC1->" # Disable runtime checks
    "$<$<CONFIG:Debug>:/Zi>" # Full debug info
  )
          
  # Release compile options 
  target_compile_options(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Release>:/fp:fast>" # Fast math
    "$<$<CONFIG:Release>:/MD>" # Multithreaded debug
    "$<$<CONFIG:Release>:/MP>" # Multi-processor compilation
    "$<$<CONFIG:Release>:/Oi>" # Intristic functions
    "$<$<CONFIG:Release>:/Ot>" # Favor fast code
    "$<$<CONFIG:Release>:/Oy>" # Frame pointer omission
    "$<$<CONFIG:Release>:/Ox>" # Maximize speed
    "$<$<CONFIG:Release>:/sdl->" # No SDL Checks
    #"$<$<CONFIG:Release>:/Qspectre>" # Prevent speculative execution side-channel attacks
    "$<$<CONFIG:Release>:/Gy>" # Function level linking
    "$<$<CONFIG:Release>:/GF>" # String pooling
    "$<$<CONFIG:Release>:/GT>" # Fiber safe optimizations
    "$<$<CONFIG:Release>:/GL>" # Whole program optimization
    "$<$<CONFIG:Release>:/GS->" # Disable buffer security
    "$<$<CONFIG:Release>:/Gw>" # Whole program global data optimization
  )
          
  # Debug link options
  target_link_options(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Debug>:/INCREMENTAL>" # Incremental linking in debug
  )
          
  # Release link options
  target_link_options(${PROJECT_NAME} PUBLIC 
    "$<$<CONFIG:Release>:/MANIFEST:NO>" # No manifest
    "$<$<CONFIG:Release>:/OPT:REF>" # Reference optimization
    "$<$<CONFIG:Release>:/OPT:ICF>" # COMDAT Folding
    "$<$<CONFIG:Release>:/INCREMENTAL:NO>" # No incremental linking in release
    "$<$<CONFIG:Release>:/OPT:LBR>" # Link-time branch optimization
    "$<$<CONFIG:Release>:/LTCG>" # Whole program optimization
  )
  
elseif (UNIX)

  message("-- Setting custom GCC / Clang flags")
  
  # Shared compile options
  target_compile_options(${PROJECT_NAME} PUBLIC 
    -fpermissive
    -Wall
    -Wshadow
    -Wpointer-arith
    -Wcast-align
    -Winit-self
    -Wuninitialized
    #-Weffc++
    #-Wfloat-equal
  )

  # Debug compile options
  target_compile_options(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:-O0> # No optimization
    $<$<CONFIG:Debug>:-g> # Full debug info
    $<$<CONFIG:Debug>:-fno-inline> # Avoid inlining
    $<$<CONFIG:Debug>:-fno-omit-frame-pointer> # Easier debugging/profiling
    $<$<CONFIG:Debug>:-gsplit-dwarf>) # Split debug -> faster linking

  # Release compile options
  target_compile_options(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Release>:-ffast-math>
    $<$<CONFIG:Release>:-Ofast> # Highest level of optimization
    $<$<CONFIG:Release>:-march=native> # Use host CPU instructions
    $<$<CONFIG:Release>:-flto> # Link-time optimization
    $<$<CONFIG:Release>:-fno-plt> # Faster function calls
    $<$<CONFIG:Release>:-funroll-loops>
    $<$<CONFIG:Release>:-fdata-sections -ffunction-sections> # Make --gc-sections effective
    $<$<CONFIG:Release>:-frename-registers -fomit-frame-pointer> # Optimize register allocation
  )
  
  # Release link options
  target_link_options(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Release>:-flto> # Link-time optimization
    $<$<CONFIG:Release>:-Wl,--gc-sections> # Remove unused code
    #$<$<CONFIG:Release>:-Wl,--icf=all> # Fold identical functions - on clang
  )
          
  if (NOT EMSCRIPTEN)
    if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
      message("-- Will produce -ftime-trace .json files")
      target_compile_options(${PROJECT_NAME} PUBLIC -ftime-trace) # clang trace
    endif ()
  endif()
endif ()
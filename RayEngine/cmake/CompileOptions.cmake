set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    CXX_STANDARD 26
    UNITY_BUILD false)

target_compile_definitions(${PROJECT_NAME} PUBLIC 
  "$<$<CONFIG:Release>:NDEBUG>"
  "$<$<CONFIG:Debug>:_DEBUG>"
  "TRACY_ENABLE")

if(MSVC)
  
  message("-- Setting custom MSVC flags")
  
  target_compile_options(${PROJECT_NAME} PUBLIC 
    "/fp:fast" # Fast math
    "$<$<CONFIG:Release>:/MD>" 
    "$<$<CONFIG:Debug>:/MDd>")

  target_compile_options(${PROJECT_NAME} PUBLIC
    "$<$<CONFIG:Release>:/Ob2>" # Inline function expansion
    "$<$<CONFIG:Release>:/Oi>" # Intristic functions
    "$<$<CONFIG:Release>:/Ot>" # Favor fast code
    "$<$<CONFIG:Release>:/Oy>" # Frame pointer omission
    "$<$<CONFIG:Release>:/O2>" # Maximize speed
    "$<$<CONFIG:Release>:/sdl->" # No SDL Checks
    "$<$<CONFIG:Release>:/permissive->" # Conformance
    "$<$<CONFIG:Release>:/GS->" # No security
    "$<$<CONFIG:Release>:/Gy>" # Function level linking
    "$<$<CONFIG:Release>:/GF>" # String pooling
    "$<$<CONFIG:Release>:/GT>" # Fiber safe optimizations
    "$<$<CONFIG:Release>:/GL>" # Whole program optimization
    "$<$<CONFIG:Release>:/Gw>") # Whole program global data optimization
    
  target_link_options(${PROJECT_NAME} PUBLIC 
    "$<$<CONFIG:Release>:/MANIFEST:NO>" # No manifest
    "$<$<CONFIG:Release>:/OPT:REF>" # Reference optimization
    "$<$<CONFIG:Release>:/OPT:ICF>" # COMDAT Folding
    "$<$<CONFIG:Release>:/INCREMENTAL:NO>" # No incremental linking
    "$<$<CONFIG:Release>:/LTCG>") # Whole program optimization
  
elseif (UNIX)

  message("-- Setting custom GCC / Clang flags")
  
  target_compile_options(${PROJECT_NAME} PUBLIC 
    -fpermissive
    -ffast-math
    -Wall
    -Wshadow
    -Wpointer-arith
    -Wcast-align
    -Winit-self
    -Wuninitialized
    #-Weffc++
    #-Wfloat-equal
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-Og>) # Highest level of optimization
  
  if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    message("-- Will produce -ftime-trace .json files")
    target_compile_options(${PROJECT_NAME} PUBLIC -ftime-trace) # clang trace
  endif ()
endif()

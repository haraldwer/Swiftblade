set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    CXX_STANDARD 20
    UNITY_BUILD true
)
target_compile_options(${PROJECT_NAME} PUBLIC /MP) # Multithreading
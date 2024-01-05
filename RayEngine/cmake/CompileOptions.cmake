set_target_properties(${PROJECT_NAME} 
  PROPERTIES 
    CXX_STANDARD 20
    UNITY_BUILD_MODE BATCH
    UNITY_BUILD_BATCH_SIZE 10)
target_compile_options(${PROJECT_NAME} PUBLIC /MP) # Multithreading
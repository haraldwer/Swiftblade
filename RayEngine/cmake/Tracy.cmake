FetchContent_Declare(
  tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy.git
  GIT_TAG 650c98ece70da9e155e7ba1c2b3ee16004aae118 # 0.12.1
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
set(TRACY_ENABLE ON) 
set(TRACY_ON_DEMAND ON)
FetchContent_MakeAvailable(tracy)

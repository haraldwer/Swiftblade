FetchContent_Declare(
  tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy.git
  GIT_TAG 5d542dc09f3d9378d005092a4ad446bd405f819a # 0.11.1
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
set(TRACY_ENABLE ON) 
set(TRACY_ON_DEMAND ON)
FetchContent_MakeAvailable(tracy)

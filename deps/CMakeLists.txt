include(ExternalProject)
ExternalProject_Add(librtprocess
  GIT_REPOSITORY    git@github.com:CarVac/librtprocess.git

  INSTALL_COMMAND cmake -E echo "[librtprocess] No install."
)

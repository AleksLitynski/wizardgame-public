
export SDL_VIDEODRIVER=x11

BUILD_DIR="cmake-build-debug"
mkdir -p $BUILD_DIR
pushd $BUILD_DIR

# paths to build tools provided by scoop
GPP_PATH=$(which g++)
GCC_PATH=$(which gcc)

# Set up build files
cmake .. \
    -DCMAKE_CXX_COMPILER="$GPP_PATH" \
    -DCMAKE_C_COMPILER="$GCC_PATH" \
    -GNinja

# Do the build
cmake --build .

popd
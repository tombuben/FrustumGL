#!/bin/bash
if ! pkg-config --exists sdl2; then
	echo "Missing the SDL2 devel library, please install it from your distribution."
	exit 1
fi

if ! pkg-config --exists gl; then
	echo "Missing the GL devel library, please install it from your distribution."
	exit 1
fi

if ! pkg-config --exists glew; then
	echo "Missing the GLEW devel library, please install it from your distribution."
	exit 1
fi

if ! pkg-config --exists glm; then
	echo "Missing the glm devel library, please install it from your distribution."
	exit 1
fi

cd FrustumGL
make
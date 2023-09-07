# OpenGL Shading and Texture Mapping

Welcome to the OpenGL Shading and Texture Mapping project! This repository extends your OpenGL skills by adding shading and texture mapping capabilities to a 3D sphere model. Whether you're a graphics enthusiast or looking for an engaging OpenGL project, this repository is for you.

## Overview

In this project, we've enhanced a 3D sphere model from a previous assignment with shading and texture mapping features. The goal is to create a visually appealing 3D rendering with realistic lighting and texture effects.

## Features

### General

- Utilizes perspective projection.
- Implements shader-based OpenGL with no deprecated functionality.
- Provides comprehensive source code documentation.

### User Interface

- Allows toggling between various rendering options using keyboard keys.

### Shading

- Implements shading with a directional light source.
- Supports Gouraud and Phong shading options (toggle with the 'S' key).
- Utilizes a modified-Phong illumination model.
- Enables users to control specular, diffuse, and ambient components individually (toggle with the 'O' key).
- Allows users to fix or move the light source along with the object ('L' key).
- Defines two material properties options: plastic and metallic ('M' key).
- Supports zoom in and out functionality ('Z' and 'W' keys).
- Enables depth testing and face culling to enhance rendering performance.

### Texture Mapping

- Utilizes ppm images as 2D texture maps for the sphere.
- Supports toggling between different texture images ('I' key).
- Parametrizes the sphere to assign texture coordinates.
- Adjusts texture filtering parameters for optimal quality, including mipmapping.

### Display Modes

- Offers three user-selectable modes: Wireframe, Shading, and Texture (toggle with the 'T' key).
- Default mode is shading.

### Input Files

- Provides texture images in ppm format for 2D texture mapping: earth.ppm and basketball.ppm.
- Requires implementing a custom ppm image loader for OpenGL.

## Getting Started

1. Clone this repository to your local machine.
2. Open the project using your preferred development environment.
3. Compile and run the application.
4. Explore the interactive 3D sphere with shading and texture mapping.

## Feedback and Contributions

Feedback and contributions are welcome! If you have suggestions, find a bug, or want to contribute, please create an issue or a pull request.

Have fun exploring shading and texture mapping in OpenGL!

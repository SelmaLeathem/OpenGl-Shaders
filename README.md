#  OpenGl and OpenGL Shader Sample Projects

Displays Code and Video Links for OpenGL and Shaders Code

## The Breezy Meadow

![Demonstration](https://raw.githubusercontent.com/SelmaLeathem/OpenGl-Shaders/main/TheMeadow.gif)

Made with OpenGL and shaders. A meadow with flowers, butterflies, and an apple tree is constructed from imported obj files. These objects are then manipulated in the vertex shader to perform the following motions:

1. The leaves on the tree shimmer in the wind.
1. The flowers oscillate in the wind.
1. Butterflies flap their wings and fly around the meadow.
1. An imported apple falls off the tree and rolls down a hill in the meadow.

* To start the wind breeze press the "w" key
* To drop the apple press the "a" key 

Textures and coloring/lighting is handled in the fragment shader.

To view a naratted video of the meadow see: https://media.oregonstate.edu/media/t/1_azchadwe

## In the Attic

![Demonstration](https://raw.githubusercontent.com/SelmaLeathem/OpenGl-Shaders/main/InTheAttic.gif)

An introductory project where different types of light sources and textures on imported objects using OpenGL was implemented.

Lighting can be switched on/off using keyboard keys:

* f,F = freeze/unfreeze motion
* 1 = turn off/on ambient light in plus XZ sector
* 2 = turn off/on ambient light in minus XZ sector
* l,L = turn off/on lamp light
* t,T = turn off/on train light
* r,R = turn off/on red spot lights

To view a naratted video of the project, see: https://media.oregonstate.edu/media/t/1_0206r23x

## Up and Away

![Demonstration](https://raw.githubusercontent.com/SelmaLeathem/OpenGl-Shaders/main/UpAndAway.gif)

A striped balloon obj file is imported and manipulated in the vertex and fragment shaders as follows:

1. In the vertex shader the balloon can be stretched and distorted.
1. In the fragment shader a spiral design on the front of the balloon shrinks, grows, distorts and changes color.

Keyboard commands include:
* a = animate everything
* s = stop all animation
* v = animate vertices - deform the shape of the balloon 
* f = animate fragments - animate the spiral 

To view a naratted video of the project, see:  https://media.oregonstate.edu/media/t/1_vu4ov7lk

## Bump and Cube Mapping, Refraction and Reflection Using Shaders

To view a naratted video of the project, see: https://media.oregonstate.edu/media/t/1_zh6v7m9t

1. In the vertex shader a quadrilateral is bent such as to create curtain pleats.
1. The fragment shader file implements the bump-mapped quasi-random creases and crevices in the pleats. In addition, the fragment shader calculates the reflection and refraction of the deformed and map-bumped quadrilateral with the surrounding cube-mapped walls.

## The Jellyfish

To view a naratted video of the jellyfish see:  https://media.oregonstate.edu/media/t/1_kknmgvdo

A jellyfish made up from Bezier curves that swims in the water using OpenGL.

The right-click menu can turn the curves or points on or off. 
To freeze the animation use the keyboard "f" key.


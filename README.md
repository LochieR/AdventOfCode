# Advent of Code _fast_

Using Vulkan compute shaders to run Advent of Code problems (which are usually very CPU intensive) on the GPU in parallel.
This unlocks faster speeds, although writing algorithms in GLSL can be difficult as it is a limited language.
Features such as strings do not exist in GLSL, so alternatives such as using int arrays must be used instead.

### Dependencies
- Vulkan SDK
# Spar Engine
A simple rendering engine built to showcase various DX11 techniques.

# Build and run
### Deps:
   1. xmake
   2. Visual Studio (works on other IDEs as well, but its preferable. Read more here if interested: https://xmake.io/mirror/plugin/builtin_plugins.html)

```
xmake project -k vsxmake
xmake run 
```
(yup this simple :D)

# Todo

1. Integrate ImGui to play with shaders/lights/textures/maps implemented subsequently
2. Texture, materials, lighting maps loading, depth stencil buffers config
3. Lights 
    - Point lights
    - Directional Lights
    - Spotlights
4. Model loading (gltf first)
5. Bling-Phong model
6. A complex scene with all features implemented as well as proper lighting
-------------------------------------------------------

**Milestone Checkpoint deadline by the end of the November '24**

-------------------------------------------------------
7. Shadows (mapping and point shadows)
8. HDR
9. Bloom
10. Deferred rendering (likely new branch)
11. SSAO and other AA techniques
12. Normal and Parallax mapping
13. PBR + IBL pipeline
-------------------------------------------------------

**Milestone Checkpoint deadline between 2nd and and 3rd week of December '24**

-------------------------------------------------------
14. Write a simple 3D game (if possible xD)

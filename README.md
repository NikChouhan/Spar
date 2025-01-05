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

- [x] Integrate ImGui to play with shaders/lights/textures/maps implemented subsequently
- [x] Textures
- [ ] Normal maps
- [x] Diffuse Maps
- [ ] Emmision and specular maps
- [x] Materials
- [ ] Lights 
    - [ ] Point lights
    - [ ] Directional Lights
    - [ ] Spotlights
- [x] Model loading (gltf first)
- [ ] Bling-Phong model
- [ ] A complex scene with all features implemented as well as proper lighting
-------------------------------------------------------

**Milestone Checkpoint deadline by the end of the November '24**

-------------------------------------------------------
- [ ] Shadows (mapping and point shadows)
- [ ] HDR
- [ ] Bloom
- [ ] Deferred rendering (likely new branch)
- [ ] SSAO and other AA techniques
- [ ] Normal and Parallax mapping
- [ ] PBR + IBL pipeline
-------------------------------------------------------

**Milestone Checkpoint deadline between 2nd and and 3rd week of December '24**

-------------------------------------------------------
- [ ] Write a simple 3D game (if possible xD)

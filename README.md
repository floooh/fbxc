# fbxc

A sorta-lika 'sed' for FBX file: takes an FBX file and a rule file as input, and
produces one or more much simpler JSON and blob files with scene structure
and vertex data for further processing by game engine specific content tools.

### Why

FBX is the de-facto data exchange file format for 3D content production tools, but
a typical FBX file is a big hairy mess of interconnected nodes
(basically an only slightly simplified dump of Maya's internal scene DAG). 

Most of the information in an FBX file is necessary for a data exchange format,
but is irrelevant for realtime engines. The FBX scene structure is designed
to preserve as much scene information as possible when moving data between
content creation tools, but is useless for efficient 3D rendering.

Writing a solid FBX exporter for 3D- (or game-) engines is **hard**, and
the exact data that should be extracted from an FBX file is different 
for each engine.

fbxc aims to take care of all the hard and performance-critical stuff as
a preprocessing step for engine-specific content tools without losing too
much flexibility.


### How

fbxc takes a rule file in a human-friendly format (e.g. YAML) and an FBX
file as input. The rule file describes what parts of the FBX file should
be exported how (details further down). Output is one (or more?) blob files for
vertex/index data and one (or more?) scene structure file which should
be both easility readable and parsable by a wide range of languages (e.g. JSON).

fbxc will be written in C++(11) and only depend on the FBX SDK. It will
compile and run on Windows, Linux and OSX using the fips cmake wrapper
(https://github.com/floooh/fips/)

Why C++: I would prefer Python for all tool stuff but that is too slow for 
vertex crunching  (here's an earlier, somewhat similar attempt of mine: 
https://github.com/floooh/drahtgitter). I failed to get the FBX SDK python
bindings to work together with a compiled python alternatives like PyPy, and
even if it worked it would be to complicated construct.

So: fbxc as compiled tool takes over the first 'tricky and expensive' part of 
the export process, and the output of fbxc can then be further processed by a 
friendlier tool in a dynamic scripting language that doesn't need to visit 
every single vertex.

### What

2 implementation steps, first the simple, static stuff, then the complicated
animation and character stuff.

- **Step 1**: static mesh data, transform hierarchy, material data, custom node
attributes
- **Step 2**: animation data (baked or unbaked), character data 
(skeleton, skinning information)

What about:

- rendered texture data (lightmaps, ...)
- mesh morphing
- scene info like lights, camera, ...
- ...?

Stuff that fbxc should take care of:

#### Mesh Processing:

- triangulate on the fly
- cleanup: remove duplicate vertices, degenerate triangles, ...
- vertex cache sorting
- configurable vertex component packing (normals as Byte4N, etc...)
- force vertex layout (add or remove vertex components)
- custom vertex components (additional uv layers, vertex colors, ...)
- sort triangles into material buckets (for multi-material-meshes)
- split meshes above 64k vertices(?)
- generate indices
- only care about triangle lists(?)
- face normals(?)
- adjacency information(?)

#### Scene Node Processing:

- filter nodes to consider, by node paths, node attributes, node types...
- flatten and reduce node hierarchy by material groups (or... is a material
group actually a 'node filter')
- ...but preserve hierarchy where desired (e.g. a tank is only a single
material, but needs a turret that can rotate, so 2 nodes)

### Samples:

Syntax may look completely different!

Sample rule file:

```yaml
# extract simple collide geometry from nodes under /collide, 
# just vertex position as float3 scaled by 0.01, everything collapsed 
# into a single mesh
---
# node path regex to filter all nodes below '/collide/' 
- part:
    name: collide
    filter:
        node: \/collide\/.*     # only process all nodes under '/collide/'
    mesh:
        indices: uint16     # or none, or uint32
        vertex-layout:
            - position:     # only interested in position, ignore all other components
                - format: float3    # ... as float3 
                - scale: 0.01       # ... and scaled down 100x
    hierarchy:  # what to do about the scene node hierarchy
        merge:  # merge nodes by specific rules (e.g. by materials, name regex, 
                # attribute value regex, ...
```

Meh, this doesn't look so hot...

(to be continued)


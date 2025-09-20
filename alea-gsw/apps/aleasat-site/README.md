### Script for generate 3d model
```bash
yarn dlx -p @gltf-transform/cli gltf-transform optimize .\sat6.glb sat6-optimized.glb --compress draco --texture-compress webp  
--texture-size 512
yarn dlx gltfjsx@6.2.16 sat6-optimized.glb
```
<div align="center">

  ## SHamalgam

  [![Stars](https://img.shields.io/github/stars/rei-2/Amalgam?style=for-the-badge&color=white&logo=github)](/../../stargazers)
  [![Discord](https://img.shields.io/discord/1227898008373297223?style=for-the-badge&color=blue&labelColor=grey&label=Discord&logo=discord)](https://discord.gg/RbP9DfkUhe)
  [![Workflow status](https://img.shields.io/github/actions/workflow/status/rei-2/Amalgam/msbuild.yml?branch=master&style=for-the-badge)](/../../actions)
  [![Commit activity](https://img.shields.io/github/commit-activity/m/rei-2/Amalgam?style=for-the-badge)](/../../commits/)
  
  [![Download](.github/assets/download.svg)](https://nightly.link/rei-2/Amalgam/workflows/msbuild/master/Amalgamx64Release.zip)
  [![PDB](.github/assets/pdb.svg)](https://nightly.link/rei-2/Amalgam/workflows/msbuild/master/Amalgamx64ReleasePDB.zip)
  [![Download AVX2](.github/assets/download_avx2.svg)](https://nightly.link/rei-2/Amalgam/workflows/msbuild/master/Amalgamx64ReleaseAVX2.zip)
  [![PDB AVX2](.github/assets/pdb.svg)](https://nightly.link/rei-2/Amalgam/workflows/msbuild/master/Amalgamx64ReleaseAVX2PDB.zip)
  <br>
  [![Freetype](.github/assets/freetype.svg)](https://nightly.link/rei-2/Amalgam/workflows/msbuild/master/Amalgamx64ReleaseFreetype.zip)
  [![PDB Freetype](.github/assets/pdb.svg)](https://nightly.link/rei-2/Amalgam/workflows/msbuild/master/Amalgamx64ReleaseFreetypePDB.zip)
  [![Freetype AVX2](.github/assets/freetype_avx2.svg)](https://nightly.link/rei-2/Amalgam/workflows/msbuild/master/Amalgamx64ReleaseFreetypeAVX2.zip)
  [![PDB Freetype AVX2](.github/assets/pdb.svg)](https://nightly.link/rei-2/Amalgam/workflows/msbuild/master/Amalgamx64ReleaseFreetypeAVX2PDB.zip)

  <sub>AVX2 may be faster than SSE2 though not all CPUs support it (`Steam > Help > System Information > Processor Information > AVX2`). Freetype uses freetype as the text rasterizer and includes some custom fonts, which results in better looking text but larger DLL sizes. PDBs are for developer use. </sub>
  <br><br>
  <sub>If nightly.link is down or can't be accessed, you can still download through [GitHub](https://github.com/rei-2/Amalgam/actions) with an account. </sub>

  ##

  Native Linux porting branch for Amalgam TF2.

</div>

## Status

SHamalgam is an in-progress native Linux TF2 internal port of Amalgam. The goal is
1:1 behavior with the Windows internal, not a reduced rewrite.

Current work is focused on:

- Building the original codebase as a Linux shared object.
- Replacing Win32/D3D9/MinHook entry, input, render, and patching layers with Linux-native equivalents.
- Revalidating every module name, interface lookup, vtable index, and signature against native Linux TF2 binaries.

Windows signatures and offsets are kept as labels and a porting map only. They are
not assumed to be valid on native Linux TF2.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_COMPILER=clang++
cmake --build build -j"$(nproc)"
```

This is not production-ready yet. Compile success is only the first gate; runtime
parity requires Linux TF2 binary verification.

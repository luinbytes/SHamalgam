# SHamalgam

SHamalgam is a native Linux TF2 internal port of Amalgam.

The goal is boringly specific: keep Amalgam's TF2 feature set and behavior as
close to 1:1 as possible, but make it work against the native Linux build of
Team Fortress 2 instead of the Windows client.

## Status

This repository is in the early porting stage.

The current focus is getting the original codebase building cleanly as a Linux
shared object, then replacing the Windows-only layers with Linux-native
equivalents:

- process entry and unload handling
- module and interface lookup
- inline hooks
- input handling
- rendering
- file and path helpers
- TF2 signatures, offsets, vtables, and interface versions

Compile success is only the first gate. Runtime correctness still needs to be
validated against the installed native Linux TF2 binaries.

## Important Porting Note

Windows signatures, offsets, module names, and vtable assumptions are not
treated as valid on Linux.

When old names appear in the source, they are being used as labels and porting
breadcrumbs. Each one still needs to be verified against the native Linux TF2
client and engine modules before it can be considered real.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_COMPILER=clang++
cmake --build build -j"$(nproc)"
```

The expected output target is a Linux shared object in the build directory.

## Native TF2 Audit

The local native Linux TF2 install used for porting is:

```text
/mnt/ssd/.games/steamapps/common/Team Fortress 2
```

Useful current checks:

```sh
tools/linux_signature_audit.py --interfaces
```

Current audit baseline:

- Linux shared object build: passing
- Interface version strings checked: 24 present
- Windows/native byte signatures checked: 258 total, 4 present and 254 missing
- Signature-backed interface lookups checked: 14 missing on native Linux TF2

`IUniformRandomStream` is now resolved through a Linux adapter backed by native
`libvstdlib.so` exports instead of the old Windows client signature.
`ISteamNetworkingUtils` is resolved through native `libsteam_api.so` exports on
Linux.
`CTFPartyClient::SendPartyChat`, `CTFPartyClient::BInQueueForMatchGroup`,
`CTFPartyClient::RequestQueueForMatch`, and `CCasualCriteriaHelper` now have
native Linux `client.so` signatures.

That means versioned `CreateInterface` lookups are the first viable runtime
surface, while every byte signature still needs native Linux replacement work.

## Repository Layout

- `Amalgam/` contains the carried-over source tree being ported.
- `Amalgam/include/platform/linux/` contains compatibility headers for code that
  still expects Win32-style types or APIs.
- `Amalgam/src/Platform/Linux/` contains native Linux replacements for process,
  module, hook, entry, and temporary platform glue.
- `CMakeLists.txt` is the Linux build entrypoint.

## Development Notes

This is intentionally not a rewrite. The plan is to keep behavior aligned with
the original project while swapping out platform-specific assumptions one layer
at a time.

The safest workflow is:

1. Build the Linux shared object.
2. Validate compile-time compatibility changes.
3. Inspect native TF2 Linux binaries for the real symbols, modules, signatures,
   interfaces, and vtables.
4. Replace temporary compatibility scaffolding with verified Linux-native code.
5. Test inside native Linux TF2.

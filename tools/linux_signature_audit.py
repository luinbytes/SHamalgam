#!/usr/bin/env python3
import argparse
import collections
import pathlib
import re


MODULE_MAP = {
    "client.dll": "tf/bin/linux64/client.so",
    "server.dll": "tf/bin/linux64/server.so",
    "engine.dll": "bin/linux64/engine.so",
    "materialsystem.dll": "bin/linux64/materialsystem.so",
    "vguimatsurface.dll": "bin/linux64/vguimatsurface.so",
    "vgui2.dll": "bin/linux64/vgui2.so",
    "vphysics.dll": "bin/linux64/vphysics.so",
    "studiorender.dll": "bin/linux64/studiorender.so",
    "inputsystem.dll": "bin/linux64/inputsystem.so",
    "datacache.dll": "bin/linux64/datacache.so",
    "filesystem_stdio.dll": "bin/linux64/filesystem_stdio.so",
    "soundemittersystem.dll": "bin/linux64/soundemittersystem.so",
    "vstdlib.dll": "bin/linux64/libvstdlib.so",
    "tier0.dll": "bin/linux64/libtier0.so",
}
STEAM_MODULE_MAP = {
    "steamclient64.dll": "linux64/steamclient.so",
}

SIGNATURE_RE = re.compile(
    r'MAKE_SIGNATURE\s*\(\s*([A-Za-z0-9_]+)\s*,\s*"([^"]+)"\s*,\s*"([0-9A-Fa-f? ]+)"',
    re.MULTILINE,
)
INTERFACE_VERSION_RE = re.compile(
    r'MAKE_INTERFACE_VERSION\s*\(\s*[^,]+,\s*([^,]+),\s*"([^"]+)"\s*,\s*("[^"]+"|[A-Za-z0-9_]+)',
    re.MULTILINE,
)
DEFINE_STRING_RE = re.compile(r"#define\s+([A-Za-z0-9_]+)\s+\"([^\"]+)\"")


def pattern_bytes(signature: str):
    return [None if "?" in token else int(token, 16) for token in signature.split()]


def find_pattern(data: bytes, pattern):
    if not pattern or len(data) < len(pattern):
        return -1

    anchor = next((i for i, byte in enumerate(pattern) if byte is not None), None)
    if anchor is None:
        return 0

    needle = bytes([pattern[anchor]])
    pos = 0
    last_start = len(data) - len(pattern)
    while True:
        found = data.find(needle, pos)
        if found < 0:
            return -1

        start = found - anchor
        if start < 0:
            pos = found + 1
            continue
        if start > last_start:
            return -1

        if all(byte is None or data[start + i] == byte for i, byte in enumerate(pattern)):
            return start
        pos = found + 1


def iter_sources(root: pathlib.Path):
    for suffix in ("*.h", "*.hpp", "*.cpp"):
        yield from root.rglob(suffix)


def collect_string_defines(root: pathlib.Path):
    defines = {}
    for path in iter_sources(root):
        text = path.read_text(errors="ignore")
        for match in DEFINE_STRING_RE.finditer(text):
            defines[match.group(1)] = match.group(2)
    return defines


def main():
    parser = argparse.ArgumentParser(description="Scan SHamalgam Windows byte signatures against native Linux TF2 modules.")
    parser.add_argument("--tf2", default="/mnt/ssd/.games/steamapps/common/Team Fortress 2", help="Native Linux TF2 install root")
    parser.add_argument("--steam", default="/home/lu/.local/share/Steam", help="Native Steam install root")
    parser.add_argument("--source", default="Amalgam/src", help="Source root to scan")
    parser.add_argument("--list", choices=("all", "hits", "misses", "missing-modules"), help="Print matching entries")
    parser.add_argument("--interfaces", action="store_true", help="Also check MAKE_INTERFACE_VERSION strings")
    args = parser.parse_args()

    tf2_root = pathlib.Path(args.tf2)
    steam_root = pathlib.Path(args.steam)
    source_root = pathlib.Path(args.source)
    modules = {name: tf2_root / rel for name, rel in MODULE_MAP.items()}
    modules.update({name: steam_root / rel for name, rel in STEAM_MODULE_MAP.items()})
    cache = {}
    results = []

    for path in iter_sources(source_root):
        text = path.read_text(errors="ignore")
        for match in SIGNATURE_RE.finditer(text):
            name, module, signature = match.group(1), match.group(2).lower(), match.group(3)
            module_path = modules.get(module)
            if not module_path or not module_path.exists():
                results.append(("missing-module", module, name, path, ""))
                continue

            data = cache.setdefault(module_path, module_path.read_bytes())
            offset = find_pattern(data, pattern_bytes(signature))
            results.append(("hit" if offset >= 0 else "miss", module, name, path, hex(offset) if offset >= 0 else ""))

    status = collections.Counter(result[0] for result in results)
    by_module = collections.defaultdict(collections.Counter)
    for state, module, *_ in results:
        by_module[module][state] += 1

    print(f"TF2 root: {tf2_root}")
    print(f"Signatures scanned: {len(results)}")
    print("Status:", dict(status))
    print("By module:")
    for module in sorted(by_module):
        print(f"  {module}: {dict(by_module[module])}")

    if args.list:
        want = args.list.rstrip("s")
        print()
        for state, module, name, path, offset in results:
            if args.list == "all" or state == want:
                suffix = f" @ {offset}" if offset else ""
                print(f"{state}: {module}: {name}: {path}{suffix}")

    if args.interfaces:
        string_defines = collect_string_defines(source_root)
        interface_results = []
        for path in iter_sources(source_root):
            text = path.read_text(errors="ignore")
            for match in INTERFACE_VERSION_RE.finditer(text):
                name, module, version_expr = match.group(1), match.group(2).lower(), match.group(3)
                version = version_expr[1:-1] if version_expr.startswith('"') else string_defines.get(version_expr, version_expr)
                module_path = modules.get(module)
                if not module_path or not module_path.exists():
                    interface_results.append(("missing-module", module, name, version, path))
                    continue

                data = cache.setdefault(module_path, module_path.read_bytes())
                interface_results.append(("present" if version.encode() in data else "missing", module, name, version, path))

        interface_status = collections.Counter(result[0] for result in interface_results)
        print()
        print(f"Interface versions checked: {len(interface_results)}")
        print("Interface status:", dict(interface_status))
        if args.list:
            print()
            for state, module, name, version, path in interface_results:
                print(f"interface-{state}: {module}: {name}: {version}: {path}")


if __name__ == "__main__":
    main()

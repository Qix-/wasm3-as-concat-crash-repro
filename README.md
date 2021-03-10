# Wasm3 assertion failure reproduction

```
repro: /src/qix-/wasm3-as-concat-crash-repro/wasm3/source/m3_compile.c:323: void AllocateRegister(IM3Compilation, u32, u16): Assertion `! IsRegisterAllocated (o, i_register)' failed.
```

## Building

Make sure the following are on your `PATH`:

- Bash
- Git
- Node.js (incl. `npm`)
- CMake
- GNU Make or something similar that CMake can default to (otherwise you'll have to edit `CMakeLists.txt`)
- Some C and C++ compiler(s)

Then run

```console
$ ./build.sh
```

This will build _and run_ the repro case. You can run `build.sh` again to rebuild and test
after making modifications.

## Testing

After building and testing, without any modification (a fresh clone) the repro case
should abort with an assertion error (this is the bug).

To see the repro case work as intended, edit `assembly/index.ts` and comment
the line marked `// ERROR` and un-comment the line marked `// OK`.

All it takes is the string concatenation to cause the fault.

Another note: Without the concatenation, the `env.abort` import is no longer required.
In the reproduction case, where this would normally error about not finding a linked
function, we ignore this particular error code.

A successful run (no bugs) should finish with `OK` in the terminal directly before
the script exits.

# License

Released into the public domain.

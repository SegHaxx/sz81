# sz81 in the browser (Timex Sinclair 1000)

A WebAssembly build of the sz81 ZX81 core, set up as a Timex Sinclair 1000:
NTSC timing, 2K RAM by default (16K, 32K or 48K selectable). The SDL front
end is replaced by `host.c` and a canvas page, `index.html`.

Try it online: <https://mohrt.github.io/sz81/>

## Build

Needs [Emscripten](https://emscripten.org/) (`emcc` on your PATH, or set
`EMCC`) and Node.js for the smoke test.

    ./web/build.sh

This compiles `web/ts1000.js` and `web/ts1000.wasm`, embeds
`data/zx81.rom`, copies the test games from `games-etc` into `web/games/`
for the page's Play buttons, and checks that the machine boots and that
`games-etc/tetris1k.p` (2K) and `games-etc/tetris.p` (16K) load.

## Run

Serve the `web` directory over HTTP (browsers won't load WebAssembly from
`file://`):

    cd web && python3 -m http.server 8000

Then open <http://localhost:8000/>.

- Type on your keyboard after clicking the screen, or use the on-screen
  keyboard. Keys map by position like the real machine: Shift+P is `"`,
  Backspace is DELETE, the arrow keys are Shift+5 to Shift+8.
- The **Test games** section under the keyboard has a Play button and the
  controls for each game in `games-etc`.
- **Load .p** resets the machine with enough RAM for the file and runs
  `LOAD ""` on it, so programs saved with auto-run start by themselves.
- **Save .p** downloads the program currently in memory. A BASIC `SAVE`
  command downloads it too.

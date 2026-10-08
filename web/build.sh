#!/bin/sh
set -e
cd "$(dirname "$0")/.."

EMCC="${EMCC:-emcc}"
if ! command -v "$EMCC" >/dev/null 2>&1; then
  echo "emcc not found; install Emscripten or set EMCC" >&2
  exit 1
fi

# Packaged Emscripten installs (e.g. Homebrew) often have a read-only cache.
export EM_CACHE="${EM_CACHE:-$PWD/web/.emcache}"
mkdir -p "$EM_CACHE"

"$EMCC" -O2 \
  -Iweb/include -I. -Iz80 \
  -DSZ81 -DOSS_SOUND_SUPPORT -D_DZ80_EXCLUDE_SCRIPT \
  -DVERSION='"ts1000"' -DPACKAGE_DATA_DIR='"."' \
  web/host.c zx81.c common.c zx81config.c z80/z80.c z80/z80_ops.c \
  dissz80.c noscript.c tables.c \
  -o web/ts1000.js \
  -sMODULARIZE=1 \
  -sEXPORT_NAME=createTS1000 \
  -sEXPORTED_RUNTIME_METHODS='["cwrap","HEAPU8"]' \
  -sEXPORTED_FUNCTIONS='["_ts1000_init","_ts1000_reset","_ts1000_frame","_ts1000_key","_ts1000_set_ram","_ts1000_ram_kb","_ts1000_screen","_ts1000_screen_size","_ts1000_ink","_ts1000_stage","_ts1000_load_staged","_ts1000_save_len","_ts1000_save_ptr","_ts1000_save_ack","_ts1000_program_len","_ts1000_program_ptr","_malloc","_free"]' \
  -sENVIRONMENT=web,node \
  -sALLOW_MEMORY_GROWTH=1 \
  --embed-file data/zx81.rom@/zx81.rom

# The page's Play buttons fetch these from web/games/.
mkdir -p web/games
cp games-etc/tetris1k.p games-etc/tetris.p games-etc/tetrishr.p \
  games-etc/mine81.p games-etc/pipepanic/pipepanic.p web/games/

node --input-type=commonjs -e '
const createTS1000 = require("./web/ts1000.js");
(async () => {
  const m = await createTS1000();
  if (m._ts1000_init() !== 0) throw new Error("init failed");
  let best = 0;
  for (let i = 0; i < 180; i++) {
    m._ts1000_frame();
    best = Math.max(best, m._ts1000_ink());
  }
  if (best < 20) throw new Error("blank screen, ink=" + best);
  const prog = require("fs").readFileSync("games-etc/tetris1k.p");
  const ptr = m._malloc(prog.length);
  m.HEAPU8.set(prog, ptr);
  if (m._ts1000_stage(ptr, prog.length) || m._ts1000_load_staged())
    throw new Error("load failed");
  for (let i = 0; i < 60; i++) m._ts1000_frame();
  const base = m._ts1000_program_ptr() - 0x4009;
  const dfile = m.HEAPU8[base + 0x400c] | (m.HEAPU8[base + 0x400d] << 8);
  const first = m.HEAPU8[base + dfile + 1];
  if (first !== 0x0f) throw new Error("tetris1k did not start, first char " + first);

  const big = require("fs").readFileSync("games-etc/tetris.p");
  const bigPtr = m._malloc(big.length);
  m.HEAPU8.set(big, bigPtr);
  if (m._ts1000_stage(bigPtr, big.length) || m._ts1000_load_staged())
    throw new Error("16K load failed");
  for (let i = 0; i < 60; i++) m._ts1000_frame();
  if (m._ts1000_ram_kb() !== 16) throw new Error("tetris.p did not get 16K");
  const bigBase = m._ts1000_program_ptr() - 0x4009;
  for (let i = 0x74; i < 0x74 + 32; i++)
    if (m.HEAPU8[bigBase + 0x4009 + i] !== big[i])
      throw new Error("tetris.p BASIC not in memory at offset " + i);
  console.log("smoke ok: boots, tetris1k.p (2K) and tetris.p (16K) load and run");
})().catch((err) => { console.error(err); process.exit(1); });
'

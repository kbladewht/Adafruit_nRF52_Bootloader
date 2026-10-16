export PATH="/c/Users/dellht/AppData/Local/Programs/Python/Python314/Scripts:$PATH"
export PATH="/c/Users/dellht/AppData/Local/Programs/Python/Python314:$PATH"
export MCU_SUB_VARIANT=nrf52833

RED='\e[1;31m'
GREEN='\e[1;32m'
YELLOW='\033[1;33m'
END='\033[0m'

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SD_HEX="$ROOT/s140_nrf52_7.3.0_softdevice.hex"
BL_HEX="$ROOT/build/bootloader.hex"
MBR_BL_HEX="$ROOT/build/bootloader_mbr.hex"
OUT_DIR="$ROOT/build"

  echo -e "${YELLOW}== Build nRF52833 bootloader ==${END}"

  rm -rf "$OUT_DIR"
  mkdir -p "$OUT_DIR"
  cd "$OUT_DIR"

  # 第1步：配置
  # 连续 2 次短上电（5 秒内又断电）后，第 3 次插上直接进 DFU
  cmake .. -DBOARD=feather_nrf52833_express -DDFU_POWER_CYCLE_WINDOW_MS=5000 -DDFU_POWER_CYCLE_COUNT=2

  # 第2步：编译 bootloader
  cmake --build . --target bootloader
  cd "$ROOT"

  # 检查输入文件
  if [ ! -f "$SD_HEX" ]; then
    echo -e "${RED}SoftDevice HEX 不存在: $SD_HEX${END}"
    exit 1
  fi

  if [ ! -f "$BL_HEX" ]; then
    echo -e "${RED}Bootloader HEX 不存在: $BL_HEX${END}"
    exit 1
  fi

  echo -e "${YELLOW}== Merge SoftDevice + Bootloader ==${END}"


  TS=$(date +%H%M%S)
  python3 tools/hexmerge.py --overlap=replace -o "$OUT_DIR/sd_bootloader.hex" "$SD_HEX" "$BL_HEX" 


  cp "$OUT_DIR/sd_bootloader.hex"  "/c/Users/dellht/Downloads/BL52833_${TS}.hex"
  cp "$BL_HEX"  "/c/Users/dellht/Downloads/bootloader_${TS}.hex"
  cp "$SD_HEX"  "/c/Users/dellht/Downloads/SD_140_${TS}.hex"
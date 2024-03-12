keyboard_name=qfly
currentdate=$(date +%Y%m%d-%H)
receiver_uf2="./receiver-${currentdate}.uf2"
receiver_hex_path="./_build/nrf52833_xxaa.hex"
bl_path="./kb64bl_bootloader-0.6.2-25-gf1e6e7c-dirty_s140_7.2.0.hex"

RED='\e[1;31m'  # 红
GREEN='\e[1;32m'  # 绿
YELLOW='\033[1;33m'  # 黄
BLUE='\E[1;34m'  # 蓝
PINK='\E[1;35m'  # 粉红
END='\033[0m'  # 清除颜色

rm -rf _build/*

mkdir -p _build
  printf "${GREEN}generating receiver...$END\n"
  
 # make CROSS_COMPILE=C:/msys64/mingw64/bin/arm-none-eabi- BOARD=feather_nrf52833_express all
  make DEBUG=1 BOARD=feather_nrf52833_express all
  #python ./uf2conv.py $receiver_hex_path -c -f 0x621e937a -o 833_receiver.uf2
 
 
 
   printf "${GREEN}Done generate receiver...$END\n"

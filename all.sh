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



qf_debug=0
if [ -n "$2" ]; then
  qf_debug=$2
fi


polling_check(){
count=0
while [ ! -f E:/CURRENT.UF2 ] && [ ! -f D:/CURRENT.UF2 ]; do
    sleep 2
    count=$((count + 1))

    if [ $count -gt 50 ]; then
        echo "\nBeyong attempt, break current build..."
        break
    fi

    if (( count == 1 )); then
        echo "looking up Drive D, polling ...every 2 seconds"
        echo -n "Detecting..."
        continue
    fi

    echo -n "."

done
}



if [ "$1" = "833" ]; then
  make BOARD=feather_nrf52833_pivot DEBUG=${qf_debug} all
  printf "${GREEN}Done generate receiver...$END\n"
  exit 0    
fi

if [ "$1" = "820" ]; then
  printf "${GREEN}Done generate feather_nrf52820_express start...$END\n"
  make BOARD=feather_nrf52820_express DEBUG=0 
  printf "${GREEN}Done generate feather_nrf52820_express...$END\n"
  polling_check
  # cp _build/update-feather_nrf52820_express_bootloader-0.8.3-61-g5c585a6-dirty_nosd.uf2 /e/
  cp _build/build-feather_nrf52820_express/update-feather_nrf52820_express_bootloader-0.8.3-61-g5c585a6-dirty_nosd.uf2 E:/flash.uf2
  exit 0    
fi
  
if [ "$1" = "840" ]; then
  make BOARD=feather_nrf52840_rasi DEBUG=0 all
  printf "${GREEN}Done generate receiver...$END\n"
  exit 0    
fi

 # make CROSS_COMPILE=C:/msys64/mingw64/bin/arm-none-eabi- BOARD=feather_nrf52833_express all
  #make  BOARD=feather_nrf52820_express all
    # make  BOARD=feather_nrf52833_express all
  #python ./uf2conv.py $receiver_hex_path -c -f 0x621e937a -o 833_receiver.uf2
 
 
 
   printf "${GREEN}Done generate receiver...$END\n"

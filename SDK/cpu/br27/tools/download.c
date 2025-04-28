// *INDENT-OFF*
#include "app_config.h"

#ifdef __SHELL__

##!/bin/sh

${OBJDUMP} -D -address-mask=0x7ffffff -print-imm-hex -print-dbg -mcpu=r3 $1.elf > $1.lst
${OBJCOPY} -O binary -j .text $1.elf text.bin
${OBJCOPY} -O binary -j .data  $1.elf data.bin
${OBJCOPY} -O binary -j .data_code $1.elf data_code.bin
${OBJCOPY} -O binary -j .overlay_aec $1.elf aec.bin
${OBJCOPY} -O binary -j .overlay_aac $1.elf aac.bin
${OBJCOPY} -O binary -j .ps_ram_data_code $1.elf ps_ram_data_code.bin
${OBJCOPY} -O binary -j .dcache_ram_data $l.elf d_ram_data.bin
${OBJCOPY} -O binary -j .icache0_ram_data_code $1.elf i0_ram_data_code.bin
${OBJCOPY} -O binary -j .icache1_ram_data_code $1.elf i1_ram_data_code.bin

${OBJDUMP} -section-headers -address-mask=0x7ffffff $1.elf > segment_list.txt
${OBJSIZEDUMP} -lite -skip-zero -enable-dbg-info $1.elf | sort -k 1 >  symbol_tbl.txt

/opt/utils/report_segment_usage --sdk_path ${ROOT} --tbl_file symbol_tbl.txt --seg_file segment_list.txt  --map_file $1.map --module_depth "{\"apps\":1,\"lib\":2,\"[lib]\":2}"

cat text.bin data.bin data_code.bin aec.bin aac.bin ps_ram_data_code.bin d_ram_data.bin i0_ram_data_code.bin i1_ram_data_code.bin > app.bin

cat segment_list.txt

/opt/utils/strip-ini -i isd_config.ini -o isd_config.ini

/* files="app.bin ${CPU}loader.* uboot* p11_code.bin isd_config.ini isd_download.exe fw_add.exe ufw_maker.exe" */
files="app.bin $1.elf  p11_code.bin ${CPU}loader.*  uboot.* isd_download.exe isd_config.ini isd_download.exe ufw_maker.exe fw_add.exe ota.* ota_debug.*"


host-client -project ${NICKNAME}$2 -f ${files} $1.elf

#else

@echo off
Setlocal enabledelayedexpansion
@echo ********************************************************************************
@echo           SDK BR27
@echo ********************************************************************************
@echo %date%


cd /d %~dp0

#if TCFG_TONE_EN_ENABLE
set TONE_EN_ENABLE=1
#else
set TONE_EN_ENABLE=0
#endif

#if TCFG_TONE_ZH_ENABLE
set TONE_ZH_ENABLE=1
#else
set TONE_ZH_ENABLE=0
#endif

#if (TCFG_UI_ENABLE &&(TCFG_LCD_OLED_ENABLE||TCFG_SPI_LCD_ENABLE))
set LCD_SOURCE_ENABLE=1
#else
set LCD_SOURCE_ENABLE=0
#endif



if not %KEY_FILE_PATH%A==A set KEY_FILE=-key %KEY_FILE_PATH%

if %PROJ_DOWNLOAD_PATH%A==A set PROJ_DOWNLOAD_PATH=..\..\..\..\..\output
copy %PROJ_DOWNLOAD_PATH%\*.bin .
if exist %PROJ_DOWNLOAD_PATH%\tone_en.cfg copy %PROJ_DOWNLOAD_PATH%\tone_en.cfg .
if exist %PROJ_DOWNLOAD_PATH%\tone_zh.cfg copy %PROJ_DOWNLOAD_PATH%\tone_zh.cfg .

if %TONE_EN_ENABLE%A==1A (
    set TONE_FILES=tone_en.cfg
)
if %TONE_ZH_ENABLE%A==1A (
    set TONE_FILES=%TONE_FILES% tone_zh.cfg
)

if %LCD_SOURCE_ENABLE%A==1A (
    set LCD_SOURCE_FILES=%LCD_SOURCE_FILES% font JL
)

if %FORMAT_VM_ENABLE%A==1A set FORMAT=-format vm
if %FORMAT_ALL_ENABLE%A==1A set FORMAT=-format all


set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe
set ELFFILE=sdk.elf
set LZ4_PACKET=.\lz4_packet.exe

REM %OBJDUMP% -D -address-mask=0x7ffffff -print-imm-hex -mcpu=r3 -print-dbg sdk.elf > sdk.lst
%OBJCOPY% -O binary -j .text %ELFFILE% text.bin
%OBJCOPY% -O binary -j .data %ELFFILE% data.bin
%OBJCOPY% -O binary -j .data_code %ELFFILE% data_code.bin
%OBJCOPY% -O binary -j .overlay_aec %ELFFILE% aec.bin
%OBJCOPY% -O binary -j .overlay_aac %ELFFILE% aac.bin
%OBJCOPY% -O binary -j .ps_ram_data_code %ELFFILE% ps_ram_data_code.bin
%OBJCOPY% -O binary -j .dcache_ram_data %ELFFILE%  d_ram_data.bin
%OBJCOPY% -O binary -j .icache0_ram_data_code %ELFFILE%  i0_ram_data_code.bin
%OBJCOPY% -O binary -j .icache1_ram_data_code %ELFFILE%  i1_ram_data_code.bin

%OBJDUMP% -section-headers -address-mask=0x7ffffff %ELFFILE%
REM %OBJDUMP% -t %ELFFILE% >  symbol_tbl.txt

copy /b text.bin + data.bin + data_code.bin + aec.bin + aac.bin + ps_ram_data_code.bin + d_ram_data.bin + i0_ram_data_code.bin + i1_ram_data_code.bin app.bin

del text.bin data.bin data_code.bin aec.bin aac.bin ps_ram_data_code.bin d_ram_data.bin i0_ram_data_code.bin i1_ram_data_code.bin


isd_download.exe -tonorflash -dev br27 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin -tone %TONE_FILES% -res cfg_tool.bin p11_code.bin stream.bin %LCD_SOURCE_FILES% %KEY_FILE% %FORMAT%
::-uboot_compress
::-format all


@rem 删除临时文件-format all
if exist *.mp3 del *.mp3
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty



@rem 生成固件升级文件
::fw_add.exe -noenc -fw jl_isd.fw  -add ota.bin -type 100 -out jl_isd.fw

ufw_maker.exe -fw_to_ufw jl_isd.fw
copy jl_isd.ufw update.ufw
del jl_isd.ufw


copy update.ufw %PROJ_DOWNLOAD_PATH%\update.ufw
copy jl_isd.bin %PROJ_DOWNLOAD_PATH%\jl_isd.bin
copy jl_isd.fw %PROJ_DOWNLOAD_PATH%\jl_isd.fw



@REM 生成配置文件升级文件
::ufw_maker.exe -chip AC800X %ADD_KEY% -output config.ufw -res bt_cfg.cfg

::IF EXIST jl_693x.bin del jl_693x.bin


@rem 常用命令说明
@rem -format vm        //擦除VM 区域
@rem -format cfg       //擦除BT CFG 区域
@rem -format 0x3f0-2   //表示从第 0x3f0 个 sector 开始连续擦除 2 个 sector(第一个参数为16进制或10进制都可，第二个参数必须是10进制)

ping /n 2 127.1>null
IF EXIST null del null

#endif

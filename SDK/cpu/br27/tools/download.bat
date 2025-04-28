




@echo off
Setlocal enabledelayedexpansion
@echo ********************************************************************************
@echo SDK BR27
@echo ********************************************************************************
@echo %date%


cd /d %~dp0


set TONE_EN_ENABLE=1





set TONE_ZH_ENABLE=1







set LCD_SOURCE_ENABLE=0




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
%OBJCOPY% -O binary -j .dcache_ram_data %ELFFILE% d_ram_data.bin
%OBJCOPY% -O binary -j .icache0_ram_data_code %ELFFILE% i0_ram_data_code.bin
%OBJCOPY% -O binary -j .icache1_ram_data_code %ELFFILE% i1_ram_data_code.bin

%OBJDUMP% -section-headers -address-mask=0x7ffffff %ELFFILE%
REM %OBJDUMP% -t %ELFFILE% > symbol_tbl.txt

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
::fw_add.exe -noenc -fw jl_isd.fw -add ota.bin -type 100 -out jl_isd.fw

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
@rem -format vm
@rem -format cfg
@rem -format 0x3f0-2

ping /n 2 127.1>null
IF EXIST null del null

// *INDENT-OFF*
#include "app_config.h"
#include "audio_config_def.h"

/* =================  BR27 SDK memory ========================
 _______________ ___ 0x2C000(176K)
|   isr base    |
|_______________|___ _IRQ_MEM_ADDR(size = 0x100)
|rom export ram |
|_______________|
|    update     |
|_______________|___ RAM_LIMIT_H
|     HEAP      |
|_______________|___ data_code_pc_limit_H
| audio overlay |
|_______________|
|   data_code   |
|_______________|___ data_code_pc_limit_L
|     bss       |
|_______________|
|     data      |
|_______________|
|   irq_stack   |
|_______________|
|   boot info   |
|_______________|
|     TLB       |
|_______________|0 RAM_LIMIT_L

 =========================================================== */

#include "maskrom_stubs.ld"

EXTERN(
_start
#include "sdk_used_list.c"
);

UPDATA_SIZE     = 0x80;
UPDATA_BEG      = _MASK_MEM_BEGIN - UPDATA_SIZE;
UPDATA_BREDR_BASE_BEG = 0x2f0000;

RAM_LIMIT_L     = 0x100000;
RAM_LIMIT_H     = UPDATA_BEG;
PHY_RAM_SIZE    = RAM_LIMIT_H - RAM_LIMIT_L;

//from mask export
ISR_BASE       = _IRQ_MEM_ADDR;
ROM_RAM_SIZE   = _MASK_MEM_SIZE;
ROM_RAM_BEG    = _MASK_MEM_BEGIN;

CMAC_RAM0_BEG 	= 0x150000;
CMAC_RAM0_END 	= RAM_LIMIT_H;
CMAC_RAM0_SIZE 	= CMAC_RAM0_END - CMAC_RAM0_BEG;



RAM0_BEG 		= RAM_LIMIT_L;

#ifdef CONFIG_SOFT_BASEBAND_ENABLE
RAM0_END 		= CMAC_RAM0_BEG;
#else
RAM0_END 		=  RAM_LIMIT_H;
#endif

RAM0_SIZE 		= RAM0_END - RAM0_BEG;

#ifdef CONFIG_NEW_CFG_TOOL_ENABLE
CODE_BEG        = 0x6000100;
#else
CODE_BEG        = 0x6000120;
#endif

#if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
PSRAM_BEGIN = 0x2000000;
PSRAM_SIZE = TCFG_PSRAM_SIZE;
PSRAM_END = PSRAM_BEGIN + PSRAM_SIZE;
#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */


//=============== About BT RAM ===================
//CONFIG_BT_RX_BUFF_SIZE = (1024 * 18);

FREE_DACHE_WAY = TCFG_FREE_DCACHE_WAY_NUM; //max is 3
FREE_IACHE0_WAY = TCFG_FREE_ICACHE0_WAY_NUM; //max is 7  // 如果同时了开启低功耗会有问题
FREE_IACHE1_WAY = TCFG_FREE_ICACHE1_WAY_NUM; //max is 7  // 如果同时了开启低功耗会有问题
DCACHE_RAM_SIZE = TCFG_FREE_DCACHE_WAY_NUM*4K;
ICACHE0_RAM_SIZE = TCFG_FREE_ICACHE0_WAY_NUM*4K;
ICACHE1_RAM_SIZE = TCFG_FREE_ICACHE1_WAY_NUM*4K;

MEMORY
{
	code0(rx)    	  : ORIGIN = CODE_BEG, LENGTH = CONFIG_FLASH_SIZE
	ram0(rwx)         : ORIGIN = RAM0_BEG,  LENGTH = RAM0_SIZE

#ifdef CONFIG_SOFT_BASEBAND_ENABLE
	cmac_ram0(rwx)     : ORIGIN = CMAC_RAM0_BEG,  LENGTH = CMAC_RAM0_SIZE
#endif /* #ifdef CONFIG_SOFT_BASEBAND_ENABLE */
#if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
	psram(rwx)        : ORIGIN = PSRAM_BEGIN,  LENGTH = PSRAM_SIZE
#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */

    dcache_ram(rw)    : ORIGIN =  0x370000+((4-FREE_DACHE_WAY)*4K), LENGTH = DCACHE_RAM_SIZE
    icache0_ram(rw)    : ORIGIN =  0x3C0000+((8-FREE_IACHE0_WAY)*4K), LENGTH = ICACHE0_RAM_SIZE
    icache1_ram(rw) : ORIGIN = 0x3D0000+((8-FREE_IACHE1_WAY)*4K), LENGTH = ICACHE1_RAM_SIZE
}


ENTRY(_start)

SECTIONS
{

    . = ORIGIN(ram0);
	//TLB 起始需要16K 对齐；
    .mmu_tlb ALIGN(0x4000):
    {
        *(.mmu_tlb_segment);
    } > ram0

	.boot_info ALIGN(32):
	{
		*(.boot_info)
        . = ALIGN(4);
        // 需要避免与uboot和maskrom冲突
        *(.debug_record)
        . = ALIGN(4);
	} > ram0

	.irq_stack ALIGN(32):
    {
        _cpu0_sstack_begin = .;
        *(.cpu0_stack)
        _cpu0_sstack_end = .;
		. = ALIGN(4);

        _cpu1_sstack_begin = .;
        *(.cpu1_stack)
        _cpu1_sstack_end = .;
		. = ALIGN(4);

    } > ram0

	.data ALIGN(32):SUBALIGN(4)
	{
		//cpu start
        . = ALIGN(4);
        *(.data_magic)

        . = ALIGN(4);
        app_mode_begin = .;
		KEEP(*(.app_mode))
        app_mode_end = .;

		. = ALIGN(4);

        local_tws_ops_begin = .;
		KEEP(*(.local_tws))
        local_tws_ops_end = .;

		. = ALIGN(4);

        *(.data*)
        *(.*.data)

		. = ALIGN(4);
	} > ram0

	.bss ALIGN(32):SUBALIGN(4)
    {
        . = ALIGN(4);
        *(.bss)
        *(.*.data.bss)
        . = ALIGN(4);
        *(.*.data.bss.nv)
        . = ALIGN(4);
        *(.volatile_ram)

		*(.btstack_pool)

        *(.mem_heap)
		*(.memp_memory_x)

        . = ALIGN(4);
#if USB_MEM_USE_OVERLAY == 0
        *(.usb.data.bss.exchange)
#endif

        . = ALIGN(4);
		*(.non_volatile_ram)

        . = ALIGN(32);

    } > ram0


	.data_code ALIGN(32):SUBALIGN(4)
	{
		data_code_pc_limit_begin = .;
		*(.flushinv_icache)
        *(.cache)
        *(.os_critical_code)
        *(.os.text*)
        *(.volatile_ram_code)
        *(.*.text.cache.L1)
        *(.*.text.const.cache.L2)
        *(.ui_ram)


         . = ALIGN(4);
         __fm_movable_slot_start = .;
         *(.movable.slot.1);
         __fm_movable_slot_end = .;

         __bt_movable_slot_start = .;
         *(.movable.slot.2);
         __bt_movable_slot_end = .;

         . = ALIGN(4);
         __aac_movable_slot_start = .;
         *(.movable.slot.3)
         __aac_movable_slot_end = .;

         . = ALIGN(4);
         __aec_movable_slot_start = .;
         *(.movable.slot.4)
         __aec_movable_slot_end = .;

		 . = ALIGN(4);
         __mic_eff_movable_slot_start = .;
         *(.movable.slot.5)
         __mic_eff_movable_slot_end = .;

         . = ALIGN(4);
        *(.movable.stub.1)
        *(.movable.stub.2)
        *(.movable.stub.3)
        *(.movable.stub.4)
        *(.movable.stub.5)


		. = ALIGN(4);
		#include "media/media_lib_data_text.ld"
		. = ALIGN(4);

#if  (TCFG_LED7_RUN_RAM)
		. = ALIGN(4);
        *(.gpio.text.cache.L2)
		*(.LED_code)
		*(.LED_const)
		. = ALIGN(4);
#endif

	} > ram0


	/* .moveable_slot ALIGN(32): */
	/* { */
    /*  */
	/* } >ram0 */

	__report_overlay_begin = .;
	overlay_code_begin = .;
	OVERLAY : AT(0x200000) SUBALIGN(4)
    {
		.overlay_aec
		{
#if (TCFG_AUDIO_CVP_CODE_AT_RAM == 2)
            . = ALIGN(4);
			aec_code_begin  = . ;
			aec_code_end = . ;
			aec_code_size = aec_code_end - aec_code_begin ;
#elif (TCFG_AUDIO_CVP_CODE_AT_RAM == 1)	//仅DNS部分放在overlay
            . = ALIGN(4);
			aec_code_begin  = . ;
			aec_code_end = . ;
			aec_code_size = aec_code_end - aec_code_begin ;

#endif/*TCFG_AUDIO_CVP_CODE_AT_RAM*/
		}
        .overlay_aac
        {

        }

	    } > ram0

   //加个空段防止下面的OVERLAY地址不对
    .ram0_empty0 ALIGN(4) :
	{
        . = . + 4;
    } > ram0

	//__report_overlay_begin = .;
	OVERLAY : AT(0x210000) SUBALIGN(4)
    {
		.overlay_aec_ram
		{
            . = ALIGN(4);
			*(.msbc_enc)
			*(.cvsd_codec)
			*(.aec_bss)
			*(.res_bss)
			*(.ns_bss)
			*(.nlp_bss)
        	*(.der_bss)
        	*(.qmf_bss)
        	*(.fft_bss)
			*(.aec_mem)
			*(.dms_bss)
		}

		.overlay_aac_ram
		{
            . = ALIGN(4);
			*(.bt_aac_dec_bss)

			. = ALIGN(4);
			*(.aac_mem)
			*(.aac_ctrl_mem)
			/* 		. += 0x5fe8 ; */
			/*		. += 0xef88 ; */
		}

		.overlay_mp3
		{
			*(.mp3_mem)
			*(.mp3_ctrl_mem)
			*(.mp3pick_mem)
			*(.mp3pick_ctrl_mem)
			*(.dec2tws_mem)
		}

    	.overlay_wav
		{
			/* *(.wav_bss_id) */
			*(.wav_mem)
			*(.wav_ctrl_mem)
		}


		.overlay_wma
		{
			*(.wma_mem)
			*(.wma_ctrl_mem)
			*(.wmapick_mem)
			*(.wmapick_ctrl_mem)
		}
		.overlay_ape
        {
            *(.ape_mem)
            *(.ape_ctrl_mem)
		}
		.overlay_flac
        {
            *(.flac_mem)
            *(.flac_ctrl_mem)
		}
		.overlay_m4a
        {
            *(.m4a_mem)
            *(.m4a_ctrl_mem)
	        *(.m4apick_mem)
			*(.m4apick_ctrl_mem)

		}
		.overlay_amr
        {
            *(.amr_mem)
            *(.amr_ctrl_mem)
		}
		.overlay_alac
		{
			*(.alac_ctrl_mem)
		}
		.overlay_dts
        {
            *(.dts_mem)
            *(.dts_ctrl_mem)
		}
		.overlay_fm
		{
			*(.fm_mem)
		}
        .overlay_pc
		{
#if USB_MEM_USE_OVERLAY
            *(.usb.data.bss.exchange)
#endif
		}
		/* .overlay_moveable */
		/* { */
			/* . += 0xa000 ; */
		/* } */
    } > ram0

    //bank code addr
    bank_code_run_addr = .;

    OVERLAY : AT(0x300000) SUBALIGN(4)
    {
        .overlay_bank0
        {
            *(.bank.code.0*)
            *(.bank.const.0*)
            . = ALIGN(4);
        }
        .overlay_bank1
        {
            *(.bank.code.1*)
            *(.bank.const.1*)
            . = ALIGN(4);
        }
        .overlay_bank2
        {
            *(.bank.code.2*)
            *(.bank.const.2*)
            *(.bank.ecdh.*)
            . = ALIGN(4);
        }
        .overlay_bank3
        {
            *(.bank.code.3*)
            *(.bank.const.3*)
            *(.bank.enc.*)
            . = ALIGN(4);
        }
        .overlay_bank4
        {
            *(.bank.code.4*)
            *(.bank.const.4*)
            . = ALIGN(4);
        }
        .overlay_bank5
        {
            *(.bank.code.5*)
            *(.bank.const.5*)
            . = ALIGN(4);
        }
        .overlay_bank6
        {
            *(.bank.code.6*)
            *(.bank.const.6*)
            . = ALIGN(4);
        }
        .overlay_bank7
        {
            *(.bank.code.7*)
            *(.bank.const.7*)
            . = ALIGN(4);
        }
        .overlay_bank8
        {
            *(.bank.code.8*)
            *(.bank.const.8*)
            . = ALIGN(4);
        }
        .overlay_bank9
        {
            *(.bank.code.9*)
            *(.bank.const.9*)
            . = ALIGN(4);
        }
    } > ram0


	data_code_pc_limit_end = .;
	__report_overlay_end = .;

	_HEAP_BEGIN = . ;
	_HEAP_END =	RAM0_END;

#ifdef CONFIG_SOFT_BASEBAND_ENABLE
    . = ORIGIN(cmac_ram0);
	.cmac ALIGN(32):SUBALIGN(4)
	{
        cmac_code_begin = .;
	    *(.cmac_code)
        cmac_code_end = . ;

		. = ALIGN(4);
        cmac_data_begin = cmac_code_begin + (48 *1024) ;
	    *(.cmac_data)
        cmac_data_end = cmac_data_begin + (10 * 1024) ;


	} >cmac_ram0

#endif /* #ifdef CONFIG_SOFT_BASEBAND_ENABLE */

#if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
	. = ORIGIN(psram);
	.ps_ram_data_code ALIGN(32):SUBALIGN(4)
	{
	    *(.psram_data)
	    *(.psram_code)
		. = ALIGN(4);
	} > psram

	.ps_ram_bss ALIGN(32):SUBALIGN(4)
	{
	    *(.psram_bss)
		. = ALIGN(4);
	} > psram

	.ps_ram_noinit ALIGN(32):SUBALIGN(4)
	{
	    *(.psram_noinit)
		. = ALIGN(4);
	} > psram

	_PSRAM_HEAP_BEGIN = .;
	_PSRAM_HEAP_END = PSRAM_END;

#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */

#if ((defined TCFG_CONFIG_CMAC_MCU_ENABLE) && TCFG_CONFIG_CMAC_MCU_ENABLE)
	. = ORIGIN(cmac_mcu_ram);
	.cmac_ram ALIGN(32):SUBALIGN(4)
	{
		cmac_mcu_code_begin = .;
	    *(.cmac_mcu_code)
        cmac_mcu_code_end = . ;
		. = ALIGN(4);
	} > cmac_mcu_ram
#endif /* #if ((defined TCFG_CONFIG_CMAC_MCU_ENABLE) && TCFG_CONFIG_CMAC_MCU_ENABLE) */


    . = ORIGIN(dcache_ram);
    .dcache_ram_data ALIGN(32):SUBALIGN(4)
    {
		. = ALIGN(4);
        *(.dcache_pool)
		. = ALIGN(4);
    } > dcache_ram

    .dcache_ram_bss ALIGN(32):SUBALIGN(4)
    {
		. = ALIGN(4);
        *(.dcache_bss)
    } > dcache_ram

    . = ORIGIN(icache0_ram);
    .icache0_ram_data_code ALIGN(32):SUBALIGN(4)
    {
		icache0_ram_data_code_pc_limit_begin = .;
		. = ALIGN(4);
        *(.icache0_code)
		. = ALIGN(4);
		icache0_ram_data_code_pc_limit_end = .;
    } > icache0_ram

    .icache0_ram_bss ALIGN(32):SUBALIGN(4)
    {
		. = ALIGN(4);
        *(.icache0_pool)
		. = ALIGN(4);
    } > icache0_ram

    . = ORIGIN(icache1_ram);
    .icache1_ram_data_code ALIGN(32):SUBALIGN(4)
    {
		icache1_ram_data_code_pc_limit_begin = .;
		. = ALIGN(4);
        *(.icache1_code)
		. = ALIGN(4);
		icache1_ram_data_code_pc_limit_end = .;
    } > icache1_ram

    .icache1_ram_bss ALIGN(32):SUBALIGN(4)
    {
		. = ALIGN(4);
        *(.icache1_pool)
		. = ALIGN(4);
    } > icache1_ram

    . = ORIGIN(code0);
    .text ALIGN(4):SUBALIGN(4)
    {
        PROVIDE(text_rodata_begin = .);

        *(.startup.text)

		*(.text)
        *(.*.text)
        *(.*.text.const)
        _SPI_CODE_START = . ;
        *(.sfc.text.cache.L2*)
		. = ALIGN(4);
        _SPI_CODE_END = . ;
		. = ALIGN(4);

        *(.*.text.cache.L2*)

		. = ALIGN(4);
	    update_target_begin = .;
	    PROVIDE(update_target_begin = .);
	    KEEP(*(.update_target))
	    update_target_end = .;
	    PROVIDE(update_target_end = .);
		. = ALIGN(4);

		*(.LOG_TAG_CONST*)
        *(.rodata*)

		. = ALIGN(4); // must at tail, make rom_code size align 4
        PROVIDE(text_rodata_end = .);

        clock_critical_handler_begin = .;
        KEEP(*(.clock_critical_txt))
        clock_critical_handler_end = .;

        chargestore_handler_begin = .;
        KEEP(*(.chargestore_callback_txt))
        chargestore_handler_end = .;

        . = ALIGN(4);
        app_msg_handler_begin = .;
		KEEP(*(.app_msg_handler))
        app_msg_handler_end = .;

        . = ALIGN(4);
        app_msg_prob_handler_begin = .;
		KEEP(*(.app_msg_prob_handler))
        app_msg_prob_handler_end = .;

        . = ALIGN(4);
        app_charge_handler_begin = .;
		KEEP(*(.app_charge_handler.0))
		KEEP(*(.app_charge_handler.1))
        app_charge_handler_end = .;

        . = ALIGN(4);
        scene_ability_begin = .;
		KEEP(*(.scene_ability))
        scene_ability_end = .;

         #include "media/framework/section_text.ld"

        fm_dev_begin = .;
        KEEP(*(.fm_dev))
        fm_dev_end = .;

        fm_emitter_dev_begin = .;
        KEEP(*(.fm_emitter_dev))
        fm_emitter_dev_end = .;

        storage_device_begin = .;
        KEEP(*(.storage_device))
        storage_device_end = .;

#if (!TCFG_LED7_RUN_RAM)
		. = ALIGN(4);
        *(.gpio.text.cache.L2)
		*(.LED_code)
		*(.LED_const)
#endif

		. = ALIGN(4);
    	key_ops_begin = .;
        KEEP(*(.key_operation))
    	key_ops_end = .;

        . = ALIGN(4);
        key_callback_begin = .;
		KEEP(*(.key_cb))
        key_callback_end = .;

		. = ALIGN(4);
		tool_interface_begin = .;
		KEEP(*(.tool_interface))
		tool_interface_end = .;

        . = ALIGN(4);
        effects_online_adjust_begin = .;
        KEEP(*(.effects_online_adjust))
        effects_online_adjust_end = .;

        . = ALIGN(4);
        vm_reg_id_begin = .;
        KEEP(*(.vm_manage_id_text))
        vm_reg_id_end = .;

        wireless_trans_ops_begin = .;
        KEEP(*(.wireless_trans))
        wireless_trans_ops_end = .;

        . = ALIGN(4);
        wireless_data_callback_func_begin = .;
        KEEP(*(.wireless_call_sync))
        wireless_data_callback_func_end = .;

        . = ALIGN(4);
        wireless_sync_call_func_begin = .;
        KEEP(*(.wireless_sync_call_func))
        wireless_sync_call_func_end = .;

        . = ALIGN(4);
        wireless_custom_data_trans_stub_begin = .;
        KEEP(*(.wireless_custom_data_trans_stub))
        wireless_custom_data_trans_stub_end = .;

        . = ALIGN(4);
        connected_sync_channel_begin = .;
        KEEP(*(.connected_call_sync))
        connected_sync_channel_end = .;

        . = ALIGN(4);
        conn_sync_call_func_begin = .;
        KEEP(*(.connected_sync_call_func))
        conn_sync_call_func_end = .;

        . = ALIGN(4);
        conn_data_trans_stub_begin = .;
        KEEP(*(.conn_data_trans_stub))
        conn_data_trans_stub_end = .;

		. = ALIGN(4);
        __fm_movable_region_start = .;
        *(.movable.region.1)
        __fm_movable_region_end = .;
        __fm_movable_region_size = ABSOLUTE(__fm_movable_region_end - __fm_movable_region_start);

		. = ALIGN(4);
        __bt_movable_region_start = .;
        *(.movable.region.2)
        __bt_movable_region_end = .;
        __bt_movable_region_size = ABSOLUTE(__bt_movable_region_end - __bt_movable_region_start);

		. = ALIGN(4);
        __aac_movable_region_start = .;
        *(.movable.region.3)
        __aac_movable_region_end = .;
        __aac_movable_region_size = ABSOLUTE(__aac_movable_region_end - __aac_movable_region_start);
        . = ALIGN(4);
        *(.bt_aac_dec_const)
        *(.bt_aac_dec_sparse_const)

		. = ALIGN(4);
        __aec_movable_region_start = .;
        *(.movable.region.4)
        __aec_movable_region_end = .;
        __aec_movable_region_size = ABSOLUTE(__aec_movable_region_end - __aec_movable_region_start);

		. = ALIGN(4);
        __mic_eff_movable_region_start = .;
        *(.movable.region.5)
        __mic_eff_movable_region_end = .;
        __mic_eff_movable_region_size = ABSOLUTE(__mic_eff_movable_region_end - __mic_eff_movable_region_start);


		/********maskrom arithmetic ****/
        *(.opcore_table_maskrom)
        *(.bfilt_table_maskroom)
        *(.bfilt_code)
        *(.bfilt_const)
        *(.bark_const)
		/********maskrom arithmetic end****/

		. = ALIGN(4);
		__VERSION_BEGIN = .;
        KEEP(*(.sys.version))
        __VERSION_END = .;

        *(.noop_version)
		. = ALIGN(4);

        #include "btctrler/btctler_lib_text.ld"

        . = ALIGN(4);

        . = ALIGN(4);
        tws_tone_cb_begin = .;
        KEEP(*(.tws_tone_callback))
        tws_tone_cb_end = .;

        . = ALIGN(4);
		_record_handle_begin = .;
		PROVIDE(record_handle_begin = .);
		KEEP(*(.debug_record_handle_ops))
		_record_handle_end = .;
		PROVIDE(record_handle_end = .);

		. = ALIGN(32);
	  } > code0

}

//#include "app.ld"
#include "update/update.ld"
#include "btstack/btstack_lib.ld"
//#include "btctrler/port/br27/btctler_lib.ld"
#include "driver/cpu/br27/driver_lib.ld"
#include "utils/utils_lib.ld"
#include "cvp/audio_cvp_lib.ld"
#include "media/media_lib.c"

#if TCFG_UI_ENABLE && !TCFG_UI_LED7_ENABLE
#include "ui/jl_ui/ui/ui.ld"
#endif

#include "system/port/br27/system_lib.ld" //Note: 为保证各段对齐, 系统ld文件必须放在最后include位置

//================== Section Info Export ====================//
text_begin  = ADDR(.text);
text_size   = SIZEOF(.text);
text_end    = text_begin + text_size;
ASSERT((text_size % 4) == 0,"!!! text_size Not Align 4 Bytes !!!");

bss_begin = ADDR(.bss);
bss_size  = SIZEOF(.bss);
bss_end    = bss_begin + bss_size;
ASSERT((bss_size % 4) == 0,"!!! bss_size Not Align 4 Bytes !!!");

data_addr = ADDR(.data);
data_begin = text_begin + text_size;
data_size =  SIZEOF(.data);
ASSERT((data_size % 4) == 0,"!!! data_size Not Align 4 Bytes !!!");

/* moveable_slot_addr = ADDR(.moveable_slot); */
/* moveable_slot_begin = data_begin + data_size; */
/* moveable_slot_size =  SIZEOF(.moveable_slot); */

data_code_addr = ADDR(.data_code);
data_code_begin =  data_begin + data_size;
data_code_size =  SIZEOF(.data_code);
ASSERT((data_code_size % 4) == 0,"!!! data_code_size Not Align 4 Bytes !!!");


//================ OVERLAY Code Info Export ==================//
aec_addr = ADDR(.overlay_aec);
aec_begin = data_code_begin + data_code_size;
aec_size =  SIZEOF(.overlay_aec);

aac_addr = ADDR(.overlay_aac);
aac_begin = aec_begin + aec_size;
aac_size =  SIZEOF(.overlay_aac);

#if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
ps_ram_data_code_addr = ADDR(.ps_ram_data_code);
ps_ram_data_code_begin = aac_begin + aac_size;
ps_ram_data_code_size =  SIZEOF(.ps_ram_data_code);

ps_ram_bss_addr = ADDR(.ps_ram_bss);
ps_ram_bss_size =  SIZEOF(.ps_ram_bss);

ps_ram_noinit_addr = ADDR(.ps_ram_noinit);
ps_ram_noinit_size =  SIZEOF(.ps_ram_noinit);

ps_ram_size =  PSRAM_SIZE;

PROVIDE(PSRAM_HEAP_BEGIN = _PSRAM_HEAP_BEGIN);
PROVIDE(PSRAM_HEAP_END = _PSRAM_HEAP_END);
_PSRAM_MALLOC_SIZE = _PSRAM_HEAP_END - _PSRAM_HEAP_BEGIN;
PROVIDE(PSRAM_MALLOC_SIZE = _PSRAM_HEAP_END - _PSRAM_HEAP_BEGIN);
#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */

//================ dcache ==================//
dcache_ram_bss_begin = ADDR(.dcache_ram_bss);
dcache_ram_bss_size  = SIZEOF(.dcache_ram_bss);
dcache_ram_bss_end    = dcache_ram_bss_begin + dcache_ram_bss_size;
ASSERT((dcache_ram_bss_size % 4) == 0,"!!! dcache_ram_bss_size Not Align 4 Bytes !!!");

dcache_ram_data_addr = ADDR(.dcache_ram_data);
#if (defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
dcache_ram_data_begin = ps_ram_data_code_begin + ps_ram_data_code_size;
#else
dcache_ram_data_begin = aac_begin + aac_size;
#endif
dcache_ram_data_size =  SIZEOF(.dcache_ram_data);
ASSERT((dcache_ram_data_size % 4) == 0,"!!! dcache_ram_data_size Not Align 4 Bytes !!!");

//================ icache ==================//
icache0_ram_data_code_addr = ADDR(.icache0_ram_data_code);
icache0_ram_data_code_begin = dcache_ram_data_begin + dcache_ram_data_size;
icache0_ram_data_code_size =  SIZEOF(.icache0_ram_data_code);
ASSERT((icache0_ram_data_code_size % 4) == 0,"!!! icache0_ram_data_code_size Not Align 4 Bytes !!!");

icache1_ram_data_code_addr = ADDR(.icache1_ram_data_code);
icache1_ram_data_code_begin = icache0_ram_data_code_begin + icache0_ram_data_code_size;
icache1_ram_data_code_size =  SIZEOF(.icache1_ram_data_code);
ASSERT((icache1_ram_data_code_size % 4) == 0,"!!! icache1_ram_data_code_size Not Align 4 Bytes !!!");

/*
lc3_addr = ADDR(.overlay_lc3);
lc3_begin = aac_begin + aac_size;
lc3_size = SIZEOF(.overlay_lc3);
*/

/* moveable_addr = ADDR(.overlay_moveable) ;
moveable_size = SIZEOF(.overlay_moveable) ; */
//===================== HEAP Info Export =====================//
ASSERT(_HEAP_BEGIN > bss_begin,"_HEAP_BEGIN < bss_begin");
ASSERT(_HEAP_BEGIN > data_addr,"_HEAP_BEGIN < data_addr");
ASSERT(_HEAP_BEGIN > data_code_addr,"_HEAP_BEGIN < data_code_addr");
/* ASSERT(_HEAP_BEGIN > moveable_slot_addr,"_HEAP_BEGIN < moveable_slot_addr"); */
ASSERT(_HEAP_BEGIN > __report_overlay_begin,"_HEAP_BEGIN < __report_overlay_begin");

PROVIDE(HEAP_BEGIN = _HEAP_BEGIN);
PROVIDE(HEAP_END = _HEAP_END);
_MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN;
PROVIDE(MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN);

ASSERT(MALLOC_SIZE  >= 0x8000, "heap space too small !")

//============================================================//
//=== report section info begin:
//============================================================//
report_text_beign = ADDR(.text);
report_text_size = SIZEOF(.text);
report_text_end = report_text_beign + report_text_size;

report_mmu_tlb_begin = ADDR(.mmu_tlb);
report_mmu_tlb_size = SIZEOF(.mmu_tlb);
report_mmu_tlb_end = report_mmu_tlb_begin + report_mmu_tlb_size;

report_boot_info_begin = ADDR(.boot_info);
report_boot_info_size = SIZEOF(.boot_info);
report_boot_info_end = report_boot_info_begin + report_boot_info_size;

report_irq_stack_begin = ADDR(.irq_stack);
report_irq_stack_size = SIZEOF(.irq_stack);
report_irq_stack_end = report_irq_stack_begin + report_irq_stack_size;

report_data_begin = ADDR(.data);
report_data_size = SIZEOF(.data);
report_data_end = report_data_begin + report_data_size;

report_bss_begin = ADDR(.bss);
report_bss_size = SIZEOF(.bss);
report_bss_end = report_bss_begin + report_bss_size;

report_data_code_begin = ADDR(.data_code);
report_data_code_size = SIZEOF(.data_code);
report_data_code_end = report_data_code_begin + report_data_code_size;

report_overlay_begin = __report_overlay_begin;
report_overlay_size = __report_overlay_end - __report_overlay_begin;
report_overlay_end = __report_overlay_end;

report_heap_beign = _HEAP_BEGIN;
report_heap_size = _HEAP_END - _HEAP_BEGIN;
report_heap_end = _HEAP_END;

BR27_PHY_RAM_SIZE = PHY_RAM_SIZE;
BR27_SDK_RAM_SIZE = report_mmu_tlb_size + \
					report_boot_info_size + \
					report_irq_stack_size + \
					report_data_size + \
					report_bss_size + \
					report_overlay_size + \
					report_data_code_size + \
					report_heap_size;
//============================================================//
//=== report section info end
//============================================================//


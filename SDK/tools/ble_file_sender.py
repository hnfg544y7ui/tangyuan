#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BLE File Sender for JL703N Device
Usage: python ble_file_sender.py music.mp3
Requirements: pip install bleak
"""

import asyncio
import sys
import os
from bleak import BleakClient, BleakScanner

# 配置
TARGET_DEVICE_NAME = "debug_4612"
# RCSP Custom Service UUID (需要根据实际设备调整)
CUSTOM_SERVICE_UUID = "0000ae00-0000-1000-8000-00805f9b34fb"
CUSTOM_CHAR_UUID = "0000ae01-0000-1000-8000-00805f9b34fb"

async def find_device(device_name):
    """扫描并找到目标蓝牙设备"""
    print(f"Scanning for device '{device_name}'...")
    devices = await BleakScanner.discover(timeout=10.0)
    
    for device in devices:
        if device.name and device_name in device.name:
            print(f"Found device: {device.name} ({device.address})")
            return device.address
    
    return None

async def send_file_via_ble(device_address, filepath):
    """
    通过BLE发送文件到设备
    Protocol: 0xAA 0x55 CMD LEN_L LEN_H [DATA...]
    CMD: 0x01=Start, 0x02=Data, 0x03=End
    """
    
    if not os.path.exists(filepath):
        print(f"Error: File '{filepath}' not found")
        return False
    
    file_size = os.path.getsize(filepath)
    if file_size > 16 * 1024:
        print(f"Error: File too large ({file_size} bytes), max 16KB")
        return False
    
    print(f"Connecting to device at {device_address}...")
    
    async with BleakClient(device_address) as client:
        if not client.is_connected:
            print("Failed to connect to device")
            return False
        
        print("Connected successfully!")
        
        # 获取服务（新版 bleak 直接使用 .services 属性）
        services = client.services
        service_list = list(services)
        print(f"Available services: {len(service_list)}")
        
        # 找到写特征
        write_char = None
        for service in service_list:
            print(f"Service: {service.uuid}")
            for char in service.characteristics:
                print(f"  Characteristic: {char.uuid}, Properties: {char.properties}")
                # 查找可写特征（通常是ae01或ae02）
                if "write" in char.properties or "write-without-response" in char.properties:
                    write_char = char.uuid
                    print(f"  -> Using this characteristic for writing")
        
        if not write_char:
            print("Error: No writable characteristic found")
            return False
        
        # 发送Start命令
        print(f"Sending start command (file size: {file_size} bytes)...")
        start_packet = bytearray([0xAA, 0x55, 0x01, file_size & 0xFF, (file_size >> 8) & 0xFF])
        await client.write_gatt_char(write_char, start_packet)
        await asyncio.sleep(0.2)
        
        # 发送文件数据
        chunk_size = 100  # BLE MTU限制，通常100-200字节
        with open(filepath, 'rb') as f:
            sent_bytes = 0
            while True:
                chunk = f.read(chunk_size)
                if not chunk:
                    break
                
                # 发送Data命令
                data_packet = bytearray([0xAA, 0x55, 0x02, len(chunk) & 0xFF, (len(chunk) >> 8) & 0xFF])
                data_packet.extend(chunk)
                await client.write_gatt_char(write_char, data_packet)
                
                sent_bytes += len(chunk)
                progress = (sent_bytes * 100) // file_size
                print(f"\rProgress: {progress}% ({sent_bytes}/{file_size} bytes)", end='')
                await asyncio.sleep(0.05)  # 控制发送速率
        
        print()
        
        # 发送End命令
        print("Sending end command...")
        end_packet = bytearray([0xAA, 0x55, 0x03, 0x00, 0x00])
        await client.write_gatt_char(write_char, end_packet)
        await asyncio.sleep(0.5)
        
        print("File transfer complete!")
        return True

async def main():
    if len(sys.argv) < 2:
        print("Usage: python ble_file_sender.py music.mp3")
        print(f"Target device: {TARGET_DEVICE_NAME}")
        sys.exit(1)
    
    filepath = sys.argv[1]
    
    # 扫描设备
    device_address = await find_device(TARGET_DEVICE_NAME)
    if not device_address:
        print(f"Error: Device '{TARGET_DEVICE_NAME}' not found")
        print("Please make sure the device is powered on and in range")
        sys.exit(1)
    
    # 发送文件
    try:
        success = await send_file_via_ble(device_address, filepath)
        if success:
            print("Success!")
        else:
            print("Failed!")
            sys.exit(1)
    except Exception as e:
        print(f"\nError during transfer: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    asyncio.run(main())

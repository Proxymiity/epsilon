INCLUDE shared_config_flash.ld

/*
 * |                     INTERNAL FLASH                        |
 * | 16K + 16K + 16k |           16K          |  64k + 7*128K  |
 * |      KERNEL     | PERSISTIG BYTES BUFFER |   USERLAND     |
 *
 */

FLASH_ORIGIN = 0x08000000;
FLASH_LENGTH = 1024K;
SMALLEST_FLASH_SECTOR_LENGTH = 16K;

KERNEL_VIRTUAL_ORIGIN = FLASH_ORIGIN;
KERNEL_LENGTH = 2*SMALLEST_FLASH_SECTOR_LENGTH;

PERSITING_BYTES_VIRTUAL_ORIGIN = KERNEL_VIRTUAL_ORIGIN + KERNEL_LENGTH;
PERSITING_BYTES_LENGTH = SMALLEST_FLASH_SECTOR_LENGTH;

USERLAND_VIRTUAL_ORIGIN = PERSITING_BYTES_VIRTUAL_ORIGIN + PERSITING_BYTES_LENGTH;
USERLAND_AND_EXTERNAL_APPS_LENGTH = FLASH_LENGTH - PERSITING_BYTES_LENGTH - KERNEL_LENGTH;

SIGNED_PAYLOAD_LENGTH = 0;
CANARY_LENGTH = 0;
SIGNATURE_LENGTH = 0;

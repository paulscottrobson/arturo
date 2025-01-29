/**
 * @file       usb_storage.c
 *
 * @brief      USB MSC Storage / FATFS link from Apple/Oric emulators.
 *
 * @author     Veselin Sladkov & Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include "tusb.h"
#include "diskio.h"

static FATFS msc_fatfs_volumes[CFG_TUH_DEVICE_MAX];
static volatile bool msc_volume_busy[CFG_TUH_DEVICE_MAX];
static scsi_inquiry_resp_t msc_inquiry_resp;
bool msc_inquiry_complete = false;



/**
 * @brief      Waits for USB to settle the filesystem.
 */
void USBSynchronise(void) {
    CONWriteString("Waiting for USB to stabilise..\r\n");
    uint16_t timeOut = 100;                                                         // 10 seconds waiting for timeout.
    while (!msc_inquiry_complete && timeOut > 0) {                                  // Until USB live, or time out.
        USBUpdate();                                                                // USB Check
        sleep_ms(100);                                                              // 0.1 second delay
        timeOut--;
    }
}

/**
 * @brief      Handle a new USB key device
 *
 * @param[in]  dev_addr  dev address
 * @param      cb_data   cb data
 *
 * @return     True if file system mounted now.
 */
bool inquiry_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const *cb_data) {
    if (cb_data->csw->status != 0) {
        //CONWriteString("MSC SCSI inquiry failed\r\n");
        return false;
    }

    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);
    CONWriteString("USB Key found [%04x:%04x]\r\n",vid,pid);

    char drive_path[3] = "0:";
    drive_path[0] += dev_addr;
    FRESULT result = f_mount(&msc_fatfs_volumes[dev_addr], drive_path, 1);
    if (result != FR_OK) {
        // CONWriteString("MSC filesystem mount failed\r\n");
        return false;
    }

    char s[2];
    if (FR_OK != f_getcwd(s, 2)) {
        f_chdrive(drive_path);
        f_chdir("/");
    }

    msc_inquiry_complete = true;

    return true;
}


/**
 * @brief      Mount device
 *
 * @param[in]  dev_addr  The dev address
 */
void tuh_msc_mount_cb(uint8_t dev_addr) {
    uint8_t const lun = 0;
    //CONWriteString("MSC mounted, inquiring\r\n");
    tuh_msc_inquiry(dev_addr, lun, &msc_inquiry_resp, inquiry_complete_cb, 0);
}

/**
 * @brief      Unmount device
 *
 * @param[in]  dev_addr  The dev address
 */
void tuh_msc_umount_cb(uint8_t dev_addr) {
    char drive_path[3] = "0:";
    drive_path[0] += dev_addr;
    f_unmount(drive_path);
}


/**
 * @brief      Fatfs interface , wait for disk IO
 *
 * @param[in]  pdrv  The pdrv
 */
static void wait_for_disk_io(BYTE pdrv) {
    while (msc_volume_busy[pdrv]) {
        tuh_task();
    }
}

/**
 * @brief      Check if disk finished
 *
 * @param[in]  dev_addr  The dev address
 * @param      cb_data   The cb data
 *
 * @return     True if finished
 */
static bool disk_io_complete(uint8_t dev_addr, tuh_msc_complete_data_t const *cb_data) {
    (void)cb_data;
    msc_volume_busy[dev_addr] = false;
    return true;
}

/**
 * @brief      Get Disk status
 *
 * @param[in]  pdrv  The pdrv
 *
 * @return     DSTATUS object
 */
DSTATUS disk_status(BYTE pdrv) {
    uint8_t dev_addr = pdrv;
    return tuh_msc_mounted(dev_addr) ? 0 : STA_NODISK;
}

/**
 * @brief      Initialise a drive
 *
 * @param[in]  pdrv  The pdrv
 *
 * @return     DSTATUS object
 */
DSTATUS disk_initialize(BYTE pdrv) {
    (void)(pdrv);
    return 0;
}

/**
 * @brief      Read disk
 *
 * @param[in]  pdrv    The pdrv
 * @param      buff    The buffer
 * @param[in]  sector  The sector
 * @param[in]  count   The count
 *
 * @return     DRESULT object
 */
DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    uint8_t const dev_addr = pdrv;
    uint8_t const lun = 0;
    msc_volume_busy[pdrv] = true;
    tuh_msc_read10(dev_addr, lun, buff, sector, (uint16_t)count, disk_io_complete, 0);
    wait_for_disk_io(pdrv);
    return RES_OK;
}

/**
 * @brief      Write to disk
 *
 * @param[in]  pdrv    The pdrv
 * @param[in]  buff    The buffer
 * @param[in]  sector  The sector
 * @param[in]  count   The count
 *
 * @return     DRESULT object
 */
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    uint8_t const dev_addr = pdrv;
    uint8_t const lun = 0;
    msc_volume_busy[pdrv] = true;
    tuh_msc_write10(dev_addr, lun, buff, sector, (uint16_t)count, disk_io_complete, 0);
    wait_for_disk_io(pdrv);
    return RES_OK;
}

/**
 * @brief      Handle IODRV commands
 *
 * @param[in]  pdrv  The pdrv
 * @param[in]  cmd   The command
 * @param      buff  The buffer
 *
 * @return     The value requested.
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    uint8_t const dev_addr = pdrv;
    uint8_t const lun = 0;
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((DWORD *)buff) = (WORD)tuh_msc_get_block_count(dev_addr, lun);
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD *)buff) = (WORD)tuh_msc_get_block_size(dev_addr, lun);
            return RES_OK;
        case GET_BLOCK_SIZE:
            *((DWORD *)buff) = 1;  // 1 sector
            return RES_OK;
        default:
            return RES_PARERR;
    }
}
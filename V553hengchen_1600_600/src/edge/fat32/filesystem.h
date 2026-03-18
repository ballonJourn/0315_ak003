#pragma once
#include <string>

namespace base {

namespace fat32 {

bool format_fat32fs(const char *block);
std::string mount_vfat(const char* dev, const char* mount_point);

bool umount(const char *mount_point);

void checkAndMount(const std::string& block, const std::string& mount_point);

int getBlockSize(const std::string& mount_point);

} /* namespace fat32 */
}  /* namespace base */

/**
 * @param record 格式化结束后，是否立即录制
 */
void formatTfcardProcess(bool record);

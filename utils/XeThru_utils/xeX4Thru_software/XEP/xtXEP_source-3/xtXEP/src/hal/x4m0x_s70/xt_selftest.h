#ifndef XT_SELFTEST_H_
#define XT_SELFTEST_H_

#define PRETEST_STORAGE_EXTRAM_L    0
#define PRETEST_STORAGE_EXTRAM_H    1
#define PRETEST_STORAGE_IO          2
#define PRETEST_STORAGE_X4_IO       3
#define PRETEST_SIZE                4

#define SELFTEST_EXT_RAM_PASSED_FIELD_SIZE  16
#define SELFTEST_EXT_RAM_PIN_SHORT_PASSED   0
#define SELFTEST_EXT_RAM_DATA_PASSED        1
#define SELFTEST_EXT_RAM_MASKING_PASSED     2
#define SELFTEST_EXT_RAM_ADDRESSING_PASSED  3

int xt_selftest_ext_ram_shorts(void);
int xt_selftest_ext_ram_functionality(void);

#endif
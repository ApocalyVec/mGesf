#include "x4driver.h"
//#include <limits.h>
#include "gmock/gmock.h"
#include "gtest/gtest.h"


#include <functional>
#include "string.h"


void init_test();

class X4driverTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  X4driverTest() {
    // You can do set-up work for each test here.
  }

  virtual ~X4driverTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
    init_test();
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};

class PifRegisters
{
    public:    
    bool handleCommand(const std::vector<unsigned char> bytes);    
    uint32_t* get_register_calls();
    void reset()
    {
        memset(register_calls,0,128);
        commands.clear();
    }
    private:
        std::vector<std::vector<unsigned char>> commands;
        uint32_t register_calls[128] = {0};
    
    uint32_t size()
    {
        uint32_t ret = 0;
        for(uint32_t i ; i <commands.size();i++ )
        {
            ret += commands[i].size();
        }
        return ret;
    }
};



class SpiRegisters
{
    public:    
    bool handleCommand(const std::vector<unsigned char> bytes);    
    uint32_t* get_register_calls();
    void reset()
    {
        memset(register_calls,0,128);
        commands.clear();
    }
    private:
        std::vector<std::vector<unsigned char>> commands;
        uint32_t register_calls[128] = {0};
    
    uint32_t size()
    {
        uint32_t ret = 0;
        for(uint32_t i ; i <commands.size();i++ )
        {
            ret += commands[i].size();
        }
        return ret;
    }
    
    
    
};


uint32_t* SpiRegisters::get_register_calls()
{
    return register_calls;
}

uint32_t* PifRegisters::get_register_calls()
{
    return register_calls;
}


bool SpiRegisters::handleCommand(const std::vector<unsigned char> bytes)
{
    
    if((bytes.size() > 2) || (bytes.size() == 0))
        return false;
    
    uint8_t address = 0x7f & bytes[0];
    //printf("0x%x, 0x%x\n",bytes[0],bytes[1]);
    register_calls[address]++;
    
    
    return true;
}

bool PifRegisters::handleCommand(const std::vector<unsigned char> bytes)
{
    
    if((bytes.size() > 2) || (bytes.size() == 0))
        return false;
    
    uint8_t address = 0x7f & bytes[0];
    //printf("0x%x, 0x%x\n",bytes[0],bytes[1]);
    register_calls[address]++;
    
    
    return true;
}


//################################################################################
//   Tests
//################################################################################/*
uint8_t last_set_enable_value = 0;
uint32_t unlock_called = 0;
uint32_t lock_called = 0;
uint32_t fail_lock = 0;
uint32_t last_spi_write_length = 0xaa;
uint32_t test_spi_write_called = 0;
void* last_user_reference;
SpiRegisters spi_registers;
std::vector<uint8_t> write_data;
std::vector<uint8_t> read_data;
void init_test()
{
    read_data.clear();
    write_data.clear();
    spi_registers.reset();
    last_set_enable_value = 0;
    unlock_called = 0;
    lock_called = 0;
    fail_lock = 0;
    last_spi_write_length = 0xaa;
    test_spi_write_called = 0;
}
static uint32_t test_enable_set(void* user_reference, uint8_t value)
{    
	//XepRadarX4DriverUserReference_t* x4driver_user_reference = user_reference;
	//UNUSED(x4driver_user_reference); 
    last_user_reference = user_reference;
    last_set_enable_value = value;
    
    return XEP_ERROR_X4DRIVER_OK;	
}




static uint32_t test_lock(void * object,uint32_t timeout)
{
    (void)timeout;
    uint32_t * lock = (uint32_t*) object;
    *lock |=  0x1;
    lock_called++;
    return XEP_LOCK_OK;
}
static void test_unlock(void * object)
{
    uint32_t * lock = (uint32_t*) object;
    *lock |= 0x2;
    unlock_called++;
}


uint32_t test_spi_write_read(void* user_reference, uint8_t* wdata, uint32_t wlength, uint8_t* rdata, uint32_t rlength)
{
    (void)user_reference;
    std::vector<uint8_t> bytes;
    for(uint32_t i = 0;i<wlength;i++)
    {        
        bytes.push_back(wdata[i]);
        write_data.push_back(wdata[i]);
    }
    spi_registers.handleCommand(bytes);
    
    
    for(uint32_t i = 0; i < rlength; i++)
    {
        if(read_data.size() != 0)
        {
            rdata[i] = read_data.front();
            
            read_data.erase(read_data.begin());
        }
    }
    return XEP_ERROR_X4DRIVER_OK;
}


uint8_t last_spi_write_data =0;

uint32_t test_spi_write(void* user_reference,uint8_t* data, uint32_t length)
{
    (void)user_reference;
    std::vector<uint8_t> bytes;

    for(uint32_t i = 0;i<length;i++)
    {            
        bytes.push_back(data[i]);
        write_data.push_back(data[i]);        
    }
    spi_registers.handleCommand(bytes);    
    last_spi_write_length = length;
    test_spi_write_called++;
    return XEP_ERROR_X4DRIVER_OK;
    
}

uint32_t cnt_not_zero(X4DriverBuffer_t *buffer)
{
    uint32_t cnt = 0;
    for(uint32_t i = 0; i< buffer->size;i++)
    {
        if(buffer->data[i] != 0)
        {
            cnt++;
        }
    }
    return cnt;
}

//XEP-62-implement-setenable
TEST_F(X4driverTest, XEP_62_set_enable)
{    
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    
    cb.pin_set_enable = test_enable_set;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    x4driver_create(&x4driver, &mem,&cb,&lock,0);
            
    x4driver_set_enable(x4driver,0);
    ASSERT_EQ(last_set_enable_value, 0);
    x4driver_set_enable(x4driver,1);
    ASSERT_EQ(last_set_enable_value, 1);    
    
}
//lock abstraction.
TEST_F(X4driverTest, x4driver_lock)
{    
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    unlock_called = 0;
    lock_called = 0;
    fail_lock = 0;
    last_set_enable_value=0;    
    x4driver_create(&x4driver, &mem,&cb,&lock,0);
    
    //x4driver_set_enable(x4driver,0);
    
    ASSERT_EQ(last_set_enable_value, 0);
    x4driver_set_enable(x4driver,1);
    ASSERT_EQ(last_set_enable_value, 1);   
        
    ASSERT_EQ(lock_object, 0x3);    
    ASSERT_EQ(unlock_called, 1); 
    ASSERT_EQ(lock_called, 1);              
}

//dev/XEP-61-implement-create-driver
TEST_F(X4driverTest, XEP_61_x4driver_create)
{    
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.timeout = 0;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    unlock_called = 0;
    lock_called = 0;
    fail_lock = 0;
    last_set_enable_value=0;    
    x4driver_create(&x4driver, &mem,&cb,&lock,0);
    
    ASSERT_EQ(x4driver->lock.timeout,100);
    ASSERT_EQ(x4driver->frame_counter,0);
	ASSERT_EQ(x4driver->frame_length, 42);
	
}




//


//XEP-67-implement-setdetectionzone-dummy
TEST_F(X4driverTest, XEP_67_setdetectionzone_dummy)
{    
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    cb.spi_write = test_spi_write;
    unlock_called = 0;
    lock_called = 0;
    fail_lock = 0;
    last_set_enable_value=0;    
    x4driver_create(&x4driver, &mem,&cb,&lock,0);              
    uint32_t ret_val = x4driver_set_detection_zone(x4driver,1.0,2.0);
    ASSERT_EQ(ret_val, XEP_ERROR_X4DRIVER_NOT_SUPPORTED);
    ASSERT_EQ(last_spi_write_length, 0);
    ASSERT_EQ(test_spi_write_called, 1);
           
}



//XEP-56-implement-x4driver-default-8051-firmware
TEST_F(X4driverTest, XEP_56_get_default_firmware_and_coefs)
{    
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    cb.spi_write = test_spi_write;
    
    x4driver_create(&x4driver, &mem,&cb,&lock,0);    
    uint32_t c1i_size = 0;
    uint32_t c1q_size = 0;
    uint32_t x8051_size = 0;
    
           
    x4driver_get_downconversion_coef_c1_i_length(&c1i_size);
    ASSERT_NE(c1i_size,0);
    x4driver_get_downconversion_coef_c1_q_length(&c1q_size);
    ASSERT_NE(c1q_size,0);
    x4driver_get_data_8051_size(&x8051_size);
    ASSERT_NE(x8051_size,0);
    
    X4DriverBuffer_t xdb;
    xdb.size = x8051_size;
    uint8_t x8051_data[x8051_size];
    xdb.data = x8051_data;
        
    x4driver_get_8051_default_firmware(&xdb);
    uint32_t nz_8051 = cnt_not_zero(&xdb);
    ASSERT_NE(nz_8051,0);
    
    
    X4DriverBuffer_t q1ib;
    q1ib.size = c1i_size;
    uint8_t c1i_data[c1i_size];
    q1ib.data = c1i_data;
    x4driver_get_downconversion_coef_c1_i(&q1ib);
    uint32_t nz_c1i = cnt_not_zero(&q1ib);
    ASSERT_NE(nz_c1i,0);
    
    
    X4DriverBuffer_t c1qb;
    c1qb.size = c1q_size;
    uint8_t c1q_data[c1q_size];
    c1qb.data = c1q_data;
    x4driver_get_downconversion_coef_c1_q(&c1qb);
    uint32_t nz_c1q = cnt_not_zero(&c1qb);
    ASSERT_NE(nz_c1q,0);
}


//XEP-106-x4driver-implement-get-settregisterspi
TEST_F(X4driverTest, XEP_106_get_set_spi_register)
{ 
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    cb.spi_write = test_spi_write;
    cb.spi_write_read = test_spi_write_read;    
    init_test();
    x4driver_create(&x4driver, &mem,&cb,&lock,0);
    ASSERT_EQ(write_data.size(), 0);
    x4driver_set_spi_register(x4driver,0x00,0x01);    
    ASSERT_EQ(write_data.size(), 2);
    
    ASSERT_EQ(write_data[0], 0x80 | 0x00);
    ASSERT_EQ(write_data[1], 1);
        
    uint8_t register_value = 0x00;
    read_data.push_back(0xaa);
    x4driver_get_spi_register(x4driver,0x04,&register_value);
    
    ASSERT_EQ(write_data.size(), 3);
    ASSERT_EQ(write_data[2], 0x04);
    
   
    ASSERT_EQ(read_data.size(), 0);
    
    ASSERT_EQ(register_value, read_data[0]);
    
}

void push_8051()
{
    uint32_t size_8051;
    x4driver_get_data_8051_size(&size_8051);
    X4DriverBuffer_t xdb;
    xdb.size = size_8051;
    uint8_t x8051_data[size_8051];
    xdb.data = x8051_data;        
    x4driver_get_8051_default_firmware(&xdb);
    read_data.push_back(0x00);
    for(uint32_t i = 0; i<size_8051;i++)
        read_data.push_back(x8051_data[i]);
}

//XEP-57-implement-upload-firmware-default
TEST_F(X4driverTest, XEP_57_implement_upload_firmware_default)
{ 
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    cb.spi_write = test_spi_write;
    cb.spi_write_read = test_spi_write_read;    
    spi_registers.reset();
    x4driver_create(&x4driver, &mem,&cb,&lock,0);
    uint32_t size_8051;
    x4driver_get_data_8051_size(&size_8051);
    //add x4driver for future.....       
    push_8051();
    
    uint32_t program_default_return_value = x4driver_program_default_8051(x4driver);
    ASSERT_EQ(XEP_ERROR_X4DRIVER_OK,program_default_return_value);
    uint32_t * spi_calls = spi_registers.get_register_calls();
    uint32_t to_mem_calls = spi_calls[ADDR_SPI_TO_MEM_WRITE_DATA_WE];
    ASSERT_EQ(to_mem_calls,size_8051);
    
    uint32_t from_mem = spi_calls[ADDR_SPI_FROM_MEM_READ_DATA_RE];
    ASSERT_EQ(from_mem,size_8051);
    //simple test should have atlease 2x data size spi bytes written.
    ASSERT_GT(write_data.size(), size_8051*2);
    
    program_default_return_value = x4driver_program_default_8051(x4driver);
    ASSERT_EQ(XEP_ERROR_X4DRIVER_8051_VERIFY_FAIL,program_default_return_value);
}

void push_pif_read(uint8_t value)
{
    read_data.push_back(0x00);
    read_data.push_back(0x02);
    read_data.push_back(0x02);
    read_data.push_back(value);
    read_data.push_back(0x00);
}

void push_pif_write()
{
    read_data.push_back(0x04);
    read_data.push_back(0x04); 
}


//XEP-107-x4driver-implement-get-setregisterpif
TEST_F(X4driverTest, XEP_107_implement_get_setregisterpif)
{ 
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    cb.spi_write = test_spi_write;
    cb.spi_write_read = test_spi_write_read;    
    spi_registers.reset();
    x4driver_create(&x4driver, &mem,&cb,&lock,0);
    uint8_t read_value = 0x00;
    uint8_t expected_read_value = 0xaa;
    
    //test get reg
    read_data.push_back(0x00);
    read_data.push_back(0x02);
    read_data.push_back(0x00);
    uint32_t get_return = x4driver_get_pif_register(x4driver,0,&read_value);
    ASSERT_EQ(get_return,XEP_ERROR_X4DRIVER_OK);
    ASSERT_EQ(read_data.size(),0);
    
    
    read_value = 0;
    //test get reg value
    push_pif_read(0xaa);
    
    get_return = x4driver_get_pif_register(x4driver,0,&read_value);    
    ASSERT_EQ(read_data.size(),0);
    ASSERT_EQ(expected_read_value,read_value);
    ASSERT_EQ(get_return,XEP_ERROR_X4DRIVER_OK);
    //test set 
    push_pif_write();
    uint32_t set_return = x4driver_set_pif_register(x4driver,0,0xaa);
    ASSERT_EQ(set_return,XEP_ERROR_X4DRIVER_OK);
    
    get_return = x4driver_get_pif_register(x4driver,0,&read_value);
    ASSERT_EQ(get_return,XEP_ERROR_X4DRIVER_PIF_TIMEOUT);
           
    set_return = x4driver_set_pif_register(x4driver,0,0xaa);
    ASSERT_EQ(set_return,XEP_ERROR_X4DRIVER_PIF_TIMEOUT);
    
}

uint32_t wait_called = 0;
uint32_t last_us = 0;
void test_wait_us(uint32_t us)
{
    last_us = us;
    wait_called++;
}



void push_clock()
{
    push_pif_read(0);
    push_pif_write();
    push_pif_read(0);
    push_pif_write();              
    push_pif_read(1<<6);
}

//XEP-68-x4driver-implement-initclock
TEST_F(X4driverTest, XEP_68_x4driver_implement_initclock)
{ 
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    cb.spi_write = test_spi_write;
    cb.spi_write_read = test_spi_write_read;    
    cb.wait_us = test_wait_us;
    spi_registers.reset();
    x4driver_create(&x4driver, &mem,&cb,&lock,0);
    
    ASSERT_EQ(read_data.size(),0);

    
    
    push_clock();
    
                  
    
    
    uint32_t setup_clock_return =0;
    setup_clock_return = x4driver_setup_clock(x4driver);    
    ASSERT_EQ(wait_called,0);
    ASSERT_EQ(setup_clock_return,XEP_ERROR_X4DRIVER_OK);
    
    
    
    
    push_pif_read(0);
    push_pif_write();
    push_pif_read(0);
    push_pif_write();              
    push_pif_read(0);
    
    
    setup_clock_return = x4driver_setup_clock(x4driver); 
    ASSERT_EQ(setup_clock_return,XEP_ERROR_X4DRIVER_OSC_LOCK_FAIL);
    //ASSERT_GT(last_us,0);
    ASSERT_EQ(wait_called,100);
    
}


#define DVDD_RX_POWER_GOOD_BIT 3
#define DVDD_TX_POWER_GOOD_BIT 2
#define AVDD_RX_POWER_GOOD_BIT 1
#define AVDD_TX_POWER_GOOD_BIT 0
#define LDO_NOK -1

void push_ldo(int32_t ldo_bit)
{
    push_pif_write();    
    push_pif_read(0);
    push_pif_write();
    if(LDO_NOK == ldo_bit)
        push_pif_read(0);                
    else
        push_pif_read(1 << ldo_bit);
        
   
    push_pif_write();
}



void push_all_ldos()
{
    push_ldo(DVDD_TX_POWER_GOOD_BIT);
    push_ldo(DVDD_RX_POWER_GOOD_BIT);
    push_ldo(AVDD_RX_POWER_GOOD_BIT);
    push_ldo(AVDD_TX_POWER_GOOD_BIT);
}


//XEP-58-implement-ldo-enable-ldo
TEST_F(X4driverTest, XEP_58_implement_ldo_enable_ldo)
{ 
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    cb.spi_write = test_spi_write;
    cb.spi_write_read = test_spi_write_read;    
    cb.wait_us = test_wait_us;
    spi_registers.reset();
    x4driver_create(&x4driver, &mem,&cb,&lock,0);
    
    
    ASSERT_EQ(read_data.size(),0);

    //test avdd_rx_ldo ok
    push_ldo(AVDD_RX_POWER_GOOD_BIT);
        
                                  
    uint32_t enable_avdd_rx_return =0;
    enable_avdd_rx_return = x4driver_enable_avdd_rx(x4driver);        
    ASSERT_EQ(enable_avdd_rx_return,XEP_ERROR_X4DRIVER_OK);
    //test avdd_rx_ldo nok
    push_ldo(LDO_NOK);
     
        
                                  
    enable_avdd_rx_return =0;
    enable_avdd_rx_return = x4driver_enable_avdd_rx(x4driver);        
    ASSERT_EQ(enable_avdd_rx_return,XEP_ERROR_X4DRIVER_LDO_ENABLE_FAIL);
    
    
    //test avdd_tx_ldo ok
    push_ldo(AVDD_TX_POWER_GOOD_BIT);

        
                                  
    uint32_t enable_avdd_tx_return =0;
    enable_avdd_rx_return = x4driver_enable_avdd_tx(x4driver);        
    ASSERT_EQ(enable_avdd_tx_return,XEP_ERROR_X4DRIVER_OK);
    //test avdd_tx_ldo nok
    push_ldo(LDO_NOK);  
        
                                  
    enable_avdd_tx_return =0;
    enable_avdd_tx_return = x4driver_enable_avdd_tx(x4driver);        
    ASSERT_EQ(enable_avdd_tx_return,XEP_ERROR_X4DRIVER_LDO_ENABLE_FAIL);
    
    
    //test dvdd_tx_ldo ok
    push_ldo(DVDD_TX_POWER_GOOD_BIT);    
        
                                  
    uint32_t enable_dvdd_tx_return =0;
    enable_dvdd_tx_return = x4driver_enable_dvdd_tx(x4driver);        
    ASSERT_EQ(enable_dvdd_tx_return,XEP_ERROR_X4DRIVER_OK);
    //test dvdd_tx_ldo nok
    push_ldo(LDO_NOK);   
        
                                  
    enable_dvdd_tx_return =0;
    enable_dvdd_tx_return = x4driver_enable_dvdd_tx(x4driver);        
    ASSERT_EQ(enable_dvdd_tx_return,XEP_ERROR_X4DRIVER_LDO_ENABLE_FAIL);
    
    
    //test dvdd_rx_ldo ok
    push_ldo(DVDD_RX_POWER_GOOD_BIT);  
      
        
                                  
    uint32_t enable_dvdd_rx_return =0;
    enable_dvdd_rx_return = x4driver_enable_dvdd_rx(x4driver);        
    ASSERT_EQ(enable_dvdd_rx_return,XEP_ERROR_X4DRIVER_OK);
    //test dvdd_rx_ldo Nok
    push_ldo(LDO_NOK);    
        
                                  
    enable_dvdd_rx_return = 0;
    enable_dvdd_rx_return = x4driver_enable_dvdd_rx(x4driver);        
    ASSERT_EQ(enable_dvdd_rx_return,XEP_ERROR_X4DRIVER_LDO_ENABLE_FAIL);
    
           
}


//dev/XEP-69-x4driver-implement-init
TEST_F(X4driverTest, XEP_69_x4driver_implement_init)
{ 
    X4Driver_t mem;
    X4Driver_t * x4driver;
    X4DriverCallbacks_t cb;
    X4DriverLock_t lock;
    uint32_t lock_object =0;
    lock.object = &lock_object;
    lock.lock = test_lock;
    lock.unlock = test_unlock;
    cb.pin_set_enable = test_enable_set;
    cb.spi_write = test_spi_write;
    cb.spi_write_read = test_spi_write_read;    
    cb.wait_us = test_wait_us;
    spi_registers.reset();
    x4driver_create(&x4driver, &mem,&cb,&lock,0);    
    push_8051();
    push_all_ldos();
    push_clock();
    uint32_t init_return = x4driver_init(x4driver);
    
    ASSERT_EQ(init_return,XEP_ERROR_X4DRIVER_OK);
    
           
}




int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}








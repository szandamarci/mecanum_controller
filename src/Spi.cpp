/*
 * Spi.cpp
 *
 *  Created on: May 31, 2015
 *      Author: miki
 */

 #include "Spi.h"
 #include <unistd.h>
 #include <ctime>
 #include <iostream>
 
 namespace mecanum {
 
 Spi::Spi() {
	 for (int i=0; i<MESSAGE_SIZE; i++)
		 RxMessage.data[i] = 0;
 
	 RxMessage.msg.speed = {0, 0, 0};
	 RxMessage.msg.conf = {0, 0, 0};
	 RxMessage.msg.sync = MESSAGE_SYNC_VALUE;
 }
 
 Spi::~Spi() {
	 bcm2835_spi_end();
 }
 
 bool Spi::init()
 {
	 if (!bcm2835_init())
	 {	 
		 std::cout << "SPI INIT FAILED! " << std::endl;
		 return false;
	 }
	 
	 bcm2835_spi_begin();
	 bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	 std::cout << "SPI SETBITORDER SIKERULT! " << std::endl;
	 bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	 std::cout << "SPI DATAMODE SIKERULT! " << std::endl;
	 bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
	 bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	 bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
	 
	 // GPIO17
	 bcm2835_gpio_fsel(SIGNAL_PIN, BCM2835_GPIO_FSEL_OUTP);
	 bcm2835_gpio_write(SIGNAL_PIN, LOW);
 
	 // GPIO18
	 bcm2835_gpio_fsel(RESPONSE_PIN, BCM2835_GPIO_FSEL_INPT);
	 std::cout << "SPI INIT SUCCESFUL! " << std::endl;
	 
	 //CS_PIN
	//  bcm2835_gpio_fsel(CS_PIN, BCM2835_GPIO_FSEL_OUTP);
	//  bcm2835_gpio_write(CS_PIN, HIGH);

	 
	 return true;
 }
 
 bool Spi::transfer(const SpiMessage& TxMessage, SpiMessage& RxMessage)
 {
	 SpiMessageU msg;
	 msg.msg = TxMessage;
	 int8_t cs_pin_state;
     std::cout << "Transmitting: ";
	 for (int i=0; i<MESSAGE_SIZE; i++)
	 {
			std::cout  << std::hex << (int)msg.data[i] << " ";
	 }
	 std::cout << std::endl;

	 for (int i=0; i<MESSAGE_SIZE; i++)
	 {
	 	 //bcm2835_gpio_write(CS_PIN, LOW);
		 //cs_pin_state = bcm2835_gpio_lev(CS_PIN);
         //std::cout << "CS_PIN state: " << (cs_pin_state == HIGH ? "HIGH" : "LOW") << std::endl;
		 this->RxMessage.data[i] = bcm2835_spi_transfer(msg.data[i]);
		 //bcm2835_gpio_write(CS_PIN, HIGH);
		 //cs_pin_state = bcm2835_gpio_lev(CS_PIN);
         //std::cout << "CS_PIN state: " << (cs_pin_state == HIGH ? "HIGH" : "LOW") << std::endl;
		}

     std::cout << "Received: ";
	 for (int i=0; i<MESSAGE_SIZE; i++)
	 {
			std::cout << std::hex << (int) this->RxMessage.data[i] << " ";
	 } 
     std::cout << std::endl;
	 RxMessage = this->RxMessage.msg;
     std::cout << "RxMessage: " <<  RxMessage.sync << std::endl;
	 return (RxMessage.sync == MESSAGE_SYNC_VALUE);
 }
 
 bool Spi::transferSignal(void)
 {
	 std::clock_t start;
	 double duration;
 
	 start = std::clock();
	 bcm2835_gpio_write(SIGNAL_PIN, HIGH);
	 while(bcm2835_gpio_lev(RESPONSE_PIN) == LOW)
	 {
		 duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
		 if (duration > 0.001)
		 {
			 
			 
			 std::cout << "SPI TRANSFER TIMEOUT!" << std::endl;
			 bcm2835_gpio_write(SIGNAL_PIN, LOW);
			 return false;
		 }
	 }
 
	 bcm2835_gpio_write(SIGNAL_PIN, LOW);
         return true;
 }
 
 } /* namespace mecanum */

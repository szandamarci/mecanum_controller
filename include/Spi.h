/*
 * Spi.h
 *
 *  Created on: May 31, 2015
 *      Author: miki
 */

#ifndef SPI_H_
#define SPI_H_

#include "../bcm2835/bcm2835.h"

namespace mecanum {

#define MESSAGE_SIZE		(2 + 6*4)
#define MESSAGE_SYNC_VALUE	(0x55AA)

// #define CS_PIN		RPI_GPIO_P1_24
#define SIGNAL_PIN		RPI_GPIO_P1_11
#define RESPONSE_PIN	RPI_GPIO_P1_12

typedef struct Speed {
	float x;
	float y;
	float w;
} Speed;

typedef struct Conf {
	float x;
	float y;
	float theta;
} Conf;

typedef struct SpiMessage {
	Speed speed;
	Conf conf;
	uint16_t sync;
} SpiMessage;

typedef union {
	SpiMessage msg;
	uint8_t data[MESSAGE_SIZE];
} SpiMessageU;


class Spi {
	SpiMessageU RxMessage;
public:
	Spi();
	~Spi();

	bool init();
	bool transfer(const SpiMessage& TxMessage, SpiMessage& RxMessage);

	bool transferSignal(void);
};

} /* namespace mecanum */

#endif /* SPI_H_ */

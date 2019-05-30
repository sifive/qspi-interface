#include <stdio.h>
#include <metal/spi.h>

int main() {
	/* Iterate over a few device numbers to select the first available SPI device */
	int number_of_tries = 0;
	struct metal_spi *spi;
	while ((spi = metal_spi_get_device(number_of_tries)) == NULL && number_of_tries < 10) {
		number_of_tries++;
	}
	
	/* Report that no SPI device can be found. */
	if (spi == NULL) {
		printf("Failed to get spi device\n");
		return 1;
	}
	
	/* Initialize SPI device to 100,000 baud rate */
	metal_spi_init(spi, 100000);

	/* CPOL = 0, CPHA = 0, MSB-first, CS active low */
	struct metal_spi_config config = {
		.protocol = METAL_SPI_SINGLE,
		.polarity = 0,
		.phase = 0,
		.little_endian = 0,
		.cs_active_high = 0,
		.csid = 0,
	};
	return 0;
}

#include <stdio.h>
#include <metal/spi.h>

/* Instruction Hex code according to Cypress datasheet. The user of the API is
 * supposed to fill the output buffer with enough dummy bytes to receive the 
 * information sent by the slave. For example, even though RDID is a one-byte command,
 * the user has to fill a couple more dummy bytes to keep the SCK signal ticking.
 * Also the first few bytes placed in the receive buffer by the spi controller usually is
 * not meaningful information because the spi device may be only able to respond to
 * master-side commands after the slave device receives the last bit of instruction.
 * Therefore, the first few bytes in the receive buffer (often 0xFF) could just mean 
 * high impedence. */

/* Single protocol commands */

/* Read 3-byte address */
static const char READ = 0x3;

/* Read device ID */
static const char RDID = 0x9F;

/* Read electronic signature */
static const char RES = 0x5;

/* Dual protocol commands */

/* Dual read out */
static const char DOR = 0x3B;

/* Quad protocol commands */

/* Quad read out */
static const char QOR = 0x6B;

int main() {
    
    /* Iterate over a few device numbers to select the first available SPI device */
    int number_of_tries = 0;
    struct metal_spi *spi;
    while ((spi = metal_spi_get_device(number_of_tries)) == NULL && number_of_tries < 10) {
        number_of_tries++;
    }
    
    /* Report that no SPI device can be found. */
    if (spi == NULL) {
        puts("Failed to get spi device\n");
        return 1;
    }

    /* Report the device ID of the SPI device */
    char buffer[50];
    sprintf(buffer, "The device id of the spi device is %d", number_of_tries);
    puts(buffer);

    /* Initialize SPI device to 100,000 baud rate */
    metal_spi_init(spi, 100000);

    /* CPOL = 0, CPHA = 0, quad spi, MSb-first, CS active low */
    struct metal_spi_config config = {
        .protocol = METAL_SPI_SINGLE,
        .polarity = 0,
        .phase = 0,
        .little_endian = 0,
        .cs_active_high = 0,
        .csid = 0,
    };

    int transfer_success;

    /* Send random four bytes to the flash device using the programmable IO interface */
    const char random_instruction[4] = {0x4, 2, 0xef, 0x3b};
    char receive_buffer[4];
    transfer_success = metal_spi_transfer(spi, &config, 4, random_instruction, receive_buffer);
    if (transfer_success != 0) {
        puts("spi transfer failed.");
        return 1;
    }

    /* Read device id information of the SPI device */
    char id_buffer[4];
    const char read_id[4] = {RDID, 0, 0, 0, 0};
    transfer_success = metal_spi_transfer(spi, &config, 4, read_id, id_buffer);
    if (transfer_success != 0) {
        puts("spi transfer failed.");
        return 1;
    }

    /* Use read to read a 3-byte address on the spi device, 3 byte memory address is
     * represented in the char array in MSB order */
    char mem_buffer[70] = {0};
    char read[70];
    memset(read, 0, 70);
    read[0] = READ;
    transfer_success = metal_spi_transfer(spi, &config, 70, read, mem_buffer);
    if (transfer_success != 0) {
        puts("spi tranfer failed.");
        return 1;
    }
    
    /* Set the protocol to dual  mode */
    config.protocol = METAL_SPI_DUAL;

    /* Send random five bytes using dual protocol */
    char rand_dual_buffer[5] = {0};
    char rand_dual[5] = {0x34, 0x12, 0x97, 0xDA};
    transfer_success = metal_spi_transfer(spi, &config, 5, rand_dual, rand_dual_buffer);
    if (transfer_success != 0) {
        puts("spi transfer failed.");
        return 1;
    }


    /* Dual read memory address */
    char d_buffer[10];
    char dual_read[10];
    memset(dual_read, 0, 10);
    dual_read[0] = DOR;
    transfer_success = metal_spi_transfer(spi, &config, 10, dual_read, d_buffer);
    if (transfer_success != 0) {
        puts("spi transfer failed.");
        return 1;
    }

    /* Set the protocol to quad mode */
    config.protocol = METAL_SPI_QUAD;

    /* Quad read memory address */
    char q_buffer[20];
    char quad_read[20];
    memset(quad_read, 0, 20);
    quad_read[0] = QOR;
    transfer_success = metal_spi_transfer(spi, &config, 20, quad_read, q_buffer);
    if (transfer_success != 0) {
        puts("spi transfer failed.");
        return 1;
    }

    return 0;
}

#include "kalyna.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

extern standart_config Kalyna_128_128;
extern standart_config Kalyna_128_256;
extern standart_config Kalyna_256_256;
extern standart_config Kalyna_256_512;
extern standart_config Kalyna_512_512;
extern const size_t rows_count;
extern const size_t dword_bsize;
const size_t BUFFER_SIZE = 64 * 1024;

typedef void (*operation)(dw *, dw *, dw *);
size_t hexs2bin(const char *hex, unsigned char **out);
int hexchr2bin(const char hex, char * out);

int parse_arguments(int argc, char ** argv, FILE ** input, FILE ** output, dw * key, bool * op,
        standart_config * standart, bool * cbc_padding);

int main(int argc, char ** argv) {
    FILE * input;
    FILE * output;
    dw key[8];
    bool op_ = true;
    operation op;
    standart_config * kalyna = &Kalyna_512_512;
    bool cbc_padding = false;
    
    if (parse_arguments(argc, argv, &input, &output, key, &op_, kalyna, &cbc_padding) != 0) {
        return 1;
    }

    if (op_) {
        op = &cipher;
    } else {
        op = &decipher;
    }

    setup(kalyna);

    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));
    key_expansion(key, w);

    dw * rbuffer = malloc(BUFFER_SIZE);
    dw * wbuffer = malloc(BUFFER_SIZE);

    size_t kalyna_block_bsize = sizeof(dw) * kalyna->block_dwsize;

    size_t reads;
    size_t before_r = 0;
    size_t read_blocks;
    while ((reads = fread(rbuffer, sizeof(byte), BUFFER_SIZE, input)) != 0) {
        read_blocks = reads / kalyna_block_bsize;

        for (size_t i = 0; i < read_blocks; i++) {
            op(rbuffer + kalyna->block_dwsize * i,
                    wbuffer + kalyna->block_dwsize * i, w);
        }

        if (reads % kalyna_block_bsize != 0) {
            if (!op_) {
                fprintf(stderr, "Corrupted file.");

                return -1;
            }

            if (cbc_padding) {
                //add_cbc_padding(rbuffer + kalyna->block_dwsize * read_blocks,
                //          reads % kalyna_block_bsize);
                //reads = (read_blocks + 1) * kalyna_block_bsize;
            } else {
                memset(rbuffer + kalyna->block_dwsize * read_blocks + reads % kalyna_block_bsize,
                        0x0, kalyna_block_bsize - reads % kalyna_block_bsize);
                reads = (read_blocks + 1) * kalyna_block_bsize;
            }

            op(rbuffer + read_blocks * kalyna->block_dwsize,
                    wbuffer + read_blocks * kalyna->block_dwsize, w);
        }

        if (reads < BUFFER_SIZE && !op_ && cbc_padding) {
            //reads -= del_cbc_padding(wbuffer + read_blocks - kalyna->block_dwsize);
        }

        if (fwrite(wbuffer, sizeof(byte), reads, output) == 0) {
            fprintf(stderr, "Error occured while writting to the output file.");
            return -1;
        }

        before_r = reads;
    }

    if (!op_ && before_r == BUFFER_SIZE && reads == 0 && cbc_padding) {
        fseek(output, 0L, SEEK_END);
        long sz = ftell(output);

        //sz -= del_cbc_padding(wbuffer + before_r / sizeof(dw) - kalyna->block_dwsize);

        if (ftruncate(fileno(output), sz) != 0) {
            fprintf(stderr, "File truncate error.");

            return -3;
        }
    }

    free(rbuffer);
    free(wbuffer);
    free(w);
    fclose(input);
    fclose(output);

    return 0;
}

int parse_arguments(int argc, char ** argv, FILE ** input, FILE ** output, dw * key, bool * op,
        standart_config * standart, bool * cbc_padding) {
    int option;
    size_t key_len;
    int is_all = 0;

    while ((option = getopt(argc, argv, "i:o:k:eds:ph")) != -1) {
        switch (option) {
            case 'i':
                *input = fopen(optarg, "r");
                is_all++;
                break;
            case 'o':
                *output = fopen(optarg, "w+");
                is_all++;
                break;
            case 'k':
                key_len = hexs2bin(optarg, (byte **)&key);
                is_all++;
                break;
            case 'e':
                *op = true;
                break;
            case 'd':
                *op = false;
                break;
            case 's':
                if (strcmp(optarg, "Kalyna_128_128") == 0) {
                    standart = &Kalyna_128_128;
                } else if (strcmp(optarg, "Kalyna_128_256") == 0) {
                    standart = &Kalyna_128_256;
                } else if (strcmp(optarg, "Kalyna_256_256") == 0) {
                    standart = &Kalyna_256_256;
                } else if (strcmp(optarg, "Kalyna_256_512") == 0) {
                    standart = &Kalyna_256_512;
                } else if (strcmp(optarg, "Kalyna_512_512") == 0) {
                    standart = &Kalyna_512_512;
                } else {
                    fprintf(stderr, "%s wrong standart name, looks -h for supported standarts.\n", optarg);
                    return -1;
                }

                break;
            case 'p':
                *cbc_padding = true;
                break;
            case '?':
                fprintf(stderr, "%s unsuported flag.", optarg);

                return -2;
            case 'h':
                printf("Usage:  kalyna -i file_path -o file_path -k key [options]\n\n\
Kalyna standart decryption\\encyption.\n\
Options:\n\
  -i    Input file path.\n\
  -o    Output file path.\n\
  -e    Encrypt operation. Encrypt by default.\n\
  -k    Given key.\n\
  -d    Decrypt operation.\n\
  -s    Standart name. Allowed options: Kalyna_128_128, Kalyna_128_256,\n\
                        Kalyna_256_256, Kalyna_256_512, Kalyna_512_512\n\
  -p    Use CBC padding.\n\
  -h    display this help.\n");

                return 1;
        }
    }
    if (is_all < 3) {
        fprintf(stderr, "Needs to satisfy all parameters, use -h for help.\n");

        return -3;
    }

    if (key_len != standart->key_dwlength * sizeof(dw)) {
        fprintf(stderr, "Key lenght(%lu) is not standart compliant(%lu).\n",
                key_len, standart->key_dwlength * sizeof(dw));

        return -4;
    }

    return 0;
}

size_t hexs2bin(const char *hex, unsigned char **out) {
	size_t len;
	char   b1;
	char   b2;
	size_t i;

	if (hex == NULL || *hex == '\0' || out == NULL)
		return 0;

	len = strlen(hex);
	if (len % 2 != 0)
		return 0;
	len /= 2;

	memset(*out, 'A', len);
	for (i=0; i<len; i++) {
		if (!hexchr2bin(hex[i*2], &b1) || !hexchr2bin(hex[i*2+1], &b2)) {
			return 0;
		}
		(*out)[i] = (b1 << 4) | b2;
	}
	return len;
}

int hexchr2bin(const char hex, char *out) {
	if (out == NULL)
		return 0;

	if (hex >= '0' && hex <= '9') {
		*out = hex - '0';
	} else if (hex >= 'A' && hex <= 'F') {
		*out = hex - 'A' + 10;
	} else if (hex >= 'a' && hex <= 'f') {
		*out = hex - 'a' + 10;
	} else {
		return 0;
	}

	return 1;
}

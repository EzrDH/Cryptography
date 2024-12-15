#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// XTEA key
static const uint32_t XTEA_KEY[4] = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210};

// XTEA encryption function
void xtea_encrypt(uint32_t v[2], const uint32_t key[4]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
    for (int i = 0; i < 32; i++) {
        v0 += ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
        sum += delta;
        v1 += ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
    }
    v[0] = v0;
    v[1] = v1;
}

// XTEA decryption function
void xtea_decrypt(uint32_t v[2], const uint32_t key[4]) {
    uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta * 32;
    for (int i = 0; i < 32; i++) {
        v1 -= ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
        sum -= delta;
        v0 -= ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
    }
    v[0] = v0;
    v[1] = v1;
}

int main() {
    FILE *inputFile = fopen("0_0.txt", "rb");
    FILE *encryptedFile = fopen("encrypt0_0.xtea", "wb");
    FILE *decryptedFile = fopen("decrypt0_0_xtea.txt", "wb");

    if (inputFile == NULL || encryptedFile == NULL || decryptedFile == NULL) {
        perror("Error opening files");
        exit(1);
    }

    uint32_t block[2] = {0};
    size_t bytesRead;

    // Read file block by block
    while ((bytesRead = fread(block, 1, sizeof(block), inputFile)) > 0) {
        if (bytesRead < sizeof(block)) {
            // Apply padding if block is incomplete
            memset((uint8_t *)block + bytesRead, 0, sizeof(block) - bytesRead);
        }

        // Encrypt the block
        xtea_encrypt(block, XTEA_KEY);
        fwrite(block, 1, sizeof(block), encryptedFile);

        // Decrypt the block for verification
        xtea_decrypt(block, XTEA_KEY);
        if (bytesRead < sizeof(block)) {
            // Remove padding from decrypted block
            fwrite(block, 1, bytesRead, decryptedFile);
        } else {
            fwrite(block, 1, sizeof(block), decryptedFile);
        }
    }

    fclose(inputFile);
    fclose(encryptedFile);
    fclose(decryptedFile);

    printf("File encrypted and decrypted successfully.\n");

    return 0;
}


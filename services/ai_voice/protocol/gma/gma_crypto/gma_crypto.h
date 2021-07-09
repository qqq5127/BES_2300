#ifndef _GMA_CRYPTO_H_
#define _GMA_CRYPTO_H_

#ifdef __cplusplus
extern "C" {
#endif

enum{
GMA_SUCCESS,
GMA_ERROR_EXEC_FAIL,
GMA_ERROR_NULL,
GMA_ERROR_INVALID_PARAM,
GMA_ERROR_DATA_SIZE    
};
#define Gma_CheckParm(code, exp) if (!(exp)) return (code)

typedef struct {
    uint8_t result;
} APP_GMA_CRYPTO_CHEACK_RST_T;

enum{
    GMA_CRYPTO_SUCCESS = 0x00,
    GMA_CRYPTO_FAIL = 0x01,
};

int gma_rand_generator(uint8_t *dest, unsigned size);
void app_gma_cal_sha256(const uint8_t* sec_random, uint8_t* out);
void app_gma_cal_aes128(const uint8_t* sec_random, uint8_t dataLen, uint8_t* key, uint8_t* out);
uint32_t gma_SHA256_hash(const void* in_data, int len, void* out_data);
void gma_crypto_encrypt(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
void gma_crypto_decrypt(uint8_t* output, uint8_t* input, uint32_t length, const uint8_t* key, const uint8_t* iv);
void app_gma_enable_security(bool isEnable);
void app_gma_encrypt_reset(void);
bool app_gma_is_security_enabled(void);
void app_gma_encrypt(uint8_t* input, uint8_t* output, uint32_t len);
void app_gma_decrypt(uint8_t* input, uint8_t* output, uint32_t len);
void gma_secret_key_send(void);
void gma_set_secret_key(char *cryptoSercet, uint32_t secretKeyLen);


#ifdef __cplusplus
}
#endif

#endif


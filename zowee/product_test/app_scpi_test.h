#ifdef __SCPI_TEST__
/*-

 */

#ifndef __SCPI_DEF_H_
#define __SCPI_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SCPI_INPUT_BUFFER_LENGTH 256
#define SCPI_ERROR_QUEUE_SIZE 17

/*  define  product information*/
#define SCPI_IDN1 "MANUFACTURE"
#define SCPI_IDN2 "INSTR2013"
#define SCPI_IDN3 NULL
#define SCPI_IDN4 "01-02"

void SCPI_Input_Fun(const char * data, int len);
void Scpi_test_init(void);


typedef struct{
	uint8_t length;
	uint8_t data[255];
} scpi_data_staruct_t;

int app_scpi_test_vbus_parse(uint8_t *cmd,uint8_t length);
int app_scpi_test_spp_parse(uint8_t *cmd,uint8_t length);


#ifdef __cplusplus
}
#endif

#endif /* __SCPI_DEF_H_ */

#endif


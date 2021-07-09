cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))

ifeq ($(DSP_LIB_ALL_ENABLE),1)

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*/Makefile))
obj-y := $(obj-y:/Makefile=/)

else

obj-y :=
obj-y += \
	CommonTables/ \
	BasicMathFunctions/arm_dot_prod_q15.c \
	FilteringFunctions/arm_biquad_cascade_df1_init_f32.c \
	FilteringFunctions/arm_biquad_cascade_df1_f32.c \
	FilteringFunctions/arm_fir_init_q15.c \
	FilteringFunctions/arm_fir_fast_q15.c \
	TransformFunctions/arm_bitreversal2.S \
	TransformFunctions/arm_cfft_f32.c \
	TransformFunctions/arm_cfft_q15.c \
	TransformFunctions/arm_cfft_radix4_q15.c \
	TransformFunctions/arm_cfft_radix8_f32.c \
	TransformFunctions/arm_rfft_fast_init_f32.c \
	TransformFunctions/arm_rfft_fast_f32.c \
	TransformFunctions/arm_rfft_init_q15.c \
	TransformFunctions/arm_rfft_q15.c \
	SupportFunctions/arm_copy_q31.c \
	SupportFunctions/arm_fill_q31.c \
	SupportFunctions/arm_q15_to_float.c \
	BasicMathFunctions/arm_dot_prod_q7.c \
	BasicMathFunctions/arm_add_q15.c \
	BasicMathFunctions/arm_shift_q15.c \
	BasicMathFunctions/arm_negate_q15.c \
	BasicMathFunctions/arm_offset_q15.c \
	BasicMathFunctions/arm_mult_q15.c \
	BasicMathFunctions/arm_sub_q15.c \
	BasicMathFunctions/arm_mult_f32.c \
	BasicMathFunctions/arm_add_f32.c \
	BasicMathFunctions/arm_scale_f32.c \
	BasicMathFunctions/arm_dot_prod_f32.c \
	SupportFunctions/arm_float_to_q15.c \
	SupportFunctions/arm_fill_f32.c \
	SupportFunctions/arm_copy_f32.c \
	FilteringFunctions/arm_fir_decimate_init_f32.c \
	FilteringFunctions/arm_fir_decimate_f32.c \
	FilteringFunctions/arm_fir_f32.c \
	FilteringFunctions/arm_biquad_cascade_df2T_f32.c \
	FilteringFunctions/arm_biquad_cascade_df2T_init_f32.c \
	ComplexMathFunctions/arm_cmplx_conj_f32.c\
	ComplexMathFunctions/arm_cmplx_mag_squared_f32.c\
	ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c\
	TransformFunctions/arm_cfft_q31.c \
	TransformFunctions/arm_cfft_radix4_q31.c \
	BasicMathFunctions/arm_add_q31.c \
	BasicMathFunctions/arm_mult_q31.c \
	BasicMathFunctions/arm_offset_q31.c \
	BasicMathFunctions/arm_scale_q31.c \
	BasicMathFunctions/arm_shift_q31.c \
	ComplexMathFunctions/arm_cmplx_mag_squared_q31.c\
	ComplexMathFunctions/arm_cmplx_mult_real_q31.c\
	SupportFunctions/arm_float_to_q31.c\
	SupportFunctions/arm_q31_to_float.c\
	ComplexMathFunctions/arm_cmplx_mult_real_f32.c\
	FilteringFunctions/arm_fir_decimate_init_q15.c \
	FilteringFunctions/arm_fir_decimate_q15.c \
	FilteringFunctions/arm_fir_interpolate_init_q15.c \
	FilteringFunctions/arm_fir_interpolate_q15.c \
	StatisticsFunctions/

#soundplus dsp api
ifeq ($(SNDP_TX_3MIC_ENABLE),1)
obj-y += \
	MatrixFunctions/arm_mat_init_f32.c \
	MatrixFunctions/arm_mat_mult_f32.c \
	BasicMathFunctions/arm_scale_f32.c \
	BasicMathFunctions/arm_dot_prod_f32.c \
	FastMathFunctions/arm_cos_f32.c \
	BasicMathFunctions/arm_mult_f32.c \
	BasicMathFunctions/arm_add_f32.c \
	BasicMathFunctions/arm_dot_prod_f32.c \
	FilteringFunctions/arm_fir_decimate_init_f32.c \
	FilteringFunctions/arm_fir_decimate_f32.c \
	FilteringFunctions/arm_fir_interpolate_init_f32.c \
	FilteringFunctions/arm_fir_interpolate_f32.c \
	ComplexMathFunctions/arm_cmplx_mag_squared_f32.c \
	ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c \
	BasicMathFunctions/arm_sub_f32.c	
endif
#soundplus end	

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

endif

ccflags-y += -DARM_MATH_LOOPUNROLL

ifeq ($(SCO_CP_ACCEL),1)
cmsis_dsp_lib-y := $(obj-y)
obj-y := cmsis_dsp_lib.o
endif


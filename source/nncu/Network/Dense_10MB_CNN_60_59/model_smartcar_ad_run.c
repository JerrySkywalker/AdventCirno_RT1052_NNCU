
#include <stdint.h>
#include <stdlib.h>
#define ACT_RELU           1
#define ACT_SOFTMAX        2
#define ACT_RELU6          6
#define ACT_RELU8          18
#define ACT_RELU4          19
#define QUANTIZE_WORD_LEN  14
// --------------------------------------------------------------------------------
// Block 0: InputLayer - input
#define INPUT0_AI                     7
#define INPUT0_IC                     9
#define INPUT0_ICALL                  9
#define INPUT0_ICOFS                  0
#define INPUT0_IX                     8
#define INPUT0_IY                     1
#define INPUT0_OC                     9
#define INPUT0_OCALL                  9
#define INPUT0_OCOFS                  0
#define INPUT0_OX                     8
#define INPUT0_OY                     1
#define INPUT0_zzdbg_5_fracBitsOut    7
// --------------------------------------------------------------------------------
// Block 1: Conv2D - conv2d_1
#define CONV1_AA                      2
#define CONV1_AI                      7
#define CONV1_IC                      9
#define CONV1_ICALL                   9
#define CONV1_ICOFS                   0
#define CONV1_IX                      8
#define CONV1_IY                      1
#define CONV1_KX                      5
#define CONV1_KY                      4
#define CONV1_OC                      32
#define CONV1_OCALL                   32
#define CONV1_OCOFS                   0
#define CONV1_OX                      8
#define CONV1_OY                      1
#define CONV1_PX                      2
#define CONV1_PY                      1
#define CONV1_SB                      5
#define CONV1_SO                      5
#define CONV1_SX                      1
#define CONV1_SY                      1
#define CONV1_zzdbg_2_SH_WT           10
#define CONV1_zzdbg_3_productShf      17
#define CONV1_zzdbg_4_SB_AMP          12
#define CONV1_zzdbg_5_fracBitsOut     12
#define CONV1_zzdbg_8_AuxAct          "TANH"
// --------------------------------------------------------------------------------
// Block 2: Conv2D - conv2d_2
#define CONV2_AA                      4
#define CONV2_AI                      13
#define CONV2_IC                      32
#define CONV2_ICALL                   32
#define CONV2_ICOFS                   0
#define CONV2_IX                      8
#define CONV2_IY                      1
#define CONV2_KX                      3
#define CONV2_KY                      3
#define CONV2_OC                      32
#define CONV2_OCALL                   32
#define CONV2_OCOFS                   0
#define CONV2_OX                      8
#define CONV2_OY                      1
#define CONV2_PX                      1
#define CONV2_PY                      1
#define CONV2_SB                      12
#define CONV2_SO                      13
#define CONV2_SX                      1
#define CONV2_SY                      1
#define CONV2_zzdbg_2_SH_WT           10
#define CONV2_zzdbg_3_productShf      23
#define CONV2_zzdbg_4_SB_AMP          11
#define CONV2_zzdbg_5_fracBitsOut     10
#define CONV2_zzdbg_8_AuxAct          "RELU"
// --------------------------------------------------------------------------------
// Block 2: BatchNormalization - batch_normalization_1
#define BN2_AA                        4
#define BN2_AI                        10
#define BN2_IC                        32
#define BN2_ICALL                     32
#define BN2_ICOFS                     0
#define BN2_IX                        8
#define BN2_IY                        1
#define BN2_KX                        1
#define BN2_KY                        1
#define BN2_OC                        32
#define BN2_OCALL                     32
#define BN2_OCOFS                     0
#define BN2_OX                        8
#define BN2_OY                        1
#define BN2_PX                        0
#define BN2_PY                        0
#define BN2_SB                        5
#define BN2_SO                        8
#define BN2_SX                        1
#define BN2_SY                        1
#define BN2_zzdbg_2_SH_WT             8
#define BN2_zzdbg_3_productShf        18
#define BN2_zzdbg_4_SB_AMP            13
#define BN2_zzdbg_5_fracBitsOut       10
// --------------------------------------------------------------------------------
// Block 2: Flatten - flatten_1
#define FLAT2_AI                      10
#define FLAT2_IC                      32
#define FLAT2_ICALL                   32
#define FLAT2_ICOFS                   0
#define FLAT2_IX                      8
#define FLAT2_IY                      1
#define FLAT2_OC                      1
#define FLAT2_OCALL                   1
#define FLAT2_OCOFS                   0
#define FLAT2_OX                      1
#define FLAT2_OY                      256
#define FLAT2_zzdbg_5_fracBitsOut     10
// --------------------------------------------------------------------------------
// Block 3: Dense - dense_1
#define FC3_AA                        2
#define FC3_AI                        10
#define FC3_IC                        1
#define FC3_ICALL                     1
#define FC3_ICOFS                     0
#define FC3_IX                        1
#define FC3_IY                        256
#define FC3_OC                        1
#define FC3_OCALL                     1
#define FC3_OCOFS                     0
#define FC3_OX                        1
#define FC3_OY                        160
#define FC3_SB                        6
#define FC3_SO                        10
#define FC3_zzdbg_2_SH_WT             12
#define FC3_zzdbg_3_productShf        22
#define FC3_zzdbg_4_SB_AMP            16
#define FC3_zzdbg_5_fracBitsOut       12
#define FC3_zzdbg_8_AuxAct            "TANH"
// --------------------------------------------------------------------------------
// Block 4: Dense - dense_2
#define FC4_AA                        2
#define FC4_AI                        13
#define FC4_IC                        1
#define FC4_ICALL                     1
#define FC4_ICOFS                     0
#define FC4_IX                        1
#define FC4_IY                        160
#define FC4_OC                        1
#define FC4_OCALL                     1
#define FC4_OCOFS                     0
#define FC4_OX                        1
#define FC4_OY                        1
#define FC4_SB                        16
#define FC4_SO                        17
#define FC4_zzdbg_2_SH_WT             16
#define FC4_zzdbg_3_productShf        29
#define FC4_zzdbg_4_SB_AMP            13
#define FC4_zzdbg_5_fracBitsOut       12
#define FC4_zzdbg_8_AuxAct            "TANH"
extern const int16_t cg_CONV1weit[]; // 5760 - Co,H,W,Ci: (32, 4, 5, 9)
extern const int16_t cg_CONV1bias[]; // 32
extern const int16_t cg_CONV2weit[]; // 9216 - Co,H,W,Ci: (32, 3, 3, 32)
extern const int16_t cg_CONV2bias[]; // 32
extern const int16_t cg_BN2weit[]; // 32 - Co,H,W,Ci: (1, 1, 1, 32)
extern const int16_t cg_BN2bias[]; // 32
extern const int16_t cg_FC3weit[]; // 40960 - Co, Di: (160, 256)
extern const int16_t cg_FC3bias[]; // 160
extern const int16_t cg_FC4weit[]; // 160 - Co, Di: (1, 160)
extern const int16_t cg_FC4bias[]; // 1

#include <stdint.h>
#include "arm_math.h"
#include "arm_nnfunctions.h"
#include "aia_cmsisnn_ext.h"
// #include <model.h>

int32_t img_buffer0[(512 + 3) / 4];
int32_t img_buffer1[(512 + 3) / 4];
int32_t col_buf[(1152 + 3) / 4]; // [2, 3, 3, 32, 2]
int16_t out_buf[1]; // FC4_OY
// total static buffer size: 2.13 kB

// generated RunModel(), returns the output buffer of the last layer
void* RunModel(const void *in_buf)
{ 
    
	{
		const int8_t *pSrc = (int8_t*)in_buf;
		int16_t *pDst = (int16_t*)img_buffer0;
		uint32_t cnt = 72;
		int normOfs = 0;
		while(cnt--){
			*pDst++ = (int16_t)((int)(*pSrc++) - normOfs);
		}
	}
	// Block 1: Conv2D - conv2d_1
    aia_convolve_HWC_q15_basic_nonsquare((int16_t*)img_buffer0/*0*/, CONV1_IX/*8*/, CONV1_IY/*1*/, CONV1_IC/*9*/
        , cg_CONV1weit/*weit*/, CONV1_OC/*32*/, CONV1_KX/*5*/, CONV1_KY/*4*/, CONV1_PX/*2*/, CONV1_PY/*1*/, CONV1_SX/*1*/
        , CONV1_SY/*1*/, cg_CONV1bias/*bias*/, CONV1_SB/*5*/, CONV1_SO/*5*/, (int16_t*)img_buffer1/*1*/, CONV1_OX/*8*/
        , CONV1_OY/*1*/, (int16_t *) col_buf, NULL);

	// Block 1: Conv2D - conv2d_1, auxact tanh
    aia_nn_activations_direct_q15((int16_t*)img_buffer1/*1*/, CONV1_OY/*1*/, CONV1_AA/*2*/, ARM_TANH, 14);

	// Block 2: Conv2D - conv2d_2
    aia_convolve_HWC_q15_fast_nonsquare((int16_t*)img_buffer1/*1*/, CONV2_IX/*8*/, CONV2_IY/*1*/, CONV2_IC/*32*/
        , cg_CONV2weit/*weit*/, CONV2_OC/*32*/, CONV2_KX/*3*/, CONV2_KY/*3*/, CONV2_PX/*1*/, CONV2_PY/*1*/, CONV2_SX/*1*/
        , CONV2_SY/*1*/, cg_CONV2bias/*bias*/, CONV2_SB/*12*/, CONV2_SO/*13*/, (int16_t*)img_buffer0/*0*/, CONV2_OX/*8*/
        , CONV2_OY/*1*/, (int16_t *) col_buf, NULL);

	// Block 2: Conv2D - conv2d_2, auxact relu
    arm_relu_q15((int16_t*)img_buffer0/*0*/, (uint32_t)(CONV2_OX * CONV2_OY * CONV2_OC)/*8x1x32*/);

	// Block 2: BatchNormalization - batch_normalization_1
    aia_depthwise_separable_conv_HWC_q15_nonsquare((int16_t*)img_buffer0/*0*/, BN2_IX/*8*/, BN2_IY/*1*/, BN2_IC/*32*/
        , cg_BN2weit/*weit*/, BN2_OC/*32*/, BN2_KX/*1*/, BN2_KY/*1*/, BN2_PX/*0*/, BN2_PY/*0*/, BN2_SX/*1*/, BN2_SY/*1*/
        , cg_BN2bias/*bias*/, BN2_SB/*5*/, BN2_SO/*8*/, (int16_t*)img_buffer1/*1*/, BN2_OX/*8*/, BN2_OY/*1*/, (int16_t *) col_buf, NULL);

	// Block 3: Dense - dense_1
    arm_fully_connected_q15((int16_t*)img_buffer1/*1*/, cg_FC3weit/*weit*/, FC3_IY/*256*/, FC3_OY/*160*/, FC3_SB/*6*/
        , FC3_SO/*10*/, cg_FC3bias/*bias*/, (int16_t*)img_buffer0/*0*/, (int16_t*)col_buf);

	// Block 3: Dense - dense_1, auxact tanh
    aia_nn_activations_direct_q15((int16_t*)img_buffer0/*0*/, FC3_OY/*160*/, FC3_AA/*2*/, ARM_TANH, 14);

	// Block 4: Dense - dense_2
    arm_fully_connected_q15((int16_t*)img_buffer0/*0*/, cg_FC4weit/*weit*/, FC4_IY/*160*/, FC4_OY/*1*/, FC4_SB/*16*/
        , FC4_SO/*17*/, cg_FC4bias/*bias*/, (int16_t*)img_buffer1/*1*/, (int16_t*)col_buf);

	// Block 4: Dense - dense_2, auxact tanh
    aia_nn_activations_direct_q15((int16_t*)img_buffer1/*1*/, FC4_OY/*1*/, FC4_AA/*2*/, ARM_TANH, 14);

	memcpy(out_buf, img_buffer1, 2);

	return out_buf;
}


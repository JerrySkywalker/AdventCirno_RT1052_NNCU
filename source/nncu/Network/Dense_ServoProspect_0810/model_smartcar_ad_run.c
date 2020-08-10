
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
#define INPUT0_IC                     1
#define INPUT0_ICALL                  1
#define INPUT0_ICOFS                  0
#define INPUT0_IX                     9
#define INPUT0_IY                     1
#define INPUT0_OC                     1
#define INPUT0_OCALL                  1
#define INPUT0_OCOFS                  0
#define INPUT0_OX                     9
#define INPUT0_OY                     1
#define INPUT0_zzdbg_5_fracBitsOut    7
// --------------------------------------------------------------------------------
// Block 0: Flatten - flatten_2
#define FLAT0_AI                      7
#define FLAT0_IC                      1
#define FLAT0_ICALL                   1
#define FLAT0_ICOFS                   0
#define FLAT0_IX                      9
#define FLAT0_IY                      1
#define FLAT0_OC                      1
#define FLAT0_OCALL                   1
#define FLAT0_OCOFS                   0
#define FLAT0_OX                      1
#define FLAT0_OY                      9
#define FLAT0_zzdbg_5_fracBitsOut     7
// --------------------------------------------------------------------------------
// Block 1: Dense - dense_5
#define FC1_AA                        2
#define FC1_AI                        7
#define FC1_IC                        1
#define FC1_ICALL                     1
#define FC1_ICOFS                     0
#define FC1_IX                        1
#define FC1_IY                        9
#define FC1_OC                        1
#define FC1_OCALL                     1
#define FC1_OCOFS                     0
#define FC1_OX                        1
#define FC1_OY                        180
#define FC1_SB                        6
#define FC1_SO                        7
#define FC1_zzdbg_2_SH_WT             12
#define FC1_zzdbg_3_productShf        19
#define FC1_zzdbg_4_SB_AMP            13
#define FC1_zzdbg_5_fracBitsOut       12
#define FC1_zzdbg_8_AuxAct            "LINEAR"
// --------------------------------------------------------------------------------
// Block 1: Activation - activation_3
#define ACT1_AA                       2
#define ACT1_ACT                      ACT_TANH
#define ACT1_AI                       12
#define ACT1_IC                       1
#define ACT1_ICALL                    1
#define ACT1_ICOFS                    0
#define ACT1_IX                       1
#define ACT1_IY                       180
#define ACT1_OC                       1
#define ACT1_OCALL                    1
#define ACT1_OCOFS                    0
#define ACT1_OX                       1
#define ACT1_OY                       180
#define ACT1_zzdbg_5_fracBitsOut      13
// --------------------------------------------------------------------------------
// Block 2: Dense - dense_6
#define FC2_AA                        2
#define FC2_AI                        13
#define FC2_IC                        1
#define FC2_ICALL                     1
#define FC2_ICOFS                     0
#define FC2_IX                        1
#define FC2_IY                        180
#define FC2_OC                        1
#define FC2_OCALL                     1
#define FC2_OCOFS                     0
#define FC2_OX                        1
#define FC2_OY                        100
#define FC2_SB                        12
#define FC2_SO                        12
#define FC2_zzdbg_2_SH_WT             11
#define FC2_zzdbg_3_productShf        24
#define FC2_zzdbg_4_SB_AMP            12
#define FC2_zzdbg_5_fracBitsOut       12
#define FC2_zzdbg_8_AuxAct            "LINEAR"
// --------------------------------------------------------------------------------
// Block 2: Activation - activation_4
#define ACT2_AA                       2
#define ACT2_ACT                      ACT_TANH
#define ACT2_AI                       12
#define ACT2_IC                       1
#define ACT2_ICALL                    1
#define ACT2_ICOFS                    0
#define ACT2_IX                       1
#define ACT2_IY                       100
#define ACT2_OC                       1
#define ACT2_OCALL                    1
#define ACT2_OCOFS                    0
#define ACT2_OX                       1
#define ACT2_OY                       100
#define ACT2_zzdbg_5_fracBitsOut      13
// --------------------------------------------------------------------------------
// Block 3: Dense - dense_7
#define FC3_AA                        4
#define FC3_AI                        13
#define FC3_IC                        1
#define FC3_ICALL                     1
#define FC3_ICOFS                     0
#define FC3_IX                        1
#define FC3_IY                        100
#define FC3_OC                        1
#define FC3_OCALL                     1
#define FC3_OCOFS                     0
#define FC3_OX                        1
#define FC3_OY                        40
#define FC3_SB                        14
#define FC3_SO                        15
#define FC3_zzdbg_2_SH_WT             12
#define FC3_zzdbg_3_productShf        25
#define FC3_zzdbg_4_SB_AMP            11
#define FC3_zzdbg_5_fracBitsOut       10
#define FC3_zzdbg_8_AuxAct            "LINEAR"
// --------------------------------------------------------------------------------
// Block 3: Activation - re_lu_2
#define ACT3_ACT                      ACT_RELU8
#define ACT3_AI                       10
#define ACT3_IC                       1
#define ACT3_ICALL                    1
#define ACT3_ICOFS                    0
#define ACT3_IX                       1
#define ACT3_IY                       40
#define ACT3_OC                       1
#define ACT3_OCALL                    1
#define ACT3_OCOFS                    0
#define ACT3_OX                       1
#define ACT3_OY                       40
#define ACT3_zzdbg_5_fracBitsOut      10
// --------------------------------------------------------------------------------
// Block 4: Dense - dense_8
#define FC4_AA                        4
#define FC4_AI                        10
#define FC4_IC                        1
#define FC4_ICALL                     1
#define FC4_ICOFS                     0
#define FC4_IX                        1
#define FC4_IY                        40
#define FC4_OC                        1
#define FC4_OCALL                     1
#define FC4_OCOFS                     0
#define FC4_OX                        1
#define FC4_OY                        1
#define FC4_SB                        12
#define FC4_SO                        17
#define FC4_zzdbg_2_SH_WT             17
#define FC4_zzdbg_3_productShf        27
#define FC4_zzdbg_4_SB_AMP            15
#define FC4_zzdbg_5_fracBitsOut       10
#define FC4_zzdbg_8_AuxAct            "LINEAR"
extern const int16_t cg_SP_FC1weit[]; // 1620 - Co, Di: (180, 9)
extern const int16_t cg_SP_FC1bias[]; // 180
extern const int16_t cg_SP_FC2weit[]; // 18000 - Co, Di: (100, 180)
extern const int16_t cg_SP_FC2bias[]; // 100
extern const int16_t cg_SP_FC3weit[]; // 4000 - Co, Di: (40, 100)
extern const int16_t cg_SP_FC3bias[]; // 40
extern const int16_t cg_SP_FC4weit[]; // 40 - Co, Di: (1, 40)
extern const int16_t cg_SP_FC4bias[]; // 1

#include <stdint.h>
#include "arm_math.h"
#include "arm_nnfunctions.h"
#include "aia_cmsisnn_ext.h"
// #include <model.h>

static int32_t img_buffer0[(200 + 3) / 4];
static int32_t img_buffer1[(360 + 3) / 4];
static int32_t col_buf[(360 + 3) / 4]; // [2, 180, 2]
static int16_t out_buf[1]; // FC4_OY
// total static buffer size: 0.90 kB

// generated RunModel(), returns the output buffer of the last layer
void* RunModel_SP(const void *in_buf)
{ 
    
	{
		const int8_t *pSrc = (int8_t*)in_buf;
		int16_t *pDst = (int16_t*)img_buffer0;
		uint32_t cnt = 9;
		int normOfs = 0;
		while(cnt--){
			*pDst++ = (int16_t)((int)(*pSrc++) - normOfs);
		}
	}
	// Block 1: Dense - dense_5
    arm_fully_connected_q15((int16_t*)img_buffer0/*0*/, cg_SP_FC1weit/*weit*/, FC1_IY/*9*/, FC1_OY/*180*/, FC1_SB/*6*/
        , FC1_SO/*7*/, cg_SP_FC1bias/*bias*/, (int16_t*)img_buffer1/*1*/, (int16_t*)col_buf);

	// Block 1: Activation - activation_3
    aia_nn_activations_direct_q15((int16_t*)img_buffer1/*1*/, ACT1_OY/*180*/, ACT1_AA/*2*/, ARM_TANH, 14);

	// Block 2: Dense - dense_6
    arm_fully_connected_q15((int16_t*)img_buffer1/*1*/, cg_SP_FC2weit/*weit*/, FC2_IY/*180*/, FC2_OY/*100*/, FC2_SB/*12*/
        , FC2_SO/*12*/, cg_SP_FC2bias/*bias*/, (int16_t*)img_buffer0/*0*/, (int16_t*)col_buf);

	// Block 2: Activation - activation_4
    aia_nn_activations_direct_q15((int16_t*)img_buffer0/*0*/, ACT2_OY/*100*/, ACT2_AA/*2*/, ARM_TANH, 14);

	// Block 3: Dense - dense_7
    arm_fully_connected_q15((int16_t*)img_buffer0/*0*/, cg_SP_FC3weit/*weit*/, FC3_IY/*100*/, FC3_OY/*40*/, FC3_SB/*14*/
        , FC3_SO/*15*/, cg_SP_FC3bias/*bias*/, (int16_t*)img_buffer1/*1*/, (int16_t*)col_buf);

	// Block 3: Activation - re_lu_2
    aia_relu8_q15((int16_t*)img_buffer1/*1*/, (uint32_t)(ACT3_IX * ACT3_IY * ACT3_IC)/*1x40x1*/, ACT3_AI);

	// Block 4: Dense - dense_8
    arm_fully_connected_q15((int16_t*)img_buffer1/*1*/, cg_SP_FC4weit/*weit*/, FC4_IY/*40*/, FC4_OY/*1*/, FC4_SB/*12*/
        , FC4_SO/*17*/, cg_SP_FC4bias/*bias*/, (int16_t*)img_buffer0/*0*/, (int16_t*)col_buf);

	memcpy(out_buf, img_buffer0, 2);

	return out_buf;
}


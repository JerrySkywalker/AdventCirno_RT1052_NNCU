#include <stdint.h>
#include <stdio.h>
#include "arm_math.h"

#include "arm_nnfunctions.h"

#ifdef _RTE_
// #include "RTE_Components.h"
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif
#endif

// oc: output channel
// so: shift (right) for output
// si#: shift (left) for input
//#include "..\nncie.h"

// like arm_softmax_q7, but supports multiple vectors, mainly for Object Detection models
void aia_softmax_multi_q7(const q7_t* vec_in, const int dim_vec, int vecCnt, q7_t* p_out)
{
	while (vecCnt--) {
		q31_t     sum;
		int16_t   i;
		uint8_t   shift;
		q15_t     base;
		base = -257;
		/* We first search for the maximum */
		for (i = 0; i < dim_vec; i++)
		{
			if (vec_in[i] > base)
			{
				base = vec_in[i];
			}
		}

		/*
		 * So the base is set to max-8, meaning
		 * that we ignore really small values.
		 * anyway, they will be 0 after shrinking to q7_t.
		 */
		base = base - 8;

		sum = 0;

		for (i = 0; i < dim_vec; i++)
		{
			if (vec_in[i] > base)
			{
				shift = (uint8_t)__USAT(vec_in[i] - base, 5);
				sum += 0x1 << shift;
			}
		}

		/* This is effectively (0x1 << 20) / sum */
		int output_base = 0x100000 / sum;

		/*
		 * Final confidence will be output_base >> ( 13 - (vec_in[i] - base) )
		 * so 128 (0x1<<7) -> 100% confidence when sum = 0x1 << 8, output_base = 0x1 << 12
		 * and vec_in[i]-base = 8
		 */
		for (i = 0; i < dim_vec; i++)
		{
			if (vec_in[i] > base)
			{
				/* Here minimum value of 13+base-vec_in[i] will be 5 */
				shift = (uint8_t)__USAT(13 + base - vec_in[i], 5);
				*p_out++ = (q7_t)__SSAT((output_base >> shift), 8);
			}
			else {
				*p_out++ = 0;
			}
		}
		vec_in += dim_vec;
	}
}

void aia_softmax_multi_q15(const q15_t* vec_in, const uint16_t dim_vec, int vecCnt, q15_t* p_out)
{
	while (vecCnt--) {
		q31_t     sum;
		int16_t   i;
		uint8_t   shift;
		q31_t     base;
		base = -1 * 0x100000;
		for (i = 0; i < dim_vec; i++)
		{
			if (vec_in[i] > base)
			{
				base = vec_in[i];
			}
		}

		/* we ignore really small values
		 * anyway, they will be 0 after shrinking
		 * to q15_t
		 */
		base = base - 16;

		sum = 0;

		for (i = 0; i < dim_vec; i++)
		{
			if (vec_in[i] > base)
			{
				shift = (uint8_t)__USAT(vec_in[i] - base, 5);
				sum += 0x1 << shift;
			}
		}

		/* This is effectively (0x1 << 32) / sum */
		int64_t div_base = 0x100000000LL;
		int output_base = (int32_t)(div_base / sum);

		/* Final confidence will be output_base >> ( 17 - (vec_in[i] - base) )
		 * so 32768 (0x1<<15) -> 100% confidence when sum = 0x1 << 16, output_base = 0x1 << 16
		 * and vec_in[i]-base = 16
		 */
		for (i = 0; i < dim_vec; i++)
		{
			if (vec_in[i] > base)
			{
				/* Here minimum value of 17+base-vec[i] will be 1 */
				shift = (uint8_t)__USAT(17 + base - vec_in[i], 5);
				*p_out++ = (q15_t)__SSAT((output_base >> shift), 16);
			}
			else
			{
				*p_out++ = 0;
			}
		}
		vec_in += dim_vec;
	}
}

void _prvConcatCopy(uint32_t inAddr, uint32_t outAddr, int inFracBits, int cbUnit, int outFracBits, int quantBits, int n) {
	int j, reg, shf;
	const q7_t* pcIn = (const q7_t*)inAddr;
	const q15_t* pcIn16 = (const q15_t*)inAddr;
	q7_t* pOut = (q7_t*)outAddr;
	q15_t* pOut16 = (q15_t*)outAddr;
	shf = inFracBits - outFracBits;
	int min = -(1 << (quantBits - 1));
	int max = -min - 1;
	int rounded;
	if (shf < 0) {
		// input tensor has less frac bits, must shift left then satur
		shf = -shf;
		rounded = 1 << (shf - 1);
		for (j = 0; j < n; j++) {
			if (cbUnit == 1) {
				reg = ((int)(*pcIn++) << shf) + rounded;
				reg = __SSAT(reg, 8);
				*pOut++ = (q7_t)reg;
			}
			else {
				reg = ((int)(*pcIn16++) << shf) + rounded;
				if (reg > max)
					reg = max;
				else if (reg < min)
					reg = min;
				*pOut16++ = (q15_t)reg;
			}
		}
	}
	else {
		// input tensor has more frac bits, must shift right
		for (j = 0; j < n; j++) {
			reg = (int)(*pcIn++) >> shf;
			if (cbUnit == 1) {
				*pOut++ = (q7_t)reg;
			}
			else {
				*pOut16++ = (q15_t)reg;
			}
		}
	}
}
//
//// @param cbUnit: bytes per parameter, q7 =1, q15 = 2
//// @param axis: which axis to concat, initially only support axis 0 (first axis) and axis -1 (last axis)
//// @param tnsCnt: How many input tensors to concat
//// @param axisCnt: How many axis per tensor
//// @param pcTnss: pointer to an array of CI_TnsDim_t, memory is allocated by caller
//// @param pvOut: output buffer, allocated by caller
//arm_status aia_concat(int quantBits, int axis, int tnsCnt, int axisCnt, const CI_TnsDim_t* pcTnss, void* pvOut, int outFracBits) {
//	uint32_t addr = (uint32_t)pvOut;
//	int cbUnit = (quantBits + 7) >> 3;
//	int i, j;
//	if (1 == axis)  // in fact this is for first dimension, as the mini-batch dimension is not available for inference
//		{
//		int tnsSize;
//		for (i = 0; i < tnsCnt; i++, pcTnss++) {
//			for (tnsSize = cbUnit, j = 0; j < axisCnt; j++) {
//				tnsSize *= pcTnss->dims[j];
//			}
//			if (pcTnss->fracBits == outFracBits) {
//				memcpy((void*)addr, pcTnss->pcq7, tnsSize);
//			}
//			else {
//				_prvConcatCopy(pcTnss->tnsAddr, addr, pcTnss->fracBits, cbUnit, outFracBits, quantBits, tnsSize / cbUnit);
//			}
//			addr += tnsSize;
//		}
//	}
//	else if (-1 == axis) {
//		axis = axisCnt - 1;
//		int barNdx, cbConcatBarLen = 0;	// a bar is all depth channels at a given (X,Y) position
//		int barCnt;
//
//		// calculate how many bars to copy, all tensors must have same dim size except last dim
//		for (barCnt = 1, j = 0; j < axisCnt - 1; j++) {
//			barCnt *= pcTnss->dims[j];
//		}
//		// calculate the length of concated bar
//		for (i = 0; i < tnsCnt; i++) {
//			cbConcatBarLen += pcTnss[i].dims[axis];
//		}
//		cbConcatBarLen *= cbUnit;
//
//		uint32_t addrBkup = addr, tnsAddr, cbEachTnsBarLen;
//		// copy tensor by tensor
//		for (i = 0; i < tnsCnt; i++, pcTnss++) {
//			cbEachTnsBarLen = pcTnss->dims[axis] * cbUnit;
//			tnsAddr = pcTnss->tnsAddr;
//			// inner loop: copy bar by bar
//			for (barNdx = 0; barNdx < barCnt; barNdx++) {
//				if (pcTnss->fracBits == outFracBits) {
//					memcpy((void*)addr, (void*)tnsAddr, cbEachTnsBarLen);
//				}
//				else {
//					_prvConcatCopy(tnsAddr, addr, pcTnss->fracBits, cbUnit, outFracBits, quantBits, pcTnss->dims[axis]);
//				}
//				addr += cbConcatBarLen, tnsAddr += cbEachTnsBarLen;
//
//			}
//			addrBkup += cbEachTnsBarLen;
//			addr = addrBkup;
//		}
//	}
//	else {
//		while (1) {}
//		// return ARM_MATH_SIZE_MISMATCH;
//	}
//	return ARM_MATH_SUCCESS;
//}

void aia_HWC_q7_add2(q7_t *outBuf, uint32_t cnt, uint32_t so, q7_t *in1, uint32_t si1, q7_t *in2, uint32_t si2)
{
	uint32_t i;
	int c;
	int so2 = si1 + si2 - so;
	
	for (i=0; i<cnt; i++) {
		c = (*in1++ << si2) + (*in2++ << si1);
		if (so2 >= 0)
			c >>= so2;
		else
			c <<= -so2;
		if (c >= 127)
			c = 127;
		else if (c < -128)
			c = -128;
		*outBuf++ = (q7_t)c;
	}
}

void aia_relu6_q7(q7_t * data, uint32_t size, uint32_t shfIn) {
	uint32_t i;
	int a, maxVal = 6 << shfIn;
	arm_relu_q7(data, size);
	for (i=0; i< size; i++) {
		a = *data++;
		if (a > maxVal)
			data[-1] = maxVal;
	}
}

void aia_relu8_q7(q7_t* data, uint32_t size, uint32_t shfIn) {
	uint32_t i;
	int a, maxVal = (8 << shfIn) - 1;
	arm_relu_q7(data, size);
	for (i = 0; i < size; i++) {
		a = *data++;
		if (a > maxVal)
			data[-1] = maxVal;
	}
}

void aia_relu4_q7(q7_t* data, uint32_t size, uint32_t shfIn) {
	uint32_t i;
	int a, maxVal = (4 << shfIn) - 1;
	arm_relu_q7(data, size);
	for (i = 0; i < size; i++) {
		a = *data++;
		if (a > maxVal)
			data[-1] = maxVal;
	}
}

void aia_HWC_q15_add2(q15_t *outBuf, uint32_t cnt, uint32_t so, q15_t *in1, uint32_t si1, q15_t *in2, uint32_t si2)
{
	uint32_t i;
	int c;
	int so2 = si1 + si2 - so;
	
	for (i=0; i<cnt; i++) {
		c = (*in1++ << si2) + (*in2++ << si1);
		if (so2 >= 0)
			c >>= so2;
		else
			c <<= -so2;
		if (c >= 32767)
			c = 32767;
		else if (c < -32768)
			c = -32768;
		*outBuf++ = (q15_t)c;
	}
}

void aia_relu6_q15(q15_t * data, uint32_t size, uint32_t shfIn) {
	uint32_t i;
	int a, maxVal = 6 << shfIn;
	arm_relu_q15(data, size);
	for (i=0; i< size; i++) {
		a = *data++;
		if (a > maxVal)
			data[-1] = maxVal;
	}
}

void aia_relu8_q15(q15_t* data, uint32_t size, uint32_t shfIn) {
	uint32_t i;
	int a, maxVal = (8 << shfIn) - 1;
	arm_relu_q15(data, size);
	for (i = 0; i < size; i++) {
		a = *data++;
		if (a > maxVal)
			data[-1] = maxVal;
	}
}

void aia_relu4_q15(q15_t* data, uint32_t size, uint32_t shfIn) {
	uint32_t i;
	int a, maxVal = (4 << shfIn) - 1;
	arm_relu_q15(data, size);
	for (i = 0; i < size; i++) {
		a = *data++;
		if (a > maxVal)
			data[-1] = maxVal;
	}
}

void aia_sigmoid_q7(q7_t * data, uint32_t size, uint32_t int_width) {
	arm_nn_activations_direct_q7(data, size, int_width, ARM_SIGMOID);
}
void aia_tanh_q7(q7_t * data, uint32_t size, uint32_t int_width) {
	arm_nn_activations_direct_q7(data, size, int_width, ARM_TANH);
}
void aia_sigmoid_q15(q15_t* data, uint32_t size, uint32_t int_width) {
	arm_nn_activations_direct_q15(data, size, int_width, ARM_SIGMOID);
}
void aia_tanh_q15(q15_t* data, uint32_t size, uint32_t int_width) {
	arm_nn_activations_direct_q15(data, size, int_width, ARM_TANH);
}

/*
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /* ----------------------------------------------------------------------
  * Project:      CMSIS NN Library
  * Title:        arm_depthwise_separable_conv_HWC_q7_nonsquare.c
  * Description:  Q7 depthwise separable convolution function (non-square shape)
  *
  * $Date:        17. January 2018
  * $Revision:    V.1.0.0
  *
  * Target Processor:  Cortex-M cores
  *
  * -------------------------------------------------------------------- */

#include "arm_math.h"
#include "arm_nnfunctions.h"

  /**
   *  @ingroup groupNN
   */

   /**
	* @addtogroup NNConv
	* @{
	*/

	/**
	 * @brief Q7 depthwise separable convolution function (non-square shape)
	 * @param[in]       Im_in         pointer to input tensor
	 * @param[in]       dim_im_in_x   input tensor dimention x
	 * @param[in]       dim_im_in_y   input tensor dimention y
	 * @param[in]       ch_im_in      number of input tensor channels
	 * @param[in]       wt            pointer to kernel weights
	 * @param[in]       ch_im_out     number of filters, i.e., output tensor channels
	 * @param[in]       dim_kernel_x  filter kernel size x
	 * @param[in]       dim_kernel_y  filter kernel size y
	 * @param[in]       padding_x     padding sizes x
	 * @param[in]       padding_y     padding sizes y
	 * @param[in]       stride_x      convolution stride x
	 * @param[in]       stride_y      convolution stride y
	 * @param[in]       bias          pointer to bias
	 * @param[in]       bias_shift    amount of left-shift for bias
	 * @param[in]       out_shift     amount of right-shift for output
	 * @param[in,out]   Im_out        pointer to output tensor
	 * @param[in]       dim_im_out_x  output tensor dimension x
	 * @param[in]       dim_im_out_y  output tensor dimension y
	 * @param[in,out]   bufferA       pointer to buffer space for input
	 * @param[in,out]   bufferB       pointer to buffer space for output
	 * @return     The function returns either
	 * <code>ARM_MATH_SIZE_MISMATCH</code> or <code>ARM_MATH_SUCCESS</code> based on the outcome of size checking.
	 *
	 * This function is the version with full list of optimization tricks, but with
	 * some contraints:
	 *   ch_im_in is multiple of 2
	 *   ch_im_out is multiple of 2
	 */

arm_status aia_depthwise_separable_conv_HWC_q15_nonsquare(const q15_t* Im_in,
	const int32_t dim_im_in_x,
	const int32_t dim_im_in_y,
	const int32_t ch_im_in,
	const q15_t* wt,
	const int32_t ch_im_out,
	const int32_t dim_kernel_x,
	const int32_t dim_kernel_y,
	const int32_t padding_x,
	const int32_t padding_y,
	const int32_t stride_x,
	const int32_t stride_y,
	const q15_t* bias,
	const int32_t bias_shift,
	const int32_t out_shift,
	q15_t* Im_out,
	const int32_t dim_im_out_x,
	const int32_t dim_im_out_y,
	q15_t* bufferA,
	q15_t* bufferB)
{

#if 0 //defined (ARM_MATH_DSP)

#else
	/* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */
	int       out_y, out_x, ch_out;
	int       ker_y, ker_x;

	/* do some checking here, basically ch_im_in == ch_im_out */
	if (ch_im_in != ch_im_out)
	{
		return ARM_MATH_SIZE_MISMATCH;
	}

	for (out_y = 0; out_y < dim_im_out_y; out_y++)
	{
		for (out_x = 0; out_x < dim_im_out_x; out_x++)
		{
			for (ch_out = 0; ch_out < ch_im_out; ch_out++)
			{
				// for each output 
				int       conv_out = ((q31_t)(bias[ch_out]) << bias_shift) + NN_ROUND(out_shift);
				for (ker_y = 0; ker_y < dim_kernel_y; ker_y++)
				{
					for (ker_x = 0; ker_x < dim_kernel_x; ker_x++)
					{
						int       in_row = stride_y * out_y + ker_y - padding_y;
						int       in_col = stride_x * out_x + ker_x - padding_x;
						if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
						{
							conv_out += Im_in[(in_row * dim_im_in_x + in_col) * ch_im_in + ch_out] *
								wt[(ker_y * dim_kernel_x + ker_x) * ch_im_out + ch_out];
						}
					}
				}
				Im_out[(out_y * dim_im_out_x + out_x) * ch_im_out + ch_out] =
					(q15_t)__SSAT((conv_out >> out_shift), 16);
			}
		}
	}

#endif                          /* ARM_MATH_DSP */


	/* Return to application */
	return ARM_MATH_SUCCESS;

}

arm_status
aia_convolve_HWC_q15_fast_nonsquare(const q15_t * Im_in,
                                    const uint16_t dim_im_in_x,
                                    const uint16_t dim_im_in_y,
                                    const uint16_t ch_im_in,
                                    const q15_t * wt,
                                    const uint16_t ch_im_out,
                                    const uint16_t dim_kernel_x,
                                    const uint16_t dim_kernel_y,
                                    const uint16_t padding_x,
                                    const uint16_t padding_y,
                                    const uint16_t stride_x,
                                    const uint16_t stride_y,
                                    const q15_t * bias,
                                    const uint16_t bias_shift,
                                    const uint16_t out_shift,
                                    q15_t * Im_out,
                                    const uint16_t dim_im_out_x,
                                    const uint16_t dim_im_out_y, 
                                    q15_t * bufferA, 
                                    q7_t * bufferB)
{
    // rocky: seems ARM_MATH_DSP optimized code has bugs. and in higher optimization
    // levels (AC5, O3, Otime), non ARM_MATH_DSP code is slightly faster!!!
    int  i, j, k, l, m, n;
    int       conv_out;
    int32_t in_row, in_col;

    if (ch_im_in % 2 != 0 || ch_im_out % 2 != 0)
    {
        /* check if the input dimension meets the constraints */
        return ARM_MATH_SIZE_MISMATCH;
    }

    for (i = 0; i < ch_im_out; i++)
    {
        for (j = 0; j < dim_im_out_y; j++)
        {
            for (k = 0; k < dim_im_out_x; k++)
            {
                conv_out = ((q31_t)bias[i] << bias_shift) + NN_ROUND(out_shift);
                for (m = 0; m < dim_kernel_y; m++)
                {
                    for (n = 0; n < dim_kernel_x; n++)
                    {
                        in_row = stride_y * j + m - padding_y;
                        in_col = stride_x * k + n - padding_x;
                        if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
                        {
                            for (l = 0; l < ch_im_in; l++)
                            {
                                conv_out +=
                                    Im_in[(in_row * dim_im_in_x + in_col) * ch_im_in +
                                          l] * wt[i * ch_im_in * dim_kernel_x * dim_kernel_y + (m * dim_kernel_x +
                                                                                            n) * ch_im_in + l];
                            }
                        }
                    }
                }
                Im_out[i + (j * dim_im_out_x + k) * ch_im_out] = (q15_t) __SSAT((conv_out >> out_shift), 16);
            }
        }
    }
    return ARM_MATH_SUCCESS;
}


/**
 * @} end of NNConv group
 */

#if defined(__CC_ARM) && defined(ARM_MATH_DSP)
__asm void* InLoopAligned(const void *pRow0, const void *pRow1, const void *pCol, const void *pOut, int sum1, int sum2, int so, int c4Cnt) {
	PUSH	{R4-R11, LR}
	LDRD	R10, R11, [SP, #36]
	LDRD	R12, R14, [SP, #44]
10
	LDRD	R4, R5, [R0], #8
	LDRD	R6, R7, [R1], #8	
	LDRD	R8, R9, [R2], #8
	SMLAD	R10, R4, R8, R10
	SMLAD	R10, R5, R9, R10
	SMLAD	R11, R6, R8, R11
	SMLAD	R11, R7, R9, R11
	SUBS	R14, R14, #1
	BNE		%b10
	ASR		R10,	R10,	R12
	ASR		R11,	R11,	R12
	SSAT	R10,	#16,	R10
	SSAT	R11,	#16,	R11
	LSL		R11,	R11,	#16
	BFI		R11,	R10,	#0,	#16
	
	STR		R11,	[R3],	#4
	MOV		R0,		R3
	POP		{R4-R11, PC}
}

__asm void* InLoop(const void *pRow0, const void *pRow1, const void *pCol, const void *pOut, int sum1, int sum2, int so, int c4Cnt) {
	PUSH	{R4-R11, LR}
	LDRD	R10, R11, [SP, #36]
	LDRD	R12, R14, [SP, #44]
10
	LDMIA	R0!, {R4, R5}
	LDMIA	R1!, {R6, R7}
	LDMIA	R2!, {R8, R9}
	SMLAD	R10, R4, R8, R10
	SMLAD	R10, R5, R9, R10
	SMLAD	R11, R6, R8, R11
	SMLAD	R11, R7, R9, R11
	SUBS	R14, R14, #1
	BNE		%b10
	ASR		R10,	R10,	R12
	ASR		R11,	R11,	R12
	SSAT	R10,	#16,	R10
	SSAT	R11,	#16,	R11
	LSL		R11,	R11,	#16
	BFI		R11,	R10,	#0,	#16
	
	STR		R11,	[R3],	#4
	MOV		R0,		R3
	POP		{R4-R11, PC}
}
#endif
typedef void* (*pfnInLoop_t)(const void *pRow0, const void *pRow1, const void *pCol, const void *pOut, int sum1, int sum2, int so, int c4Cnt);

//arm_status aia_convolve_1x1_HWC_q15_fast_nonsquare(const q15_t* Im_in,
//	const int32_t dim_im_in_x,
//	const int32_t dim_im_in_y,
//	const int32_t ch_im_in,
//	const q15_t* wt,
//	const int32_t ch_im_out,
//	const int32_t dim_kernel_x,
//	const int32_t dim_kernel_y,
//	const int32_t padding_x,
//	const int32_t padding_y,
//	const int32_t stride_x,
//	const int32_t stride_y,
//	const q15_t* bias,
//	const int32_t bias_shift,
//	const int32_t out_shift,
//	q15_t* Im_out,
//	const int32_t dim_im_out_x,
//	const int32_t dim_im_out_y,
//	q15_t* bufferA,
//	q15_t* bufferB)
//{
//
//#if defined (ARM_MATH_DSP)
//	/* Run the following code for Cortex-M4 and Cortex-M7 */
//
//	int32_t   out_y, out_x;
//	int32_t   ch_out;
//
//	/* -----------------------
//	 *  Here we use bufferA as q15_t internally as computation are done with q15_t level
//	 *  im2col are done to output in q15_t format from q15_t input
//	 */
//
//	q15_t* pOut = Im_out;
//
//	if (ch_im_in % 4 != 0 || ch_im_out % 4 != 0 || dim_kernel_x != 1 || dim_kernel_y != 1
//		|| padding_x != 0 || padding_y != 0/*||  Crist used stride 2 in 1x1 conv! stride_x != 1 || stride_y != 1*/)
//	{
//		/* check if the input dimension meets the constraints */
//		return ARM_MATH_SIZE_MISMATCH;
//	}
//
//	#if 1
//	pfnInLoop_t pfn;
//	if (0 == ((uint32_t)wt & 3) && 0 == ((uint32_t)Im_in & 3))
//		pfn = InLoopAligned;
//	else
//		pfn = InLoop;
//	#else
//	q31_t     inW00, inW02, inW10, inW12;
//	q31_t     inD0, inD2;
//	#endif
//	q31_t sum1, sum2;
//	for (out_y = 0; out_y < dim_im_out_y; out_y++)
//	{
//		for (out_x = 0; out_x < dim_im_out_x; out_x++)
//		{
//			const q31_t *pWt32A = (const q31_t*) wt;
//			const q31_t *pWt32B = pWt32A + (ch_im_in >> 1);
//			const q31_t *pCol32, *pCol32Bkup;
//			pCol32Bkup = (const q31_t*)(Im_in + (out_y * dim_im_in_x * stride_y + out_x * stride_x) * ch_im_in);
//			for (ch_out = 0; ch_out < ch_im_out; ch_out+=2) {
//				pCol32 = pCol32Bkup;
//				int icRem = ch_im_in >> 2;
//				sum1 = ((q31_t)bias[ch_out] << bias_shift) + NN_ROUND(out_shift);
//				sum2 = ((q31_t)bias[ch_out+1] << bias_shift) + NN_ROUND(out_shift);
//				#if 1
//				pOut = pfn(pWt32A, pWt32B, pCol32, pOut, sum1, sum2, out_shift, icRem);
//				pWt32A += (ch_im_in);
//				pWt32B += (ch_im_in);
//				#else
//				while (icRem--) {
//					inW00 = *pWt32A++, inW02 = *pWt32A++;
//					inW10 = *pWt32B++, inW12 = *pWt32B++;
//					inD0 = *pCol32++, inD2 = *pCol32++;
//					sum1 = __SMLAD(inW00, inD0, sum1);
//					sum1 = __SMLAD(inW02, inD2, sum1);
//					sum2 = __SMLAD(inW10, inD0, sum2);
//					sum2 = __SMLAD(inW12, inD2, sum2);
//				}
//				*pOut++ = (q15_t) __SSAT((sum1 >> out_shift), 16);
//				*pOut++ = (q15_t) __SSAT((sum2 >> out_shift), 16);
//				pWt32A += (ch_im_in >> 1);
//				pWt32B += (ch_im_in >> 1);
//				#endif
//			}
//		}
//	}
//
//#else
//	/* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */
//
//	int       i, j, k, l, m, n;
//	int       conv_out;
//	int       in_row, in_col;
//
//	if (ch_im_in % 4 != 0 || ch_im_out % 2 != 0 || dim_kernel_x != 1 || dim_kernel_y != 1
//		|| padding_x != 0 || padding_y != 0 /*||  Crist used stride 2 in 1x1 conv! stride_x != 1 || stride_y != 1*/)
//	{
//		/* check if the input dimension meets the constraints */
//		return ARM_MATH_SIZE_MISMATCH;
//	}
//
//	for (i = 0; i < ch_im_out; i++)
//	{
//		for (j = 0; j < dim_im_out_y; j++)
//		{
//			for (k = 0; k < dim_im_out_x; k++)
//			{
//				conv_out = ((q31_t)(bias[i]) << bias_shift) + NN_ROUND(out_shift);
//				for (m = 0; m < dim_kernel_y; m++)
//				{
//					for (n = 0; n < dim_kernel_x; n++)
//					{
//						// if-for implementation
//						in_row = stride_y * j + m - padding_y;
//						in_col = stride_x * k + n - padding_x;
//						if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
//						{
//							for (l = 0; l < ch_im_in; l++)
//							{
//								conv_out += Im_in[(in_row * dim_im_in_x + in_col) * ch_im_in + l] *
//									wt[i * ch_im_in * dim_kernel_y * dim_kernel_x + (m * dim_kernel_y + n) * ch_im_in + l];
//							}
//						}
//					}
//				}
//				Im_out[i + (j * dim_im_out_x + k) * ch_im_out] = (q15_t)__SSAT((conv_out >> out_shift), 16);
//			}
//		}
//	}
//
//#endif                          /* ARM_MATH_DSP */
//
//	/* Return to application */
//	return ARM_MATH_SUCCESS;
//}

/**
 * @} end of NNConv group
 */

arm_status aia_depthwise_separable_conv_HWC_q15(const q15_t* Im_in,
	const int32_t dim_im_in,
	const int32_t ch_im_in,
	const q15_t* wt,
	const int32_t ch_im_out,
	const int32_t dim_kernel,
	const int32_t padding,
	const int32_t stride,
	const q15_t* bias,
	const int32_t bias_shift,
	const int32_t out_shift,
	q15_t* Im_out,
	const int32_t dim_im_out,
	q15_t* bufferA,
	q15_t* bufferB)
{

#if defined (ARM_MATH_DSP)
	/* Run the following code for Cortex-M4 and Cortex-M7 */

	int32_t   out_y, out_x;
	int32_t   ker_y, ker_x;
	q15_t* colBuffer = (q15_t*)bufferA;
	q15_t* pBuffer = colBuffer;
	const q15_t* pBias = bias;
	q15_t* pOut = Im_out;
	int32_t  rowCnt;
	int32_t  row_shift;

	/* do some checking here, basically ch_im_in == ch_im_out */
	if (ch_im_in != ch_im_out || (ch_im_in & 1))
	{
		return ARM_MATH_SIZE_MISMATCH;
	}

	for (out_y = 0; out_y < dim_im_out; out_y++)
	{
		for (out_x = 0; out_x < dim_im_out; out_x++)
		{
			/* we first do im2col here */
			for (ker_y = out_y * stride - padding; ker_y < out_y * stride - padding + dim_kernel; ker_y++)
			{
				for (ker_x = out_x * stride - padding; ker_x < out_x * stride - padding + dim_kernel; ker_x++)
				{
					if (ker_y < 0 || ker_y >= dim_im_in || ker_x < 0 || ker_x >= dim_im_in)
					{
						/* arm_fill_q7(0, pBuffer, ch_im_in); */
						memset(pBuffer, 0, ch_im_in * 2);
					}
					else
					{
						/* arm_copy_q7((q7_t *) Im_in + (ker_y * dim_im_in + ker_x) * ch_im_in, pBuffer, ch_im_in); */
						memcpy(pBuffer, Im_in + (ker_y * dim_im_in + ker_x) * ch_im_in, ch_im_in * 2);
					}
					pBuffer += ch_im_in;
				}
			}

			/* we will do the computation here for each channel */
			rowCnt = ch_im_out >> 1;
			row_shift = 0;
			pBias = bias;

			while (rowCnt)
			{
                q31_t     sum =  ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
                q31_t     sum2 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);

				int32_t  colCnt = (dim_kernel * dim_kernel) >> 1;
				q15_t* pB = colBuffer + row_shift;
				const q15_t* pA = wt + row_shift;
				row_shift += 2;

#ifdef USE_INTRINSIC

#ifndef ARM_MATH_BIG_ENDIAN

				while (colCnt)
				{
					q31_t opA, opB;
					q31_t inA0, inA1;
					q31_t inB0, inB1;
					// q31_t inA2, inA3;
					// q31_t inB2, inB3;

					inB0 = *__SIMD32(pB);
					pB += ch_im_in;
					inB1 = *__SIMD32(pB);
					pB += ch_im_in;
					inA0 = *__SIMD32(pA);
					pA += ch_im_in;
					inA1 = *__SIMD32(pA);
					pA += ch_im_in;	
					opA = __PKHBT(inA0, inA1, 16);
					opB = __PKHBT(inB0, inB1, 16);
					sum = __SMLAD(opA, opB, sum);
					opA = __PKHTB(inA0, inA1, 16);
					opB = __PKHTB(inB0, inB1, 16);
					sum2 = __SMLAD(opA, opB, sum2);					

//					inB2 = *__SIMD32(pB);
//					pB += ch_im_in;
//					inB3 = *__SIMD32(pB);
//					pB += ch_im_in;
//					inA2 = *__SIMD32(pA);
//					pA += ch_im_in;
//					inA3 = *__SIMD32(pA);
//					pA += ch_im_in;
//					
//					opA = __PKHBT(inA2, inA3, 16);
//					opB = __PKHBT(inB2, inB3, 16);
//					sum = __SMLAD(opA, opB, sum);

//					opA = __PKHTB(inA2, inA3, 16);
//					opB = __PKHTB(inB2, inB3, 16);
//					sum2 = __SMLAD(opA, opB, sum2);						
					
					colCnt--;
				}
#else

				while (colCnt)
				{
					q31_t     inA1, inA2, inB1, inB2, opA, opB;

					inB1 = *__SIMD32(pB);
					pB += ch_im_in;
					opB = *__SIMD32(pB);
					pB += ch_im_in;
					inB2 = __PKHBT(opB, inB1, 16);
					inB1 = __PKHTB(inB1, opB, 16);
					inA1 = *__SIMD32(pA);
					pA += ch_im_in;
					opB = *__SIMD32(pA);
					pA += ch_im_in;
					inA2 = __PKHBT(opB, inA1, 16);
					inA1 = __PKHTB(inA1, opB, 16);
					opA = __SXTB16(inA1);
					opB = __SXTB16(inB1);
					sum2 = __SMLAD(opA, opB, sum2);
					opA = __SXTB16(__ROR(inA1, 8));
					opB = __SXTB16(__ROR(inB1, 8));
					sum = __SMLAD(opA, opB, sum);
					opA = __SXTB16(inA2);
					opB = __SXTB16(inB2);
					sum4 = __SMLAD(opA, opB, sum4);
					opA = __SXTB16(__ROR(inA2, 8));
					opB = __SXTB16(__ROR(inB2, 8));
					sum3 = __SMLAD(opA, opB, sum3);
					colCnt--;
				}

#endif                          /* ARM_MATH_BIG_ENDIAN */

#else

#ifndef ARM_MATH_BIG_ENDIAN
				/*
				 *   r0    r1    r2    r3    r4   r5
				 *  inA1, inA2, inB1, inB2, opA, opB
				 */

				asm volatile ("COL_LOOP_%=:\n"
					"ldr.w r2, [%[pB], #0]\n"
					"add.w %[pB], %[pB], %[ch_im_in]\n"
					"ldr.w r5, [%[pB], #0]\n"
					"add.w %[pB], %[pB], %[ch_im_in]\n"
					"pkhtb r3, r5, r2, ASR #16\n"
					"pkhbt r2, r2, r5, LSL #16\n"
					"ldr.w r0, [%[pA], #0]\n"
					"add.w %[pA], %[pA], %[ch_im_in]\n"
					"ldr.w r5, [%[pA], #0]\n"
					"add.w %[pA], %[pA], %[ch_im_in]\n"
					"pkhtb r1, r5, r0, ASR #16\n"
					"pkhbt r0, r0, r5, LSL #16\n"
					"sxtb16 r4, r0\n"
					"sxtb16 r5, r2\n"
					"smlad %[sum], r4, r5, %[sum]\n"
					"mov.w r4, r0, ror #8\n"
					"mov.w r5, r2, ror #8\n"
					"sxtb16 r4, r4\n"
					"sxtb16 r5, r5\n"
					"smlad %[sum2], r4, r5, %[sum2]\n"
					"sxtb16 r4, r1\n"
					"sxtb16 r5, r3\n"
					"smlad %[sum3], r4, r5, %[sum3]\n"
					"mov.w r4, r1, ror #8\n"
					"mov.w r5, r3, ror #8\n"
					"sxtb16 r4, r4\n"
					"sxtb16 r5, r5\n"
					"smlad %[sum4], r4, r5, %[sum4]\n"
					"subs %[colCnt], #1\n"
					"bne COL_LOOP_%=\n": [sum]
					"+r"(sum), [sum2] "+r"(sum2),
					[sum3] "+r"(sum3),
					[sum4] "+r"(sum4), [pB] "+r"(pB),
					[pA] "+r"(pA) : [colCnt]
					"r"(colCnt), [ch_im_in] "r"(ch_im_in) : "r0", "r1", "r2", "r3", "r4", "r5");
#else
				/*
				 *  r0    r1    r2    r3    r4   r5
				 * inA1, inA2, inB1, inB2, opA, opB
				 */
				asm volatile ("COL_LOOP_%=:\n"
					"ldr.w r2, [%[pB], #0]\n"
					"add.w %[pB], %[pB], %[ch_im_in]\n"
					"ldr.w r5, [%[pB], #0]\n"
					"add.w %[pB], %[pB], %[ch_im_in]\n"
					"pkhbt r3, r5, r2, LSL #16\n"
					"pkhtb r2, r2, r5, ASR #16\n"
					"ldr.w r0, [%[pA], #0]\n"
					"add.w %[pA], %[pA], %[ch_im_in]\n"
					"ldr.w r5, [%[pA], #0]\n"
					"add.w %[pA], %[pA], %[ch_im_in]\n"
					"pkhbt r1, r5, r0, LSL #16\n"
					"pkhtb r0, r0, r5, ASR #16\n"
					"sxtb16 r4, r0\n"
					"sxtb16 r5, r2\n"
					"smlad %[sum2], r4, r5, %[sum2]\n"
					"mov.w r4, r0, ror #8\n"
					"mov.w r5, r2, ror #8\n"
					"sxtb16 r4, r4\n"
					"sxtb16 r5, r5\n"
					"smlad %[sum], r4, r5, %[sum]\n"
					"sxtb16 r4, r1\n"
					"sxtb16 r5, r3\n"
					"smlad %[sum4], r4, r5, %[sum4]\n"
					"mov.w r4, r1, ror #8\n"
					"mov.w r5, r3, ror #8\n"
					"sxtb16 r4, r4\n"
					"sxtb16 r5, r5\n"
					"smlad %[sum3], r4, r5, %[sum3]\n"
					"subs %[colCnt], #1\n"
					"bne COL_LOOP_%=\n": [sum]
					"+r"(sum), [sum2] "+r"(sum2),
					[sum3] "+r"(sum3),
					[sum4] "+r"(sum4), [pB] "+r"(pB),
					[pA] "+r"(pA) : [colCnt]
					"r"(colCnt), [ch_im_in] "r"(ch_im_in) : "r0", "r1", "r2", "r3", "r4", "r5");

#endif                          /* ARM_MATH_BIG_ENDIAN */

#endif                          /* USE_INTRINSIC */

				colCnt = (dim_kernel * dim_kernel) & 0x3;
				while (colCnt)
				{
					union arm_nnword inA, inB;
					inA.word = *__SIMD32(pA);
					pA += ch_im_in;
					inB.word = *__SIMD32(pB);
					pB += ch_im_in;
					sum += inA.half_words[0] * inB.half_words[0];
					sum2 += inA.half_words[1] * inB.half_words[1];
					colCnt--;
				}

				*pOut++ = (q15_t)__SSAT((sum >> out_shift), 16);
				*pOut++ = (q15_t)__SSAT((sum2 >> out_shift), 16);

				rowCnt--;
			}

			/* clear counter and pointers */
			pBuffer = colBuffer;
		}
	}

#else
	/* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */
	int       out_y, out_x, ch_out, ker_x, ker_y;
	int       conv_out;

	/* do some checking here, basically ch_im_in == ch_im_out */
	if (ch_im_in != ch_im_out)
	{
		return ARM_MATH_SIZE_MISMATCH;
	}
	int loc, wloc, x, w;
	for (out_y = 0; out_y < dim_im_out; out_y++)
	{
		for (out_x = 0; out_x < dim_im_out; out_x++)
		{
			for (ch_out = 0; ch_out < ch_im_out; ch_out++)
			{
				// for each output
				conv_out = ((q31_t)(bias[ch_out]) << bias_shift) + NN_ROUND(out_shift);
				int in_row = stride * out_y - padding;
				for (ker_y = 0; ker_y < dim_kernel; ker_y++)
				{
					int in_col = stride * out_x - padding;
					loc = (in_row * dim_im_in + in_col) * ch_im_in + ch_out;					
					for (ker_x = 0; ker_x < dim_kernel; ker_x++)
					{
						if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in && in_col < dim_im_in)
						{
							wloc = (ker_y * dim_kernel + ker_x) * ch_im_out + ch_out;
							x = Im_in[loc];
							w = wt[wloc];
							conv_out += w * x;
						}
						loc += ch_im_in;
						in_col++;
					}
					in_row++;
				}
				Im_out[(out_y * dim_im_out +
					out_x) * ch_im_out + ch_out] = (q15_t)__SSAT((conv_out >> out_shift), 16);
			}
		}
	}

#endif                          /* ARM_MATH_DSP */

	/* Return to application */
	return ARM_MATH_SUCCESS;

}


arm_status aia_convolve_HWC_q15_basic_nonsquare(
    const q15_t * Im_in,
    const int32_t dim_im_in_x,
    const int32_t dim_im_in_y,
    const int32_t ch_im_in,
    const q15_t * wt,
    const int32_t ch_im_out,
    const int32_t dim_kernel_x,
    const int32_t dim_kernel_y,
    const int32_t padding_x,
    const int32_t padding_y,
    const int32_t stride_x,
    const int32_t stride_y,
    const q15_t * bias,
    const int32_t bias_shift,
    const int32_t out_shift,
    q15_t * Im_out,
    const int32_t dim_im_out_x,
    const int32_t dim_im_out_y, 
    q15_t * bufferA, 
    q7_t * bufferB)
{
    /* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */
    uint16_t  i, j, k, l, m, n;
    int       conv_out;
    int32_t in_row, in_col;

    for (i = 0; i < ch_im_out; i++)
    {
        for (j = 0; j < dim_im_out_y; j++)
        {
            for (k = 0; k < dim_im_out_x; k++)
            {
                conv_out = ((q31_t)bias[i] << bias_shift) + NN_ROUND(out_shift);
                for (m = 0; m < dim_kernel_y; m++)
                {
                    for (n = 0; n < dim_kernel_x; n++)
                    {
                        in_row = stride_y * j + m - padding_y;
                        in_col = stride_x * k + n - padding_x;
                        if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
                        {
                            for (l = 0; l < ch_im_in; l++)
                            {
                                conv_out +=
                                    Im_in[(in_row * dim_im_in_x + in_col) * ch_im_in +
                                          l] * wt[i * ch_im_in * dim_kernel_x * dim_kernel_y + (m * dim_kernel_x +
                                                                                            n) * ch_im_in + l];
                            }
                        }
                    }
                }
                Im_out[i + (j * dim_im_out_x + k) * ch_im_out] = (q15_t) __SSAT((conv_out >> out_shift), 16);
            }
        }
    }
    /* Return to application */
    return ARM_MATH_SUCCESS;
}

arm_status
aia_convolve_HWC_q15_RGB(const q15_t* Im_in,
	const int32_t dim_im_in,
	const int32_t ch_im_in,
	const q15_t* wt,
	const int32_t ch_im_out,
	const int32_t dim_kernel,
	const int32_t padding,
	const int32_t stride,
	const q15_t* bias,
	const int32_t bias_shift,
	const int32_t out_shift,
	q15_t* Im_out, const int32_t dim_im_out, q15_t* bufferA, q15_t* bufferB)
{
	return arm_convolve_HWC_q15_basic(Im_in, dim_im_in, ch_im_in, wt, ch_im_out, dim_kernel, padding, stride, bias, bias_shift, out_shift, 
		Im_out, dim_im_out, bufferA, (q7_t*)bufferB);
}
#ifdef ARM_MATH_DSP
static void compare_and_replace_if_larger_q15(q15_t * base,   // base data
                                             q15_t * target, // compare target
                                             const int32_t length  // data length
    )
{
    q15_t     *pIn = base;
    q15_t     *pCom = target;
    union arm_nnword in;
    union arm_nnword com;
    int32_t  cnt = length >> 1;

    while (cnt > 0u)
    {
        in.word = *__SIMD32(pIn);
        com.word = *__SIMD32(pCom)++;

        // if version
        if (com.half_words[0] > in.half_words[0])
            in.half_words[0] = com.half_words[0];
        if (com.half_words[1] > in.half_words[1])
            in.half_words[1] = com.half_words[1];
        *__SIMD32(pIn)++ = in.word;
        cnt--;
    }
}
#endif
void aia_maxpool_q15_HWC(q15_t* Im_in,
	const int32_t dim_im_in,
	const int32_t ch_im_in,
	const int32_t dim_kernel,
	const int32_t padding,
	const int32_t stride, const int32_t dim_im_out, q15_t* bufferA, q15_t* Im_out)
{

#if defined (ARM_MATH_DSP)
	/* Run the following code for Cortex-M4 and Cortex-M7 */

	int   x, y;

	/* first does the pooling along x axis */
	for (y = 0; y < dim_im_in; y++)
	{

		for (x = 0; x < dim_im_out; x++)
		{
			/* for each output pixel */
			q15_t* target = Im_in + (y * dim_im_in + x) * ch_im_in;
			q15_t* win_start;
			q15_t* win_stop;
			if (x * stride - padding < 0)
			{
				win_start = target;
			}
			else
			{
				win_start = Im_in + (y * dim_im_in + x * stride - padding) * ch_im_in;
			}

			if (x * stride - padding + dim_kernel >= dim_im_in)
			{
				win_stop = Im_in + (y * dim_im_in + dim_im_in) * ch_im_in;
			}
			else
			{
				win_stop = Im_in + (y * dim_im_in + x * stride - padding + dim_kernel) * ch_im_in;
			}

			/* first step is to copy over initial data */
			/* arm_copy_q7(win_start, target, ch_im_in); */
			memmove(target, win_start, ch_im_in * 2);

			/* start the max operation from the second part */
			win_start += ch_im_in;
			for (; win_start < win_stop; win_start += ch_im_in)
			{
				compare_and_replace_if_larger_q15(target, win_start, ch_im_in);
			}
		}
	}

	/* then does the pooling along y axis */
	for (y = 0; y < dim_im_out; y++)
	{

		/* for each output row */
		q15_t* target = Im_out + y * dim_im_out * ch_im_in;
		q15_t* row_start;
		q15_t* row_end;
		/* setting the starting row */
		if (y * stride - padding < 0)
		{
			row_start = Im_in;
		}
		else
		{
			row_start = Im_in + (y * stride - padding) * dim_im_in * ch_im_in;
		}
		/* setting the stopping row */
		if (y * stride - padding + dim_kernel >= dim_im_in)
		{
			row_end = Im_in + dim_im_in * dim_im_in * ch_im_in;
		}
		else
		{
			row_end = Im_in + (y * stride - padding + dim_kernel) * dim_im_in * ch_im_in;
		}

		/* copy over the first row */
		/* arm_copy_q7(row_start, target, dim_im_out * ch_im_in); */
		memmove(target, row_start, dim_im_out * ch_im_in * 2);

		/* move over to next row */
		row_start += ch_im_in * dim_im_in;

		for (; row_start < row_end; row_start += dim_im_in * ch_im_in)
		{
			compare_and_replace_if_larger_q15(target, row_start, dim_im_out * ch_im_in);
		}
	}

#else
	/* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */

	int   ch_in, x, y;
	int   k_x, k_y;

	for (ch_in = 0; ch_in < ch_im_in; ch_in++)
	{
		for (y = 0; y < dim_im_out; y++)
		{
			for (x = 0; x < dim_im_out; x++)
			{
				int       max = -32769;
				for (k_y = y * stride - padding; k_y < y * stride - padding + dim_kernel; k_y++)
				{
					for (k_x = x * stride - padding; k_x < x * stride - padding + dim_kernel; k_x++)
					{
						if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in && k_x < dim_im_in)
						{
							if (Im_in[ch_in + ch_im_in * (k_x + k_y * dim_im_in)] > max)
							{
								max = Im_in[ch_in + ch_im_in * (k_x + k_y * dim_im_in)];
							}
						}
					}
				}
				Im_out[ch_in + ch_im_in * (x + y * dim_im_out)] = max;
			}
		}
	}

#endif                          /* ARM_MATH_DSP */

}

void accumulate_q15(q15_t * base, q15_t * target, const int32_t length)
{
    q15_t    *pCnt = base;
    q15_t     *pV = target;
    int32_t  cnt = length >> 2;
    q31_t     in;

    while (cnt > 0u)
    {
        q31_t     value1 = *__SIMD32(pV)++;
		q31_t value2 = *__SIMD32(pV)++;

        in = *__SIMD32(pCnt);
        *__SIMD32(pCnt)++ = __QADD16(value1, in);

        in = *__SIMD32(pCnt);
        *__SIMD32(pCnt)++ = __QADD16(value2, in);

        cnt--;
    }
    cnt = length & 0x3;
    while (cnt > 0u)
    {
        *pCnt++ += *pV++;
        cnt--;
    }
}

/**
 * @brief Q7 average pooling function
 * @param[in,out]   Im_in       pointer to input tensor
 * @param[in]       dim_im_in   input tensor dimention
 * @param[in]       ch_im_in    number of input tensor channels
 * @param[in]       dim_kernel  filter kernel size
 * @param[in]       padding     padding sizes
 * @param[in]       stride      convolution stride
 * @param[in]       dim_im_out  output tensor dimension
 * @param[in,out]   bufferA     pointer to buffer space for input
 * @param[in,out]   Im_out      pointer to output tensor
 * @return none.
 *
 * @details
 *
 * <b>Buffer size:</b>
 *
 * bufferA size:  2*dim_im_out*ch_im_in
 *
 * The pooling function is implemented as split x-pooling then
 * y-pooling.
 *
 * This pooling function is input-destructive. Input data is undefined
 * after calling this function.
 *
 */

void
aia_avepool_q15_HWC(q15_t* Im_in,
	const int32_t dim_im_in,
	const int32_t ch_im_in,
	const int32_t dim_kernel,
	const int32_t padding,
	const int32_t stride, const int32_t dim_im_out, q15_t* bufferA, q15_t* Im_out)
{

#if 0// defined (ARM_MATH_DSP)
	/* Run the following code for Cortex-M4 and Cortex-M7 */

	q15_t* buffer = (q15_t*)bufferA;
	int16_t   x, y;
	int16_t   count = 0;

	/* first does the pooling along x axis */
	for (y = 0; y < dim_im_in; y++)
	{

		for (x = 0; x < dim_im_out; x++)
		{
			/* for each output pixel */
			q15_t* target = Im_in + (y * dim_im_in + x) * ch_im_in;
			q15_t* win_start;
			q15_t* win_stop;
			if (x * stride - padding < 0)
			{
				win_start = target;
			}
			else
			{
				win_start = Im_in + (y * dim_im_in + x * stride - padding) * ch_im_in;
			}

			if (x * stride - padding + dim_kernel >= dim_im_in)
			{
				win_stop = Im_in + (y * dim_im_in + dim_im_in) * ch_im_in;
			}
			else
			{
				win_stop = Im_in + (y * dim_im_in + x * stride - padding + dim_kernel) * ch_im_in;
			}

			/* first step is to copy over initial data */
			arm_q7_to_q15_no_shift(win_start, buffer, ch_im_in);
			count = 1;

			/* start the max operation from the second part */
			win_start += ch_im_in;
			for (; win_start < win_stop; win_start += ch_im_in)
			{
				accumulate_q15(buffer, win_start, ch_im_in);
				count++;
			}
		}
	}

	/* then does the pooling along y axis */
	for (y = 0; y < dim_im_out; y++)
	{
		/* for each output row */
		q7_t* target = Im_out + y * dim_im_out * ch_im_in;
		q7_t* row_start;
		q7_t* row_end;
		/* setting the starting row */
		if (y * stride - padding < 0)
		{
			row_start = Im_in;
		}
		else
		{
			row_start = Im_in + (y * stride - padding) * dim_im_in * ch_im_in;
		}
		/* setting the stopping row */
		if (y * stride - padding + dim_kernel >= dim_im_in)
		{
			row_end = Im_in + dim_im_in * dim_im_in * ch_im_in;
		}
		else
		{
			row_end = Im_in + (y * stride - padding + dim_kernel) * dim_im_in * ch_im_in;
		}

		/* copy over the first row */
		arm_q7_to_q15_no_shift(row_start, buffer, dim_im_out * ch_im_in);
		count = 1;

		/* move over to next row */
		row_start += ch_im_in * dim_im_in;

		for (; row_start < row_end; row_start += dim_im_in * ch_im_in)
		{
			accumulate_q7_to_q15(buffer, row_start, dim_im_out * ch_im_in);
			count++;
		}
		buffer_scale_back_q15_to_q7(buffer, target, dim_im_out * ch_im_in, count);
	}

#else
	/* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */

	int   ch_in, x, y;
	int   k_x, k_y;

	for (ch_in = 0; ch_in < ch_im_in; ch_in++)
	{
		for (y = 0; y < dim_im_out; y++)
		{
			for (x = 0; x < dim_im_out; x++)
			{
				int       sum = 0;
				int       count = 0;
				for (k_y = y * stride - padding; k_y < y * stride - padding + dim_kernel; k_y++)
				{
					for (k_x = x * stride - padding; k_x < x * stride - padding + dim_kernel; k_x++)
					{
						if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in && k_x < dim_im_in)
						{
							sum += Im_in[ch_in + ch_im_in * (k_x + k_y * dim_im_in)];
							count++;
						}
					}
				}
				Im_out[ch_in + ch_im_in * (x + y * dim_im_out)] = sum / count;
			}
		}
	}

#endif                          /* ARM_MATH_DSP */

}

arm_status
aia_convolve_HWC_q15_fast(const q15_t * Im_in,
                          const int32_t dim_im_in,
                          const int32_t ch_im_in,
                          const q15_t * wt,
                          const int32_t ch_im_out,
                          const int32_t dim_kernel,
                          const int32_t padding,
                          const int32_t stride,
                          const q15_t * bias,
                          const int32_t bias_shift,
                          const int32_t out_shift,
                          q15_t * Im_out, 
                          const int32_t dim_im_out, 
                          q15_t * bufferA, 
                          q7_t * bufferB)
{

#if defined (ARM_MATH_DSP)
    int   i_out_y, i_out_x, i_ker_y, i_ker_x;

    q15_t    *pBuffer = bufferA;
    q15_t    *pOut = Im_out;

    if (ch_im_in % 2 != 0 || ch_im_out % 2 != 0)
    {
        /* check if the input dimension meets the constraints */
        return ARM_MATH_SIZE_MISMATCH;
    }

    /* Run the following code for Cortex-M4 and Cortex-M7 */

    /* This part implements the im2col function */
    for (i_out_y = 0; i_out_y < dim_im_out; i_out_y++)
    {
        for (i_out_x = 0; i_out_x < dim_im_out; i_out_x++)
        {
            for (i_ker_y = i_out_y * stride - padding; i_ker_y < i_out_y * stride - padding + dim_kernel; i_ker_y++)
            {
                for (i_ker_x = i_out_x * stride - padding; i_ker_x < i_out_x * stride - padding + dim_kernel; i_ker_x++)
                {
                    if (i_ker_y < 0 || i_ker_y >= dim_im_in || i_ker_x < 0 || i_ker_x >= dim_im_in)
                    {
                        /* arm_fill_q15(0, pBuffer, ch_im_in); */
                        memset(pBuffer, 0, sizeof(q15_t)*ch_im_in);
                    } else
                    {
                        /* arm_copy_q15((q15_t *) Im_in + (i_ker_y * dim_im_in + i_ker_x) * ch_im_in, pBuffer, ch_im_in); */
                        memcpy(pBuffer, (q15_t *) Im_in + (i_ker_y * dim_im_in + i_ker_x) * ch_im_in, sizeof(q15_t)*ch_im_in);
                    }
                    pBuffer += ch_im_in;
                }
            }

            // if (i_out_x & 0x1)
            {
                int       i;
                /* initialize the matrix pointers for A */
                const q15_t *pA = wt;

                /* set up the second output pointers */
                // q15_t    *pOut2 = pOut + ch_im_out;

                /* this loop over rows in A */
                for (i = 0; i < ch_im_out; i += 2)
                {
                    /* setup pointers for B */
                    q15_t    *pB = bufferA;
                    // const q15_t *pB2 = pB + ch_im_in * dim_kernel * dim_kernel;

                    /* aling the second pointer for A */
                    const q15_t *pA2 = pA + ch_im_in * dim_kernel * dim_kernel;

                    /* init the sum with bias */
                    q31_t     sum =  ((q31_t)bias[i] << bias_shift) + NN_ROUND(out_shift);
                    // q31_t     sum2 = ((q31_t)bias[i] << bias_shift) + NN_ROUND(out_shift);
                    q31_t     sum3 = ((q31_t)bias[i + 1] << bias_shift) + NN_ROUND(out_shift);
                    // q31_t     sum4 = ((q31_t)bias[i + 1] << bias_shift) + NN_ROUND(out_shift);

                    int  colCnt = ch_im_in * dim_kernel * dim_kernel >> 1;
                    /* accumulate over the vector */
                    while (colCnt)
                    {
                        q31_t     inA1 = *__SIMD32(pA)++;
                        q31_t     inB1 = *__SIMD32(pB)++;
                        q31_t     inA2 = *__SIMD32(pA2)++;
                        // q31_t     inB2 = *__SIMD32(pB2)++;

                        sum = __SMLAD(inA1, inB1, sum);
                        // sum2 = __SMLAD(inA1, inB2, sum2);
                        sum3 = __SMLAD(inA2, inB1, sum3);
                        // sum4 = __SMLAD(inA2, inB2, sum4);

                        colCnt--;
                    }           /* while over colCnt */
                    colCnt = ch_im_in * dim_kernel * dim_kernel & 0x1;
                    while (colCnt)
                    {
                        q15_t     inA1 = *pA++;
                        q15_t     inB1 = *pB++;
                        q15_t     inA2 = *pA2++;
                        // q15_t     inB2 = *pB2++;

                        sum += inA1 * inB1;
                        // sum2 += inA1 * inB2;
                        sum3 += inA2 * inB1;
                        // sum4 += inA2 * inB2;
                        colCnt--;
                    }           /* while over colCnt */
                    *pOut++ = (q15_t) __SSAT(sum >> out_shift, 16);
                    *pOut++ = (q15_t) __SSAT(sum3 >> out_shift, 16);
                    // *pOut2++ = (q15_t) __SSAT(sum2 >> out_shift, 16);
                    // *pOut2++ = (q15_t) __SSAT(sum4 >> out_shift, 16);

                    /* skip the row computed with A2 */
                    pA += ch_im_in * dim_kernel * dim_kernel;
                }               /* for over ch_im_out */

                // pOut += ch_im_out;
                /* counter reset */
                pBuffer = bufferA;
            }
        }
    }

#else
    /* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */
    int32_t  i, j, k, l, m, n;
    int       conv_out;
    int32_t in_row, in_col;

    if (ch_im_in % 2 != 0 || ch_im_out % 2 != 0)
    {
        /* check if the input dimension meets the constraints */
        return ARM_MATH_SIZE_MISMATCH;
    }

    for (i = 0; i < ch_im_out; i++)
    {
        for (j = 0; j < dim_im_out; j++)
        {
            for (k = 0; k < dim_im_out; k++)
            {
                conv_out = ((q31_t)bias[i] << bias_shift) + NN_ROUND(out_shift);
                for (m = 0; m < dim_kernel; m++)
                {
                    for (n = 0; n < dim_kernel; n++)
                    {
                        in_row = stride * j + m - padding;
                        in_col = stride * k + n - padding;
                        if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in && in_col < dim_im_in)
                        {
                            for (l = 0; l < ch_im_in; l++)
                            {
                                conv_out +=
                                    Im_in[(in_row * dim_im_in + in_col) * ch_im_in +
                                          l] * wt[i * ch_im_in * dim_kernel * dim_kernel + (m * dim_kernel +
                                                                                            n) * ch_im_in + l];
                            }
                        }
                    }
                }
                Im_out[i + (j * dim_im_out + k) * ch_im_out] = (q15_t) __SSAT((conv_out >> out_shift), 16);
            }
        }
    }

#endif                          /* ARM_MATH_DSP */

    /* Return to application */
    return ARM_MATH_SUCCESS;
}

arm_status
aia_convolve_HWC_q7_fast(const q7_t* Im_in,
	const int32_t dim_im_in,
	const int32_t ch_im_in,
	const q7_t* wt,
	const int32_t ch_im_out,
	const int32_t dim_kernel,
	const int32_t padding,
	const int32_t stride,
	const q7_t* bias,
	const int32_t bias_shift,
	const int32_t out_shift,
	q7_t* Im_out,
	const int32_t dim_im_out,
	q15_t* bufferA,
	q7_t* bufferB)
{

#if defined (ARM_MATH_DSP)
	/* Run the following code for Cortex-M4 and Cortex-M7 */

	int16_t   i_out_y, i_out_x, i_ker_y, i_ker_x;

	/*
	 *  Here we use bufferA as q15_t internally as computation are done with q15_t level
	 *  im2col are done to output in q15_t format from q7_t input
	 */

	q15_t* pBuffer = bufferA;
	q7_t* pOut = Im_out;

	if (ch_im_in % 4 != 0 || ch_im_out % 2 != 0)
	{
		/* check if the input dimension meets the constraints */
		return ARM_MATH_SIZE_MISMATCH;
	}

	/*
	 *  Here we split the entire matrix into three regions depending on the padding situation
	 *    Top: i_out_y from 0 to padding - 1
	 * Middle: i_out_y from padding to dim_im_out-padding-1
	 * Bottom: i_out_y from dim_im_out-padding to dim_im_out-1
	 */

	 /* top part */
	for (i_out_y = 0; i_out_y < padding; i_out_y++)
	{
		for (i_out_x = 0; i_out_x < dim_im_out; i_out_x++)
		{
			/* This part implements the im2col function */
			for (i_ker_y = i_out_y * stride - padding; i_ker_y < i_out_y * stride - padding + dim_kernel; i_ker_y++)
			{
				for (i_ker_x = i_out_x * stride - padding; i_ker_x < i_out_x * stride - padding + dim_kernel; i_ker_x++)
				{
					if (i_ker_y < 0 || i_ker_y >= dim_im_in || i_ker_x < 0 || i_ker_x >= dim_im_in)
					{
						/* arm_fill_q15(0, pBuffer, ch_im_in); */
						memset(pBuffer, 0, sizeof(q15_t) * ch_im_in);
					}
					else
					{
						arm_q7_to_q15_reordered_no_shift
						((q7_t*)Im_in + (i_ker_y * dim_im_in + i_ker_x) * ch_im_in, pBuffer, ch_im_in);
					}
					pBuffer += ch_im_in;
				}
			}

			if (pBuffer == bufferA + 2 * ch_im_in * dim_kernel * dim_kernel)
			{
				pOut =
					arm_nn_mat_mult_kernel_q7_q15_reordered(wt,
						bufferA,
						ch_im_out,
						ch_im_in
						*
						dim_kernel * dim_kernel, bias_shift, out_shift, bias, pOut);
				/* counter reset */
				pBuffer = bufferA;
			}
		}
	}

	/* middle part, here we also divide the x into left, mid and right */
	for (; i_out_y < dim_im_out - padding; i_out_y++)
	{

		/* left part */
		for (i_out_x = 0; i_out_x < padding; i_out_x++)
		{
			/* This part implements the im2col function */
			for (i_ker_y = i_out_y * stride - padding; i_ker_y < i_out_y * stride - padding + dim_kernel; i_ker_y++)
			{
				for (i_ker_x = i_out_x * stride - padding; i_ker_x < i_out_x * stride - padding + dim_kernel; i_ker_x++)
				{
					if (i_ker_x < 0 || i_ker_x >= dim_im_in)
					{
						/* arm_fill_q15(0, pBuffer, ch_im_in); */
						memset(pBuffer, 0, sizeof(q15_t) * ch_im_in);
					}
					else
					{
						arm_q7_to_q15_reordered_no_shift
						((q7_t*)Im_in + (i_ker_y * dim_im_in + i_ker_x) * ch_im_in, pBuffer, ch_im_in);
					}
					pBuffer += ch_im_in;
				}
			}

			if (pBuffer == bufferA + 2 * ch_im_in * dim_kernel * dim_kernel)
			{
				pOut =
					arm_nn_mat_mult_kernel_q7_q15_reordered(wt,
						bufferA,
						ch_im_out,
						ch_im_in
						*
						dim_kernel * dim_kernel, bias_shift, out_shift, bias, pOut);
				/* counter reset */
				pBuffer = bufferA;
			}
		}

		/* mid part */
		for (; i_out_x < dim_im_out - padding; i_out_x++)
		{
			/* This part implements the im2col function */
			for (i_ker_y = i_out_y * stride - padding; i_ker_y < i_out_y * stride - padding + dim_kernel; i_ker_y++)
			{
				if (i_ker_y < 0 || i_ker_y >= dim_im_in) {
					memset(pBuffer, 0, sizeof(q15_t) * ch_im_in * dim_kernel);
					pBuffer += ch_im_in * dim_kernel;
					continue;
				}
				i_ker_x = i_out_x * stride - padding;
				if (i_ker_x < 0 || i_ker_x + dim_kernel >= dim_im_in)
				{
					for (; i_ker_x < i_out_x * stride - padding + dim_kernel; i_ker_x++)
					{
						if (i_ker_x < 0 || i_ker_x >= dim_im_in)
						{
							/* Filling 0 for out-of-bound paddings */
							/* arm_fill_q15(0, pBuffer, ch_im_in); */
							memset(pBuffer, 0, sizeof(q15_t) * ch_im_in);
						}
						else
						{
							/* Copying the pixel data to column */
							arm_q7_to_q15_reordered_no_shift((q7_t*)
								Im_in + (i_ker_y * dim_im_in + i_ker_x) * ch_im_in, pBuffer, ch_im_in);
						}
						pBuffer += ch_im_in;
					}
				}
				else {
					arm_q7_to_q15_reordered_no_shift((q7_t*)Im_in
						+
						(i_ker_y *
							dim_im_in +
							i_out_x *
							stride - padding) * ch_im_in, pBuffer, ch_im_in * dim_kernel);
					pBuffer += ch_im_in * dim_kernel;
				}
			}

			if (pBuffer == bufferA + 2 * ch_im_in * dim_kernel * dim_kernel)
			{
				pOut =
					arm_nn_mat_mult_kernel_q7_q15_reordered(wt,
						bufferA,
						ch_im_out,
						ch_im_in
						*
						dim_kernel * dim_kernel, bias_shift, out_shift, bias, pOut);
				/* counter reset */
				pBuffer = bufferA;
			}
		}

		/* right part */
		for (; i_out_x < dim_im_out; i_out_x++)
		{
			/* This part implements the im2col function */
			for (i_ker_y = i_out_y * stride - padding; i_ker_y < i_out_y * stride - padding + dim_kernel; i_ker_y++)
			{
				for (i_ker_x = i_out_x * stride - padding; i_ker_x < i_out_x * stride - padding + dim_kernel; i_ker_x++)
				{
					if (i_ker_x < 0 || i_ker_x >= dim_im_in)
					{
						/* arm_fill_q15(0, pBuffer, ch_im_in); */
						memset(pBuffer, 0, sizeof(q15_t) * ch_im_in);
					}
					else
					{
						arm_q7_to_q15_reordered_no_shift
						((q7_t*)Im_in + (i_ker_y * dim_im_in + i_ker_x) * ch_im_in, pBuffer, ch_im_in);
					}
					pBuffer += ch_im_in;
				}
			}

			if (pBuffer == bufferA + 2 * ch_im_in * dim_kernel * dim_kernel)
			{
				pOut =
					arm_nn_mat_mult_kernel_q7_q15_reordered(wt,
						bufferA,
						ch_im_out,
						ch_im_in
						*
						dim_kernel * dim_kernel, bias_shift, out_shift, bias, pOut);
				/* counter reset */
				pBuffer = bufferA;
			}
		}
	}

	for (; i_out_y < dim_im_out; i_out_y++)
	{
		for (i_out_x = 0; i_out_x < dim_im_out; i_out_x++)
		{
			/* This part implements the im2col function */
			for (i_ker_y = i_out_y * stride - padding; i_ker_y < i_out_y * stride - padding + dim_kernel; i_ker_y++)
			{
				for (i_ker_x = i_out_x * stride - padding; i_ker_x < i_out_x * stride - padding + dim_kernel; i_ker_x++)
				{
					if (i_ker_y < 0 || i_ker_y >= dim_im_in || i_ker_x < 0 || i_ker_x >= dim_im_in)
					{
						/* arm_fill_q15(0, pBuffer, ch_im_in); */
						memset(pBuffer, 0, sizeof(q15_t) * ch_im_in);
					}
					else
					{
						arm_q7_to_q15_reordered_no_shift
						((q7_t*)Im_in + (i_ker_y * dim_im_in + i_ker_x) * ch_im_in, pBuffer, ch_im_in);
					}
					pBuffer += ch_im_in;
				}
			}

			if (pBuffer == bufferA + 2 * ch_im_in * dim_kernel * dim_kernel)
			{
				pOut =
					arm_nn_mat_mult_kernel_q7_q15_reordered(wt,
						bufferA,
						ch_im_out,
						ch_im_in
						*
						dim_kernel * dim_kernel, bias_shift, out_shift, bias, pOut);
				/* counter reset */
				pBuffer = bufferA;
			}
		}
	}

	/* check if there is left-over for compute */
	if (pBuffer != bufferA)
	{
		const q7_t* pA = wt;
		int       i;

		for (i = 0; i < ch_im_out; i++)
		{
			q31_t     sum = ((q31_t)bias[i] << bias_shift) + NN_ROUND(out_shift);
			q15_t* pB = bufferA;
			/* each time it process 4 entries */
			uint16_t  colCnt = ch_im_in * dim_kernel * dim_kernel >> 2;

			while (colCnt)
			{

				q31_t     inA1, inA2;
				q31_t     inB1, inB2;

				pA = (q7_t*)read_and_pad_reordered((void*)pA, &inA1, &inA2);

				inB1 = *__SIMD32(pB)++;
				sum = __SMLAD(inA1, inB1, sum);
				inB2 = *__SIMD32(pB)++;
				sum = __SMLAD(inA2, inB2, sum);

				colCnt--;
			}
			colCnt = ch_im_in * dim_kernel * dim_kernel & 0x3;
			while (colCnt)
			{
				q7_t      inA1 = *pA++;
				q15_t     inB1 = *pB++;
				sum += inA1 * inB1;
				colCnt--;
			}
			*pOut = (q7_t)__SSAT((sum >> out_shift), 8);
			pOut++;

		}

	}
#else
	/* Run the following code as reference implementation for Cortex-M0 and Cortex-M3 */

	uint16_t  i, j, k, l, m, n;
	int       conv_out;
	int32_t in_row, in_col;

	if (ch_im_in % 4 != 0 || ch_im_out % 2 != 0)
	{
		/* check if the input dimension meets the constraints */
		return ARM_MATH_SIZE_MISMATCH;
	}

	for (i = 0; i < ch_im_out; i++)
	{
		for (j = 0; j < dim_im_out; j++)
		{
			for (k = 0; k < dim_im_out; k++)
			{
				conv_out = (bias[i] << bias_shift) + NN_ROUND(out_shift);
				for (m = 0; m < dim_kernel; m++)
				{
					for (n = 0; n < dim_kernel; n++)
					{
						// if-for implementation
						in_row = stride * j + m - padding;
						in_col = stride * k + n - padding;
						if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in && in_col < dim_im_in)
						{
							for (l = 0; l < ch_im_in; l++)
							{
								conv_out +=
									Im_in[(in_row * dim_im_in + in_col) * ch_im_in +
									l] * wt[i * ch_im_in * dim_kernel * dim_kernel + (m * dim_kernel +
										n) * ch_im_in + l];
							}
						}
					}
				}
				Im_out[i + (j * dim_im_out + k) * ch_im_out] = (q7_t)__SSAT((conv_out >> out_shift), 8);
			}
		}
	}

#endif                          /* ARM_MATH_DSP */

	/* Return to application */
	return ARM_MATH_SUCCESS;
}

void aia_maxpool_q7_HWC_nonsquare(
	const q7_t* Im_in,           // input image
	const int32_t dim_im_in_x,   // input image dimension
	const int32_t dim_im_in_y,   // input image dimension
	const int32_t ch_im_in,      // number of input image channels
	const int32_t dim_kernel_x,  // window kernel size
	const int32_t dim_kernel_y,  // window kernel size
	const int32_t padding_x,     // padding sizes
	const int32_t padding_y,     // padding sizes
	const int32_t stride_x,      // stride
	const int32_t stride_y,      // stride
	const int32_t dim_im_out_x,  // output image dimension
	const int32_t dim_im_out_y,  // output image dimension
	q7_t* bufferA,               // a buffer for local storage
	q7_t* Im_out,                // output feature
	const int32_t out_lshift)    // output left shift (scaling)
{
	int32_t ch_in, x, y;
	int32_t k_x, k_y;

	for (ch_in = 0; ch_in < ch_im_in; ch_in++) {
		for (y = 0; y < dim_im_out_y; y++) {
			for (x = 0; x < dim_im_out_x; x++) {
				int maxVal = -128;
                int v;
				for (k_y = y * stride_y - padding_y; k_y < y * stride_y - padding_y + dim_kernel_y; k_y++) {
					for (k_x = x * stride_x - padding_x; k_x < x * stride_x - padding_x + dim_kernel_x; k_x++) {
						if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x) 
                        {
                            v = Im_in[ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)];
							maxVal = maxVal < v ? v : maxVal;
						}
					}
				}
				Im_out[ch_in + ch_im_in * (x + y * dim_im_out_x)] = (q7_t) maxVal;
			}
		}
	}
}

void aia_maxpool_q15_HWC_nonsquare(
	const q15_t* Im_in,           // input image
	const int32_t dim_im_in_x,   // input image dimension
	const int32_t dim_im_in_y,   // input image dimension
	const int32_t ch_im_in,      // number of input image channels
	const int32_t dim_kernel_x,  // window kernel size
	const int32_t dim_kernel_y,  // window kernel size
	const int32_t padding_x,     // padding sizes
	const int32_t padding_y,     // padding sizes
	const int32_t stride_x,      // stride
	const int32_t stride_y,      // stride
	const int32_t dim_im_out_x,  // output image dimension
	const int32_t dim_im_out_y,  // output image dimension
	q15_t* bufferA,               // a buffer for local storage
	q15_t* Im_out,                // output feature
	const int32_t out_lshift)    // output left shift (scaling)
{
	int32_t ch_in, x, y;
	int32_t k_x, k_y;

	for (ch_in = 0; ch_in < ch_im_in; ch_in++) {
		for (y = 0; y < dim_im_out_y; y++) {
			for (x = 0; x < dim_im_out_x; x++) {
				int maxVal = -32768;
                int v;
				for (k_y = y * stride_y - padding_y; k_y < y * stride_y - padding_y + dim_kernel_y; k_y++) {
					for (k_x = x * stride_x - padding_x; k_x < x * stride_x - padding_x + dim_kernel_x; k_x++) {
						if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x) 
                        {
                            v = Im_in[ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)];
							maxVal = maxVal < v ? v : maxVal;
						}
					}
				}
				Im_out[ch_in + ch_im_in * (x + y * dim_im_out_x)] = (q15_t) maxVal;
			}
		}
	}
}

void aia_avepool_q7_HWC_nonsquare(
	const q7_t* Im_in,           // input image
	const int32_t dim_im_in_x,   // input image dimension
	const int32_t dim_im_in_y,   // input image dimension
	const int32_t ch_im_in,      // number of input image channels
	const int32_t dim_kernel_x,  // window kernel size
	const int32_t dim_kernel_y,  // window kernel size
	const int32_t padding_x,     // padding sizes
	const int32_t padding_y,     // padding sizes
	const int32_t stride_x,      // stride
	const int32_t stride_y,      // stride
	const int32_t dim_im_out_x,  // output image dimension
	const int32_t dim_im_out_y,  // output image dimension
	q7_t* bufferA,               // a buffer for local storage
	q7_t* Im_out,                // output feature
	const int32_t out_lshift)    // output left shift (scaling)
{
	int32_t ch_in, x, y;
	int32_t k_x, k_y;

	for (ch_in = 0; ch_in < ch_im_in; ch_in++) {
		for (y = 0; y < dim_im_out_y; y++) {
			for (x = 0; x < dim_im_out_x; x++) {
				int sum = 0;
				int count = 0;
				for (k_y = y * stride_y - padding_y; k_y < y * stride_y - padding_y + dim_kernel_y; k_y++) {
					for (k_x = x * stride_x - padding_x; k_x < x * stride_x - padding_x + dim_kernel_x; k_x++) {
						if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x) {
							sum += Im_in[ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)];
							count++;
						}
					}
				}
				Im_out[ch_in + ch_im_in * (x + y * dim_im_out_x)] = (q7_t)(sum * (0x1 << out_lshift) / count);
			}
		}
	}
}

void aia_avepool_q15_HWC_nonsquare(
	const q15_t* Im_in,           // input image
	const int32_t dim_im_in_x,   // input image dimension
	const int32_t dim_im_in_y,   // input image dimension
	const int32_t ch_im_in,      // number of input image channels
	const int32_t dim_kernel_x,  // window kernel size
	const int32_t dim_kernel_y,  // window kernel size
	const int32_t padding_x,     // padding sizes
	const int32_t padding_y,     // padding sizes
	const int32_t stride_x,      // stride
	const int32_t stride_y,      // stride
	const int32_t dim_im_out_x,  // output image dimension
	const int32_t dim_im_out_y,  // output image dimension
	q15_t* bufferA,               // a buffer for local storage
	q15_t* Im_out,                // output feature
	const int32_t out_lshift)    // output left shift (scaling)
{
	int32_t ch_in, x, y;
	int32_t k_x, k_y;

	for (ch_in = 0; ch_in < ch_im_in; ch_in++) {
		for (y = 0; y < dim_im_out_y; y++) {
			for (x = 0; x < dim_im_out_x; x++) {
				int sum = 0;
				int count = 0;
				for (k_y = y * stride_y - padding_y; k_y < y * stride_y - padding_y + dim_kernel_y; k_y++) {
					for (k_x = x * stride_x - padding_x; k_x < x * stride_x - padding_x + dim_kernel_x; k_x++) {
						if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in_y && k_x < dim_im_in_x) {
							sum += Im_in[ch_in + ch_im_in * (k_x + k_y * dim_im_in_x)];
							count++;
						}
					}
				}
				Im_out[ch_in + ch_im_in * (x + y * dim_im_out_x)] = (q15_t)(sum * (0x1 << out_lshift) / count);
			}
		}
	}
}

void aia_nn_activations_direct_q15(q15_t* data, int size, int int_width, arm_nn_activation_type type, int quantBits)
{
	int  i = size;
	q15_t* pIn = data;
	q15_t* pOut = data;
	const q15_t* lookup_table;

	switch (type)
	{
	case ARM_SIGMOID:
		lookup_table = sigmoidTable_q15;
		break;
	case ARM_TANH:
	default:
		lookup_table = tanhTable_q15;
		break;
	}
	while (i--)
	{
		int     out1, out2, out;
		short     in = *pIn++;
#if 1
		int max = 1 << (quantBits - 1);
		int min = -max;
		if (in >= max)
			in = max - 1;
		if (in < min)
			in = min;
#else
		in = __SSAT(in, quantBits);
#endif		
		in <<= (16 - quantBits);
		in >>= (3 - int_width);
		uint32_t ndx0 = (in >> 8) & 0xFF;
		uint32_t ndx1 = (ndx0 + 1) & 0xFF;
		uint32_t frac = in & 0xFF;
		if (ndx1 == 0x80)
			ndx1 = ndx0;
		out1 = lookup_table[ndx0];
		out2 = lookup_table[ndx1];
		// interpolation, assume each of 256 slices is linear
		out = out1 + ((out2 - out1) * frac >> 8);

		*pOut++ = (q15_t)out >> (16 - quantBits); 
	}
}

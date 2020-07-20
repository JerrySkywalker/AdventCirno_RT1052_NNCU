/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		IAR 8.3 or MDK 5.26
 * @Target core		NXP RT1021DAG5A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-02-18
 ********************************************************************************************************************/

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
volatile uint32_t g_tick;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

 #include "nncu/nncie.h"
 #define IMAGE_SIZE    (8 + 32*32*3)  // 3080

extern void* RunModel(const void *in_buf);
//q7_t *g_out;
int16_t g_out[1];


typedef enum _enum_OutType
{
	outtype_cls = 0,
	outtype_lgsbin = 1,
	outtype_rgrcls = 2,
	outtype_angle = 3,
	outtype_recon = 4,
	outtype_raw = 100,
}outType_e;

typedef struct _ClsTest_t
{
	uint32_t clsCnt;
}ClsTest_t;

typedef struct _TestFileHdr_t
{
	char sMgc[4];	// NCTT
	uint32_t tvSize;
	uint32_t tvCnt;
	uint8_t outType;
	uint8_t isExtTV;	// is using extended test vector format
	uint16_t outDim;
	union {
		float f32Params[2];
		int32_t i32Params[2];
	};
	uint32_t rsvd[2];
}TestFileHdr_t;

typedef struct _TestVec_t
{
	char _0xA5;	// must be 0xA5, else check for TestVecExt_t
	uint8_t w, h, c;
	union {
		uint32_t label;
	};
}TestVec_t;

typedef struct _TestVecExt_t
{
	char magic[4];	//
	uint16_t w, h, c, z;
	union {
		uint32_t label;
	};
}TestVecExt_t;

typedef struct _TestCtx_t
{
	TestFileHdr_t hdr;
	const void* pvTVs;
	uint32_t tvNdx;
	uint8_t isXIP;
}TestCtx_t;

const TestVec_t* TestVecPopNext(TestCtx_t* pCtx) {
	if (pCtx->tvNdx >= pCtx->hdr.tvCnt)
		return 0;
	uint32_t adr = (uint32_t)pCtx->pvTVs + pCtx->tvNdx * pCtx->hdr.tvSize;
	pCtx->tvNdx++;
	return (const TestVec_t*)adr;
}

int TestVecCreateCtxXIP(const void *pvTestData, TestCtx_t* pCtx) {
	int ret = 0;
	memset(pCtx, 0, sizeof(*pCtx));
	memcpy(&pCtx->hdr, pvTestData, sizeof(pCtx->hdr));
	pCtx->pvTVs = (const void*)((uint32_t)pvTestData + sizeof(pCtx->hdr));
	pCtx->isXIP = 1;
	return ret;
}

#if 0
int TestVecCreateCtx(const char* pszTestFile, TestCtx_t* pCtx) {
	int ret = 0;
	FILE* fp = fopen(pszTestFile, "rb");
	uint32_t tvsSize;
	if (0 == fp) {
		ret = -1L;
		goto cleanup;
	}
	memset(pCtx, 0, sizeof(*pCtx));
	fread(&pCtx->hdr, sizeof(pCtx->hdr), 1, fp);
	tvsSize = pCtx->hdr.tvCnt * pCtx->hdr.tvSize;
	pCtx->pvTVs = malloc(tvsSize);
	fread((void*)pCtx->pvTVs, 1, tvsSize, fp);
	fclose(fp);
cleanup:
	return ret;
}
#endif

int TestVecDestroyCtx(TestCtx_t* pCtx) {
	if (!pCtx->isXIP)
		free((void*)pCtx->pvTVs);
	return 0;
}

float GetAngle(signed char *pv1, signed char *pv2)
{
	int32_t iM1, iM2, iDot;
	float fM1, fM2, fDot;
	int i;
	for (i = 0, iM1 = 0, iM2 = 0, iDot = 0; i<128; i++) {
		iDot += (int32_t)pv1[i] * pv2[i];
		iM1 += (int32_t)pv1[i] * pv1[i];
		iM2 += (int32_t)pv2[i] * pv2[i];
	}
	fDot = (float)iDot;
	fM1 = sqrtf((float)iM1);
	fM2 = sqrtf((float)iM2);
	float cosVal = fDot / (fM1 * fM2);
	float angle = acosf(cosVal) * 180 / 3.141592654f;
	return angle;
}


#define ANGLE_BORDER	59.8f // 64.569822f
int CIETest(const void *pvModel, TestCtx_t *pCtx) {
	int i, j, okCnt = 0;
	const char* pImg;
	float avgAngs[2] = { 0.0f, 0.0f };
	int cnts[2] = { 0, 0 };
	int isSame = 0;
	int avg_mse = 0;
	ModelInfo_t inf;
	CI_GetModelInfoXIP(pvModel, &inf);
	if (inf.pszString)
		PRINTF("%s\r\n", inf.pszString);
	#ifdef SRC_DEBUG
	uint8_t *pOut;
	int max, maxAt;
	#else

	 // CI_RunModelXIP(pvModel, pImg + 8, g_out);
	#endif
	uint32_t outType = pCtx->hdr.outType;
	uint32_t t0, t1;
	t0 = g_tick;
	for (i = 0; i < 100000; i++)
	{
		pImg = (const char*) TestVecPopNext(pCtx);
		// pImg = GetTestSample(i);
		if (0 == pImg)
			break;
		#ifdef SRC_DEBUG
		uint8_t *pOut;
		int max, maxAt;
		pOut = (uint8_t*)RunModel(pImg + 8);
		memcpy(g_out, pOut, 128);
		int16_t *pOut16 = (int16_t*)g_out;
		pOut = (int8_t *)g_out;
		for (j = 0, max = -128; j < pCtx->hdr.outDim; j++)
		{
			if (pOut[j] > max)
			{
				max = pOut[j];
				maxAt = j;
			}
		}
		g_out[0] = maxAt;
		outType = outtype_cls;
		#else
		const CI_OutMetricInfo* pOM = CI_GetOutMetricInfo(0);
		//CI_RunModelXIP_NoCopyOutput(pvModel, pImg + 8, (void**)&g_out);
		CI_RunModelXIP(pvModel, pImg + 8, g_out);

		#endif
		int16_t g_servoValue;
		if(inf.quantBits>7)
		{
			int16_t *g_out16 = (int16_t *)g_out;
			g_servoValue = g_out16[0] >> (inf.quantBits - pOM->fracBits - 1);
			g_servoValue = g_out[0]>>3;
		}
		else
		{
			g_servoValue = g_out[0];
		}

		//PRINTF("%04d %d got %d -- %d\r\n", i + 1, label,(g_out16[0]), avg_mse);
		avg_mse += (pImg[4] - g_servoValue) * (pImg[4] - g_servoValue);
	}


	t1 = g_tick - t0;
	j = (10000 * okCnt) / i;
	int printOvhd = 0;
	avg_mse=sqrt(avg_mse/i);
	j = avg_mse;
	//t1 = t1 / i;
	PRINTF("%ddms, average %d.%02ddms/inference %d\r\n", t1, t1 / i - printOvhd, (t1 * 100 / i) % 100, i);

	PRINTF("%05d\r\n", j);
	return j;
}


#define SYSTICK_PRESCALE 10

int NNCU_Deploy(void)
{
//	DisableGlobalIRQ();
//    board_init();//务必保留，本函数用于初始化MPU 时钟 调试串口
//    EnableGlobalIRQ(0);

	char ch;
	TestCtx_t tstCtx;

    while (1)
    {
		TestVecCreateCtxXIP(test1, &tstCtx);
		CIETest(model1, &tstCtx);
        //ch = GETCHAR();
        //PUTCHAR(ch);
    }
}


#define PROFILING	1
#ifdef 	PROFILING

#define PROF_CNT		20
#define PROF_ERR		16
#define PROF_MASK 		(~(PROF_ERR - 1))
#define PROF_HITCNT_INC	100
// whether hitCnt should decay, faster decay makes most time consuming functions
// seems to have even more hit count
#define PROF_DECAY
uint32_t s_ignrList[] = {
	0,
	// (uint32_t)sensor_snapshot + 0x40 ,
};

typedef struct {
	uint32_t baseAddr;	// (aligned) base address range of PC sample
	uint32_t hitCnt;    // hit count (a decay mecahnism automatically drops it)
	uint32_t hitRatio;	// 10-bit resolution hit ratio,
	uint32_t rsvd;		// reserved for better view in memory window
} ProfUnit_t;

typedef struct {
	uint8_t decayNdx;  // which item to decay its hitCnt
	uint32_t profCnt;  // totoal hit count of profiling
	ProfUnit_t items[PROF_CNT];
}Prof_t;
Prof_t s_prof;

void _ProfOnHit(ProfUnit_t *pItem, uint32_t pc)
{
	pItem->baseAddr = pc & PROF_MASK;
	s_prof.profCnt+= PROF_HITCNT_INC;
	pItem->hitCnt += PROF_HITCNT_INC;
	pItem->hitRatio =
		(uint32_t)(((uint64_t)(pItem->hitCnt) << 10) / s_prof.profCnt);
	// sort items descending
	ProfUnit_t tmpItem;
	for (;pItem != s_prof.items && pItem[0].hitCnt > pItem[-1].hitCnt; pItem--)
	{
		tmpItem = pItem[0]; pItem[0] = pItem[-1] ; pItem[-1] = tmpItem;
	}
}
void ProfUpdateRate(float res)
{
	uint32_t i;
	float profCnt = s_prof.profCnt;
	ProfUnit_t *pItem = s_prof.items;
	for (i=0; i<PROF_CNT; i++, pItem++)
	{
		pItem->hitRatio = (uint32_t)( (float)(pItem->hitCnt) * res / profCnt);

	}
}
void ProfReset(void)
{
  memset(&s_prof, 0, sizeof(s_prof));
}
void Profiling(uint32_t pc)
{
	uint32_t i;
	ProfUnit_t *pItem = &s_prof.items[0];
	// filter ignore list
	for (i=0; i<ARRAY_SIZE(s_ignrList); i++)
	{
		if (pc - s_ignrList[i] < PROF_ERR)
			return;
	}
	#ifdef PROF_DECAY
		// apply decaying, we do not decay to zero, this means PC samples do not
		// get removed automatically, only can be pushed by later new more frequent
		// hit PC samples
		if (s_prof.items[s_prof.decayNdx].hitCnt > 1) {
			s_prof.items[s_prof.decayNdx].hitCnt--;
			s_prof.profCnt--;
		}
		if (++s_prof.decayNdx == PROF_CNT)
			s_prof.decayNdx = 0;
	#endif
	uint32_t freeNdx = PROF_CNT;
	// >>> traverse to search existing same PC sample
	for (i=0, pItem = s_prof.items; i<PROF_CNT; i++, pItem++) {
		if (pItem->baseAddr == (pc & PROF_MASK)) {
			_ProfOnHit(pItem, pc);
			break;
		} else if (freeNdx == PROF_CNT && pItem->hitCnt == 0){
			// record empty item, in case need to use for new PC sample
			freeNdx = i;
		}
	}
	if (i == PROF_CNT) {
		if (freeNdx < PROF_CNT) {
			// does not find, allocate for new
			_ProfOnHit(s_prof.items + freeNdx, pc);
		} else {
		  // replace the last one. We must give new samples chance to compete to
		  // get into the list.
		  freeNdx = PROF_CNT - 1;
		  s_prof.profCnt -= s_prof.items[freeNdx].hitCnt;
		  s_prof.items[freeNdx].hitCnt = 0;
		  _ProfOnHit(s_prof.items + freeNdx, pc);
		}
	}
}

#endif
typedef struct {
    uint32_t    r0, r1, r2, r3, r12, lr, pc, xpsr;
} ExceptionRegisters_t;


volatile uint32_t s_prescale;
void SysTick_C_Handler(ExceptionRegisters_t *regs) {
    extern uint32_t uwTick;

	#if SYSTICK_PRESCALE > 1
	if (++s_prescale < SYSTICK_PRESCALE)
		return;
	s_prescale = 0;
	#endif

	#ifdef PROFILING
	Profiling(regs->pc);
	#endif
	g_tick++;
}


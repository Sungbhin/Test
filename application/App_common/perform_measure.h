#ifndef _PERFORM_MEASURE_H
	#define _PERFORM_MEASURE_H

	#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
	extern "C"
	{
	#endif
	/*---------------------------------------------------------------------------*/
	/*  Copyright (C) 2012 Siemens Aktiengesellschaft. All Rights Reserved.      */
	/*  This program is protected by German copyright law and international      */
	/*  treaties.                                                                */
	/*  The use of this software including but not limited to its Source Code    */
	/*  is subject to restrictions as agreed in the license agreement between    */
	/*  you and Siemens.                                                         */
	/*  Copying or distribution is not allowed unless expressly permitted        */
	/*  according to your license agreement with Siemens.                        */
	/*---------------------------------------------------------------------------*/
	/*  Project             : PROFInet Runtime Software                          */
	/*  File                : perform_measure                                    */
    /*  Version             : V4.0                                               */
	/*                                                                           */
	/*---------------------------------------------------------------------------*/
	/*                                                                           */
	/*  D e s c r i p t i o n :                                                  */
	/*                                                                           */
	/*  header file to usriod_utils.c.                                           */
	/*---------------------------------------------------------------------------*/
	/*                                                                           */
	/*  H i s t o r y :                                                          */
	/*                                                                           */
	/*  Date        Version        Who  What                                     */
	/*                                                                           */
	/*---------------------------------------------------------------------------*/

	// *------------defines ----------*
	//#define INCLUDE_PERFORMANCE_MEASURE		1 // include a performance measure feature


	PNIO_UINT32 InitPerformanceMeasure     (void);
	PNIO_UINT32 ExecutePerformanceMeasure (void);



	#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
	}
	#endif

#endif

/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2012 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/

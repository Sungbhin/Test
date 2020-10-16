/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2013 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*---------------------------------------------------------------------------*/
#include "compiler.h"
#if (PNIOD_PLATFORM & PNIOD_PLATFORM_ECOS)

    #include "compiler_stdlibs.h"
    #include "lsa_cfg.h"
    #include "os.h"
    #include "os_taskprio.h"
    #include "pniousrd.h"
    #include "perform_measure.h"

	#include <stdio.h>                      /* printf */
	#include <string.h>                     /* strlen */
	#include <cyg/kernel/kapi.h>            /* All the kernel specific stuff */
	#include <cyg/io/io.h>                  /* I/O functions */
	#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
	#include <cyg/infra/diag.h>


	/* DEFINES */

	#define NTHREADS 1
	#define STACKSIZE ( CYGNUM_HAL_STACK_SIZE_TYPICAL + 4096 )

	/* STATICS */

	static cyg_handle_t thread[NTHREADS];
	static cyg_thread thread_obj[NTHREADS];
	static char stack[NTHREADS][STACKSIZE];

    int main_ecos(void);

	static void sample_thread_entry(CYG_ADDRESS data)
	{
		main_ecos();
	}

	void cyg_user_start(void)
	{
		cyg_thread_create(4, sample_thread_entry, (cyg_addrword_t) 0, "DK401",
						  (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
		cyg_thread_resume(thread[0]);
	}


    extern void MainAppl   (void);

    LSA_UINT32 TskId_Main;


    int main_ecos(void)
    {
		PNIO_UINT32 Status;
        // *------------------------------------------------*
        // *  initialize PNIO stack
        // *------------------------------------------------*
        PNIO_init ();

        #ifdef INCLUDE_PERFORMANCE_MEASURE
		// *----------------------------------------------------------
		// * Do the reference performance measure before starting the
		// * pnio stack. This is a test-option only and not necessary
		// * for PNIO.
		// *----------------------------------------------------------
		{
			Status = InitPerformanceMeasure ();
			if (Status != LSA_OK)
			{
				PNIO_printf ("error in InitPerformanceMeasure()\n");
			}
		}
        #endif // INCLUDE_PERFORMANCE_MEASURE
 
		// *------------------------------------------------*
		// *  create and start PROFINET user application
		// *------------------------------------------------*
		Status = OsCreateThread (MainAppl, (PNIO_UINT8*)"MainAppl", TASK_PRIO_MAIN, &TskId_Main);
        if (Status != PNIO_OK)
        {
            printf ("error OsCreateThread\n");
        }

		Status = OsCreateMsgQueue (TskId_Main);
        if (Status != PNIO_OK)
        {
            printf ("error OsCreateThread\n");
        }

		Status = OsStartThread (TskId_Main);
        if (Status != PNIO_OK)
        {
            printf ("error OsCreateThread\n");
        }

	    while (1)
	    {
		    #ifndef DEBUG
		    cyg_thread_suspend(cyg_thread_self()); // sleep forever
		    #else
		    OsWait_ms(1000); // for debug
		    #endif
	    }
	    return (0); /*lint !e527 Unreachable code */
    }

#endif
/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2013 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/

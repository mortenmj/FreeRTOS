/*
 * display.c
 *
 * Created: 02.10.2013 23:45:09
 *  Author: mortenmj
 */

#include "FreeRTOS.h"
#include "task.h"

#include "u8glib/u8g.h"
#include "m2tklib/m2.h"
#include "display.h"

/* Task frequency */
#define dispTASK_FREQUENCY		( ( const portTickType ) 200 )

/* Delay needed before initializing display */
#define dispINIT_DELAY			500

void vDisplay ( void *pvParameters )
{
	portTickType xLastWakeTime;
	u8g_t u8g;
	
	xLastWakeTime = xTaskGetTickCount ();
	
	/* Display requires a short wait before initialization */
	vTaskDelay ( dispINIT_DELAY / portTICK_RATE_MS );	
	u8g_Init8BitFixedPort(&u8g, &u8g_dev_ssd1308_128x64_parallel, 0, 0, 0, 0, 0);
	u8g_SetFont (&u8g, u8g_font_5x8);
	
/*
	m2_Init (&hello_world_label, m2_es_avr_u8g, m2_eh_6bs, m2_gh_u8g_bfs);
	m2_SetU8g(&u8g, m2_u8g_box_icon);
	m2_SetFont(0, (const void *)u8g_font_5x8);
*/
	
	while (1)
	{
		vTaskDelayUntil ( &xLastWakeTime, dispTASK_FREQUENCY );
		
		u8g_FirstPage(&u8g);
		do
		{
			u8g_DrawStr ( &u8g, 0, 10, "CANSTAT: " );
			//u8g_DrawStr ( &u8g, 45, 10, cCanstat );
			
			u8g_DrawStr ( &u8g, 0, 20, "CANINTF: " );
			//u8g_DrawStr ( &u8g, 45, 20, cCanintf );

			u8g_DrawStr ( &u8g, 0, 30, "CNF1: " );
			//u8g_DrawStr ( &u8g, 45, 30, cCnf1 );
			
			u8g_DrawStr ( &u8g, 0, 40, "CNF2: " );
			//u8g_DrawStr ( &u8g, 45, 40, cCnf2 );
			
			u8g_DrawStr ( &u8g, 0, 50, "CNF3: " );
			//u8g_DrawStr ( &u8g, 45, 50, cCnf3 );
			
			u8g_DrawLine ( &u8g, 64, 0, 64, 64 );
			
			//u8g_DrawStr ( &u8g, 70, 10, (const char *) xInFrame.data );
		} while ( u8g_NextPage( &u8g ) );
	}
}

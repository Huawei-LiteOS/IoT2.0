/* Includes LiteOS------------------------------------------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "los_base.h"
#include "los_config.h"
#include "los_typedef.h"
#include "los_hwi.h"
#include "los_task.ph"
#include "los_sem.h"
#include "los_event.h"
#include "los_memory.h"
#include "los_queue.ph"
#include "cmsis_os.h"
#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "bsp_led.h" 
#include "bsp_debug_usart.h"
#include "dwt.h"
#include "bsp_key.h"
#include "LAN8742A.h"
#include "ethernetif.h"
#include "netconf.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "net.h"
#include "ssl.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
KEY Key1,Key2;
UINT32 g_TskHandle;
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */

struct netif gnetif;
//#define USE_DHCP 0


/* Private function prototypes -----------------------------------------------*/
static void TIM3_Config(uint16_t period,uint16_t prescaler);
/* Private functions ---------------------------------------------------------*/
void TIM3_IRQHandler(void);
void hardware_init(void)
{
	LED_GPIO_Config();
	Key1_GPIO_Config();
	Key2_GPIO_Config();
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	Debug_USART_Config();
	DelayInit(SystemCoreClock);
	LOS_HwiCreate(TIM3_IRQn, 0,0,TIM3_IRQHandler,NULL);
	TIM3_Config(999,899);
	printf("Sysclock is %d\r\n",SystemCoreClock);
}


static void TIM3_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_Period=period;   
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 
	TIM_Cmd(TIM3,ENABLE); 
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) 
	{
		LocalTime+=10;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  
}

#if USE_DHCP
__IO uint8_t DHCP_state = DHCP_OFF;
#endif

VOID task1()
{

    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    printf("LAN8720A Ethernet Demo\n");

    /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
    ETH_BSP_Config();	
    printf("LAN8720A BSP INIT AND COMFIGURE SUCCESS\n");

    tcpip_init(NULL, NULL);
#if USE_DHCP
    struct dhcp *dhcp;
    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else
    IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
    IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
    IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, 
        &ethernetif_init, &tcpip_input);
    netif_set_default(&gnetif);

  
    if (netif_is_link_up(&gnetif))
    {
        gnetif.flags |= NETIF_FLAG_LINK_UP;
        netif_set_up(&gnetif);
    }
    else
    {
        netif_set_down(&gnetif);
    }
	
#if USE_DHCP
    DHCP_state = DHCP_START;
    for(;;)
    {	
    if(DHCP_state == DHCP_ADDRESS_ASSIGNED){
        printf("get dhcp successfully,ip is %d-%d-%d-%d\n",
			ip4_addr1(&gnetif.ip_addr),ip4_addr2(&gnetif.ip_addr),
			ip4_addr3(&gnetif.ip_addr),ip4_addr4(&gnetif.ip_addr));
		break;
    }
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        ip_addr_set_zero_ip4(&gnetif.ip_addr);
        ip_addr_set_zero_ip4(&gnetif.netmask);
        ip_addr_set_zero_ip4(&gnetif.gw);       
        dhcp_start(&gnetif);
        DHCP_state = DHCP_WAIT_ADDRESS;
      }
      break;
      
    case DHCP_WAIT_ADDRESS:
      {                
        if (dhcp_supplied_address(&gnetif)) 
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;	
          
          
        }
        else
        {
          dhcp = (struct dhcp *)netif_get_client_data(&gnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    
          /* DHCP timeout */
          if (dhcp->tries > 10)
          {
            printf("dhcp timeout!\n");
            DHCP_state = DHCP_TIMEOUT;
            
            /* Stop DHCP */
            dhcp_stop(&gnetif);
            
            /* Static address used */
            IP_ADDR4(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
            IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
            IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
            netif_set_addr(&gnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));

           
            
          }
          else
          {
                
          }
        }
      }
      break;
  case DHCP_LINK_DOWN:
    {
      /* Stop DHCP */
      dhcp_stop(&gnetif);
      DHCP_state = DHCP_OFF; 
    }
    break;
    default: break;
    }
    
    /* wait 250 ms */
    osDelay(250*4);
    printf("wait for ip!!!\n");
  }
#endif
	
#if 0

    extern int test_dtls2(void);
    test_dtls2();
#endif

		
#if 1
    extern int lwm2m_main(int argc, char *argv[]);       
    lwm2m_main(1, NULL);
#endif
		




}


UINT32 creat_task1()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "task1";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task1;
    task_init_param.uwStackSize = 0x8000;

    uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
        
}

VOID task2()
{
	//UINT32 count = 0;
	while(1)
	{
		//printf("This is task 2,count is %d \r\n",count++);
		LOS_TaskDelay(1000);
	}
}


UINT32 creat_task2()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "task2";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task2;
    task_init_param.uwStackSize = 0x800;

    uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
        
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    LOS_KernelInit();//�ں˳�ʼ��	
    hardware_init();//Ӳ����ʼ��
    uwRet = creat_task1();
    if(uwRet != LOS_OK)
    {
        return uwRet;
    }
    uwRet = creat_task2();
    if(uwRet != LOS_OK)
    {
        return uwRet;
    }
		
    LOS_Start();//����LiteOS
}

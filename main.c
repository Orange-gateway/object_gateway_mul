#include "include.h"
void handler(int s)
{
	if(s == SIGBUS)
	{
		system("rm /bin/gateway");
		system("killall gateway");
	}
}
int main(int argc,char* argv[])
{
	umask(0);
	signal(SIGBUS, handler);
	memset(gw_version,0,9);
	memcpy(gw_version,"20200702",strlen("20200702"));
	printf("gateway_version:%s\n",gw_version);
	memset(zigbee_channel,0,2);
	memset(zigbee_id,0,5);
	onekey_or_study_flag=2;
	resend_head = resend_z = resend_d = NULL;
	human_head = human_z = human_d = NULL;
	delay_head = delay_z = delay_d = NULL;
	int ret_usart,ret0,ret1,ret2,ret3,ret4,ret5,ret6,ret7,ret8;          //接受返回值，用于判断
	identify_flag = 0;
	go_net_flag = 0;
	NET_FLAG = 0;
	device_flag = 0;
	down_gw_flag = 0;
	alive = 0;
	pthread_cond_init(&cond,NULL);
	pthread_mutex_init_func();
	fd = usart_open(fd,argv[1]);
	do
	{
		ret_usart = usart_init(fd,115200,0,8,'n',1);
	}while(ret_usart == -1 || fd == -1);
	
	if(open_wifi_file() <0 )
	{
		uint8_t voice_notice[23] = {0x5a,0xa5,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x45,0x01,0x00,0x00,0x08,0xa5,0xfa,0x00,0x03,0xbc,0x00,0x5e,0xfb};
		//设备启动完成，请配置网络
		usart_send(fd,voice_notice,23);
		id_gpio=pth_creat_my((void *)pthread_gpio_led,NULL);
		while(1)
		{
			if(server_wifi())
				break;
		}
	}
	else
		system("echo out > /sys/class/gpio/gpio12/direction");
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction( SIGPIPE, &sa, 0 );
	file_init_common();
	pthread_t id_client,id_usart,id_heart_jump,id_resend,id_timer,id_get_status,id_human_body,id_my_delay,id_led;
	ret0 = pthread_create(&id_usart,NULL,(void*)pthread_usart_receive,NULL);
	if(ret0 < 0)
		{	
			printf("id_usart create fault\n");
			return -1;
		}
	read_channel();
	ret3 = pthread_create(&id_timer,NULL,(void*)my_timer,NULL);
	if(ret3 < 0)
		{
			printf("id_timer create fault\n");
		}
	
	ret4 = pthread_create(&id_resend,NULL,(void*)re_send,NULL);
	if(ret4 < 0)
		{
			printf("id_resend create fault\n");
		}
	ret5 = pthread_create(&id_get_status,NULL,(void*)get_status,NULL);
	if(ret5 < 0)
		{
			printf("id_get_status create fault\n");
		}
	ret6 = pthread_create(&id_human_body,NULL,(void*)human_check,NULL);
	if(ret6 < 0)
		{
			printf("id_human_body create fault\n");
		}
	ret8 = pthread_create(&id_my_delay,NULL,(void*)my_delay,NULL);
	if(ret8 < 0)
		{
			printf("id_my_delay create fault\n");
		}
	ret7 = pthread_create(&id_led,NULL,(void*)gateway_on_off,NULL);
	if(ret7 < 0)
		{
			printf("id_led create fault\n");
		}
	client_init();
	ret1 = pthread_create(&id_client,NULL,(void*)pthread_client_receive,NULL);
	if(ret1 < 0)
		{
			printf("id_client create fault\n");
			return -1;
		}
	ret2 = pthread_create(&id_heart_jump,NULL,(void*)heart_jump,NULL);
	if(ret2 < 0)
		{
			printf("id_heart_jump create fault\n");
		}
	pthread_join(id_get_status,NULL);
	pthread_join(id_usart,NULL);
	pthread_join(id_client,NULL);
	pthread_join(id_heart_jump,NULL);
	pthread_join(id_timer,NULL);
	pthread_join(id_resend,NULL);
	pthread_join(id_human_body,NULL);
	close(fd);
	close(cd);
	return 0;
}



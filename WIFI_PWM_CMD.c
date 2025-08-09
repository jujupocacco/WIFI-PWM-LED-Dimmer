#include <stdio.h>
#include <string.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "lwip/sockets.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"

void WIFI_INIT(void){

	esp_netif_init();
	esp_event_loop_create_default();
	esp_netif_create_default_wifi_ap();

	wifi_config_t wifi_conf = {
		.ap = {
			.ssid = "ESPROVERTEST",
			.ssid_len = strlen("ESPROVERTEST"),
			.channel = 1,
			.password = "hellothere090",
			.max_connection = 1,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK
		},
	};

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	esp_wifi_init(&cfg);
	esp_wifi_set_mode(WIFI_MODE_AP);
	esp_wifi_set_config(WIFI_IF_AP, &wifi_conf);
	esp_wifi_start();

}
void GPIO_INIT(void){

	gpio_config_t gpio2_conf = {
		.pin_bit_mask = (1U<<GPIO_NUM_2),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};

	gpio_config(&gpio2_conf);

}
void LEDC_PWM_INIT(void){

	ledc_timer_config_t ledctimer = {
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.duty_resolution = LEDC_TIMER_8_BIT,
		.timer_num = LEDC_TIMER_0,
		.freq_hz = 5000,
		.clk_cfg = LEDC_AUTO_CLK
	};

	ledc_channel_config_t ledcchannel = {
		.gpio_num = GPIO_NUM_2,
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.channel = LEDC_CHANNEL_0,
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel = LEDC_TIMER_0,
		.duty = 0,
		.hpoint = 0
	};

	ledc_timer_config(&ledctimer);
	ledc_channel_config(&ledcchannel);

}
void PARSE_CMD(char buffer[]){

	char cmd[4] = {0};
	char percent[4];
	for(int i = 0; i <= 2; i++){
		cmd[i] = buffer[i];
	}

	cmd[3] = '\0';
	int j = 0;

	for(int i = 3; buffer[i]!='\0'; i++){
		percent[j] = buffer[i];
		j++;
	}

	int num = atoi(percent);
	int bitconv = (num * 255) / 100;

	if(strcmp(cmd, "LED")==0){
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,bitconv);
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
	}

}
void UDP_INIT(void){

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1){
		printf("%s\n", "SOCKET CREATION FAILED");
	}

	struct sockaddr_in serverinfo;
	serverinfo.sin_family = AF_INET;
	serverinfo.sin_addr.s_addr = INADDR_ANY;
	serverinfo.sin_port = htons(8080);

	if(bind(sockfd,(struct sockaddr *) &serverinfo, sizeof(serverinfo)) == -1){
		printf("%s\n", "FAILED TO BIND SOCKET");
	}
	struct sockaddr_in clientinfo;
	socklen_t clientaddrlen = sizeof(clientinfo);

	char recvbuffer[32];
	while(1){
		int bytes_recvd = recvfrom(sockfd, recvbuffer, sizeof(recvbuffer), 0, (struct sockaddr *) &clientinfo, &clientaddrlen);
		if (bytes_recvd > 0){
			recvbuffer[bytes_recvd] = '\0';
			printf("%s\n%d", "RECIEVED:", bytes_recvd);
			PARSE_CMD(recvbuffer);
		}
		memset(recvbuffer, 0, sizeof(recvbuffer));
	}

}
void app_main(void){

	esp_err_t ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	WIFI_INIT();
	GPIO_INIT();
	LEDC_PWM_INIT();
	UDP_INIT();
}

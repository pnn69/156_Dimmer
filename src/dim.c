/*
    timer stuff
*/
#include "global.h"
#include "esp_log.h"
#include <driver/gpio.h>
#include <driver/timer.h>
#include <driver/periph_ctrl.h>
#include "dim.h"




#define ESP_INTR_FLAG_DEFAULT 0
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
static intr_handle_t s_timer_handle;

static const char *TAG = "timer";
static bool ZerroCross = 0;
static uint16_t cnt = 0;

int dim1 = 0;
int dim2 = 0;
int dim3 = 0;
int lstcnt = 0;

/*
	Zerro cross stuff
*/
void IRAM_ATTR gpio_isr_handler(void *arg) {
	uint32_t gpio_num = (uint32_t)arg;
	if (gpio_num == ZerroCrossPin) {
		if (cnt > 500) {
				ZerroCross = true;
			lstcnt = cnt;
			cnt = 0;
		}
	}
}

void init_zerocross(void) {
	gpio_config_t io_conf;
	io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pull_up_en = 0;
	gpio_config(&io_conf);
	gpio_set_intr_type(ZerroCrossPin, GPIO_INTR_POSEDGE);
	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	gpio_isr_handler_add(ZerroCrossPin, gpio_isr_handler, (void *)ZerroCrossPin);
}
/*
	Zerro cross stuff end
*/


static int16_t fireCNT = 0;
static void IRAM_ATTR timer_isr(void *arg) {
	//portENTER_CRITICAL(&timerMux);
	TIMERG0.int_clr_timers.t0 = 1;
	TIMERG0.hw_timer[0].config.alarm_en = 1;
	cnt++;
	fireCNT++;
	if (ZerroCross == true){
		ZerroCross = false;
		fireCNT =0;
		gpio_set_level(AC_pin1, 0);			//turn optocupler off
		gpio_set_level(AC_pin2, 0);			//turn optocupler off
		gpio_set_level(AC_pin3, 0);			//turn optocupler off
		//portEXIT_CRITICAL(&timerMux);
		return;
	}

	if(fireCNT-150 >= 1000){					//1000 interrupts during one period. Reset to zero for negative part of the mains cycle
		fireCNT = 0;
	}
	if(fireCNT-150 >= 1000 - dim1 ){ 		//150 period delay between zero cross detection and the real zero cross
		gpio_set_level(AC_pin1, 1);			//activate optocupler
	}
	if(fireCNT-150 >= 1000 - dim2 ){
		gpio_set_level(AC_pin2, 1);
	}
	if(fireCNT-150 >= 1000 - dim3 ){
		gpio_set_level(AC_pin3, 1);
	}	//portEXIT_CRITICAL(&timerMux);
}



/*
    setup timer auto reload//
    100us interval at 50Hz
    83us  interval at 60Hz
*/
void init_timer(int ipp) {

	int timer_period_us;
	timer_config_t config = {
		.alarm_en = true,
		.counter_en = false,
		.intr_type = TIMER_INTR_LEVEL,
		.counter_dir = TIMER_COUNT_UP,
		.auto_reload = true,
		.divider = 8 // 1 us per tick 
	};

	if (ipp >= 900) {
		timer_period_us = 100;
		ESP_LOGI(TAG, "setup->50Hz");
	} else {
		timer_period_us = 83;
		ESP_LOGI(TAG, "setup->60Hz");
	}

	timer_init(TIMER_GROUP_0, TIMER_0, &config);
	timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);
	timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, timer_period_us);
	timer_enable_intr(TIMER_GROUP_0, TIMER_0);
	timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_isr, NULL, 1, &s_timer_handle);
	timer_start(TIMER_GROUP_0, TIMER_0);
	gpio_pad_select_gpio(AC_pin1);gpio_set_direction(AC_pin1,GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(AC_pin2);gpio_set_direction(AC_pin2,GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(AC_pin3);gpio_set_direction(AC_pin3,GPIO_MODE_OUTPUT);

}

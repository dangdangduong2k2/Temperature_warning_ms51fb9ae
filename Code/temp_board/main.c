#include "main.h"
#include "string.h"
/* Define Value*/
uint16_t A0,A1,A2,A3,A4,A5,FB_B;
uint16_t Thresh_hole1 = 0;
uint16_t Thresh_hole2 = 0;
uint8_t Sensor_is_on =0;
uint8_t buzzer_is_on =0;
uint16_t count_to_off_buzzer=0;
uint8_t using_ADC ; //0:LOW,1:HIGH
uint8_t buff[20];
uint16_t temp[6]={0,0,0,0,0,0};
float FB;
int tick;
uint8_t hall_is_on=0;
uint8_t off_t_low=0;
uint8_t turn_5s_buzzer=0;
float avg_low_temperature,avg_high_temperature;
uint8_t is_on_thresh_hold=0;
uint8_t is_on_thresh_hold1=0;
uint8_t but;
uint8_t set_mode=0;
uint8_t set_done=1;
uint8_t but_count[6]={0,0,0,0,0,0};
uint8_t seg_code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // common cathode
uint16_t count=0;
uint8_t data_fl[1];
/* Define Pin By Config_GPIO */
#define AN0 P17 //
#define AN1 P30 //
#define AN2 P07 //
#define AN3 P06 //
#define AN4 P05 //
#define AN5 P04 //
#define FB_BUZZER P03 //
#define PWM P01  //
#define RELAY P00 //
#define SENSOR P12 //
#define BUZZER P15 // 
#define SDA P14 //
#define SCL P13 //
#define SDA_BUT P11 
#define SCK_BUT P10

#define BRIGHTNESS_LV1 0x10
#define BRIGHTNESS_LV2 0x20
#define BRIGHTNESS_LV3 0x30
#define BRIGHTNESS_LV4 0x40
#define BRIGHTNESS_LV5 0x50
#define BRIGHTNESS_LV6 0x60
#define BRIGHTNESS_LV7 0x70
#define BRIGHTNESS_LV8 0x00

void soft_i2c_start(void)
{
	SCK_BUT = 1;
	SDA_BUT = 1;
	delay_us(10);
	SDA_BUT = 0;
	delay_us(10);
	SCK_BUT = 0;
}
void soft_i2c_write(uint8_t data_send) {
	int i;
	for ( i = 0; i < 8; i++) {
		delay_us(10);
		if (data_send & 0x80) {
			SDA_BUT = 1;
		}
		else SDA_BUT = 0;
		data_send <<= 1;

		SCK_BUT = 1;
		delay_us(10);
		SCK_BUT = 0;
	}

	delay_us(20);
	SCK_BUT = 1;
	delay_us(25);
	SCK_BUT = 0;
	SDA_BUT = 0;
}
void soft_i2c_stop() {
	// LCD_SCL_PIN = 0; 
	// LCD_SDA_PIN = 0;
	delay_us(10);
	SCK_BUT = 1;
	delay_us(10);
	SDA_BUT = 1;
}
void TM1650_init(void) {
	SDA_BUT = 1;
	SCK_BUT = 1;

	soft_i2c_start();
	soft_i2c_write(0x48);
	soft_i2c_write(BRIGHTNESS_LV5 | 0x01);
	soft_i2c_stop();
}


void TM1650_write(uint8_t address, uint8_t value) { // 68 6a 6c 6e
  soft_i2c_start();
	soft_i2c_write(address);
	soft_i2c_write(value);
	soft_i2c_stop();
}


uint8_t soft_i2c_read(uint8_t ack) {
	uint8_t i   = 0;
	uint8_t dat = 0;
//	Config_GPIO(I2C->SDA, Quasi); // Config SDA to input
	SDA_BUT=1;
	for (i = 0; i < 8; i++) {
		delay_us(10);
		SCK_BUT=1;
		delay_us(7);
		dat = dat * 2;
		if (SDA_BUT) dat = dat + 1;
		delay_us(3);
		SCK_BUT=0;
	}
	delay_us(10);
	SDA_BUT=!ack;
	delay_us(10);
	SCK_BUT=1;
	delay_us(50);
	SCK_BUT=0;
	return dat;
}

uint8_t TM1650_read(void) {
	uint8_t tm_data = 0;
	soft_i2c_start();
	
  soft_i2c_write(0x49);
  tm_data =soft_i2c_read(1);
  soft_i2c_stop();
	return tm_data;
}


void abs_16(uint16_t value)
{
	if(value<0)
	{
		value=-value;
	}
	else
	{
		value=value;
	}
}
void string_to_lcd(uint8_t *buff)
{
	while(*buff)
	{
			lcd_soft_i2c_data(*buff++);
	}
}
float map(double value, double fromMin, double fromMax, double toMin, double toMax) 
{   
		
   
		float scale,result;
    if (value < fromMin) value = fromMin;
    if (value > fromMax) value = fromMax;
    scale = (toMax - toMin) / (fromMax - fromMin);
    result = toMin + (value - fromMin) * scale;
    
    return result;
}

/* Define Pin By Config_GPIO */
void GPIO_INIT() {
	/* ===== GPIO INIT =====
	|  Port  |    Px7    |    Px6    |    Px5    |    Px4    |    Px3    |    Px2    |    Px1    |    Px0    |
	|   P0   |Input-only |Input-only |Input-only |Input-only |Input-only |Open-drain | Push-pull | Push-pull |
	|   P1   |Input-only |Input-only | Push-pull | Push-pull | Push-pull |Input-only |Input-only | Push-pull |
	|   P3   |    ---    |    ---    |    ---    |    ---    |    ---    |    ---    |    ---    |Input-only |
	===== GPIO INIT =====*/
	P0M1=0xFC;
	P0M2=0x03;
	P1M1=0xC6;
	P1M2=0x39;
	P3M1=0x01;
	P3M2=0x00;
	/* ===== GPIO DEFAULT STATE =====
	|  Port  |    Px7    |    Px6    |    Px5    |    Px4    |    Px3    |    Px2    |    Px1    |    Px0    |
	|   P0   |     0     |     0     |     0     |     0     |     0     |     0     |     0     |     0     |
	|   P1   |     0     |     0     |     0     |     0     |     0     |     0     |     0     |     0     |
	|   P3   |    ---    |    ---    |    ---    |    ---    |    ---    |    ---    |    ---    |     0     |
	===== GPIO DEFAULT STATE =====*/
	P0=0x00;
	P1=0x00;
	P3=0x00;
	P13 =0;
	P14 =0;
	P11_Quasi_Mode;
	P16_PushPull_Mode;
	P16=1;
}								
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////
/////////////////////////////////
struct eeprom_type {
	uint8_t buff1;
	uint8_t buff2;
	uint8_t buff3;
} eData;



	


void GPIO_check(void)
{
	/////HALL_SENSOR////
	if(P12==0)
	{
		hall_is_on=1;
	}
	else
	{
		hall_is_on=0;
	}
	/////BUZZER_OFF////
}

void DISPLAY_check(void)
{
		if(tick>=500)
		{
					lcd_soft_i2c_command(ADDR_LINE1);
					sprintf(buff,"-------T LOW--------");
					string_to_lcd(buff);
					lcd_soft_i2c_command(ADDR_LINE2);
					//T1
					lcd_soft_i2c_data('T');
					lcd_soft_i2c_data('4');
					lcd_soft_i2c_data(':');
					if(temp[0]>0 && 999>temp[0])
					{
						if(temp[0]>99)
						{
							sprintf(buff,"%d ",temp[0]);
							string_to_lcd(buff);
						}
						else
						{
							sprintf(buff,"%d  ",temp[0]);
							string_to_lcd(buff);
						}
					}
					else 
					{
						sprintf(buff,"    ",temp[0]);
						string_to_lcd(buff);
					}
					//T2
					lcd_soft_i2c_data('T');
					lcd_soft_i2c_data('5');
					lcd_soft_i2c_data(':');
					if(temp[1]>0 && 999>temp[1])
					{
						if(temp[1]>99)
						{
							sprintf(buff,"%d ",temp[1]);
							string_to_lcd(buff);
						}
						else
						{
							sprintf(buff,"%d  ",temp[1]);
							string_to_lcd(buff);
						}
					}
					else 
					{
						sprintf(buff,"    ",temp[1]);
						string_to_lcd(buff);
					}
					//T3
					lcd_soft_i2c_data('T');
					lcd_soft_i2c_data('6');
					lcd_soft_i2c_data(':');
					if(temp[2]>0 && 999>temp[2])
					{
						if(temp[2]>99)
						{
							sprintf(buff,"%d ",temp[2]);
							string_to_lcd(buff);
						}
						else
						{
							sprintf(buff,"%d",temp[2]);
							string_to_lcd(buff);
						}
					}
					else 
					{
						sprintf(buff,"   ",temp[2]);
						string_to_lcd(buff);
					}
					lcd_soft_i2c_command(ADDR_LINE3);
					sprintf(buff,"-------T HIGH-------");
					string_to_lcd(buff);
					lcd_soft_i2c_command(ADDR_LINE4);
					//T4
					lcd_soft_i2c_data('T');
					lcd_soft_i2c_data('1');
					lcd_soft_i2c_data(':');
					if(temp[3]>0 && 999>temp[3])
					{
						if(temp[3]>99)
						{
							sprintf(buff,"%d ",temp[3]);
							string_to_lcd(buff);
						}
						else
						{
							sprintf(buff,"%d  ",temp[3]);
							string_to_lcd(buff);
						}
					}
					else 
					{
						sprintf(buff,"    ",temp[3]);
						string_to_lcd(buff);
					}
					//T5
					lcd_soft_i2c_data('T');
					lcd_soft_i2c_data('2');
					lcd_soft_i2c_data(':');
					if(temp[4]>0 && 999>temp[4])
					{
						if(temp[4]>99)
						{
							sprintf(buff,"%d ",temp[4]);
							string_to_lcd(buff);
						}
						else
						{
							sprintf(buff,"%d  ",temp[4]);
							string_to_lcd(buff);
				
						}
					}
					else 
					{
						sprintf(buff,"    ",temp[4]);
						string_to_lcd(buff);
					}
					//T6
					lcd_soft_i2c_data('T');
					lcd_soft_i2c_data('3');
					lcd_soft_i2c_data(':');
					if(temp[5]>0 && 999>temp[5])
					{
						if(temp[5]>99)
						{
							sprintf(buff,"%d",temp[5]);
							string_to_lcd(buff);
						}
						else
						{
							sprintf(buff,"%d ",temp[5]);
							string_to_lcd(buff);
						}
					}
					else 
					{
						sprintf(buff,"   ",temp[5]);
						string_to_lcd(buff);
					}
//					lcd_soft_i2c_command(ADDR_LINE3);
//					lcd_soft_i2c_data('T');
//					lcd_soft_i2c_data('2');
//					lcd_soft_i2c_data(':');
//					sprintf(buff,"%d",temp[1]);
//					string_to_lcd(buff);
//					lcd_soft_i2c_command(ADDR_LINE4);
//					lcd_soft_i2c_data('T');
//					lcd_soft_i2c_data('3');
//					lcd_soft_i2c_data(':');
//					sprintf(buff,"%d",temp[2]);
//					string_to_lcd(buff);
					
					
//					lcd_soft_i2c_data(' ');
//					lcd_soft_i2c_data(' ');		
//					lcd_soft_i2c_data('T');
//					lcd_soft_i2c_data('2');
//					lcd_soft_i2c_data(':');
//					sprintf(buff,"%d",temp[1]);
//					string_to_lcd(buff);
//					lcd_soft_i2c_data(' ');
//					lcd_soft_i2c_data(' ');		
//					lcd_soft_i2c_data('T');
//					lcd_soft_i2c_data('3');
//					lcd_soft_i2c_data(':');
//					sprintf(buff,"%d",temp[2]);
//					string_to_lcd(buff);	
//				
//				
//					
//					lcd_soft_i2c_command(ADDR_LINE2);
//					lcd_soft_i2c_data('T');
//					lcd_soft_i2c_data('4');
//					lcd_soft_i2c_data(':');
//					sprintf(buff,"%d",temp[3]);
//					string_to_lcd(buff);
//					lcd_soft_i2c_data(' ');
//					lcd_soft_i2c_data(' ');		
//					lcd_soft_i2c_data('T');
//					lcd_soft_i2c_data('5');
//					lcd_soft_i2c_data(':');
//					sprintf(buff,"%d",temp[4]);
//					string_to_lcd(buff);
//					lcd_soft_i2c_data(' ');
//					lcd_soft_i2c_data(' ');		
//					lcd_soft_i2c_data('T');
//					lcd_soft_i2c_data('6');
//					lcd_soft_i2c_data(':');
//					sprintf(buff,"%d",temp[5]);
//					string_to_lcd(buff);	

				tick=0;
			}
}
void ADC_check(void)
{	
	if(tick==10)
	{
		A0=Read_ADC(AIN0,0x02);
		temp[0]=Thermistor(A0,5000);
	}
	else if(tick==20)
	{
		A1=Read_ADC(AIN1,0x02);
		temp[1]=Thermistor(A1,5000);
	}
	else if(tick==30)
	{
		A2=Read_ADC(AIN2,0x02);
		temp[2]=Thermistor(A2,5000);
	}
	else if(tick==40)
	{
		A3=Read_ADC(AIN3,0x02);
		temp[3]=Thermistor(A3,5000);
	}
	else if(tick==50)
	{
		A4=Read_ADC(AIN4,0x02);
		temp[4]=Thermistor(A4,5000);
	}
	else if(tick==60)
	{
		A5=Read_ADC(AIN5,0x02);
		temp[5]=Thermistor(A5,5000);
	}
	else if(tick==70)
	{
		FB_B=Read_ADC(AIN6,0x02);
	}
	
}

void PWM0_INIT(void)
{
/**********************************************************************
		PWM frequency = Fpwm/((PWMPH,PWMPL) + 1) <Fpwm = Fsys/PWM_CLOCK_DIV> 
									= (16MHz/8)/(0x7CF + 1)
									= 1KHz (1ms)
***********************************************************************/
    CKCON&=0xBF;   //	use sys frq
		PWMCON1&=0xF8; // 
		PWMCON1|=0x01; // div 2
		PIOCON0|=0x10; // enable_pwm_ch4_p01_output;
}

void PWM0_ConfigOutputChannel(float value)
{
	unsigned long u32PWM0Frequency;

	u32PWM0Frequency = map(value,0,120,55,90);
	BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS=1;EA=BIT_TMP;
  PWMCON1&=0x3F; //Independent
	PWMCON1&=0xEF; //EdgeAligned
//	PWM4H=(0x1E)>>8; 
//	PWM4L=(0x1E);
//	PWMPH = 0x58 >>8;
//  PWMPL = 0x58;
	PWM4H =(u32PWM0Frequency*3/10)>>8;
	PWM4L =(u32PWM0Frequency*3/10);
	PWMPH = u32PWM0Frequency>>8;
  PWMPL = u32PWM0Frequency;
	BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS=0;EA=BIT_TMP;
	PWMRUN= 1;
}


void USER_CONDITION_check(void)
{
	avg_low_temperature=(temp[0]+temp[1]+temp[2])/3;
	avg_high_temperature=(temp[4]+temp[5]+temp[6])/3;
	if(temp[0]>=Thresh_hole1 || temp[1]>=Thresh_hole1 ||temp[2]>=Thresh_hole1)
	{
		if(hall_is_on==0 && is_on_thresh_hold==0)
		{	
			P15=1;
			P16=0;
			//PWM0_ConfigOutputChannel(avg_low_temperature);
			P00=1;
			turn_5s_buzzer=0;
			is_on_thresh_hold=1;
		}
		if(hall_is_on==1 && turn_5s_buzzer==0 && is_on_thresh_hold==0)
		{
			turn_5s_buzzer=1;
			is_on_thresh_hold=1;
		}
		if(P02==0 && hall_is_on==0)
		{
			P15=0;
			P16=1;
		}
	}
	if(temp[0]<Thresh_hole1 && temp[1]<Thresh_hole1 && temp[2]<Thresh_hole1)
	{
			if(hall_is_on==0)
			{
				is_on_thresh_hold=0;
				P15=0;
				P16=1;
				P00=0;
			}
			if(hall_is_on==1)
			{
				turn_5s_buzzer=0;
			}		
	}	
	
	if(turn_5s_buzzer==1)
	{
			count_to_off_buzzer++;
			P15=1;
			P16=0;
			//PWM0_ConfigOutputChannel(avg_high_temperature);
			if(count_to_off_buzzer>1000)
			{
				P15=0;
				P16=1;
				//P00=0;//cmt
				turn_5s_buzzer=2;
				count_to_off_buzzer=0;
			}
	}
	if(temp[3]>=Thresh_hole2 || temp[4]>=Thresh_hole2 ||temp[5]>=Thresh_hole2)
	{
			if(hall_is_on == 1 && is_on_thresh_hold == 0 /*1*/ && turn_5s_buzzer==2)
			{	
				P15=1;
				P16=0;
				//PWM0_ConfigOutputChannel(avg_low_temperature);
				P00=1;
				is_on_thresh_hold = 1 /*2*/ ;
			}
			if(hall_is_on == 1 && P02==0)
			{
					P15=0;
					P16=1;
			}
	}
	else if(temp[3]<Thresh_hole2 && temp[4]<Thresh_hole2 && temp[5]<Thresh_hole2 )
	{
		if(hall_is_on==1)
		{	
			if(turn_5s_buzzer==2)
			{
				P15=0;
				P16=1;
			}
			
			P00=0;
			is_on_thresh_hold=0;
		}
	}	
}


uint8_t i=0;
void BUTTON_check(void)
{
		if(but==0x44)
		{
			i++;
			delay_ms(200);
		}
		if(i>=2)
		{
			i=0;
			
		}
		if(i==0)
		{
			TM1650_write(0x6a,seg_code[but_count[0]]);
			TM1650_write(0x6c,seg_code[but_count[1]]);
			TM1650_write(0x6e,seg_code[but_count[2]]);
			TM1650_write(0x68,0x38);
			if(Thresh_hole1 != (but_count[0]*100+but_count[1]*10+but_count[2]))
			{
				Thresh_hole1 = (but_count[0]*100+but_count[1]*10+but_count[2]);
			}
			else if (Thresh_hole1 == (but_count[0]*100+but_count[1]*10+but_count[2]))
			{
				if(count==1500)
				{
					if(Thresh_hole1>120)
					{
						Thresh_hole1= 120;
					}
					if (eData.buff1 != Thresh_hole1) 
					{
						eData.buff1  = Thresh_hole1;
						AP_EEPROM_Commit();
						TM1650_write(0x68,0x5E);
						delay_ms(1000);
						TM1650_write(0x68,0x38);
						delay_ms(20);
					} //ghi
					count=0;
				}
				count++;
			}
			else
			{
			count=0;
			}

			if(but==0x45||but==0x4D )
			{
				TM1650_write(0x6a,seg_code[but_count[0]]);
				but_count[0]++;
				if(but_count[0]>=10)
				{
					but_count[0]=0;
				}
				TM1650_write(0x6a,seg_code[but_count[0]]);
				delay_ms(200);
			}	
			else if(but==0x46 || but==0x56)
			{
				TM1650_write(0x6c,seg_code[but_count[1]]);
				but_count[1]++;
				if(but_count[1]>=10)
				{
					but_count[1]=0;
				}
				TM1650_write(0x6c,seg_code[but_count[1]]);
				delay_ms(200);
			}	
			else if(but==0x47 || but==0x5F)
			{
				TM1650_write(0x6e,seg_code[but_count[2]]);
				but_count[2]++;
				if(but_count[2]>=10)
				{
					but_count[2]=0;
				}
				TM1650_write(0x6e,seg_code[but_count[2]]);
				delay_ms(200);
			}	
		}
		else if(i==1)
		{
				TM1650_write(0x6a,seg_code[but_count[3]]);
				TM1650_write(0x6c,seg_code[but_count[4]]);
				TM1650_write(0x6e,seg_code[but_count[5]]);
				TM1650_write(0x68,0x76);
				if(Thresh_hole2 != (but_count[3]*100+but_count[4]*10+but_count[5]))
				{
					Thresh_hole2= (but_count[3]*100+but_count[4]*10+but_count[5]);
				}
				else if (Thresh_hole2 == (but_count[3]*100+but_count[4]*10+but_count[5]))
				{
					if(count==1500)
					{
						if(Thresh_hole2>120)
						{
							Thresh_hole2= 120;
						}
						if (eData.buff2 != Thresh_hole2) 
						{
							eData.buff2  = Thresh_hole2;
							AP_EEPROM_Commit();
							TM1650_write(0x68,0x5E);
							delay_ms(1000);
							TM1650_write(0x68,0x76);
						} //ghi
						count=0;
					}
					count++;
				}
				else
				{
				count=0;
				}

				if(but==0x45 || but==0x4D)
				{
					TM1650_write(0x6a,seg_code[but_count[3]]);
					but_count[3]++;
					if(but_count[3]>=10)
					{
						but_count[3]=0;
					}
					TM1650_write(0x6a,seg_code[but_count[3]]);
					delay_ms(200);
				}	
				else if(but==0x46 || but==0x56)
				{
					TM1650_write(0x6c,seg_code[but_count[4]]);
					but_count[4]++;
					if(but_count[4]>=10)
					{
						but_count[4]=0;
					}
					TM1650_write(0x6c,seg_code[but_count[4]]);
					delay_ms(200);
				}	
				else if(but==0x47 ||but==0x5F )
				{
					TM1650_write(0x6e,seg_code[but_count[5]]);
					but_count[5]++;
					if(but_count[5]>=10)
					{
						but_count[5]=0;
					}
					TM1650_write(0x6e,seg_code[but_count[5]]);
					delay_ms(200);
				}	
			}
}
void main(void) 
{
	MODIFY_HIRC_166();
	
	GPIO_INIT();	
	
	ADC_Set_Enable(1);
	
	lcd_soft_i2c_setup();
	delay_ms(10);
	lcd_soft_i2c_setup();
	delay_ms(10);
	lcd_soft_i2c_setup();
	delay_ms(10);
	lcd_soft_i2c_setup();
	delay_ms(10);
	lcd_soft_i2c_setup();
	delay_ms(10);
	//PWM0_INIT();

	TM1650_init();
	
	//68 6a 6c 6e
	TM1650_write(0x68,0x3F);
	TM1650_write(0x6a,0x3F);
	TM1650_write(0x6c,0x3F);
	TM1650_write(0x6e,0x3F);
	AP_EEPROM_Init((uint8_t *)&eData, sizeof(eData));
  Thresh_hole1 = eData.buff1;
	Thresh_hole2 = eData.buff2;
	AP_EEPROM_Commit();
	delay_ms(20);
	
	but_count[0]= Thresh_hole1/100;
	but_count[1]=(Thresh_hole1 / 10) % 10;
	but_count[2]= Thresh_hole1 % 10;
	
	but_count[3]= Thresh_hole2/100;
	but_count[4]=(Thresh_hole2 / 10) % 10;
	but_count[5]= Thresh_hole2 % 10;

	while (1) 
	{
		but=TM1650_read();
		BUTTON_check();
		if(set_mode==0)
		{
			ADC_check();
			GPIO_check();
			DISPLAY_check();		
			USER_CONDITION_check();
			tick++;	
			delay_ms(1);
		}
	}
}

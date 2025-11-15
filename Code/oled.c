//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ÖĞ¾°Ô°µç×Ó
//µêÆÌµØÖ·£ºhttp://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  ÎÄ ¼ş Ãû   : main.c
//  °æ ±¾ ºÅ   : v2.0
//  ×÷    Õß   : Evk123
//  Éú³ÉÈÕÆÚ   : 2014-0101
//  ×î½üĞŞ¸Ä   : 
//  ¹¦ÄÜÃèÊö   : 0.69´çOLED ½Ó¿ÚÑİÊ¾Àı³Ì(STM32F103ZEÏµÁĞIIC)
//              ËµÃ÷: 
//              ----------------------------------------------------------------
//              GND   µçÔ´µØ
//              VCC   ½Ó5V»ò3.3vµçÔ´
//              SCL   ½ÓPD6£¨SCL£©
//              SDA   ½ÓPD7£¨SDA£©            
//              ----------------------------------------------------------------
//Copyright(C) ÖĞ¾°Ô°µç×Ó2014/3/16
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////£

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
#include "iic.h"
//OLEDµÄÏÔ´æ
//´æ·Å¸ñÊ½ÈçÏÂ.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
/**********************************************
//IIC Start
**********************************************/
/**********************************************
//IIC Start
**********************************************/
void IIC_Start()	//·¢ËÍÆğÊ¼ĞÅºÅ
{
	iic_start();	
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()		//·¢ËÍÍ£Ö¹ĞÅºÅ
{
	iic_stop();
}

void IIC_Wait_Ack()		//µÈ´ı´ÓÉè±¸Ó¦´ğĞÅºÅ
{
	iic_recv_ack();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)		//Ïò IIC ×ÜÏß·¢ËÍÒ»¸ö×Ö½ÚÊı¾İ
{
	iic_send_byte(IIC_Byte);
}
/**********************************************
// IIC Write Command Ïò OLED ·¢ËÍÃüÁî£¨Í¨¹ı IIC ×ÜÏß£¬Éè±¸µØÖ· 0x78£¬ÃüÁî±êÖ¾ 0x00£©
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
	Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
	Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
	IIC_Stop();
}
/**********************************************
// IIC Write Data Ïò OLED ·¢ËÍÏÔÊ¾Êı¾İ£¨Éè±¸µØÖ· 0x78£¬Êı¾İ±êÖ¾ 0x40£©
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
	Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
	Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();	
	IIC_Stop();
}
void OLED_WR_Byte(unsigned dat,unsigned cmd)	//Ïò OLED Ğ´ÈëÃüÁî»òÊı¾İ
{
	if(cmd)
	{
		Write_IIC_Data(dat);
	}
	else 
	{
		Write_IIC_Command(dat);
	}


}


/********************************************
// fill_PictureÈ«ÆÁÌî³äÖ¸¶¨Êı¾İ£¨0 ÎªºÚ£¬1 ÎªÁÁ£©£¬ÓÃÓÚ¿ìËÙÇåÆÁ»òÈ«ÆÁÏÔÊ¾µ¥Ò»ÑÕÉ«¡£
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WR_Byte(fill_Data,1);
			}
	}
}


/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
	unsigned int m;
	for(;Del_50ms>0;Del_50ms--)
		for(m=6245;m>0;m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
	unsigned char j;
	while(Del_1ms--)
	{	
		for(j=0;j<123;j++);
	}
}

//ÉèÖÃÏÔÊ¾ÆğÊ¼×ø±ê¡£xÎªÁĞ£¨0~127£©£¬yÎªÒ³£¨0~7£¬OLED °´ "Ò³" Ñ°Ö·£¬Ã¿Ò³¶ÔÓ¦ 8 ĞĞÏñËØ£©
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);	//Ö¸¶¨ ¡°Ò³µØÖ·¡±£¨¼´ Y ·½ÏòµÄÆğÊ¼Ò³£©
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);	//Ö¸¶¨ ¡°ÁĞµØÖ·µÄ¸ß 4 Î»¡±£¨X ·½ÏòµÄ¸ß 4 Î»µØÖ·£©
	OLED_WR_Byte((x&0x0f),OLED_CMD); 	//Ö¸¶¨ ¡°ÁĞµØÖ·µÄµÍ 4 Î»¡±£¨X ·½ÏòµÄµÍ 4 Î»µØÖ·£©
}   	  
//¿ªÆôOLEDÏÔÊ¾    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDCÃüÁî
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//¹Ø±ÕOLEDÏÔÊ¾     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDCÃüÁî
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//ÇåÆÁº¯Êı,ÇåÍêÆÁ,Õû¸öÆÁÄ»ÊÇºÚÉ«µÄ!ºÍÃ»µãÁÁÒ»Ñù!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //ÉèÖÃÒ³µØÖ·£¨0~7£©
		OLED_WR_Byte (0x00,OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁĞµÍµØÖ·
		OLED_WR_Byte (0x10,OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁĞ¸ßµØÖ·   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //¸üĞÂÏÔÊ¾
}
void OLED_On(void)  //È«ÁÁ£¨½«ËùÓĞÏñËØÉèÎª 1£©
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //ÉèÖÃÒ³µØÖ·£¨0~7£©
		OLED_WR_Byte (0x00,OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁĞµÍµØÖ·
		OLED_WR_Byte (0x10,OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁĞ¸ßµØÖ·   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //¸üĞÂÏÔÊ¾
}
//ÔÚÖ¸¶¨Î»ÖÃÏÔÊ¾Ò»¸ö×Ö·û,°üÀ¨²¿·Ö×Ö·û
//x:0~127
//y:0~63
//mode:0,·´°×ÏÔÊ¾;1,Õı³£ÏÔÊ¾				 
//size:Ñ¡Ôñ×ÖÌå 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//µÃµ½Æ«ÒÆºóµÄÖµ			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
//¼ÆËãm^nº¯Êı
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//ÏÔÊ¾2¸öÊı×Ö
//x,y :Æğµã×ø±ê	 
//len :Êı×ÖµÄÎ»Êı
//size:×ÖÌå´óĞ¡
//mode:Ä£Ê½	0,Ìî³äÄ£Ê½;1,µş¼ÓÄ£Ê½
//num:ÊıÖµ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//ÏÔÊ¾Ò»¸ö×Ö·û´®
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}

//ÏÔÊ¾ºº×Ö
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{
    u8 t=0;
    if(x>Max_Column-1) {
        x=0;
        y=y+2;
    }

    OLED_Set_Pos(x,y);
    for(t=0; t<16; t++)
    {
        OLED_WR_Byte(Hzk[no].code[t],OLED_DATA);
    }

    OLED_Set_Pos(x,y+1);
    for(t=0; t<16; t++)
    {
        OLED_WR_Byte(Hzk[no].code[16+t],OLED_DATA);
    }
}

// ÅĞ¶ÏÒ»¸ö×Ö·ûÊÇ·ñÊÇÖĞÎÄ×Ö·û (GB2312)
int is_chinese_char_gb2312(const unsigned char *ch) {
    if((ch[0] >= 0xA1 && ch[0] <= 0xF7) && (ch[1] >= 0xA1 && ch[1] <= 0xFE))
        return 1;
    
    return 0;
}

// ÅĞ¶ÏÒ»¸ö×Ö·ûÊÇ·ñÊÇÓ¢ÎÄ×Ö·û
int is_english_char(const unsigned char ch) {
    return ch >= 0x00 && ch <= 0x7F;
}

// »ñÈ¡×Ö¿âÖĞµÄÖĞÎÄ×Ö·ûµÄË÷Òı
int get_chinese_index(const unsigned char *ch) {
    for( int i = 0; i < sizeof(Hzk)/sizeof(Hzk[0]); i++) {
        if(Hzk[i].name[0] == ch[0] && Hzk[i].name[1] == ch[1]) {
            return i;
        }
    }
    return -1;
}

//ÏÔÊ¾ÎÄ±¾
void OLED_ShowTest(u8 x,u8 y,char *str)
{
	unsigned char chinese_chars[2] = {0};
	uint8_t len = strlen(str);
	
	for(int index = 0; index < len; index++)
    {
        if( is_english_char(str[index]))
        {
            OLED_ShowChar( x, y, str[index], 16); 
            x += 8;
        }
        else if( is_chinese_char_gb2312((const unsigned char *)&str[index]))
        {
            chinese_chars[0] = str[index];
            chinese_chars[1] = str[index+1];
            int num = get_chinese_index(chinese_chars);
            if( num > -1)
			{
                OLED_ShowCHinese(x, y, num);
				index++;
				x += 16;
			}
        }
    }
}

/***********¹¦ÄÜÃèÊö£ºÏÔÊ¾ÏÔÊ¾BMPÍ¼Æ¬128¡Á64ÆğÊ¼µã×ø±ê(x,y),xµÄ·¶Î§0¡«127£¬yÎªÒ³µÄ·¶Î§0¡«7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 

//³õÊ¼»¯SSD1306					    
void OLED_Init(void)
{ 	
	iic_init();		//³õÊ¼»¯IIC½Ó¿Ú
	
	delay_ms(200);

	OLED_WR_Byte(0xAE,OLED_CMD);//--display off ¹Ø±ÕOLEDÏÔÊ¾
	
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address ÉèÖÃÁĞµÍµØÖ·
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address ÉèÖÃÁĞ¸ßµØÖ·
	
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  ÉèÖÃÏÔÊ¾ÆğÊ¼ĞĞ
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address ÉèÖÃÒ³µØÖ·
	
	OLED_WR_Byte(0x81,OLED_CMD); // contract control ÉèÖÃÏÔÊ¾¶Ô±È¶È£¨ÁÁ¶È£©
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   ¶Ô±È¶ÈÖµ
	
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap ÉèÖÃ¶Î£¨Segment£©ÖØÓ³Éä£¨¶Î¶ÔÓ¦ÁĞÇı¶¯Ïß£©
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse ÉèÖÃÏÔÊ¾Ä£Ê½£¨Õı³£ / ·´É«£©
	
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64) ÉèÖÃ¶àÂ·¸´ÓÃÂÊ£¨¿ØÖÆ COM Òı½ÅÊıÁ¿£¬COM ¶ÔÓ¦ĞĞÇı¶¯Ïß£©
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty ¸´ÓÃÂÊ
	
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction ÉèÖÃCOMÉ¨Ãè·½Ïò
	
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset ÉèÖÃÏÔÊ¾Æ«ÒÆ
	OLED_WR_Byte(0x00,OLED_CMD);//ÎŞÆ«ÒÆÁ¿
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division ÉèÖÃÕñµ´Æ÷·ÖÆµºÍÊ±ÖÓÆµÂÊ
	OLED_WR_Byte(0x80,OLED_CMD);//¸ß 4 Î»Îª·ÖÆµÏµÊı£¨0x8¡ú1 ·ÖÆµ£©£¬µÍ 4 Î»ÎªÊ±ÖÓÆµÂÊ£¨Ä¬ÈÏÍÆ¼öÖµ£©
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off ÉèÖÃÇøÓòÑÕÉ«Ä£Ê½
	OLED_WR_Byte(0x05,OLED_CMD);//0x05±íÊ¾¹Ø±ÕÇøÓòÑÕÉ«Ä£Ê½£¨µ¥É« OLED ÎŞĞèÑÕÉ«¿ØÖÆ£©
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period ÉèÖÃÔ¤³äµçÖÜÆÚ£¨COM Òı½Å³äµçÊ±¼ä£©
	OLED_WR_Byte(0xF1,OLED_CMD);//0xF1ÖĞ¸ß 4 Î»£¨0xF£©Îª ¡°Ô¤³äµçÇ°µÄÊ±¼ä¡±£¨15 ¸öÊ±ÖÓÖÜÆÚ£©£¬µÍ 4 Î»£¨0x1£©Îª ¡°Ô¤³äµçÊ±¼ä¡±£¨1 ¸öÊ±ÖÓÖÜÆÚ£©
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion ÉèÖÃ COM Òı½ÅÓ²¼şÅäÖÃ
	OLED_WR_Byte(0x12,OLED_CMD);//0x12±íÊ¾ COM Òı½ÅÎª ¡°½»ÌæÄ£Ê½¡±£¨ÊÊÓÃÓÚ 64 ĞĞ OLED£©£¬ÇÒ½ûÓÃ COM ×ó / ÓÒÖØÓ³Éä
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh ÉèÖÃ Vcomh£¨¹«¹²¶ËµçÑ¹£©
	OLED_WR_Byte(0x30,OLED_CMD);//0x30±íÊ¾ Vcomh µçÑ¹Îª0.83 ¡Á Vcc£¨Vcc ÎªµçÔ´µçÑ¹£©
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable ÉèÖÃµçºÉ±Ã£¨ÄÚ²¿ÉıÑ¹µçÂ·£©Ê¹ÄÜ
	OLED_WR_Byte(0x14,OLED_CMD);//0x14±íÊ¾¿ªÆôµçºÉ±Ã£»0x10±íÊ¾¹Ø±Õ
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel ¿ªÆô OLED ÏÔÊ¾
}  


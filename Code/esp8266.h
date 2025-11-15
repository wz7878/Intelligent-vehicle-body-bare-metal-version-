#ifndef __ESP8266_H__
#define __ESP8266_H__


#define EN_DEBUG_ESP8266	1

extern volatile uint8_t  g_esp8266_rx_buf[512];
extern volatile uint32_t g_esp8266_rx_cnt;
extern volatile uint32_t g_esp8266_rx_end;

extern volatile uint32_t g_esp8266_transparent_transmission_sta;

extern void 	esp8266_init(void);
extern int32_t  esp8266_self_test(void);
extern int32_t 	esp8266_exit_transparent_transmission (void);
extern int32_t 	esp8266_entry_transparent_transmission(void);
extern int32_t 	esp8266_connect_ap(char* ssid,char* pswd);
extern int32_t 	esp8266_connect_server(char* mode,char* ip,uint16_t port);
extern int32_t 	esp8266_disconnect_server(void);
extern int32_t  esp8266_disconnect_ap(void);
extern void 	esp8266_send_bytes(uint8_t *buf,uint32_t len);
extern void 	esp8266_send_str(char *buf);
extern void 	esp8266_send_at(char *str);
extern int32_t  esp8266_enable_multiple_id(uint32_t b);
extern int32_t 	esp8266_create_server(uint16_t port);
extern int32_t 	esp8266_close_server(uint16_t port);
extern int32_t  esp8266_create_ap(char* name,char* pass,uint8_t chl,uint8_t ecn);
extern int32_t  esp8266_find_str_in_rx_packet(char *str,uint32_t timeout);
extern int32_t  esp8266_reset(void);

#endif





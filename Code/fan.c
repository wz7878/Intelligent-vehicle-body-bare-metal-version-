#include "fan.h"

#define FAN_TIM               TIM4
#define FAN_TIM_CLK           RCC_APB1Periph_TIM4  // TIM4时钟在APB1总线上
#define FAN_GPIO_PORT         GPIOB                // 对应GPIO端口B
#define FAN_GPIO_CLK          RCC_AHB1Periph_GPIOB // GPIOB时钟
#define FAN_TIM_PIN           GPIO_Pin_6           // PB6引脚
#define FAN_TIM_AF            GPIO_AF_TIM4         // 复用为TIM4功能
#define FAN_TIM_CHANNEL       TIM_Channel_1        // 使用通道1

/**
 * @brief  初始化PWM定时器和GPIO
 * @param  arr: 自动重装载值
 * @param  psc: 预分频系数
 */
void Fan_PWM_Init(uint16_t arr, uint16_t psc) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 使能时钟（TIM4和GPIOB）
    RCC_APB1PeriphClockCmd(FAN_TIM_CLK, ENABLE);  // 使能TIM4时钟
    RCC_AHB1PeriphClockCmd(FAN_GPIO_CLK, ENABLE); // 使能GPIOB时钟

    // 配置GPIO为复用功能（PB6 → TIM4_CH1）
    GPIO_PinAFConfig(FAN_GPIO_PORT, GPIO_PinSource6, FAN_TIM_AF); // 引脚源6复用为TIM4
    GPIO_InitStructure.GPIO_Pin = FAN_TIM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用功能模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出（适合PWM）
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉（防止悬空）
    GPIO_Init(FAN_GPIO_PORT, &GPIO_InitStructure);

    // 初始化TIM4时基参数（PWM频率计算：频率 = 定时器时钟 / [(psc+1) * (arr+1)]）
    // STM32F407的APB1定时器时钟为84MHz（系统时钟168MHz时）
    TIM_TimeBaseStructure.TIM_Period = arr;           // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;        // 预分频系数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 时钟不分频
    TIM_TimeBaseInit(FAN_TIM, &TIM_TimeBaseStructure);

    // 配置PWM模式（通道1）
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // PWM模式1（CNT < CCR时输出有效电平）
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;     // 有效电平为高（风扇通常高电平有效）
    TIM_OCInitStructure.TIM_Pulse = 0;  // 显式设置初始比较值为0（占空比0%）
    TIM_OC1Init(FAN_TIM, &TIM_OCInitStructure);        // 初始化通道1

    // 使能预装载（确保PWM参数修改生效）
    TIM_OC1PreloadConfig(FAN_TIM, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(FAN_TIM, ENABLE);

    // 启动定时器
    TIM_Cmd(FAN_TIM, ENABLE);
}

/**
 * @brief  设置风扇PWM占空比
 * @param  duty: 占空比（0~1000 → 对应0%~100%）
 */
void Fan_SetDuty(uint16_t duty) {
    if (duty > 1000) duty = 1000;  // 限制最大值（防止超出范围）
    TIM_SetCompare1(FAN_TIM, duty); // 设置通道1的比较值（决定占空比）
}

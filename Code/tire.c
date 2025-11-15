#include "tire.h"

// 宏定义：ADC配置（更新为PA4 - ADC1_IN4）
#define PRESS_ADCx            ADC1                   // 选用ADC1
#define PRESS_ADC_CLK         RCC_APB2Periph_ADC1    // ADC1时钟（APB2总线）
#define PRESS_GPIO_PORT       GPIOA                  // PA4所属GPIO端口
#define PRESS_GPIO_CLK        RCC_AHB1Periph_GPIOA   // GPIOA时钟（AHB1总线）
#define PRESS_GPIO_PIN        GPIO_Pin_4             // 核心修改：引脚改为PA4
#define PRESS_ADC_CHANNEL     ADC_Channel_4          // 核心修改：ADC通道改为IN4

// 宏定义：ADC固定参数（12位分辨率，3.3V参考电压）
#define ADC_REF_VOLTAGE       3.3f    // STM32 ADC参考电压（默认3.3V）
#define ADC_RESOLUTION        4096.0f // 12位ADC最大值（2^12 = 4096）

// 胎压转换系数（来自文档3的电压-胎压关系式：y=177.83x-88.525）
#define PRESS_K1              177.83f // 关系式斜率
#define PRESS_B1              -88.525f// 关系式截距

/**
 * @brief  ADC初始化（适配PA4引脚，单端单次采集）
 */
void Press_ADC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;

    // 1. 使能GPIO和ADC时钟
    RCC_AHB1PeriphClockCmd(PRESS_GPIO_CLK, ENABLE);  // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(PRESS_ADC_CLK, ENABLE);   // 使能ADC1时钟

    // 2. 配置PA4为模拟输入（关键：模拟模式无上下拉，避免干扰）
    GPIO_InitStructure.GPIO_Pin = PRESS_GPIO_PIN;    // PA4引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;     // 模拟输入模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 无上下拉（模拟信号必须）
    GPIO_Init(PRESS_GPIO_PORT, &GPIO_InitStructure);

    // 3. 配置ADC通用参数（多ADC共享配置，单ADC也需初始化）
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式（仅用ADC1）
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // ADC时钟=84MHz/4=21MHz（≤36MHz）
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; // 禁用DMA（简单采集）
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; // 采样延迟
    ADC_CommonInit(&ADC_CommonInitStructure);

    // 4. 配置ADC1核心参数
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // 12位分辨率（精度足够）
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 禁用扫描模式（仅单通道）
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 禁用连续模式（单次触发单次采集）
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // 无外部触发（软件触发）
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 数据右对齐（方便计算）
    ADC_InitStructure.ADC_NbrOfConversion = 1; // 转换通道数：1（仅采集PA4）
    ADC_Init(PRESS_ADCx, &ADC_InitStructure);

    // 5. 配置ADC1的采集通道（PA4对应ADC1_IN4）
    ADC_RegularChannelConfig(
        PRESS_ADCx,          // ADC1
        PRESS_ADC_CHANNEL,   // 通道4（IN4）
        1,                   // 转换序列优先级（仅1通道，无影响）
        ADC_SampleTime_15Cycles // 采样时间15周期（平衡速度与精度）
    );

    // 6. 使能ADC1
    ADC_Cmd(PRESS_ADCx, ENABLE);
}

/**
 * @brief  读取ADC原始采样值（PA4的模拟信号）
 * @return uint16_t：ADC原始值（范围0~4095）
 */
uint16_t Press_ADC_ReadRaw(void) {
    ADC_SoftwareStartConv(PRESS_ADCx); // 软件触发ADC转换
    while (!ADC_GetFlagStatus(PRESS_ADCx, ADC_FLAG_EOC)); // 等待转换完成（EOC=转换结束标志）
    return ADC_GetConversionValue(PRESS_ADCx); // 返回转换结果
}

/**
 * @brief  计算传感器输出电压（从ADC值转换）
 * @return float：电压值（范围0~3.3V）
 */
float Press_GetVoltage(void) {
    uint16_t adc_raw = Press_ADC_ReadRaw();
    // 公式：电压 = (ADC原始值 / ADC最大值) * 参考电压
    return (adc_raw / ADC_RESOLUTION) * ADC_REF_VOLTAGE;
}

/**
 * @brief  计算胎压值（基于文档的电压-胎压关系式）
 * @return float：胎压值（单位kPa，范围0~350kPa，匹配传感器量程）
 */
float Press_GetTirePressure(void) {
    float voltage = Press_GetVoltage();
    // 文档3的关系式：胎压(kPa) = 177.83 * 电压(V) - 88.525
    float pressure = PRESS_K1 * voltage + PRESS_B1;

    // 过滤无效值（传感器量程参考文档1：最大350kPa，最小0kPa）
    if (pressure < 0.0f) pressure = 0.0f;
    if (pressure > 350.0f) pressure = 350.0f;

    return pressure;
}

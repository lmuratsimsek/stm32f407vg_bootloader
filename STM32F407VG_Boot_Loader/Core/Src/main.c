/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  *
  *  Created on: Jul 5, 2023
  *      Author: Murat Şimşek
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t txBootData[]="boot_loader jump to user app \r\n";
uint8_t bl_rx_buffer[BL_RX_LEN];

uint32_t mem_address;
uint32_t value;
uint32_t output_data;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**********************************************************************************
 *  Fonksiyon Adı: combine_uint8
 *  Açıklama: Bu fonksiyon, dört adet 8-bit tamsayıyı birleştirerek bir 32-bit tamsayı oluşturur.
 *
 *  Parametreler:
 *  - arr: 8-bit tamsayıları içeren bir dizi (4 elemanlı)
 *
 *  Dönüş Değeri: Oluşturulan 32-bit tamsayı değeri
 *
 *  Açıklama: Dört adet 8-bit tamsayıyı kullanarak bir 32-bit tamsayı oluşturur.
 *  İlk 8-bit tamsayıyı sonuçla OR işlemine tabi tutar.
 *  İkinci 8-bit tamsayıyı, 8 bit sola kaydırıp sonuçla OR işlemine tabi tutar.
 *  Üçüncü 8-bit tamsayıyı, 16 bit sola kaydırıp sonuçla OR işlemine tabi tutar.
 *  Dördüncü 8-bit tamsayıyı, 24 bit sola kaydırıp sonuçla OR işlemine tabi tutar.
 *  Oluşturulan 32-bit tamsayıyı döndürür.
 *
 **********************************************************************************/
uint32_t combine_uint8(uint8_t arr[4])
{
    uint32_t result = 0;

    result |= arr[0];
    result |= (uint32_t)arr[1] << 8;
    result |= (uint32_t)arr[2] << 16;
    result |= (uint32_t)arr[3] << 24;

    return result;
}

/**********************************************************************************
 *  Fonksiyon Adı: bootloader_uart_read_data
 *  Açıklama: Bu fonksiyon, bootloader tarafından UART üzerinden veri okur ve işlemler gerçekleştirir.
 *  - Yeşil LED'i tersler (HAZIR ve BOOT_LOAD konumunu gösterir).
 *
 *  Açıklama:
 *
 *  - YÜKLEME YAPMADAN ÖNCE SEKTÖR 2 TEMİZLENMELİDİR. (ST-LINK UTILITY)
 *
 *  - UART2 kanalını sürekli dinleyerek gelecek programı bekler.
 *  - Sonsuz bir döngü içinde POLL REQUEST modunda çalışır.
 *  - UART2 üzerinden BL_RX_LEN uzunluğunda veri alır. (15 Telemetri formatı.)
 *  - Alınan verinin başlangıç biti 0xA3 ise işlemlere devam eder. (Başlangıç Biti)
 *  - Veri 15 bitlik bl_rx_buffer değişkeninde tutulur.
 *  - Alınan verinin 14. indeksi 0x25 ise işlemlere devam eder. (Bitiş biti)
 *      - Mavi LED'i tersler. (Uygulama yüklenmeye başladı.)
 *      - Bellek adresini belirlemek için gerekli 4 adet veriyi (bl_rx_buffer[4] - bl_rx_buffer[7]) birleştirir.
 *      - Bellek adresini combine_uint8 fonksiyonu kullanarak 32-bit bir tamsayıya dönüştürür.
 *      - Bellek adresinin baytlarını uygun şekilde yerleştirerek output_data oluşturur.
 *      - Veri değerini belirlemek için gerekli 4 adet veriyi (bl_rx_buffer[10] - bl_rx_buffer[13]) birleştirir.
 *      - Veri değerini combine_uint8 fonksiyonu kullanarak 32-bit bir tamsayıya dönüştürür.
 *      - myVariable değişkenine 0x08000000 (SYSTEM_MEMORY_BASE) değerini atar.
 *      - myVariable ile value toplamını sum değişkenine atar.
 *      - FLASH belleğini kilitleyerek, sum adresine output_data'yı yazar.
 *    - Eğer alınan verinin başlangıç biti 0xA3 değilse, 0x55 değerini göndererek NACK (yanıt yok) gönderir.
 *      - bl_rx_buffer'ı sıfırlayarak gelen komutları dinler.
 *
 **********************************************************************************/
void bootloader_uart_read_data(void){
	erase_flash_sector();
	while(1){
		HAL_GPIO_TogglePin(greenLED_GPIO_Port, greenLED_Pin);
		HAL_UART_Receive(&huart2, (uint8_t*)&bl_rx_buffer, BL_RX_LEN, HAL_MAX_DELAY);

		if (bl_rx_buffer[0] == 0xA3){ // Başlangıç biti.
			HAL_GPIO_TogglePin(redLED_GPIO_Port, redLED_Pin); // HAZIR KONUMDADIR.
			if(bl_rx_buffer[14] == 0x25){
				HAL_GPIO_TogglePin(blueLED_GPIO_Port, blueLED_Pin);

				HAL_GPIO_TogglePin(redLED_GPIO_Port, redLED_Pin);
				//  02FC41EA
				uint8_t memory_address[4] = {
						bl_rx_buffer[4], bl_rx_buffer[5],bl_rx_buffer[6],bl_rx_buffer[7]
				};
				mem_address = combine_uint8(memory_address) ;
				uint32_t output_data = ((mem_address & 0xFF) << 24) | ((mem_address & 0xFF00) << 8) |
									  ((mem_address & 0xFF0000) >> 8) | ((mem_address >> 24) & 0xFF);

				// 0X08000220 - 0
				uint8_t data[4] = {
					   bl_rx_buffer[10], bl_rx_buffer[11],bl_rx_buffer[12],bl_rx_buffer[13]
				};

				value = combine_uint8(data);

				uint32_t myVariable = 0x08000000; // SYSTEM_MEMORY_BASE

				uint32_t sum = myVariable + value;
				HAL_FLASH_Unlock();
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,sum,output_data);
				HAL_FLASH_Lock();
			}
		}else{
			uint8_t dataNack = 0x55; // 'U' byte'i HOST bilgisayara gönderilir.
			HAL_UART_Transmit(&huart2, &dataNack, 1, HAL_MAX_DELAY); // Veri indexlemesinde problem var.
			memset(bl_rx_buffer,0,BL_RX_LEN); // HOST yeniden başlat, verileri temizle.
		}
	}
}
/**********************************************************************************
 *
 * @brief Flash bellek sektörünü silen işlev.
 * Bu işlev, mikrodenetleyici üzerindeki flash bellek sektörünü silmek için kullanılır.
 * İşlev, belirli bir flash sektörünü silme işlemi gerçekleştirir ve bu sektörün içindeki tüm verileri temizler.
 * İşlevin çalışması için flash bellek önceden kilitleme işlemi yapılır ve sektör silme işlemi tamamlandıktan sonra
 * flash bellek tekrar kilitleme işlemi gerçekleştirilir.
 * Açıklama: Bu işlevi kullanmadan önce, flash bellek kilidinin açılması ve kilidin kapatılması işlemleri yapılmalıdır.
 *
 **********************************************************************************/

void erase_flash_sector(void)
{
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();
    // Flash bellek sektörünü sil
    FLASH_Erase_Sector(FLASH_SECTOR_2, VOLTAGE_RANGE_3);

    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
}
/**********************************************************************************
 * -> Fonksiyon Adı: bootloader_jump_to_user_app()
 * -> Açıklama: BOOT esnasında, button kesmesi alınmadığı durumda; Kullanıcı uygulamasına atlama fonksiyonu.
 * Fonksiyon sektör 2 bölümüne atanan kullanıcı uygulamasının MSP adresini çekerek user_app MSP adresi ile değiştirilir.
 * Değiştirilen MSP adresi ile direkt olarak sektör 2 (0x8008000) adresindeki uygulama çağırılır.
 * -> Parametreler:
 * param1: void
 * Dönüş Değeri: (fonksiyonun dönüş değeri ve neyi temsil ettiğiyle ilgili açıklama)
 * -> Kullanılan kütüphaneler: __set_MSP (CMSIS)
 * -> Dikkat Edilmesi Gereken Noktalar:
 * STM32F407DISC1 kartına güç verildiğinde, blue-push button konumuna göre kullanıcı uygulamasına atlanır.
 * -> Örnek Kullanım: FLASH_SECTOR2_BASE_ADDRESS SECTOR2 üzerinde kayıtlı olan kullanıcı uygulamasına atla.
 * bootloader_jumpt_to_user_app(void);
 * -> Notlar: .hex dosyası içeriğinde, MSP, Reset Handler konumu
 * 0x8008000     = FLASH_SECTOR2_BASE_ADDRESS
 * 0x8008000 [1] = MSP (Main Stack Pointer)
 * 0x8008000 [2] = Reset Handler
**********************************************************************************/
void bootloader_jump_to_user_app(void){
	void (*app_reset_handler) (void); // Kullanıcı programının reset handler'ini tutan pointer fonksiyonu

	uint32_t msp_value = *(volatile uint32_t *) FLASH_SECTOR_2_BASE_ADDRESS; // 0x8008000(Flash Memory Sector-2 MSP reading) (İlk terim == MSP_VALUE)

	//SCB---VTOR == FLASH_SECTOR1_BASE_ADDRESS; // Taşıma vektör işlemleri yapıldı.

	__set_MSP(msp_value); // başlangıç MSP değeri, sektör 2ye atanan MSP değeri ile değiştirilir.

	uint32_t resethandler_address = *(volatile uint32_t *) (FLASH_SECTOR_2_BASE_ADDRESS + 4);//Kullanıcı uygulamasının reset_handler adresi çekilir. (FLASH_SECTOR2_BASE_ADDRESS + 4)

	 app_reset_handler = (void*) resethandler_address; // bootloader reset handler yerine kullanıcı uygulaması resethandler yerleştirilir.

	 app_reset_handler(); // Reset atılır, kullanıcı uygulaması başlatılır.

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /**********************************************************************************
   *  İlk güç verildiği esnada BlueButton_Pin durumu aranır. BlueButton basılı tutularak RESET butonuna basılırsa;
   *  bootloader_uart_read_data(); fonksiyonu çağırılır.
   *  Boot edilmesine izin verildiği durumda ise;
   *  bootloader_jump_to_user_app(); fonksiyonu çağırılır.
   **********************************************************************************/
  if (HAL_GPIO_ReadPin(BlueButton_GPIO_Port, BlueButton_Pin) != GPIO_PIN_RESET){ // User buttonuna basılı tutarak reset düğmesine basılırsa boot moduna girilir.
      bootloader_uart_read_data();
    }else{
  	  bootloader_jump_to_user_app(); // Başlangıçta herhangi bir işlem yapılmadı. 0x8008000 adresine git ve kullanıcı uygulamasını çalıştır.
    }
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/*
* @brief  System Clock Configuration
*         The system Clock is configured as follow :
*            System Clock source            = PLL HSE
*            HSE Frequency(Hz)              = 8000000
*            SYSCLK(Hz)                     = 16800000
*            AHB Prescaler                  = 1
*            APB1 Prescaler                 = 4
*            APB2 Prescaler                 = 2
*            PLL_M                          = 4
*            PLL_N                          = 168
*            PLL_P                          = 2
*            PLL_Q                          = 4
*            VDD(V)                         = 3.3
*            Main regulator output voltage  = Scale1 mode
*            Flash Latency(WS)              = 5 CYC
*			 APB1 P.C                       = 42 MHZ
*            APB2 P.C                       = 84 MHZ
*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

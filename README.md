Bu README dosyası, STM32F407VG_User_APP_BLINK_At_Sector_2 projesi hakkında bilgiler içermektedir. Bu projede, belirli aralıklarla yanıp sönen bir LED tanımlanmıştır. Uygulama, FLASH belleğinin SECTOR2 bölgesine yüklenmiştir.

Bellek Ataması
Projenin bellek ataması aşağıdaki adımlarla gerçekleştirilmiştir:

STM32F407VGTX_FLASH.ld dosyası açılarak aşağıdaki hafıza tanımlamaları bulunmuştur:

MEMORY
{
  CCMRAM    (xrw)    : ORIGIN = 0x10000000,   LENGTH = 64K
  RAM    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 128K
  FLASH    (rx)    : ORIGIN = 0x8000000,   LENGTH = 1024K
}

ORIGIN = 0x8000000 ifadesi ORIGIN = 0x8008000 olarak değiştirilmiştir.

Vektör Tablo Adresi
Projenin system_stm32f4xx.c dosyasında USER_VECT_TAB_ADDRESS tanımlanmıştır. Aşağıdaki değişiklik yapılmıştır:

#define VECT_TAB_OFFSET         0x00000000U
#define VECT_TAB_OFFSET         0x8000U

.hex Dosyaları

STM32CUBEIDE uygulamasıyla oluşturulan .hex dosyaları şunlardır:

all_blink_app.hex
blue_blink_app.hex
red_blink_app.hex
Bu dosyalar Boot_Loader_HOST klasörüne kopyalanmıştır.

Bootloader'a .hex Dosyalarını Yükleme

.hex dosyalarını bootloader üzerine yüklemek için aşağıdaki adımları takip edin:

Boot_Loader_HOST klasörüne gidin.
host_boot_loader.py dosyasını çalıştırın.
serial kütüphanesini indirin.
STM32F407VG cihazını boot moduna alın: Mavi butona basılı tutarken bir kez reset tuşuna basın.
Yeşil LED'in yanmasını gözlemleyin.
host_boot_loader.py dosyasını açın ve parsed_data = parse_hex_file("all_blink_app.hex") kodundaki "all_blink_app.hex" dosya adını istediğiniz .hex dosyasının adıyla değiştirin.
Kod visual studio ile yazıldığı için, dosya yolunu ayarlayınız.

Bu bilgiler, STM32F407VG_User_APP_BLINK_At_Sector_2 projesi hakkında genel bir anlatım sunmaktadır. Projeyle ilgili detaylı bilgilere ilgili dosyalardan ve proje belgelerinden ulaşabilirsiniz.

VIDEO_BootLoader_TEST dosyası içerisinde projenin çalışan halinin test videosu bulunmaktadır.

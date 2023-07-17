"""
Bu kod dosyası, seri haberleşme kullanarak bir HEX dosyasını işler ve haberleşme paketlerini hazırlar ve gönderir.
STM32F407VG cihazı paketleri karşılayarak adreslere verileri dağıtır ve bootload işlemi tamamlanır.

Yazar: Murat Şimşek
Tarih: 06.07.2023

Modüller:
- struct: Veri paketinin doğru şekilde paketlenmesi ve çözümlenmesi için kullanılır.
- serial: Seri haberleşme bağlantısı için kullanılır.
- time: Zaman aralıklarının kontrolü için kullanılır.

Değişkenler:
- port (str): Seri portun adı veya COM numarası
- hiz (int): Seri port hızı

Sınıflar:
- HaberlesmePaketi: Telemetri paketini oluşturan ve hazırlayan sınıf.

Fonksiyonlar:
- reverse_byte_order(data): Verilen verinin byte sırasını tersine çevirir.
- parse_hex_file(file_path): HEX dosyasını işleyerek verileri döndürür.
- pakethazirla(): HEX dosyasını işleyerek haberleşme paketlerini oluşturur ve gönderir.

"""

import struct

import serial
import time
port = "COM14"  # Seri portun adı veya COM numarası
hiz = 115200  # Seri port hızı
ser = serial.Serial(port, hiz) #Seri port açılır.
"""Class Açıklaması
class HaberlesmePaketi():
Telemetri Paketini değişkenlerde tutar.
Telemetri Paketini hazırlar.

Parametreler:
Başlangıç 		            1 Byte 	£ (b'xa3')
Emir numarası 		        2 Byte	uint16_t
Birim numarası(SlaveNo)     1 Byte	uint8_t
Emir Adresi 		        4 Byte	uint32_t
Read/Write Komutu	        1 Byte	uint8_t
Data tipi		            1 Byte  uint8_t
Data bloğu		            4 Byte	data
Bitiş			            1 Byte	% (b'x25')

Dönüş:
Paket bytearray döndürür. (Telemetri Paketi.)
"""
class HaberlesmePaketi:
    def __init__(self):
        self.baslangic = b'\xa3' 
        self.emir_numarasi = 0
        self.birim_numarasi = 3 #stm32f407vg
        self.emir_adresi = 0 
        self.read_write_komutu = 1 #write
        self.data_tipi = 1 #Char
        self.data_bloku = 0
    
        self.bitis = b'\x25'

    def paketi_hazirla(self):
        paket = self.baslangic
        paket += struct.pack('>H', self.emir_numarasi)
        paket += struct.pack('B', self.birim_numarasi)
        paket += struct.pack('>I', self.emir_adresi)
        paket += struct.pack('B', self.read_write_komutu)
        paket += struct.pack('B', self.data_tipi)
        paket += struct.pack('<I', self.data_bloku)
        paket += self.bitis
        return paket
 
def reverse_byte_order(data):
    """
    Verilen verinin byte sırasını tersine çevirir.
    Args:
        data (str): Tersine çevrilecek veri, 8 haneli hexadecimal formatta.
    Returns:
        int: Byte sırası tersine çevrilen veri, 32 bit unsigned integer olarak.
    Example:
        >>> reverse_byte_order('0xABCDEF12')
        3054198962
    """
    # Veriyi 32 bit unsigned integer olarak alıyoruz
    value = int(data, 16)
    
    # Veriyi 32 bitlik byte dizisine dönüştürüyoruz
    byte_array = value.to_bytes(4, byteorder='big', signed=False)
    
    # Byte dizisini tersine çeviriyoruz
    reversed_bytes = byte_array[::-1]
    
    # Tersine çevrilen byte dizisini 32 bit unsigned integer olarak dönüştürüyoruz
    reversed_value = int.from_bytes(reversed_bytes, byteorder='big', signed=False)
    
    # Sonucu 8 haneli hexadecimal formatta döndürüyoruz
    #return f'0x{reversed_value:08X}'
    return reversed_value

def parse_hex_file(file_path):
    """
    HEX dosyasını işleyerek verileri döndürür.

    Args:
        file_path (str): İşlenecek HEX dosyasının dosya yolu.

    Returns:
        list: Veri öğelerinin listesi. Her bir öğe, (veri, adres) çiftini içeren bir tuple.

    Example:
        >>> parse_hex_file('example.hex')
        [(3054198962, 1024), (2576980378, 1028), (3543348010, 1032)]
    """
    data = []
    
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
            if line.startswith(':'):
                
                byte_count = int(line[1:3], 16)
                address = int(line[3:7], 16)
               
                record_type = int(line[7:9], 16)

                if record_type == 0x00:  # Data record
                    for i in range(byte_count // 4):  # 4 bytes for each address
                        data_word = int(line[9 + i*8:17 + i*8], 16)
                        data_word =  reverse_byte_order (hex(data_word))
                        data.append((data_word, address + i*4)) 
    
    return data

def pakethazirla():
    """
    HEX dosyasını işleyerek haberleşme paketlerini oluşturur ve gönderir.

    Bu fonksiyon, "all_blink_app.hex / red_blink_app.hex / blue_blink_app.hex" dosyasını kullanarak haberleşme paketlerini hazırlar ve gönderir.
    Her bir paket için ilgili bilgileri alır, paketi hazırlar ve gönderir. Gönderilen paket sayısı ve kalan paket sayısı
    sürekli olarak ekrana yazdırılır. Seri haberleşme bağlantısında veri alınmışsa okunan veri ekrana yazdırılır.
    Eğer seri haberleşme bağlantısında bir hata oluşursa program yeniden başlatılır.

    Returns:
        None

    Example:
        >>> pakethazirla()
        Gönderilen Paket:  1 Kalan Paket:  2 Toplam Paket: 3
        Okunan veri: b'A'
        Gönderilen Paket:  2 Kalan Paket:  1 Toplam Paket: 3
        Gönderilen Paket:  3 Kalan Paket:  0 Toplam Paket: 3
    """
    parsed_data = parse_hex_file("blue_blink_app.hex")
    haberlesme_paketi = HaberlesmePaketi()
    bozuk_veri = []
    for i in range(0,len(parsed_data)):
        haberlesme_paketi = HaberlesmePaketi()
        haberlesme_paketi.emir_numarasi = int(i)
        haberlesme_paketi.emir_adresi = parsed_data[i][0]
        haberlesme_paketi.data_bloku = parsed_data[i][1]
        data = haberlesme_paketi.paketi_hazirla()
        ser.write(data)
        print (data)
        print("Gönderilen Paket: " , i+1 , "Kalan Paket: ", len(parsed_data) - i -1 , "Toplam Paket:", len(parsed_data))
        if ser.in_waiting > 0:
            # Veri varsa oku
            okunan_veri = ser.read(1)
            if (okunan_veri) == b'':
               i = (i - 1)
               time.sleep(2)
               raise SystemExit("Restarting program")
            
            print("Okunan veri:", okunan_veri)
        else:
            # Veri yoksa devam et
            continue

#Başlangıç
pakethazirla()

/* 
 * File:   main.c
 * Author: mehmet_dora
 *
 * Created on January 31, 2026, 8:42 PM
 */

#include <stdio.h>
#include <stdlib.h>




#include <xc.h>


// Bu ayarlar PIC'e enerji verildiği an nas?l davranaca??n? belirler.
// SONRA DETAYLI BAKILACAK
#pragma config FOSC = INTRC_NOCLKOUT // Osilatˆr: Dahili Osilatˆr kullan komutu (Harici kristal yok)
#pragma config WDTE = OFF            // Watchdog Timer: KAPALI (AÁ?k olursa i?lemci s¸rekli reset atar)
#pragma config PWRTE = OFF           // Power-up Timer: Kapal?
#pragma config MCLRE = ON            // Master Clear: Reset pini aktif
#pragma config CP = OFF              // Code Protection: Kapal?
#pragma config CPD = OFF             // Data Code Protection: Kapal?
#pragma config BOREN = ON            // Brown-out Reset: AÁ?k
#pragma config IESO = OFF            // Internal/External Switchover: Kapal?
#pragma config FCMEN = OFF           // Fail-Safe Clock Monitor: Kapal?
#pragma config LVP = OFF             // Low Voltage Programming: KAPALI (RB3 pinini kullanabilmek iÁin kapal? olmal?)



#define _XTAL_FREQ 4000000


volatile unsigned int ms_tick = 0;  // Zaman sayan global değişken

void __interrupt() isr(void){
    
    if(INTCONbits.T0IF){        // Timer0 her taşmasında flag 1 olur , yani 250 prescaler(1 ms) geçmiş olur
        INTCONbits.T0IF = 0;    // Tekrar flag i reset et sonraki Timer0 taşması için
        TMR0 = 6;               // Timer0 clock değerini tekrar 6 dan başlat
        ms_tick++;              // global volatile olan değişkene 1 ms ekle
    }
}




int main(int argc, char** argv) {

 

    // OSCCILATOR AYARLARI
    OSCCONbits.IRCF = 0b110; // Bit 6-4: 110 yaparak 4 MHz seçildi
    OSCCONbits.SCS = 1;      // kendi osilatörünü kullansın
    
    
    // TIMER AYARLARI
    OPTION_REGbits.T0CS = 0;    // 0 : dahili clock, 1: harici clock 
    OPTION_REGbits.PSA = 0;     // 0: Timer0, 1: Watchdog
    OPTION_REGbits.PS = 0b001;  // Prescaler oranı seçimi
    
    // Timer0 8 bit(256 değer) tutabildiği için ve 1000/4 = 250 olduğundan tam 1 ms lik ölçüm için clock sayacını 6 dan başlat
    TMR0 = 6;   
    
    INTCONbits.T0IF = 0;        // Timer0 flag resetleme
    INTCONbits.T0IE = 1;        // Timer0 interrupt enable yap
    INTCONbits.GIE = 1;         // Global interrupt enable yap
    
    // Timer0 Çalışma Mantığı Hakkında
    /*
    
     *  MCU 4 MHz ile çalışıyorsa instruction clock 1 MHz ile çalışır. Yani 1 instruciton 1 mikrosaniye.
     *  Bu mikrosaniyeler sayılarak zaman ölçülür ama tek tek sayacı her bir instruction clock için saymak maliyetlidir.
     *  Bunun yerine prescaler ile mesela 4 inctruction clock da bir sayaç arttırılabilir. Bu sayede 1 ms için 250 sayaç gerekir
     *  Timer0 8 bit olduğu için 256 değer tutabilir , Her bir timer0 ile 1 ms yi temsil etmek için de Interrupt fonksiyonunda 
     *  TMR0 başlangıç değeri 6 ile başlatılır ki 250 değer saysın. Bu sayede her Timer0 256 değerini aştığında T0IF flag i 1 olur,
     *  bu sayede 1 ms geçtiği anlaşılır ve global volatile olan zaman sayacı değişkeni 1 arttırılır. 
     * 
     * Bu yapı ile Timer0 arka planda ilk çalışmadan itibaren saymaya devam eder, bu sayacın değerleri kullanılarak zamansal işlemler
     * CPU yu kitlemeden yapılabilir. Diğer timer türlerine sonra bakılacak. 
     
    */
    
    
    
    
    
    
    
    
    
    
    // Tüm pinleri dijital olarak başlat
    ANSEL = 0;
    ANSELH = 0;
    
    
    // Butondan veri okumak için butonun bağlandığı pini input olarak ayarlanmalıdır
    TRISB = 0x00;   // hepsi output
    PORTB = 0x00;   // Tüm bitler kapalı
    TRISC = 0x40;
    
    
    // pull-up / pull-down konusu
    // bir pin input yapıldığında boşta iken her zaman 0 yada 1 olmaz, parazitli olur
    // bunu engellemek için devreye bağlantı şekline göre direç bağlanır , bu sayede net bir sonuç elde edilir.
    // bu özellik sadece B portunda dahili vardır, aşağıda bunun ayarı açılmıştır, diğer pinler için devre buna göre düzenlenmelidir.
    // OPTION_REGbits.nRBPU = 0;
     

    
    
    
    // LED yakmak için beklemeden butona basılıp çekilince yanar ,
    // butona led kapalı iken 2 sn boyunca basılıp çekilirse led söner.
    
    // TIMER KULLANIMI
    unsigned int ms_counter = 0;    // butona basıldığı andan itibaren geçen sürenin tutulacağı değişken
    char button_pressed = 0;        // butona basılıp basılmadığı flag i

    while(1){
        
            if(RC6 == 1 && button_pressed == 0){        // butona basıldıysa zamanı kaydet ve flag i aç    
                button_pressed = 1;
                ms_counter = ms_tick;   // zamanı ayarla
            }
            
            
            if(RC6 == 0 && button_pressed == 1){        // buton kalkınca ve öncesinde butona basılmışsa sonuç olarak yapılacak işlemleri yap
                
                button_pressed = 0;     // buton durumunu sıfırla
                
                if(((ms_tick - ms_counter) > 2000) && RB2 == 1){    // eğer uzun basılmışsa ve led açıksa ledi kapat
                    RB2 = 0;
                }else{                                              // değilde aç
                    RB2 = 1;
                }
            }

    }
    
    

    
    
    return (EXIT_SUCCESS);
}



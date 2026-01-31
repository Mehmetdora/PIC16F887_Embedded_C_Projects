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


int main(int argc, char** argv) {

 

    OSCCONbits.IRCF = 0b110; // Bit 6-4: 110 yaparak 4 MHz seçildi
    OSCCONbits.SCS = 1;      // kendi osilatörünü kullansın
    
    
    // Tüm pinleri dijital olarak başlat
    ANSEL = 0;
    ANSELH = 0;
    
    
    // Butondan veri okumak için butonun bağlandığı pini input olarak ayarlanmalıdır
    TRISB = 0x03;   // B0 ve B1 pinleri input(dışarıdan veri okur, dışarıya sinyal vermez), diğer tüm B pinleri output olur
    PORTB = 0x00;   // Tüm bitlere güç verme 
    
    
    
    // pull-up / pull-down konusu
    // bir pin input yapıldığında boşta iken her zaman 0 yada 1 olmaz, parazitli olur
    // bunu engellemek için devreye bağlantı şekline göre direç bağlanır , bu sayede net bir sonuç elde edilir.
    // bu özellik sadece B portunda dahili vardır, aşağıda bunun ayarı açılmıştır, diğer pinler için devre buna göre düzenlenmelidir.
    // OPTION_REGbits.nRBPU = 0;
    
    
    // devrenin B2 pininde led bağlanmıştır
    
    
    // led B0 a bağlı buton ile açılır, B1 ile kapatılır
    while(0){
        
        if(RC6 == 1){
            PORTBbits.RB2 = 1;  // led yak
        }
        
        if(RC7 == 1){
            PORTBbits.RB2 = 0;  // led kapa
        }
    }
    
    
    
    // LED B0 a bağlı butona basılıp çekilince yanar, tekrar basılıp çekilince söner. Butona basma işlemi sonunda led durumu değişir.
    while(0){
         
        if(RC6 == 1){     // eğer butona basılırsa ve led kapalı ise yak
            
            __delay_ms(20);     // buton kısa bir süreliğine tam net olmaz , bu parazitin bitmesini bekle
            if(RC6 == 1){
             
                while(RC6 == 1);        // Basılı olduğu sürece hiçbir şey yapılmasın
                                        // basılı olduğu süre boyunca burada bekle
                
                if(RC6 == 0){   // buton kapandıktan sonra led durumunu değiştir
                    PORTBbits.RB2 = !PORTBbits.RB2;  // led açıksa kapa, kapalı ise aç
                }   
                
            }
        }
    }
    
    
    
    
    // LED yakmak için beklemeden butona basılıp çekilince yanar ,
    // butona led kapalı iken 2 sn boyunca basılıp çekilirse led söner.
    
    // Burada kullanılan delay mantığı doğru değildir, timer kullanılmalı
    int ms_counter = 0;
    while(0){
        if(RC6 == 1){     // eğer butona basılırsa ve led kapalı ise yak
            
            __delay_ms(20);     // buton kısa bir süreliğine tam net olmaz , bu parazitin bitmesini bekle
            if(RC6 == 1){
             
                while(RC6 == 1){    // butona basılı olan süreyi değişkene kaydet
                    __delay_ms(100);
                    ms_counter += 100;
                }
                
                if(RC6 == 0 && RB2 == 0){   // eğer led kapalı ise ve butona basılıp çekilmişse led yanar
                    RB2 = 1;
                }
                
                // eğer led açık ise, butona basılıp çekilmişse ve 2 sn boyunca butona basılı tutulmuşsa led söner
                if(RC6 == 0 && RB2 == 1 && ms_counter > 2000){
                    RB2 = 0;
                }
                
                ms_counter = 0;

                   
            }
        }
    }
    
    
    
    
    // 2 buton ile 8 led sağa ve sola yakılarak yer değiştirmesi örneği
    TRISB = 0x00;   // Tüm b pinleri dışarı güç versin
    TRISC = 0xC0;
    PORTB = 0x01;   // başlangıçta B0 pini çalışsın
    while(1){
        
        if(RC6 == 1){
            __delay_ms(20);
            if(RC6 == 1){
                while(RC6 == 1);
                if(RB0 == 1){   // Eğer ilk led yanarken sol butonuna basılırsa son led yansın
                    PORTB <<= 7;
                }else{
                    PORTB >>= 1; // eğer ilk ledden farklı bir led yanıyorsa ledi bir sola kaydır, önceki pini aç
                }
            }
        }
        
        
        if(RC7 == 1){
            __delay_ms(20);
            if(RC7 == 1){
                while(RC7 == 1);
                
                if(RB7 == 1){   // Eğer ilk led yanarken sol butonuna basılırsa son led yansın
                    PORTB >>= 7;
                }else{
                    PORTB <<= 1; // eğer ilk ledden farklı bir led yanıyorsa ledi bir sola kaydır, önceki pini aç
                }
            }
        }
    }
    
    
    
    return (EXIT_SUCCESS);
}



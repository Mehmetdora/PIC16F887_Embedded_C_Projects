/* 
 * File:   main.c
 * Author: mehmet_dora
 *
 * Created on January 30, 2026, 10:38 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include <xc.h> // Ana pic dosyaları


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




// delay methodlar?n?n kullan?lmas? iÁin ˆnceden bu macro tan?mlanmak zorunda, de?eri MCU de?erine gˆre do?ru belirlenmeli
#define _XTAL_FREQ 4000000  // 4 milyon hz, 4Mhz
// E?er harici olarak bir osilatˆr ba?lanm??sa bunun ¸zerinde yazan de?ere gˆre bu de?er belirlenmelidir


#include <pic16f887.h>



int main(void) {
    
    
    OSCCONbits.IRCF = 0b110; // Bit 6-4: 110 yaparak 4 MHz seçildi
    OSCCONbits.SCS = 1;      // kendi osilatörünü kullansın
    
    
    // ANSEL = 0;
    ANSELH = 0;     // B0 pini için AN12 , yani ANSELH dir
    
    TRISB = 0x00;    // B pinlerini çıkış ayarla
    PORTB = 0x00;    // B pinlerine güç verilmesin
    
    unsigned char ports = 1;    // 8 bit , ilk biti 1 olan değişken, pinleri temsil etmesi için 
    
    
    // 1. PİNDEN 7 PİNE GİDEN VE BUNU SÜREKLİ YAPAN VERSİYON
    while(0){                   
        
        PORTB = ports;     // B0 ile başlayarak yakmaya başlansın
        __delay_ms(1000);       // 1000 ms bekle
        
        ports <<= 1;    // ports ile gelen bitleri 1 bit sola(sonraki pine) kaydır ve yine ports a eşitle
        if(ports == 0x00){      // eğer ports(pin) (10000000) B7 olmuşsa başa döndür(B1)
            ports |= 1;
        }
    }
    
    
    
    // 1. PİNDEN 7. PİNE VE TEKRAR 1. PİNE GERİ DÖNEN VE BUNU SÜRELİ YAPAN VERSİYON
    while(1){
        // ilk bitten başlayarak son bite kadar yak
        while(ports != 0x00){
            PORTB = ports;     // B0 ile başlayarak yakmaya başlansın
            __delay_ms(1000);       // 1000 ms bekle

            ports <<= 1; 
        }
        ports = 0x40;           // ports artık sıfırlanınca geriye doğru sayması için yeni değeri veriliyor  
        
        
        // son bitten başlayarak ilk bite doğru yak
        while(ports != 0x00){
            PORTB = ports;     // B0 ile başlayarak yakmaya başlansın
            __delay_ms(1000);       // 1000 ms bekle

            ports >>= 1; 
        }
        ports = 2;              // ports sıfırlanınca ileri doğru sayması için yeni değeri veriliyor
        
    }
    
    

    return (EXIT_SUCCESS);
}


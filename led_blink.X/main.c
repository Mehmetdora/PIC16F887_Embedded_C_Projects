

#include <stdio.h>
#include <stdlib.h>
// mehmet_dora


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

#include <xc.h>



// delay methodlar?n?n kullan?lmas? iÁin ˆnceden bu macro tan?mlanmak zorunda, de?eri MCU de?erine gˆre do?ru belirlenmeli
#define _XTAL_FREQ 4000000  // 4 milyon hz, 4Mhz
// E?er harici olarak bir osilatˆr ba?lanm??sa bunun ¸zerinde yazan de?ere gˆre bu de?er belirlenmelidir


#include <pic16f887.h>





// #pragma config FOSC = INTRC_NOCLKOUT     //  hangi osilatˆr kayna??n?n seÁilece?ini belirlemek iÁin yap?l?yor bu ayar
// Bu ayar pic aÁ?ld???nda internal osilatˆr kullan demektir, bunu ayarlamak gerekir e?er harici bir osilatˆr kullan?lacaksa


// OSCCONbits.IRCF = 0b110; // 4 MHz   bu de?erin bit olarak do?ru de?eri datasheet iÁinden kontrol edilmelidir 
// bu ayar ise as?l Áal??ma frekans?n? belirler, donan?m iÁin yap?lan bir ayard?r, delay in gerÁek s¸resini belirler



// #define _XTAL_FREQ 4000000
// bu de?er ise derleyicinin Áal??mas? s?ras?ndaki hesaplamalar iÁindir, Áip ayaland?ktan sonra Áipin bu ayarda 
// Áal??t???n? derleyiciye sˆyleyerek ayn? ayarda kodlar? yazmak iÁin kullan?l?r. 


// delay methodu kullan?l?rken asl?nda Áip belli bir tur say?s? kadar bo? i?lem yapar
// delay(500) iÁin ?u hesaplan?r, 500 ms kaÁ instruction eder ?

/*
 Pic de zaman diye bir kavram yoktur, clock vard?r bu darbe ile instruction Áal???r ve bir s¸re geÁer , 
 * bu insruction say?s? kadar s¸re geÁmi? olur ve pic belli bir s¸re kadar bekletilmi? olur
 * 
 * 
 * PIC 16 iÁin 1 instruction = 4 clock cycle demektir. 
 * instruction freqency = FOSC / 4 ---> 4 MHz / 4 = 1 MHz frequency , 
 * yani 1 instruction = 1/1.000.000 hz = 1 mikrosaniye olur .
 * 
 * 
 * Yani pic sadece istene say?da instruction ? Áal??t?r?r, bu s¸re boyunca sleep olmu? olur
 * 
 * SonuÁ olarak;
 * config -> hangi osilatˆr kullan?laca??
 * OSCCON -> pic in hangi MHz de Áal??aca??n?n ayarlanmas?
 * _XTAL  -> derleyicinin Áal??ma s?ras?nda kaÁ MHz ile Áal??t?r?laca?? 
 * 
 * Do?ru bir zaman ayar? iÁin OSCCON == _XTAL e?it olmal?d?r. 
 * 
 */



// PIC'in baz? pinleri (bacaklar?) Áift yeteneklidir. Hem "Dijital" (0-1) hem de "Analog" (Voltaj de?i?imini ˆlÁen) olabilirler.
// ANSEL: D¸?¸k numaral? analog kanallar? (AN0 - AN7) kontrol eder.
// ANSELH (High): Y¸ksek numaral? analog kanallar? (AN8 - AN13) kontrol eder.

// istenen bir pinin de?erini ansel olarak de?i?tirmek iÁin data sheet ¸zerinden pinlerin yan?nda yazan AN numaralar?na bakarak 
// istenen pinin analog mu dijital mi olaca??n? belirlenir 

// bu ayar t¸m portlar iÁin kullanlabilir de?ildir, baz?lar? sadece dijitaldir , analog desteklemez, data sheet e bak



int main(int argc, char** argv) {
    
    
    
    
    OSCCONbits.IRCF = 0b110; // Bit 6-4: 110 yaparak 4 MHz seÁtik
    OSCCONbits.SCS = 1;
    
    
    // ANSEL = 0;
    ANSELHbits.ANS12 = 0;     // B0 pini iÁin AN12 , yani ANSELH d?r
    
    
    __delay_ms(1000);   // 1000 ms bekletmek     
    
    
    // TRIS register ? ilgili pinlerin d??ar?ya g¸Á m¸ verecek yoksa d??ar?dan de?er mi okuyaca??na karar verir
    
    //TRISB = 0b00000001;     // sadece B0 pinini input yap , gerisi output olsun
    //TRISB = 0x01;
    //TRISBbits.TRISB0 = 1;   // direkt pine eri?erek de?erini de?i?tirme
    
    
    //TRISB &= (0b11000000);  // sadece B7 ve B6 pinlerini input yap , gerisi output olsun demek
    
    //TRISB0 = 0;     // direkt ilgili Tris pinine eri?im 

    
    
    //PORT register ? ilgili pinlerin g¸c¸n¸ belirler, g¸Á verilsin mi yoksa verilmesin mi 
    //PORTB = 0b00000001;     // B0 pinine g¸Á ver sadece
    //PORTB &= 0b11000000;    // B7 ve B6 pinine g¸Á ver sadece
    
    //PORTBbits.RB1 = 1;      // Sadece B1 pinine g¸Á verilsin
    
    
    
    // Bu ?ekilde pinleri kontrol etmeye polling denir , tam olarak do?ru bir yˆntem de?ildir
    //if (RB0 == 1){          // B0 pini high ise ...
        // i?lem yap 
    //}
    
    TRISBbits.TRISB0 = 0x00;    // B0 pinini 0 yap(output mode)
    PORTBbits.RB0 = 0;          // Ba?lang?Áta B0 pinine g¸Á verilmesin
    
    while(1){                   // S¸rekli Áal??acak bir dˆng¸ ba?latmak
        
        PORTBbits.RB0 = 1;      // B0 pinine g¸Á verilir
        __delay_ms(1000);       // 1000 ms bekletme-pic i oyalamak
        
        PORTBbits.RB0 = 0;      // B0 pininin g¸c¸n¸ kes - kapat
        __delay_ms(1000);
        
    }
    
    
    return (EXIT_SUCCESS);
}


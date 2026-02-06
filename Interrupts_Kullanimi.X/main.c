/*
 * File:   main.c
 * Author: mehmet_dora
 *
 * Created on February 6, 2026, 11:17 AM
 */


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


// FOSC/4 için clock sinyali frekansı , pic için de aynı değer seçilmelidir.
#define _XTAL_FREQ 4000000



volatile unsigned int timer0 = 0;   // global ms sayacı
volatile unsigned int timer1 = 0;   // global sn sayacı
volatile unsigned char timer1_overflow_counter = 0;     // timer1 ile 1 sn saymak için gereken overflow counter 


/*
 
 Aşağıdaki interrupt örneğinde timer kullanarak 1 sn aralıklar ile led blink örneği yapılmıştır
 Bu örnekte bir while loop içinde blocking delay kullanmadan(CPU durdurulmadan) LED bir sn aralıklarla kontrol ediliyor.
 Aynı zamanda yine while loop içinde sürekli butona basılma aksiyonu kontrol etmek yerine external interrupt kullanılarak 
 gerçek zamanlı buton kontrolü yapılabiliyor. Böylece buton tepkisinin ölçülmesi sırasında herhangi bir gecikme gerçekleşmez. 
 
 Ayrıca ISR() içinde olabildiğince az kod eklenmelidir. Mesela while loop içindeki LED kontrolü interrupt içinde de yapılabilir, 
 her timer1 arttırıldığında LED durumu değitirilebilirdi. Fakat bu tür interrupt içindeki işlemler gecikmelere neden olabileceği 
 için olabildiğince bu tarz işlemler main() içinde yapılmalı, interrupt içinde sade bir yapı oluşturulmalıdır. 
 * Aslında buton kontrolüde main() içerisinde yapılabilir. 
 
  
 Ek olarak interrupt kullanımlarında global olarak interrupt içinde değiştirilecek olan ve main() içerisinde kullanılacak olan
 değişkenlerin tanımlanması sırasında "volatile" anahtar kelimesi kullanılmalıdır. Bu kullanım kodların çalıştırılması sırasında 
 değişkenin değerinin her çalışmasında RAM içerisindeki değerine bakılarak kullanılmasını emreder. Yani aslında main() içinde bu 
 değişken değişmediği için compiler bu değişkenin değerini sabitmiş gibi görüp hatalı bir kullanıma sebep olur. Compiler, "bu değişken
 zaten değişmiyor, RAM e bakmadan hızlıca kodları çalıştırayım" der. Bunu engellemek için
 volatile ile değişken tanımlanmalıdır! 
 
 Ek olarak aynı anda birden fazla interrupt tetiklenmiş olabilir , bu durumda pic17f887 için interrupt önceliğini belirleyen bir
 yapı bulunmaz. Bu nedenle if statement'larının sıralaması yapılacak olan uygulamaya göre sıralanmalıdır.
 
 
 */

// Tüm interrupt'ların giriş fonksiyonu, hepsi buradan kontrol edilir.
void __interrupt() ISR(void){
    
    // Tek bir interrupt girişi olduğundan her biri if statement ile flag durumu kontrol edilir
    

    
    if(INTCONbits.TMR0IF && INTCONbits.TMR0IE){ // timer0 için flag ve enable kontrolü
        
        INTCONbits.TMR0IF = 0;                  // Sonraki flag kontrolü için flag i reset et
        TMR0 = 6;                               // Timer0 sayacının tam 250 değer sayabilmesi için 6 dan başlat
        timer0++;
        
    }
    
    if(PIR1bits.TMR1IF && PIE1bits.TMR1IE){     // timer1 için flag ve enable kontrolü
        
        
        PIR1bits.TMR1IF = 0;
        timer1_overflow_counter++;
        
        TMR1H = 3036 >> 8;
        TMR1L = 3036 & 0xFF;
        
        if(timer1_overflow_counter == 2){
            timer1_overflow_counter = 0;
            timer1++;
        }
    }
    
    
    
    if(INTCONbits.INTF && INTCONbits.INTE){ // Bu interrupt türü sadece external interrupt pini(B0 pini) tetiklendiğinde çalışır
        
        /*
         Bu kod sadece B0(external interrupt pini) tetiklendiğinde çalışacaktır, 
         Butona basılması ile flag set edilir ve bu if statement içideki kodlar execute edilir. 
          
         Tekrar butona basılma durumunu kontrol edilebilmesi için bu flag tekrar reset edilmelidir!
         */
        
        RD1 = ~RD1;
        INTCONbits.INTF = 0;
        
    }
    
    
    // diğer interrupt'lar burada aynı şekilde kullanılabilir...
}


void init_timer1(void);
void init_timer0(void);
void init_interrupt_button(void);

void main(void) {
    
    
    // OSCCILATOR AYARLARI
    OSCCONbits.IRCF = 0b110; // MCU nun çalışma hızı 4 MHz seçildi
    OSCCONbits.SCS = 1;      // kendi(internal) osilatörünü kullansın
    
    
    // Tüm pinleri dijital olarak başlat
    ANSEL = 0;
    ANSELH = 0;
    
    
    // Timer Ayarları
    init_timer0();
    init_timer1();
    init_interrupt_button();
    
    
    
        
    TRISB = 0x01;
    PORTB = 0x00;
    
    TRISD = 0x00;
    PORTD = 0x00;
    
    
    TRISC = 0x00;
    PORTC = 0x01;
    
    unsigned int timer1_copy = 1;
    while(1){
        
        if(RC7 == 0 && (timer1 - timer1_copy) >= 1){
            RC7 = 1;
            timer1_copy = timer1;
        }
        
        if(RC7 == 1 && (timer1 - timer1_copy) >= 1){
            RC7 = 0;
            timer1_copy = timer1;
        }
        
    }
    
    
    TRISB = 0x00;
    PORTB = 0x01;
    
    unsigned int timer0_copy = 0;
    while(0){
        
        if(RB2 == 0 && (timer0 - timer0_copy) >= 1000){
            RB2 = 1;
            timer0_copy = timer0;
        }
        
        if(RB2 == 1 && (timer0 - timer0_copy) >= 1000){
            RB2 = 0;
            timer0_copy = timer0;
        }
        
    }
    
    return;
}


// Timer0 için configuration
void init_timer0(void){
    
    
    INTCONbits.GIE = 1;         // Global interrupt enable yap
    INTCONbits.T0IE = 1;        // Timer0 interrupt enable yap 

    OPTION_REGbits.T0CS = 0;    // clock source seçimi  -  0 : dahili clock, 1: harici clock 
    OPTION_REGbits.PSA = 0;     // prescaler hangisi için kullanılacağı  -  0: Timer0, 1: Watchdog
    OPTION_REGbits.PS = 0b001;  // Prescaler oranı seçimi  -  1:4 (sayacın her bir flag için tam bir ms sayması için)
    
    TMR0 = 0;                   // Timer0 sayacının başlangıç değeri
    
    INTCONbits.T0IF = 0;        // Timer0 interrupt flag i reset et 
            
}

// Timer1 için configuration
void init_timer1(void){
    
    T1CON = 0x00;      // Tüm bitleri ve önceki ayarları sıfırla, temiz başlangıç

    TMR1CS = 0;       // Internal clock (FOSC/4)(CPU nun kendi clock kaynağı) kullanılsın
    T1CKPS0 = 1;      // Prescaler 1:8  oranı seçimi
    T1CKPS1 = 1;

    TMR1H = 0x00;     // Timer sayacının sıfırlanması
    TMR1L = 0x00;

    TMR1IF = 0;       // Interrupt flag temizle, önceki bir sayaçtan set edili şekilde kalması durumuna karşı
    TMR1IE = 1;       // Timer1 interrupt enable, timer1 interrupt a erişebilsin
    PEIE  = 1;        // Peripheral interrupt enable, timer1 peripheral olduğundan bu kapı da açılmalı
    GIE   = 1;        // Global interrupt enable, ana interrupt kapısı gibi düşünülebilir

    TMR1ON = 1;       // Timer1 sayacını başlat
    
    
}

// External interrupt pin(B0) configuration
void init_interrupt_button(void){
    
    
    INTCONbits.GIE = 1;         // Global interrupt enable 
    OPTION_REGbits.INTEDG = 0;  // 0: düşen kenarda tetikle, 1: yükselen kenarda tetikle
    INTCONbits.INTE = 1;         // RB0 external interrupt enable
            
}

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


// FOSC/4 için clock sinyali frekansı , pic için de aynı değer seçilmelidir.
#define _XTAL_FREQ 4000000



// TIMER0  İLE TIMER1 ARASINDAKİ FARK

/*
 
 * Timer0 8 bit olduğundan çabuk taşar(interrupt çabuk tetiklenir) , zaman bu taşmaları sayarak ölçülür
 * Timer1 ise 16 bit olduğu için hemen dolmaz , az interrupt tetiklenir. Bu CPU yu az yorar 
 * 
 * Timer0 ile çabuk interrupt tetkilendiği için ve bu da sürekli interrupt kodlarının çalışması anlamına geldiğinden 
 * uzun zaman ölçümleri için timer1 kullanmak daha hassas bir ölçüm sağlar, yani her taşma ufak bir zaman gecikmesi
 * olabilir.
 * 
 * Osilatör , sinyal üreten donanımdır. Kaynaktır. 
 * Timer0 kendine ait bir osilatör e sahip değildir , sadece gelen darbeleri sayar. Ya pic clock ile çalışır,
 * yada bir pin ile gelen clock ile çalışır. Yani sadece harici clock kullanımını destekler. 
 * 
 * 
 * Timer1 ise harici osilatör(kristal) destekler. 
 * 
 * 
 * 
 * 
 
 */


volatile unsigned char tmr1_overflow_count = 0; // 1 sn için overflow sayacı‚
volatile unsigned int timer1 = 0;              // global sn sayacı
volatile unsigned int timer0 = 0;              // global ms sayacı

void __interrupt() ISR(void)                    // __interrupt özel bir kelimedir, compiler tarafından anlaşılan özel bir komuttur.
{                                               
    
    // Timer0 Interrupt Control
    if(INTCONbits.T0IF && INTCONbits.T0IE){     // Timer0 overflow olup flag set edilmiş mi - aktif mi kontrol et
                                            
        INTCONbits.T0IF = 0;                    // flag i tekrar resetle
        
        // Timer0 8 bit(256 değer) tutabildiği için ve 1000/4 = 250 olduğundan tam 1 ms lik ölçüm için clock sayacını 6 dan başlat
        TMR0 = 6;                               
        timer0++;                               // ms sayacını arttır.
    }
    
    // Timer1 Interrupt Control
    if (TMR1IF && TMR1IE)                       // Timer1 overflow olup flag set edilmiş mi - aktif mi kontrol et
    {
        TMR1IF = 0;                             // Flag reset et, Tekrar flag i temizle ki diğer flag geldiğini kontrol edebilelim
        
        // 1:8 oranı kullanıldığı için 2 kez overflow gerekiyor, tam olarak 1 sn elde edebilmek için bu değerden başlatıldı
        TMR1H = 3036 >> 8;                      // Sayacın tam 1 sn olarak sayabilmesi için başlangıç değeri set etme
        TMR1L = 3036 & 0xFF;
               
        tmr1_overflow_count++;                  // overflow sayacı , 2 tanesi 1 sn yapıyor - prescaler oranı ile tekrar ayarlanabilir

        if (tmr1_overflow_count >= 2)   // 2 sn de bir timer1 sayacı artsın, her artışta overflow sayacı da sıfırlansın 
        {
            tmr1_overflow_count = 0;
            timer1++;  // SADECE HABER VER      // Ana sn sayacı
        }
    }
    
}



void init_timer1(void);
void init_timer0(void);


void clear_pins(void);




int main(int argc, char** argv) {

 

    // OSCCILATOR AYARLARI
    OSCCONbits.IRCF = 0b110; // Bit 6-4: 110 yaparak 4 MHz seçildi
    OSCCONbits.SCS = 1;      // kendi osilatörünü kullansın
    
    
    // TIMER AYARLARI
    init_timer1();
    init_timer0();
    
    
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
    
    
    
    // LED i 2sn aralıklarla yakıp söndürme
    
    RB2 = 1;
    unsigned int led_sure = timer1;
    int loop_time = 1;          // kaç sn bir led durumu değişeceği
    while(0){
        
        if(RB2 == 1 && ((timer1 - led_sure) == loop_time)){
            RB2 = 0;
            led_sure = timer1;

        }else if(RB2 == 0 && ((timer1 - led_sure) == loop_time)){
            RB2 = 1;
            led_sure = timer1;

        }
        
    }
     

    
    
    

    // timer0 ile buton debounce kontrolü ve led blink
    
    // kontrol değişkenleri
    unsigned int timer0_copy = 0;    // butona basıldığı andan itibaren geçen sürenin tutulacağı değişken
    unsigned int timer1_copy = 0;
    char button_pressed = 0;        // butona basılıp basılmadığı flag i
    char button_debounced = 0;       // buton bırakıldığının flag i 
   
    

    while(0){
        
            if(RC6 == 1 && button_pressed == 0){           
                button_pressed = 1;
                timer0_copy = timer0;   // basılma zamanını kaydet, debounce beklemesi için 
            }
            
            
            if(RC6 == 1 && button_pressed == 1 && button_debounced == 0 && (timer0 - timer0_copy) > 20){        
                  
                button_debounced = 1;
                timer1_copy = timer1;
                
            }
            
            if(RC6 == 0 && button_debounced == 1){
                
                RB2 = !RB2;
                
                button_debounced = 0;
                button_pressed = 0;
            }

    }
    
    
    
    
    // pin 2-3-4-5 ile loop şeklinde led yapmak , timer1 sürekli sayar, loop içinde her döngüde sayacın değerinin kaç olduğuna göre işlem yaptırılır
    // timer kullanımı ile cpu kitlenmiyor, diğer işlemler etkilenmemiş olur, yani döngü dönmeye devam eder.
    
    PORTB = 0x00;    
    timer1_copy = 0;
    char led_index = 0;
    char is_started = 0;
    
    
    while(0){
        
        // ilk başlangıçta is_started set edilir ve sonra süreki döngü bu değişken ile devam eder
        if(PORTBbits.RB2 == 0 && is_started == 0){
            
            timer1_copy = timer1;
            RB2 = 1;
            is_started =1;
        }
        
        if(is_started == 1){
            
            if((timer1 - timer1_copy) == 1){     // timer1 1 sn aralıklar ile saydığı için en hızlı 1 sn'de bir led değişimi yapılabilir
                
                clear_pins();           // Tüm pinlerin sıfırlanması
                timer1_copy = 0;
                
                
                led_index++;
                if(led_index > 3){
                    led_index = 0;
                }
                
                
                switch(led_index){          // yeni index e göre pin yakma
                    case 0: RB2 = 1; break;
                    case 1: RB3 = 1; break;
                    case 2: RB4 = 1; break;
                    case 3: RB5 = 1; break;
                }
                
                timer1_copy = timer1;   // led yanma sayacını sıfırla
                
            }
        }
        
        
    }
   
    
    
   
    
    // buton ile tetiklenince led loop başlasın
    // led loop her butona basıp bırakılınca reset olsun ve baştan dönmeye başlasın(ilk loop başlangıcıda aslında bir resettir)
    
    
    TRISB = 0x00;   // Tüm B pinlerini output yap
    PORTB = 0x00;
    TRISC = 0x40;   // C6 pinini input yap
    
    unsigned int led_on_time = 0;   // led in ne kadar süre yanacağının ölçülmesi için zaman tutacak değişken
    char is_led_loop_started = 0;   // led döngüsünün durumunu tutan boolen
    button_pressed = 0;             // butona ilk kez basılma durumunu tutan boolen
    button_debounced = 0;           // butonun debounce olması durumunu tutan değer
    led_index = 0;                  // led loop içindeki led indexi
    timer0_copy = 0;                // buton debounce sayacı kopyası
    
    while(0){
        
        // LED döngüsü sıfırlama için ilk butona basılma anı
        if(RC6 == 1 && button_pressed == 0){           
            button_pressed = 1;  
            timer0_copy = timer0;
        }
        
        
        // butonun debounce olması durumu
        if(RC6 == 1 && button_pressed == 1 && button_debounced == 0 && (timer0 - timer0_copy) > 20){
            
            button_debounced = 1;
        }
        
        // butonun debounce sonrasında LED döngüsünün baştan başlatılması
        if(RC6 == 0 && button_debounced == 1){
            
            // LED loop en baştan tekrar başlasın
            clear_pins();
            is_led_loop_started = 1;
            led_on_time = timer1;
            led_index = 0;
            RB2 = 1;
            
            button_pressed = 0;
            button_debounced = 0;

        }
        
        // led loop başlatılması ve sürekli dönmesi
        if(is_led_loop_started == 1){
            
            
            if((timer1 - led_on_time) == 1){        // 1 sn geçmiş ise led değiştir
                
                
                clear_pins();
                led_index++;
                if(led_index > 3){
                    led_index = 0;
                }
                
                switch(led_index){
                    case 0: RB2 = 1; break;
                    case 1: RB3 = 1; break;
                    case 2: RB4 = 1; break;
                    case 3: RB5 = 1; break;
                }
                
                led_on_time = timer1;   
            }
        }
    }
    
    
    

    
     
    
    
    // Aynı led loop örneği ama bu sefer led loop ilk proğram başlatılınca dönmeye başlayacak
    // eğer butona bir kısa(1 sn altında) basılırsa led loop hangi ledde ise bir sonrakine hemen atlasın
    // eğer butona bir uzun(1 sn üstü) basılırsa led loop kapansın.
    // led loop kapalı iken butona 1sn altında basılırsa döngü tekrar başlasın
    
    
    
    // RC6 pinine buton bağlı
    // RB2, RB3, RB4 , RB5 pinlerine ledler bağlı
    TRISB = 0x00;   // Tüm B pinlerini output yap
    PORTB = 0x00;
    TRISC = 0x40;   // C6 pinini input yap
    
    
    unsigned int button_press_time_ms = 0; // butona basılma süresi - ms cinsinden
    unsigned int button_press_time_sn = 0; // butona basılma süresi - sn cinsinden
    
    button_pressed = 0;                 // butona basılma durumu
    button_debounced = 0;               // buton debounce durumu
    led_on_time = 0;                    // led yanma süresi
    is_led_loop_started = 1;            // led döngüsü otomatik başlayacak
    led_index = 4;                      // led döngüsünde hangi led yanıyor bilgisi, son led ile başlasın ki ilk iterasyonda ilk led yanması ile başlasın
    
    
    while(1){
        
        
        if(RC6 == 1 && button_pressed == 0){
            button_pressed = 1;
            timer0_copy = timer0;
        }

        // butonun debounce kontrolü
        if(RC6 == 1 && button_pressed == 1 && button_debounced == 0 && (timer0 - timer0_copy) >= 20){
            button_debounced = 1;
            button_press_time_ms = timer0;        // butona ne zaman basıldığı bilgisi 
            button_press_time_sn = timer1;        // butona ne zaman basıldığı bilgisi 
            
        }
        
        
        
        // butona basılıp çekilmişse
        if(RC6 == 0 && button_debounced == 1){
            
            
            // eğer kısa basılmışsa
            if((timer0 - button_press_time_ms) < 500){
                
                // aynı zamanda buton bir kez basmak led döngüsünü başlatmışda olur, eğer döngü durdurulmuşsa
                if(is_led_loop_started == 0){
                    
                    is_led_loop_started = 1;
                    led_index = 4;  // Döngüyü ilk ledden başlat
                    
                }else{
                                                                // eğer döngü çalışıyorsa butona basılırsa bir sonraki led e atlasın
                    led_index++;                                // bir sonraki led e atla
                                                                // led yanmasından itibaren süreyi saymaya başla
                    clear_pins();
                    switch(led_index){
                        case 0: RB2 = 1; break;
                        case 1: RB3 = 1; break;
                        case 2: RB4 = 1; break;
                        case 3: RB5 = 1; break;
                    }
                    led_on_time = timer1;
                }
                
                
                
            }else if((timer1 - button_press_time_sn) >= 2){        // eğer 2 sn ve üzeri boyunca butona basılmışsa döngüyü durdur
                is_led_loop_started = 0;
                clear_pins();       // led döngüsünü resetle
            }
                
            button_pressed = 0;     // buton durumunu resetle
            button_debounced = 0;
        }
        
        
        
        if(is_led_loop_started == 1){
            if((timer1 - led_on_time) == 1){    // her 1 saniyede bir led değiştir
                

                clear_pins();

                led_index++;
                if(led_index > 3){
                    led_index = 0;
                }

                switch(led_index){
                    case 0: RB2 = 1; break;
                    case 1: RB3 = 1; break;
                    case 2: RB4 = 1; break;
                    case 3: RB5 = 1; break;
                }
                
                led_on_time = timer1;
            }   
        }
    }
    
    
    
    
    
    
    
    
    return (EXIT_SUCCESS);
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

void init_timer1(void){
    T1CON = 0x00;      // Tüm bitleri ve önceki ayarları sıfırla, temiz başlangıç

    TMR1CS = 0;       // Internal clock (FOSC/4), CPU nun kendi clock kaynağı kullanılsın
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


// Tüm B portlarının clear edilmesi
void clear_pins(void){
    PORTB = 0x00;
}



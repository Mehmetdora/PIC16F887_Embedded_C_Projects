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


void clear_pins(void);




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

    while(0){
        
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
    
    
    
    // pin 2-3-4-5 ile loop şeklinde led yapmak , timer kullanarak bekletmeli(aslında beklemiyor , sadece geçen zaman boyunca clock değerleri 8 bit üzerinde sürekli saydırılıyor)
    // timer kullanımı ile cpu kitlenmiyor, diğer işlemler etkilenmemiş olur
    
    PORTB = 0x00;    
    unsigned int led_counter_ms = 0;
    char led_index = 0;
    char is_started = 0;
    
    
    while(0){
        
        // ilk başlangıçta is_started set edilir ve sonra süreki döngü bu değişken ile devam eder
        if(PORTBbits.RB2 == 0 && is_started == 0){
            
            led_counter_ms = ms_tick;
            RB2 = 1;
            is_started =1;
        }
        
        if(is_started == 1){
            
            if((ms_tick - led_counter_ms) > 500){
                
                clear_pins();           // Tüm pinlerin sıfırlanması
                led_counter_ms = 0;
                
                
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
                
                led_counter_ms = ms_tick;   // led yanma sayacını sıfırla
                
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
    led_index = 0;                  // led loop içindeki led indexi
    
    while(0){
        
        // butona ilk kez basma
        if(RC6 == 1 && button_pressed == 0){           
            button_pressed = 1;           
        }
        
        
        // butonun release olması durumu, tüm loop ve değişkenlerin sıfırlanması
        if(RC6 == 0 && button_pressed == 1){
            // LED loop en baştan tekrar başlasın
            
            clear_pins();
            
            button_pressed = 0;
            is_led_loop_started = 1;
            led_index = 0;
            led_on_time = ms_tick;
            
            RB2 = 1;
            
        }
        
        // led loop başlatılması ve sürekli dönmesi
        if(is_led_loop_started == 1){
            
            
            if((ms_tick - led_on_time) > 1000){
                
                
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
                
                led_on_time = ms_tick;   
            }
        }
    }
    
    
    

    
    
    
    
    // Aynı led loop örneği ama bu sefer led loop ilk proğram başlatılınca dönmeye başlayacak
    // eğer butona bir kısa(500 ms altında) basılırsa led loop hangi ledde ise bir sonrakine hemen atlasın
    // eğer butona bir uzun(1000 ms üstü) basılırsa led loop kapansın.
    
    
    
    // RC6 pinine buton bağlı
    // RB2, RB3, RB4 , RB5 pinlerine ledler bağlı
    TRISB = 0x00;   // Tüm B pinlerini output yap
    PORTB = 0x00;
    TRISC = 0x40;   // C6 pinini input yap
    
    
    unsigned int button_press_time = 0; // butona basılma süresi
    button_pressed = 0;                 // butona basılma durumu
    led_on_time = 0;                    // led yanma süresi
    is_led_loop_started = 1;            // led döngüsü otomatik başlayacak
    led_index = 4;                      // led döngüsünde hangi led yanıyor bilgisi, son led ile başlasın ki ilk iterasyonda ilk led yanması ile başlasın
    
    
    while(1){
        
        
        if(RC6 == 1 && button_pressed == 0){
            button_pressed = 1;
            button_press_time = ms_tick;        // butona ne zaman basıldığı bilgisi 
        }
        
        if(RC6 == 0 && button_pressed == 1 && (ms_tick - button_press_time) > 20){
            // eğer kısa basılırsa bir sonraki led indexine geç, uzun basılırsa döngüyü durdur.
            
            
            if((ms_tick - button_press_time)<500){
                
                // aynı zamanda buton bir kez basmak led döngüsünü başlatmışda olur, eğer döngü durdurulmuşsa
                if(is_led_loop_started == 0){
                    
                    is_led_loop_started = 1;
                    led_index = 4;  // Döngüyü ilk ledden başlat
                    
                }else{
                    
                    led_index++;                                    // bir sonraki led e atla
                                                                // led yanmasından itibaren süreyi saymaya başla
                    clear_pins();
                    switch(led_index){
                        case 0: RB2 = 1; break;
                        case 1: RB3 = 1; break;
                        case 2: RB4 = 1; break;
                        case 3: RB5 = 1; break;
                    }
                    led_on_time = ms_tick;
                }
                
                
                
            }else if((ms_tick - button_press_time)> 1000){
                is_led_loop_started = 0;
                clear_pins();       // led döngüsünü resetle
            }
                
            button_pressed = 0;     // buton durumunu resetle
        }
        
        
        
        if(is_led_loop_started == 1){
            if((ms_tick - led_on_time) > 1000){
                

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
                
                led_on_time = ms_tick;
            }
            
            
            
            
            
        }
        
        
        
    }
    
    
    
    
    
    
    
    return (EXIT_SUCCESS);
}



// Tüm B portlarının clear edilmesi
void clear_pins(void){
    PORTB = 0x00;
}



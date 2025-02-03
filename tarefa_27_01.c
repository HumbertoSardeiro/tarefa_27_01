#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define ledB_pin 12 // Pino do LED azul => GPIO12
#define ledG_pin 11 // Pino do LED verde => GPIO11
#define ledR_pin 13 // Pino do LED vermelho => GPIO13
#define button_0 5 // Botão A => GPIO5
#define button_1 6 // Botão B => GPIO6
#define button_2 22 // Botão de controle direcional => GPIO22
#define tempo 150
#define tempo_1 50

// Variáveis globais para controle do LED (matriz) e cores
uint8_t selected_led = 1; // Índice do LED a ser controlado (0 a 24)
uint8_t selected_r = 1;   // Intensidade do vermelho (0 a 255)
uint8_t selected_g = 1;   // Intensidade do verde (0 a 255)
uint8_t selected_b = 1;   // Intensidade do azul (0 a 255)

// Variáveis globais
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

// Definição dos arrays que representam os LEDs de cada número (com base em uma matriz 5x5)
int num_0[] = {1, 2, 3, 6, 8, 11, 13, 16, 18, 21, 22, 23};  
int num_1[] = {2, 7, 12, 16, 17, 22};   
int num_2[] = {1, 2, 3, 6, 11, 12, 13, 18, 21, 22, 23};       
int num_3[] = {1, 2, 3, 8, 11, 12, 13, 18, 21, 22, 23};     
int num_4[] = {1, 8, 11, 12, 13, 16, 18, 21, 23}; 
int num_5[] = {1, 2, 3, 8, 11, 12, 13, 16, 21, 22, 23};  
int num_6[] = {1, 2, 3, 6, 8, 11, 12, 13, 16, 21, 22, 23}; 
int num_7[] = {2, 7, 12, 18, 21, 22, 23}; 
int num_8[] = {1, 2, 3, 6, 8, 11, 12, 13, 16, 18, 21, 22, 23}; 
int num_9[] = {1, 2, 3, 8, 11, 12, 13, 16, 18, 21, 22, 23};  

// Ponteiros para os arrays de LEDs que representam números
int* num[] = {num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9};

// Tamanhos individuais de cada array (quantos LEDs são acesos por cada número)
int tamanhos[] = {12, 6, 11, 11, 9, 11, 12, 7, 13, 12};  

int array_atual = 0; // Índice do número atual sendo exibido

// Prototipação da função de interrupção de GPIO (botões)
static void gpio_irq_handler(uint gpio, uint32_t events);

// Função para enviar um valor para o pino de controle do LED
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Função para gerar um valor de cor (R, G, B)
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Buffer para armazenar os estados de todos os LEDs
uint32_t led_buffer[NUM_PIXELS] = {0}; 

// Função para atualizar o buffer de LEDs com base no número atual
void update_led_buffer() {
    // Apaga todos os LEDs
    for (int i = 0; i < NUM_PIXELS; i++) {
        led_buffer[i] = 0; // Desliga todos os LEDs
    }

    // Atualiza os LEDs com base no número selecionado
    for (int i = 0; i < tamanhos[array_atual]; i++) {
        led_buffer[num[array_atual][i]] = urgb_u32(selected_r, selected_g, selected_b);
    }
}

// Função para enviar os estados do buffer para os LEDs
void set_leds_from_buffer() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(led_buffer[i]);
    }
}

int main() {
    stdio_init_all();

    // Inicializa os pinos dos LEDs (R, G, B) e configura como saída
    gpio_init(ledR_pin);              
    gpio_set_dir(ledR_pin, GPIO_OUT); 
    gpio_init(button_0);              // Inicializa o pino do Botão 0
    gpio_set_dir(button_0, GPIO_IN);  // Configura o pino como entrada
    gpio_pull_up(button_0);           // Habilita o pull-up interno
    gpio_init(button_1);              // Inicializa o pino do Botão 1
    gpio_set_dir(button_1, GPIO_IN);  // Configura o pino como entrada
    gpio_pull_up(button_1);           // Habilita o pull-up interno

    // Configuração da interrupção com callback para os botões
    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_1, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Inicializa o PIO para controle dos LEDs WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    // Inicializa o programa para os LEDs WS2812
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    while (1) {
        // Pisca o LED vermelho para indicar o funcionamento
        gpio_put(ledR_pin, true);
        sleep_ms(tempo);
        gpio_put(ledR_pin, false);
        sleep_ms(tempo_1);

        // Atualiza o buffer dos LEDs com a cor e LED selecionado
        update_led_buffer();

        // Envia o buffer para os LEDs
        set_leds_from_buffer();
    }

    return 0;
}

// Função de interrupção para manipular os botões
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    
    // Verifica se passou tempo suficiente desde o último evento (debouncing)
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento
        
        // Botão 0: Incrementa o índice do número exibido
        if (gpio == 5) {  // Botão 0
            array_atual = (array_atual + 1) % 10;  // Incrementa e garante que o índice fique entre 0 e 9
            printf("Botão 0 pressionado! Incrementado para: %d\n", array_atual);
        }
        
        // Botão 1: Decrementa o índice do número exibido
        if (gpio == 6) {  // Botão 1
            array_atual = (array_atual - 1 + 10) % 10;  // Decrementa e garante que o índice não vá abaixo de 0
            printf("Botão 1 pressionado! Decrementado para: %d\n", array_atual);
        }

        // Atualiza o buffer com os LEDs do número atual
        update_led_buffer();
    }
}

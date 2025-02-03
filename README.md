# Projeto de Controle de Matriz de LEDs e LED RGB com a Placa BitDogLab

Este projeto foi desenvolvido para controlar LEDs WS2812, especificamente uma matriz de LEDs, utilizando a placa **BitDogLab** baseada no **Raspberry Pi Pico**. O código foi projetado para controlar a exibição de números na matriz de LEDs, com o uso de dois botões para navegar entre os números.

## Descrição

Este sistema permite o controle de uma matriz de LEDs de 5x5, onde cada LED pode ser aceso ou apagado para representar números de 0 a 9. O usuário pode navegar entre esses números utilizando dois botões de controle, e os LEDs são controlados via pinos GPIO do Raspberry Pi Pico.

A placa BitDogLab, junto com o código fornecido, oferece uma maneira prática de implementar e entender como controlar LEDs endereçáveis, como os **WS2812**, que são populares em projetos de iluminação e displays.

## Funcionalidade

- A **matriz de LEDs 5x5** é composta por 25 LEDs no total.
- Cada número (de 0 a 9) é exibido utilizando diferentes LEDs acesos na matriz.
- O projeto utiliza **dois botões** para navegação entre os números:
  - **Botão A** (GPIO5) incrementa o número exibido.
  - **Botão B** (GPIO6) decrementa o número exibido.
- O sistema pode exibir rapidamente os números correspondentes na matriz de LEDs.
  
## Componentes Utilizados

- **BitDogLab (Raspberry Pi Pico)**.
- **LEDs WS2812**: LEDs endereçáveis que permitem controle individual de cor e intensidade.
- **Botões**: Dois botões de controle para alterar os números exibidos na matriz de LEDs.
- **LED RGB**

## Diagrama de Conexões

- **Pino 7 (GPIO7)**: Pino de controle para os LEDs WS2812.
- **Pino 13 (GPIO13)**: Pino para o LED vermelho (usado para indicar funcionamento).
- **Pino 12 (GPIO12)**: Pino para o LED azul.
- **Pino 11 (GPIO11)**: Pino para o LED verde.
- **Botão A**: Conectado ao pino GPIO5 (botão para incrementar o número).
- **Botão B**: Conectado ao pino GPIO6 (botão para decrementar o número).
  
## Como Funciona

1. Ao iniciar o sistema, a matriz de LEDs começa com o número 0 aceso e a led rgb fica psicando em vermelho (5 vezes por segundo).
2. Quando o **Botão A** (GPIO5) é pressionado, o número exibido na matriz aumenta para o próximo valor (circular entre 0 e 9).
3. Quando o **Botão B** (GPIO6) é pressionado, o número exibido diminui para o número anterior (circular entre 0 e 9).
4. A matriz de LEDs é atualizada a cada vez que um botão é pressionado, e a cor dos LEDs é mantida conforme configurada.


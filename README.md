# Automatic Pet Feeder Project

Este projeto está sendo desenvolvido para criar um alimentador automático para animais de estimação, utilizando uma ESP32. Ele utiliza sensores para monitorar a quantidade de ração no pote, medir a temperatura ambiente e exibir informações em um display LCD. Todas as funcionalidades são controladas e monitoradas remotamente via aplicativo Blynk.

## Componentes Utilizados

- **ESP32:** Microcontrolador utilizado como base para o projeto.
- **HC-SR04:** Sensor ultrassônico para medir a quantidade de ração no pote.
- **NTC KY-013:** Sensor de temperatura para monitorar a temperatura ambiente.
- **Display LCD:** Para exibir informações como temperatura, status da ração, etc.
- **Servo Motor:** Utilizado para girar a hélice que dispensa a ração.
- **Blynk:** Plataforma de IoT para controle remoto e monitoramento dos dados.

## Funcionalidades

- **Medição de Quantidade de Ração:** O sensor HC-SR04 é usado para medir a distância até a ração no pote, calculando assim a quantidade restante.
- **Monitoramento de Temperatura:** O sensor NTC KY-013 mede a temperatura ambiente, exibindo-a no display LCD e no aplicativo Blynk.
- **Controle Remoto via Blynk:** Todas as funcionalidades do alimentador podem ser controladas remotamente através do aplicativo Blynk, permitindo ajustes de horário de alimentação, monitoramento da quantidade de ração, etc.
- **Feedback Visual no LCD:** Informações como temperatura atual, quantidade de ração e status do alimentador são exibidas em tempo real no display LCD.

## Estado Atual

Este projeto ainda está em fase de desenvolvimento. As funcionalidades básicas estão sendo implementadas e testadas. A integração com o Blynk está sendo configurada para permitir um controle completo através de dispositivos móveis.

## Próximos Passos

- Finalização da integração com o Blynk para monitoramento remoto completo.
- Ajustes finos nos sensores para melhor precisão na medição da quantidade de ração e temperatura.
- Implementação de lógica adicional para ajuste automático da quantidade de ração baseado no clima ou no comportamento do animal.

## Licença

Este projeto é licenciado sob a Licença MIT. Para mais detalhes, consulte o arquivo [LICENSE](./LICENSE).

---

Este projeto é parte de um esforço para automatizar e melhorar o cuidado com animais de estimação, proporcionando alimentação regular e monitoramento das condições ambientais. Qualquer contribuição ou sugestão é bem-vinda!

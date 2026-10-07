# CP5 - ESP32 com DHT22 e LDR enviando para o HiveMQ

Simulação no Wokwi de um ESP32 que lê temperatura, umidade (DHT22) e luminosidade (LDR), publica os dados no broker público HiveMQ por MQTT e recebe um comando para ligar/desligar um LED. Um dashboard no Node-RED mostra os dados.

**Projeto no Wokwi:** https://wokwi.com/projects/477180866075917313

## Arquivos

| Arquivo | O que é |
|---|---|
| `sketch.ino` | Código do ESP32 |
| `diagram.json` | Circuito do Wokwi (ESP32, DHT22, LDR, LED) |
| `libraries.txt` | Bibliotecas usadas no Wokwi |
| `node-red-flow.json` | Fluxo do Node-RED com o dashboard |

## Como fazer a sua cópia

### 1. Wokwi

1. Faça login em https://wokwi.com.
2. Abra o projeto pelo link acima.
3. Clique na seta ao lado de **Save** e escolha **Save a copy**. A cópia fica na sua conta, com um link só seu.
4. No `sketch.ino`, troque `savio` pelo seu nome nos quatro tópicos:

   ```cpp
   const char* TOPICO_TEMPERATURA = "fiap/cp5/SEUNOME/temperatura";
   const char* TOPICO_UMIDADE = "fiap/cp5/SEUNOME/umidade";
   const char* TOPICO_LUMINOSIDADE = "fiap/cp5/SEUNOME/luminosidade";
   const char* TOPICO_LED = "fiap/cp5/SEUNOME/led";
   ```

   Isso é obrigatório: o HiveMQ é público, e quem usar o mesmo tópico recebe os dados misturados com os de outra pessoa.
5. No `diagram.json`, troque o nome em `"author"` pelo seu.
6. Salve, clique em **Share** e copie o seu link.

### 2. Node-RED

1. Instale e inicie o Node-RED:

   ```
   npm install -g node-red
   npm install -g node-red-dashboard
   node-red
   ```

2. Abra http://localhost:1880, vá em **Menu > Importar** e selecione o `node-red-flow.json`.
3. Nos três nós `mqtt in` e no nó `mqtt out`, troque `savio` pelo mesmo nome que você usou no `sketch.ino`. No nó **Switch LED**, troque também o campo de tópico.
4. Clique em **Implementar** (Deploy).
5. Abra o dashboard em http://localhost:1880/ui.

### 3. Testar

1. Inicie a simulação no Wokwi. O monitor serial deve mostrar `MQTT conectado!` e as leituras a cada 2 segundos.
2. Clique no DHT22 e no LDR no Wokwi para mudar os valores e veja o dashboard acompanhar.
3. Use o interruptor **LED do ESP32** no dashboard para acender o LED vermelho.

Deixe a aba do Wokwi visível durante o teste: a simulação pausa quando a aba fica em segundo plano.

## Tópicos MQTT

| Tópico | Direção | Conteúdo |
|---|---|---|
| `fiap/cp5/savio/temperatura` | ESP32 publica | Temperatura em °C |
| `fiap/cp5/savio/umidade` | ESP32 publica | Umidade em % |
| `fiap/cp5/savio/luminosidade` | ESP32 publica | Luminosidade em % (0 = escuro, 100 = claro) |
| `fiap/cp5/savio/led` | ESP32 assina | `ON` ou `OFF` |

Broker: `broker.hivemq.com`, porta `1883`, sem usuário e senha.

## Ligações

| Componente | Pino do ESP32 |
|---|---|
| DHT22 (dados) | GPIO 15 |
| LDR (saída analógica AO) | GPIO 34 |
| LED (com resistor de 220 Ω) | GPIO 2 |

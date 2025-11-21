# 🌐 Global Solution 2025 – Monitoramento Inteligente de Ergonomia e Bem-Estar no Trabalho

## 👨‍💼 Tema

O futuro do trabalho já está entre nós — e a tecnologia tem papel fundamental em tornar o ambiente mais seguro, saudável e produtivo.

Nossa solução utiliza **ESP32 com IoT**, sensores ambientais e ergonomia, enviando as informações via **MQTT para o Node-RED**, possibilitando monitoramento em tempo real de:

- Temperatura
- Umidade
- Luminosidade
- Postura do usuário

Quando uma condição fora do normal é detectada, o sistema gera avisos visuais, sonoros e de software, ajudando o profissional a manter maior conforto, saúde e rendimento.

---

## 🚩 Problema

Com a adoção massiva de trabalho remoto e híbrido, trabalhadores passam longos períodos em ambientes inadequados, enfrentando problemas como:

- Má postura
- Iluminação insuficiente
- Temperatura desconfortável
- Falta de pausas
- Estresse físico e mental

Esses fatores geram dores, cansaço, menor produtividade e impactos diretos no bem-estar e na saúde a médio e longo prazo.

---

## 💡 Solução Proposta

A solução criada funciona da seguinte forma:

1. **Coleta de Dados com ESP32**

   - Leitura de postura via ultrassônico
   - Luminosidade via LDR
   - Temperatura e umidade via DHT22

2. **Envio dos dados via MQTT**

   - O ESP32 envia leituras periódicas em formato JSON
   - Comunicação com o Node-RED via broker Mosquitto

3. **Dashboard no Node-RED**

   - Exibição de indicadores
   - Gráficos atualizados em tempo real
   - Indicadores de status (OK, ATENÇÃO, ALERTA)

4. **Sistema Inteligente de Alertas**
   - Postura inadequada → alerta para reposicionamento
   - Ambiente escuro → sugestão de iluminação
   - Temperatura fora da faixa ideal → aviso de desconforto

Com esse fluxo, o sistema ajuda o trabalhador a melhorar sua ergonomia e manter um ambiente mais saudável e produtivo ao longo do dia.

---

## 🧩 Arquitetura do Sistema

ESP32 → MQTT Broker → Node-RED → Dashboard Web

- **ESP32** → Coleta e envia dados
- **MQTT Broker (Mosquitto)** → Faz o transporte das mensagens
- **Node-RED** → Processa e exibe as informações
- **Dashboard Web** → Interface visual para o usuário

Exemplo de mensagem enviada pelo ESP32:

```json
{
  "temperatura": 24.7,
  "umidade": 56,
  "luz": 80,
  "postura": 150,
  "status": "MEDIO"
}
```

---

## 🛠 Tecnologias Utilizadas

- **ESP32**
- **MQTT (Mosquitto)**
- **Node-RED**
- **Painel Dashboard UI**
- **Sensores:**
  - DHT22 (temperatura e umidade)
  - LDR (luminosidade)
  - Ultrassônico (simulando postura)
- **Buzzer e LEDs para alertas**

---

## 📡 Fluxo de Comunicação

1. ESP32 realiza a leitura dos sensores
2. Dados são convertidos para **JSON**
3. Valores são publicados em tópicos **MQTT**
4. Node-RED recebe, processa, interpreta e atualiza o dashboard em tempo real
5. Caso necessário, o ESP32 também pode receber comandos do Node-RED (controle de atuadores)

---

## 👥 Integrantes do Grupo

- Eduardo Duran Del Ciel - RM:562017
- Henrique Guedes Silvestre - RM:562474

---

## 📎 Links do Projeto

🔗 **Projeto no Wokwi:**

https://wokwi.com/projects/448196209528593409

🔗 **Projeto no Video:**

https://www.canva.com/design/DAG5XhKs2K0/6R4kKLMRJ5GAZw1m3FxbuQ/watch?utm_content=DAG5XhKs2K0&utm_campaign=designshare&utm_medium=link2&utm_source=uniquelinks&utlId=h62eafa49f6

🔗 **Repositório no GitHub:**

https://github.com/EduDelCiel/Edge?tab=readme-ov-file

---

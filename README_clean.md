# 🌐 Global Solution 2025 — Monitoramento Inteligente de Ergonomia

## Tema

Projeto de monitoramento de ergonomia e bem-estar no trabalho usando ESP32, sensores e MQTT. O objetivo é coletar dados de temperatura, umidade, luminosidade e postura, enviar para um broker MQTT e exibir em um dashboard no Node-RED com alertas em tempo real.

---

## Problema

Ambientes de trabalho remotos ou híbridos frequentemente possuem condições que afetam saúde e produtividade: má postura, iluminação inadequada, temperatura desconfortável e falta de pausas.

---

## Solução proposta

1. Coleta de dados pelo ESP32:

   - Ultrassônico → postura
   - LDR → luminosidade
   - DHT22 → temperatura e umidade

2. Envio por MQTT (payload JSON) para um broker (ex.: Mosquitto)

3. Processamento e visualização no Node-RED (dashboard em tempo real)

4. Alertas locais (LEDs, buzzer) e na interface quando condições estiverem fora do ideal

---

## Arquitetura

ESP32 → MQTT Broker (Mosquitto) → Node-RED → Dashboard Web

Exemplo de payload JSON enviado pelo ESP32:

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

## Tecnologias

- ESP32
- MQTT (Mosquitto)
- Node-RED
- node-red-dashboard
- Sensores: DHT22, LDR, sensor ultrassônico
- Atuadores: buzzer, LEDs

---

## Fluxo de comunicação

1. ESP32 lê sensores e monta JSON
2. Publica em tópicos MQTT (ex.: `ergosense/dados`, `ergosense/led`)
3. Node-RED recebe, processa e atualiza o dashboard
4. Usuário visualiza e recebe alertas em tempo real

---

## Equipe

- Eduardo Duran Del Ciel — RM: 562017
- Henrique Guedes Silvestre — RM: 562474

---

## Links úteis

- Projeto Wokwi: https://wokwi.com/projects/448196209528593409
- Apresentação (Canva): https://www.canva.com/design/DAG5XhKs2K0/6R4kKLMRJ5GAZw1m3FxbuQ/watch
- Repositório: https://github.com/EduDelCiel/Edge

---

# Como executar o projeto (Node-RED)

## Pré-requisitos

- Node.js (v14+)
- Node-RED
- Git (opcional)

## Instalação básica

Instale o Node-RED e nodes úteis globalmente:

```bash
npm install -g node-red
npm install -g node-red-dashboard
npm install -g node-red-node-ui-table
```

Verifique a versão do Node-RED:

```bash
node-red --version
```

## Obter o fluxo Node-RED

Opção 1 — clonar o repositório:

```bash
git clone https://github.com/EduDelCiel/Edge.git
cd Edge
```

Opção 2 — baixar manualmente o arquivo `node-red.json` do GitHub e salvá-lo localmente.

## Importar fluxo no Node-RED (Interface Web)

1. Inicie o Node-RED:

```bash
node-red
```

2. Abra no navegador: http://localhost:1880
3. Menu ≡ → Import → Clipboard
4. Cole o conteúdo do `node-red.json` e clique em Import
5. Clique em Deploy

## Substituir fluxo padrão (opcional)

No Windows, a pasta do Node-RED costuma ser:

```
C:\Users\[SeuUsuário]\.node-red\
```

Procedimento:

1. Pare o Node-RED
2. Faça backup do `flows.json` atual
3. Copie o `node-red.json` para a pasta e renomeie para `flows.json`
4. Inicie o Node-RED

## Rodar a simulação (Wokwi)

1. Inicie o Node-RED e o dashboard (http://localhost:1880/ui)
2. No Wokwi, abra o projeto e clique em "Run Simulation"
3. Verifique tópicos MQTT no Node-RED e a aba Debug

---

## Solução de problemas

- Node-RED não inicia:

```bash
node --version
netstat -an | findstr 1880
node-red -p 1881  # iniciar em porta alternativa
```

- MQTT não conecta:

```bash
ping test.mosquitto.org
# testar com mosquitto_sub / mosquitto_pub
```

- Dashboard não carrega:

```bash
npm list -g node-red-dashboard
# reinstale o node e reinicie o Node-RED
```

---

## Estrutura recomendada do projeto

```
ergosense-global-solution/
├── node-red.json
├── package.json
├── README.md
└── assets/
    ├── images/
    └── docs/
```

---

# 🗑️ Projeto IoT - Monitoramento de Lixeiras (Campus Inteligente)

Este projeto faz parte de uma solução de **Campus Inteligente**, cujo objetivo é monitorar o estado de lixeiras distribuídas pelo ambiente, utilizando microcontroladores ESP32 com sensores ultrassônicos. As informações são enviadas via protocolo MQTT e visualizadas através de uma aplicação web full-stack.

---

## 📦 Estrutura do Projeto

```
.
├── be/           # Código-fonte do backend (Go)
├── fe/           # Código-fonte do frontend (React)
├── Dockerfile    # Dockerfile do banco de dados
├── docker-compose.yml
└── README.md
```

---

## ⚙️ Tecnologias Utilizadas

- 🛠️ Backend: Go + Gorilla Mux + MQTT (Paho) + PostgreSQL (lib/pq)
- 🌐 Frontend: React + Tailwind CSS
- 🧠 Banco de Dados: PostgreSQL
- 📡 Comunicação: MQTT (HiveMQ)
- 🐳 Containerização: Docker + Docker Compose
- 📶 Dispositivo: ESP32 com sensor ultrassônico (HC-SR04)
- 📦 JSON via ArduinoJson

---

## 🚀 Como Executar o Projeto

### Pré-requisitos

- [Docker](https://www.docker.com/get-started)
- [Docker Compose](https://docs.docker.com/compose/install/)

### Passos

1. Clone este repositório:

2. Execute o ambiente completo:

```bash
docker-compose up --build
```

3. Os serviços ficarão disponíveis nas seguintes portas:

| Serviço      | Endereço                      | Porta     |
|--------------|-------------------------------|-----------|
| Frontend     | http://localhost:3000         | 3000      |
| Backend (API)| http://localhost:7000         | 7000      |
| PostgreSQL   | localhost                     | 5432      |
| MQTT Broker  | mqtt://localhost              | 1883      |
| HiveMQ UI    | http://localhost:8080         | 8080      |

---

## 🧪 Testando a Aplicação

- Acesse http://localhost:3000 para visualizar as estações (lixeiras).
- O backend fornece os seguintes endpoints REST em http://localhost:7000:

```
GET    /estacoes         -> Lista todas as estações
GET    /estacao/{id}     -> Retorna uma estação
POST   /estacao          -> Cria uma nova estação
PUT    /estacao/{id}     -> Atualiza o status da estação
DELETE /estacao/{id}     -> Deleta a estação
```

- O ESP32 publica mensagens JSON no tópico MQTT `sensor/lixeira`, como no exemplo:

```json
{
  "id": "AB:CD:EF:12:34:56",
  "status": "full"
}
```

---

## 📁 Sobre os Containers

O `docker-compose.yml` define 4 serviços:

- `db`: Container do PostgreSQL com volume persistente `postgres_data`
- `mqtt`: Broker HiveMQ MQTT
- `be`: Backend Go com listener MQTT e API REST
- `fe`: Frontend React com atualização automática a cada 20s

Todos conectados na mesma rede interna `backend-network`.

---

## 📡 ESP32 - Funcionamento

- Lê distância com sensor ultrassônico (pinos `trigPin` e `echoPin`)
- Define `status = "full"` se a distância for menor que 10cm
- Envia mensagem JSON via MQTT com ID (MAC) e status da lixeira
- Código completo está na pasta `firmware/` (ou no anexo do projeto)

---

## 📂 Volumes e Logs

- Logs do backend são armazenados em `./logs`
- Dados do banco de dados persistem no volume Docker `postgres_data`

---

## 👨‍🏫 Execução pelo Professor

1. Verifique se Docker e Docker Compose estão instalados.
2. Clone o repositório e execute o comando:

```bash
docker-compose up --build
```

3. Acesse a aplicação em `http://localhost:3000` e visualize o funcionamento.

---

## 👤 Autores

- Helio Henrique Medeiros Silva
- Éric
- Maurício

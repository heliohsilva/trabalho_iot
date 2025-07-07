package main

import (
	"fmt"
	"log"
	"net/http"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/gorilla/mux"
	"github.com/rs/cors"
)

const (
	mqttBroker = "tcp://mqtt:1883"
	mqttTopic  = "sensor/lixeira"
)

func main() {
	InitDB()

	opts := mqtt.NewClientOptions().AddBroker(mqttBroker)
	opts.SetClientID("bd_caller" + time.Now().Format("150405"))
	opts.SetDefaultPublishHandler(func(client mqtt.Client, msg mqtt.Message) {
		messageHandler(client, msg)
	})

	client := mqtt.NewClient(opts)

	if token := client.Connect(); token.Wait() && token.Error() != nil {
		log.Fatal(token.Error)
	}

	if token := client.Subscribe(mqttTopic, 0, nil); token.Wait() && token.Error() != nil {
		log.Fatal(token.Error())
	}

	fmt.Println("Subscribed to topic: ", mqttTopic)

	r := mux.NewRouter()
	r.HandleFunc("/estacao", CreateEstacao).Methods("POST")
	r.HandleFunc("/estacoes", GetEstacoes).Methods("GET")
	r.HandleFunc("/estacao/{id}", GetEstacao).Methods("GET")
	r.HandleFunc("/estacao/{id}", UpdateEstacao).Methods("PUT")
	r.HandleFunc("/estacao/{id}", DeleteEstacao).Methods("DELETE")

	c := cors.New(cors.Options{
		AllowedOrigins:   []string{"http://localhost:3000"},
		AllowedMethods:   []string{"GET", "POST", "PUT", "DELETE"},
		AllowCredentials: true,
	})

	log.Println("API running at http://localhost:7000")
	log.Fatal(http.ListenAndServe(":7000", c.Handler(r)))
}

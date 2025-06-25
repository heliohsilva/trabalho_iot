package main

import (
	"log"
	"net/http"

	"github.com/rs/cors"

	"github.com/gorilla/mux"
)

func main() {
	InitDB()

	r := mux.NewRouter()
	r.HandleFunc("/estacao", CreateEstacao).Methods("POST")
	r.HandleFunc("/estacoes", GetEstacoes).Methods("GET")
	r.HandleFunc("/estacao/{id}", GetEstacao).Methods("GET")
	r.HandleFunc("/estacao/{id}", UpdateEstacao).Methods("PUT")
	r.HandleFunc("/estacao/{id}", DeleteEstacao).Methods("DELETE")

	log.Println("API running at http://localhost:8080")

	c := cors.New(cors.Options{
		AllowedOrigins:   []string{"http://localhost:3000"},
		AllowedMethods:   []string{"GET", "POST", "PUT", "DELETE"},
		AllowCredentials: true,
	})

	log.Fatal(http.ListenAndServe(":8080", c.Handler(r)))
}

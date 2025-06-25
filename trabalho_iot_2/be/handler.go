package main

import (
	"database/sql"
	"encoding/json"
	"log"
	"net/http"

	"github.com/gorilla/mux"
)

type Estacao struct {
	ID     string `json:"id"`     // MAC address
	Status string `json:"status"` // e.g., "full", "empty"
}

func CreateEstacao(w http.ResponseWriter, r *http.Request) {
	var e Estacao
	err := json.NewDecoder(r.Body).Decode(&e)
	if err != nil {
		http.Error(w, "Invalid JSON", http.StatusBadRequest)
		return
	}

	_, err = DB.Exec(`INSERT INTO estacao (id, status) VALUES ($1, $2)
                      ON CONFLICT (id) DO UPDATE SET status = EXCLUDED.status`, e.ID, e.Status)
	if err != nil {
		log.Println("Insert error:", err)
		http.Error(w, "Database error", http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusCreated)
}

func GetEstacoes(w http.ResponseWriter, r *http.Request) {
	rows, err := DB.Query("SELECT id, status FROM estacao")
	if err != nil {
		log.Println("Query error:", err)
		http.Error(w, "Database error", http.StatusInternalServerError)
		return
	}
	defer rows.Close()

	var estacoes []Estacao
	for rows.Next() {
		var e Estacao
		if err := rows.Scan(&e.ID, &e.Status); err != nil {
			log.Println("Row scan error:", err)
			http.Error(w, "Database error", http.StatusInternalServerError)
			return
		}
		estacoes = append(estacoes, e)
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(estacoes)
}

func GetEstacao(w http.ResponseWriter, r *http.Request) {
	vars := mux.Vars(r)
	id := vars["id"]

	var e Estacao
	err := DB.QueryRow("SELECT id, status FROM estacao WHERE id = $1", id).Scan(&e.ID, &e.Status)
	if err != nil {
		if err == sql.ErrNoRows {
			http.Error(w, "Estacao not found", http.StatusNotFound)
		} else {
			log.Println("Query error:", err)
			http.Error(w, "Database error", http.StatusInternalServerError)
		}
		return
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(e)
}

func UpdateEstacao(w http.ResponseWriter, r *http.Request) {
	vars := mux.Vars(r)
	id := vars["id"]

	var e Estacao
	err := json.NewDecoder(r.Body).Decode(&e)
	if err != nil {
		http.Error(w, "Invalid JSON", http.StatusBadRequest)
		return
	}

	if e.ID != id {
		http.Error(w, "ID mismatch", http.StatusBadRequest)
		return
	}

	_, err = DB.Exec("UPDATE estacao SET status = $1 WHERE id = $2", e.Status, id)
	if err != nil {
		log.Println("Update error:", err)
		http.Error(w, "Database error", http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusNoContent)
}

func DeleteEstacao(w http.ResponseWriter, r *http.Request) {
	vars := mux.Vars(r)
	id := vars["id"]

	_, err := DB.Exec("DELETE FROM estacao WHERE id = $1", id)
	if err != nil {
		log.Println("Delete error:", err)
		http.Error(w, "Database error", http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusNoContent)
}

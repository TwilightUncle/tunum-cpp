package setting

import (
	"encoding/json"
	"os"
)

type Env struct {
	Name string `json:"env"`
}

func GetEnv() (string, error) {
	raw_json, err := os.ReadFile("./env.json")
	if err != nil {
		return "", err
	}

	var env Env
	json.Unmarshal(raw_json, &env)

	return env.Name, nil
}

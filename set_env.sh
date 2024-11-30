#!/bin/bash

ENV_FILE="./.env"

if [ ! -f "$ENV_FILE" ]; then
    echo "Error: $ENV_FILE not found."
    exit 1
fi

while IFS='=' read -r key value; do
    if [[ "$key" =~ ^#.*$ ]] || [[ -z "$key" ]]; then
        continue
    fi
    export "$key"="$value"
	echo "$key"
done < "$ENV_FILE"

echo "Environment variables loaded from $ENV_FILE."

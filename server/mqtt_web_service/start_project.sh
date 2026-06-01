#!/bin/bash
# Example startup script — adjust PROJECT_DIR for your server.
set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
VENV_DIR="${PROJECT_DIR}/venv"
LOG_DIR="${PROJECT_DIR}/logs"
GUNICORN_CMD="${VENV_DIR}/bin/gunicorn"
APP_MODULE="run:app"
HOST="${HOST:-127.0.0.1}"
PORT="${PORT:-8000}"
WORKERS="${WORKERS:-4}"

mkdir -p "${LOG_DIR}"
source "${VENV_DIR}/bin/activate"
set -a
[ -f "${PROJECT_DIR}/.env" ] && source "${PROJECT_DIR}/.env"
set +a

exec "${GUNICORN_CMD}" \
  --name "mqtt-web-service" \
  --chdir "${PROJECT_DIR}" \
  --workers "${WORKERS}" \
  --bind "${HOST}:${PORT}" \
  --access-logfile "${LOG_DIR}/access.log" \
  --error-logfile "${LOG_DIR}/error.log" \
  --log-level info \
  "${APP_MODULE}"

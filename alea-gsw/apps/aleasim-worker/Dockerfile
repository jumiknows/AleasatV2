# ----------------------
# Stage 1: Build
# ----------------------

FROM python:3.11-slim AS builder

ARG APP_PATH=/app
ARG POETRY_VERSION=1.8.3

ENV POETRY_VIRTUALENVS_CREATE=1 \
    POETRY_VIRTUALENVS_IN_PROJECT=1 \
    PYTHONDONTWRITEBYTECODE=1 \
    PYTHONUNBUFFERED=1

WORKDIR ${APP_PATH}

# Copy alea-sim local path dependency BEFORE poetry install
COPY alea-fsw/python/alea-sim /alea-fsw/python/alea-sim

# Copy poetry config, lockfile, and system package list
COPY alea-gsw/apps/aleasim-worker/pyproject.toml .
COPY alea-gsw/apps/aleasim-worker/poetry.lock .
COPY alea-gsw/apps/aleasim-worker/packages.debian .

# Install system packages and poetry
RUN apt-get update && \
    xargs -a packages.debian apt-get install -y --no-install-recommends && \
    pip install --no-cache-dir pipx && \
    pipx install poetry==${POETRY_VERSION}

# Install Python dependencies (with alea-sim path available)
RUN /root/.local/bin/poetry install --no-root --only main

# Copy the actual worker source code after dependencies
COPY alea-gsw/apps/aleasim-worker/alea ${APP_PATH}/alea


# ----------------------
# Stage 2: Runtime
# ----------------------

FROM python:3.11-slim AS runtime

ARG APP_PATH=/app

# Set PYTHONPATH so alea.sim module can be found at runtime
ENV PYTHONPATH="/alea-fsw/python:${PYTHONPATH}"

# Activate virtual environment and set app-specific env vars
ENV PATH="${APP_PATH}/.venv/bin:$PATH" \
    VIRTUAL_ENV=${APP_PATH}/.venv \
    ALEASIM_RUN_WORKER=1 \
    PORT=3000

# Create unprivileged user
RUN useradd --create-home appuser

WORKDIR ${APP_PATH}

# Copy worker app code and virtual environment from builder
COPY --from=builder --chown=appuser:appuser ${APP_PATH}/alea ${APP_PATH}/alea
COPY --from=builder --chown=appuser:appuser ${APP_PATH}/.venv ${APP_PATH}/.venv

# Copy alea-sim local dependency for runtime
COPY --from=builder --chown=appuser:appuser /alea-fsw /alea-fsw

RUN apt-get update && apt-get install -y --no-install-recommends curl && rm -rf /var/lib/apt/lists/*

HEALTHCHECK --interval=30s --timeout=5s --start-period=10s --retries=3 \
  CMD curl -f http://localhost:${PORT:-3000}/healthz || exit 1

USER appuser

# Run the worker module
CMD ["python", "-m", "alea.location_calculation"]

# `python-base` sets up all our shared environment variables
FROM python:3.10-slim as python-base

ENV PACKAGE_PATH="alea-gsw/apps/obc-grpc"

# python
ENV PYTHONUNBUFFERED=1 \
  # prevents python creating .pyc files
  PYTHONDONTWRITEBYTECODE=1 \
  \
  # pip
  PIP_NO_CACHE_DIR=off \
  PIP_DISABLE_PIP_VERSION_CHECK=on \
  PIP_DEFAULT_TIMEOUT=100 \
  \
  # poetry
  # https://python-poetry.org/docs/configuration/#using-environment-variables
  POETRY_VERSION=1.8.3 \
  # make poetry install to this location
  POETRY_HOME="/opt/poetry" \
  # make poetry create the virtual environment in the project's root
  # it gets named `.venv`
  POETRY_VIRTUALENVS_IN_PROJECT=true \
  # do not ask any interactive question
  POETRY_NO_INTERACTION=1 \
  \
  # paths
  # this is where our requirements + virtual environment will live
  VENV_PATH="/app/$PACKAGE_PATH/.venv"

# prepend poetry and venv to path
ENV PATH="$VENV_PATH/bin:$PATH"

# `builder-base` stage is used to build deps + create our virtual environment
FROM python-base as builder-base
ENV PATH="$POETRY_HOME/bin:$PATH"

RUN apt-get update \
  && apt-get install --no-install-recommends -y \
  # deps for installing poetry
  curl \
  # deps for building python deps
  build-essential

# install poetry - respects $POETRY_VERSION & $POETRY_HOME
RUN curl -sSL https://install.python-poetry.org | python

# copy project requirement files here to ensure they will be cached.
WORKDIR /app
COPY . .

WORKDIR /app/$PACKAGE_PATH

# Change install path/devlop/editable package with copy instead of symlink
# So I don't need to copy the whole project or find the package it depend on
RUN cd /app && find . -type d \( -name node_modules -o -name cache \) -prune -o  -name pyproject.toml -exec sed -i 's/develop = true/develop = false/g' {} \;
RUN cd /app && find . -type d \( -name node_modules -o -name cache \) -prune -o  -name poetry.lock -exec sed -i 's/develop = true/develop = false/g' {} \;
RUN poetry lock --no-update

# install runtime deps - uses $POETRY_VIRTUALENVS_IN_PROJECT internally
RUN poetry install --without dev

# `production` image used for runtime
FROM python-base as production

COPY --from=builder-base /app/$PACKAGE_PATH/.venv /app/$PACKAGE_PATH/.venv
COPY ./$PACKAGE_PATH /app/$PACKAGE_PATH

WORKDIR /app/$PACKAGE_PATH

CMD ["opentelemetry-instrument", "python","-m","alea.obc_grpc"]
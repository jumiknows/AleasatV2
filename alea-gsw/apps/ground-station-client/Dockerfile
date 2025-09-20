ARG APP_PATH=/app

FROM python:3.11-slim AS builder

ARG APP_PATH
ARG POETRY_VERSION=1.8.3

ENV POETRY_VIRTUALENVS_CREATE=1
ENV POETRY_VIRTUALENVS_IN_PROJECT=1
ENV POETRY_VIRTUALENVS_OPTIONS_SYSTEM_SITE_PACKAGES=1
ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1

WORKDIR ${APP_PATH}

COPY pyproject.toml poetry.lock poetry.toml packages.debian ./

# Install various dependecies
# - packages.debian: various for building and running the application
# - pipx: to install poetry
# - build-essential: needed for `poetry install`
# - python3-dev: needed for `poetry install`
RUN apt-get update \
	&& xargs -a packages.debian apt-get install -qy pipx build-essential python3-dev

RUN pipx install poetry==${POETRY_VERSION}

# Install dependencies using poetry
# /root/.local/bin/poetry: because pipx doesn't add poetry to path apparently
# --no-root: dependecies only
# --only main: don't include dev dependencies
RUN /root/.local/bin/poetry install --no-root --only main

FROM debian:12-slim

ARG APP_PATH
ARG GROUND_STATION_CLIENT_UID=1000
ARG GROUND_STATION_CLIENT_VARSTATEDIR=/var/lib/ground-station-client

ENV API_PORT=3000
ENV PATH="${APP_PATH}/.venv/bin:$PATH"
ENV VIRTUAL_ENV=${APP_PATH}/.venv

EXPOSE ${API_PORT}

# Add unprivileged system user
RUN groupadd -r -g ${GROUND_STATION_CLIENT_UID} ground-station-client \
	&& useradd -r -u ${GROUND_STATION_CLIENT_UID} \
		-g ground-station-client \
		-d ${GROUND_STATION_CLIENT_VARSTATEDIR} \
		-s /bin/false \
		-G audio,dialout,plugdev \
		ground-station-client

# Create application varstate directory
# I'm honestly not sure what this is really for so I'm just copying from the original Dockerfile
RUN install -d -m 1777 -o ${GROUND_STATION_CLIENT_UID} -g ${GROUND_STATION_CLIENT_UID} ${GROUND_STATION_CLIENT_VARSTATEDIR}

RUN apt-get update

# Reinstall packages because those aren't copied over and are needed for the dependencies
COPY packages.debian ./
RUN xargs -a packages.debian apt-get install -qy

# gr-satnogs + GNU Radio
RUN apt install -y \
    libboost-dev \
    libitpp-dev \
    libboost-date-time-dev \
    libboost-filesystem-dev \
    libboost-program-options-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-regex-dev \
    libboost-test-dev \
    swig \
    cmake \
    build-essential \
    pkg-config \
    gnuradio-dev \
    libconfig++-dev \
    libgmp-dev \
    liborc-0.4-0 \
    liborc-0.4-dev \
    liborc-0.4-dev-bin \
    nlohmann-json3-dev \
    libpng++-dev \
    libvorbis-dev \
    git \
    libhamlib-dev \
    libhamlib++-dev \
    libgsl-dev \
    # for gr-satnogs, unmentioned dependency
    libsndfile-dev

RUN git clone https://gitlab.com/librespacefoundation/satnogs/gr-satnogs.git \
    && cd gr-satnogs \
    # latest on main as of 2024-10-03
    && git checkout 69f1bb73 \
    && git submodule update --init --recursive \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make -j $(nproc --all) \
    && make install \
    && ldconfig

# gr-difi
RUN rm /usr/lib/python3.11/EXTERNALLY-MANAGED
RUN apt-get install -y python3-pip cmake liborc-dev doxygen
RUN pip install pytest pybind11
RUN git clone https://github.com/DIFI-Consortium/gr-difi \
    && cd gr-difi \
    # latest on main as of 2024-08-21
    && git checkout 4f1ab74 \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make -j4 \
    && make install \
    && ldconfig

# Clean up
RUN rm -r /var/lib/apt/lists/*
RUN rm packages.debian

# Copy application files
WORKDIR ${APP_PATH}
# Ownership of the app path is needed because SQLite needs it https://stackoverflow.com/a/3330616
RUN chown 1000 ${APP_PATH}
COPY --chown=${GROUND_STATION_CLIENT_UID} --from=builder ${VIRTUAL_ENV} ${VIRTUAL_ENV}
COPY --chown=${GROUND_STATION_CLIENT_UID}  . .

# Install production WSGI server
RUN pip install waitress

# Switch to unprivileged user
USER ${GROUND_STATION_CLIENT_UID}

CMD waitress-serve --host=0.0.0.0 --port=$API_PORT groundstationclient:app
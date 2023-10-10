FROM ubuntu:22.04

RUN apt update -y && apt upgrade -y
COPY . /app
WORKDIR /app

RUN ./install.sh

ENTRYPOINT ./run.sh

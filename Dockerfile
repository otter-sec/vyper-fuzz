FROM ubuntu:22.04

COPY . /app

WORKDIR /app
RUN apt update -y
RUN ./install.sh

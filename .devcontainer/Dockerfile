# Minimal Docker image for CompactTree development using Ubuntu base
FROM ubuntu:latest
MAINTAINER Niema Moshiri <niemamoshiri@gmail.com>
RUN apt-get update && apt-get -y upgrade && \
    apt-get install -y g++ git make && \
    rm -rf /root/.cache /tmp/*

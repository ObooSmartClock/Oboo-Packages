FROM gcc
# gcc image details: https://hub.docker.com/_/gcc/``

COPY . /usr/src/myapp

WORKDIR /usr/src/myapp/oboo-runtime/

RUN apt-get update && apt-get install -y \
	libcurl4-gnutls-dev \
	libmosquitto-dev \
	mosquitto-clients \
	mosquitto


#CMD service mosquitto start
CMD ["make"]

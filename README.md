# Productor-Consumidor

Para compilar: 

creater: gcc creater.c -o c -lrt -pthread

producer: gcc producer.c -o p -lrt -pthread -lm

consumer: gcc consumer.c -o con -lrt -pthread -lm

finalizer: gcc finalizer.c -o f

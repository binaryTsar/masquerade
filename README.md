# masquerade

This project is dependant on OpenSSL 1.1.0f. By default, that is installed in /usr/local/openssl. It must be moved to usr/include/openssl.


Each user must have a certificate signed by the root, using their username (19 char max) as the common name. They also need a folder and
inbox on the server. 

$ openssl genrsa -aes128 -out user.key 2048
$ openssl req -new -key user.key -out user.csr
$ openssl x509 -req -days 360 -in user.csr -CA root.pem -CAkey root.key -CAserial root.srl -out user.pem



#!/bin/sh

SSL_KEY_FILE=${PWD}/ssl.key
SSL_CSR_FILE=${PWD}/ssl.csr

if [ -f "$SSL_KEY_FILE" ] && [ -f "$SSL_CSR_FILE" ]; then
	echo "ssl.key & ssl.csr file exist"
else
	/usr/bin/expect make_cert.exp
fi

# make crt
openssl x509 -req -days 365 -in ssl.csr -signkey ssl.key -out cert.crt

# make key
cp ssl.key ssl.key.secure
openssl rsa -in ssl.key.secure -out cert.key
rm ssl.key.secure

mkdir cert
mv cert.* cert/.

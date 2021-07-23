#include <stdio.h>
#include <string.h>

void encrypt(int key, char* buf) {
	char temp;
	for(int i=0; i<strlen(buf); i++) {
		temp = buf[i] + key;
		buf[i] = temp % 256;
	}
}

void decrypt(int key, char* buf) {
	char temp;
	for(int i=0; i<strlen(buf); i++) {
		temp = buf[i] - key;
		buf[i] = temp % 256;
	}
}

